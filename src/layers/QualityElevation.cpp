
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <atomic>

#include <ogr_api.h>
#include <ogr_spatialref.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#pragma warning (pop)
#include <cpl_vsi.h>

#include <omp.h>

#include <ZFXMath.h>

#include "../WebMapService.h"
#include "../utils/ImageProcessor.h"
#include "../utils/Filesystem.h"
#include "../utils/Elevation.h"

using namespace std;
using namespace std::chrono;

using namespace ZFXMath;

using namespace dw;
using namespace dw::utils;

namespace dw
{
namespace Layers
{
	const string LayerName = "QualityElevation";
	const string LayerTitle = "ASTER + SRTMv4 High Quality Elevation Service";
	const string LayerAbstract =
		"The native resolution of this layer is 1 arc second per pixel."
		"A request's bounding box must not exceed 1° in width and height."
		"It will throw an InvalidBBox Service Exception otherwise.";

	class QualityElevation : public WebMapService::Layer
	{
		struct SrcDestTransfromId
		{
			const OGRSpatialReference* src;
			const OGRSpatialReference* dst;

			bool operator< (const QualityElevation::SrcDestTransfromId& other) const
			{
				return memcmp(this, &other, sizeof(QualityElevation::SrcDestTransfromId)) < 0;
			}
		};

		struct ASTERTile
		{
			int longitude;
			int latitude;
			string filename_dem;
			string filename_num;
		};

		struct ASTERTileContent
		{
			s16* elevation;
			//s16* quality;

			int width;
			int height;
			int pitchInPixels;

			double geoTransform[6];
		};

		map<string, OGRSpatialReference*> supportedCRS;
		OGRSpatialReference* ASTER_SpatRef;
		OGRSpatialReference* SRTM_SpatRef;
		map<SrcDestTransfromId, OGRCoordinateTransformation*> srsTransforms;

		ASTERTile* asterTiles;
		int asterTileStartLatitude;
		int asterTileEndLatitude;
		const int AsterTileStartLongitude = -180;
		const int AsterTileEndLongitude = 179;
		const int NumASTERTilesX = AsterTileEndLongitude - AsterTileStartLongitude + 1;
		const int MissingTileCoordinate = -1000;
		const int AsterPixelsPerDegree = 3600;
		const double AsterDegreesPerPixel = 1.0 / (double)AsterPixelsPerDegree;

	public:

		virtual bool Init(/* layerconfig */) override
		{
			GDALRegister_GTiff();

			supportedCRS =
			{
				//pair<string, OGRSpatialReference*>("EPSG:3857", (OGRSpatialReference*)OSRNewSpatialReference(NULL)),
				pair<string, OGRSpatialReference*>("EPSG:4326",  (OGRSpatialReference*)OSRNewSpatialReference(NULL))
			};

			for (auto it = supportedCRS.begin(); it != supportedCRS.end(); it++)
			{
				int epsgCode = atoi(it->first.c_str() + 5);
				OGRErr err = it->second->importFromEPSG(epsgCode);
				if (err == OGRERR_UNSUPPORTED_SRS)
				{
					return false;
				}
			}

			ASTER_SpatRef = supportedCRS["EPSG:4326"];
			SRTM_SpatRef = supportedCRS["EPSG:4326"];

			// create all possible SRS transformation permutations
			for (auto it1 = supportedCRS.begin(); it1 != supportedCRS.end(); it1++)
			{
				for (auto it2 = it1; it2 != supportedCRS.end(); it2++)
				{
					if (it2 == supportedCRS.end()) break;

					CreateTransform(it1->second, it2->second);
					CreateTransform(it2->second, it1->second);
				}
			}

			// load antarctic data
			{
				const path antarcticDataPath("E:/NSIDC-0082/ramp200dem_wgs_v2.bin");

				shared_ptr<Image> antarcticData;
				Image::LoadContentFromFile(antarcticDataPath.string(), CT_Image_Raw_S16, antarcticData);
				Image& ad = *antarcticData.get();
				ConvertContentTypeToRawImage(ad);
				ad.width = 28680;
				ad.height = 24580;
				ad.rawPixelSize = DataTypePixelSize[DT_S16];

				ConvertBigEndianToLocalEndianness(ad);
			}

			// load TOC from disk
			//const path ASTERSourceDir("Z:/InnovationLab/ASTER/");
			//const path ASTERSourceDir("C:/Dev/temp/ASTER/");
			const path ASTERSourceDir("E:/ASTER/");
			//const path ASTERSourceDir("/home/rsc/Desktop/ASTER/");

			unique_ptr<u8> fileExists(new u8[NumASTERTilesX * 180]);
			memset(fileExists.get(), 0, NumASTERTilesX * 180);
			asterTileStartLatitude = 90;
			asterTileEndLatitude = -90;
			for (directory_iterator di(ASTERSourceDir); di != fsend(di); di++)
			{
				const auto& entity = *di;
				const auto extension = entity.path().extension();
				if (is_regular_file(entity.status()) && extension.generic_string() == ".zip")
				{
					string filepath = entity.path().string();
					int latitudeSign = 1;
					int longitudeSign = 1;

					size_t coordinateStart = filepath.find_last_of('_');
					size_t latOffset = filepath.find_last_of('N');
					if (latOffset == std::string::npos || latOffset < coordinateStart)
					{
						latOffset = filepath.find_last_of('S');
						latitudeSign = -1;
					}
					size_t lonOffset = filepath.find_last_of('E');
					if (lonOffset == std::string::npos || lonOffset < coordinateStart)
					{
						lonOffset = filepath.find_last_of('W');
						longitudeSign = -1;
					}
					if (latOffset != std::string::npos)
					{
						int latitude = atoi(filepath.c_str() + latOffset + 1) * latitudeSign;
						int longitude = atoi(filepath.c_str() + lonOffset + 1) * longitudeSign;

						fileExists.get()[(latitude + 90) * NumASTERTilesX + longitude + 180] = true;

						asterTileStartLatitude = min(asterTileStartLatitude, latitude);
						asterTileEndLatitude = max(asterTileEndLatitude, latitude);
					}
				}
			}

			const int NumASTERTilesX = 360;
			const int NumASTERTilesY = max(0, asterTileEndLatitude - asterTileStartLatitude + 1);

			if (NumASTERTilesY == 0)
			{
				cout << "Quality Elevation Layer: " << "ASTER data not found in " << ASTERSourceDir << endl;
				return false;
			}

			asterTiles = new ASTERTile[NumASTERTilesX * NumASTERTilesY];
			for (int y = asterTileStartLatitude; y <= asterTileEndLatitude; y++)
			{
				for (int x = AsterTileStartLongitude; x <= AsterTileEndLongitude; x++)
				{
					auto& tile = asterTiles[(y - asterTileStartLatitude) * NumASTERTilesX + (x - AsterTileStartLongitude)];
					tile.longitude = x;
					tile.latitude = y;

					int absX = abs(x);
					int absY = abs(y);

					string filename = ((y < 0) ? "S" : "N");
					if (absY < 10) filename += "0";
					filename += to_string(absY) + ((x < 0) ? "W" : "E");
					if (absX < 10) filename += "00";
					else if (absX < 100) filename += "0";
					filename += to_string(absX);

					string zipFilename = ASTERSourceDir.string() + "ASTGTM2_" + filename + ".zip";

					if (fileExists.get()[(y + 90) * NumASTERTilesX + x + 180])
					{
						tile.filename_dem = "/vsizip/" + zipFilename + "/ASTGTM2_" + filename + "_dem.tif";
						tile.filename_num = "/vsizip/" + zipFilename + "/ASTGTM2_" + filename + "_num.tif";
					}
					else
					{
						tile.latitude = MissingTileCoordinate;
						tile.longitude = MissingTileCoordinate;
					}
				}
			}

			return true;
		}

		virtual const char* GetName() const override
		{
			return LayerName.c_str();
		}

		virtual const char_t* GetTitle() const override
		{
			return LayerTitle.c_str();
		}

		virtual const char_t* GetAbstract()  const override
		{
			return LayerAbstract.c_str();
		};

		virtual const vector<DataType>& GetSuppordetFormats() const override
		{
			static const vector<DataType> SuppordetFormats = { DT_U8, DT_S16 };
			return SuppordetFormats;
		}

		virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, Image& img) override
		{
			auto crs = supportedCRS.find(gmr.crs);
			if (crs == supportedCRS.end())
			{
				return HGMRR_InvalidSRS;
			}

			switch (gmr.dataType)
			{
				case DT_S16:
				{
					return HandleGetMapRequest(gmr, crs->second, img);
				}
				case DT_U8:
				{
					Image s16Img(gmr.width, gmr.height, DT_S16);
					auto result = HandleGetMapRequest(gmr, crs->second, s16Img);
					if (result != HGMRR_OK) return result;
					u8* dstGreyScaleEnd = img.rawData + gmr.width * gmr.height;
					u8* dstGreyScale = img.rawData;
					s16* elevation = (s16*)s16Img.rawData;
					while(dstGreyScale < dstGreyScaleEnd) // convert elevation data to visual greyscale inplace
					{
						u8 elevationAsGrayScale = (u8)Clamp<s32>(*elevation / 32, 0, 255);

						*dstGreyScale = elevationAsGrayScale;

						elevation++;
						dstGreyScale++;
					}
					return result;
				}
				default:
					return HGMRR_InvalidFormat;
			}
		}

	private:

		virtual ~QualityElevation() override
		{
			delete[] asterTiles;

			for (auto crs : supportedCRS)
			{
				OGRSpatialReference::DestroySpatialReference(crs.second);
			}

			for (auto trans : srsTransforms)
			{
				OGRCoordinateTransformation::DestroyCT(trans.second);
			}
		}

		void UnloadASTERTileContent(ASTERTileContent& content)
		{
			delete[] content.elevation;
			//delete[] content.quality;
		}

		bool LoadASTERTileContent(const ASTERTile* tile, ASTERTileContent& content, const int* firstLineIncl = NULL, const int* lastLineExcl = NULL)
		{
			GDALDataset* demDS = NULL;
			//GDALDataset* numDS = NULL;
			GDALRasterBand* demRasterBand = NULL;

			demDS = (GDALDataset*)GDALOpen(tile->filename_dem.c_str(), GA_ReadOnly);
			if (!demDS) goto err;

			//numDS = (GDALDataset*)GDALOpen(tile->filename_num.c_str(), GA_ReadOnly);
			//if (!numDS) goto err;

			if (demDS->GetGeoTransform(content.geoTransform) != CE_None) goto err;

			demRasterBand = demDS->GetRasterBand(1);
			if (!demRasterBand || demRasterBand->GetRasterDataType() != GDT_Int16) goto err;

			content.width = demRasterBand->GetXSize();
			content.height = demRasterBand->GetYSize();

			if (firstLineIncl)
			{
				content.height -= *firstLineIncl;
			}
			if (lastLineExcl)
			{
				content.height -= demRasterBand->GetYSize() - *lastLineExcl;
			}

			if (content.elevation)
			{
				assert(content.pitchInPixels > 0);
			}
			else
			{
				content.pitchInPixels = content.width;
				content.elevation = new s16[content.width * content.height];
			}
			//content.quality = new s16[content.width * content.height];

			int blockSizeX, blockSizeY;
			demRasterBand->GetBlockSize(&blockSizeX, &blockSizeY);
			if (blockSizeX != content.width && blockSizeY != 1) goto err;

			int startY = firstLineIncl ? *firstLineIncl : 0;
			int endY = startY + (lastLineExcl ? *lastLineExcl : content.height);

			for (int y = startY; y < endY; y++)
			{
				if (demRasterBand->ReadBlock(0, y, &content.elevation[(y - startY) * content.pitchInPixels]) != CE_None)
				{
					goto err;
				}
			}

			/*auto numRasterBand = numDS->GetRasterBand(1);
			if (!numRasterBand || numRasterBand->GetRasterDataType() != GDT_Int16) goto err;

			for (int y = 0; y < content.height; y++)
			{
				if (numRasterBand->ReadBlock(0, y, &content.quality[y * content.width]) != CE_None)
				{
					goto err;
				}
			}*/

			GDALClose(demDS);
			//GDALClose(numDS);

			return true;
		err:
			GDALClose(demDS);
			//GDALClose(numDS);
			return false;
		}


		HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, Image& img)
		{
			assert(img.rawDataType == DT_S16);

			BBox asterBBox;
			if (!TransformBBox(gmr.bbox, asterBBox, requestSRS, ASTER_SpatRef))
			{
				return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
			}

			BBox extendedAsterBBox(asterBBox);
			const double RequestedDegreesPerPixelX = asterBBox.GetWidth() / img.width;
			const double RequestedDegreesPerPixelY = asterBBox.GetHeight() / img.height;
			utils::ExtendBoundingBoxForLanczos(extendedAsterBBox, AsterDegreesPerPixel, AsterDegreesPerPixel, RequestedDegreesPerPixelX, RequestedDegreesPerPixelY);

			const int MaxNumAsterTilesX = 4;
			const int MaxNumAsterTilesY = 4;

			vector<ASTERTile*> asterTilesTouched;
			asterTilesTouched.reserve(MaxNumAsterTilesX * MaxNumAsterTilesY);
			int asterStartX, asterStartY, numAsterTilesX, numAsterTilesY;
			GetASTERTiles(asterTilesTouched, extendedAsterBBox, asterStartX, asterStartY, numAsterTilesX, numAsterTilesY);

			if (numAsterTilesX > MaxNumAsterTilesX || numAsterTilesY > MaxNumAsterTilesY) // prevent requests which would take too much resources
			{
				return HGMRR_InvalidBBox;
			}

			const int numPixelsX = numAsterTilesX * AsterPixelsPerDegree + 1;
			const int numPixelsY = numAsterTilesY * AsterPixelsPerDegree + 1;

			Image elevation(numPixelsX, numPixelsY, DT_S16);
			SetTypedMemory((s16*)elevation.rawData, InvalidValueASTER, elevation.width * elevation.height);

			HandleGetMapRequestResult result = HGMRR_OK;
			const int numTiles = (int)asterTilesTouched.size();
			#pragma omp parallel for
			for (int t = 0; t < numTiles; t++)
			{
				ASTERTileContent asterTileContent;
				asterTileContent.pitchInPixels = numPixelsX;

				const auto& tile = asterTilesTouched[t];

				int x = (tile->longitude - asterStartX - AsterTileStartLongitude + NumASTERTilesX) % NumASTERTilesX;
				int y = tile->latitude - asterStartY - asterTileStartLatitude;

				int pixelOffset = ((numAsterTilesY - y - 1) * numPixelsX + x) * AsterPixelsPerDegree * sizeof(s16);
				asterTileContent.elevation = (s16*)&elevation.rawData[pixelOffset];

				if (!LoadASTERTileContent(tile, asterTileContent))
				{
					result = HGMRR_InternalError;
					break;
				}
			}
			if (result != HGMRR_OK)
			{
				return result;
			}

			BBox loadedBBox;
			loadedBBox.minX = asterStartX + AsterTileStartLongitude - AsterDegreesPerPixel * 0.5;
			loadedBBox.minY = asterStartY + asterTileStartLatitude - AsterDegreesPerPixel * 0.5;
			loadedBBox.maxX = loadedBBox.minX + numAsterTilesX + AsterDegreesPerPixel;
			loadedBBox.maxY = loadedBBox.minY + numAsterTilesY + AsterDegreesPerPixel;

			SampleTransform st;
			st.scaleX = RequestedDegreesPerPixelX * AsterPixelsPerDegree;
			st.scaleY = RequestedDegreesPerPixelY * AsterPixelsPerDegree;
			st.offsetX = (asterBBox.minX - loadedBBox.minX) * AsterPixelsPerDegree;
			st.offsetY = (loadedBBox.maxY - asterBBox.maxY) * AsterPixelsPerDegree;

			BBox srtmBBox;
			if (!TransformBBox(gmr.bbox, srtmBBox, requestSRS, SRTM_SpatRef))
			{
				return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
			}

			// TODO: fill up invalid values within sampling area with SRTMv4 data
			// SRTMv4 starts at -180°;60° up to 180°;-60° in 5° steps
			// if (srtmBBox overlaps with full SRTMBBOX) { fill the gaps... }

			high_resolution_clock::time_point t1 = high_resolution_clock::now();

			Variant iv(InvalidValueASTER);
			SampleWithLanczos(elevation, img, st, iv);

			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
			std::cout << "SampleWithLanczos was processed within " << std::setprecision(5) << time_span.count() << " ms" << endl;

			if(false) // debug output of loaded region of ASTER tiles
			{
				elevation.SaveToPNG<s16, u8>("stitchedAster.png", [](s16 e) { return (u8)Clamp<s32>(e / 6, 0, 255); });
				img.SaveToPNG<s16, u8>("sampledAster.png", [](s16 e) { return (u8)Clamp<s32>(e / 6, 0, 255); });
			}

			return result;
		}

		bool TransformBBox(
			const BBox& srcBBox, BBox& dstBBox,
			const OGRSpatialReference* srcSRS, const OGRSpatialReference* dstSRS) const
		{
			bool success = true;
			auto transform = GetTransform(srcSRS, dstSRS);
			if (transform)
			{
				double x[2] = { srcBBox.minX, srcBBox.maxX };
				double y[2] = { srcBBox.minY, srcBBox.maxY };
				success = transform->Transform(2, x, y) == TRUE;
				dstBBox.minX = x[0];
				dstBBox.minY = y[0];
				dstBBox.maxX = x[1];
				dstBBox.maxY = y[1];
			}
			else
			{
				memcpy(&dstBBox, &srcBBox, sizeof(BBox));
			}
			return success;
		}

		// TODO: this should go into utils or base layer
		OGRCoordinateTransformation* GetTransform(const OGRSpatialReference* src, const OGRSpatialReference* dst) const
		{
			SrcDestTransfromId transId;
			transId.src = src;
			transId.dst = dst;

			auto transform = srsTransforms.find(transId);
			return transform->second;
		}

		void CreateTransform(OGRSpatialReference* src, OGRSpatialReference* dst)
		{
			SrcDestTransfromId transId;
			transId.src = src;
			transId.dst = dst;

			auto existingTransform = srsTransforms.find(transId);
			if (existingTransform != srsTransforms.end()) return;

			srsTransforms[transId] = (src == dst) ? NULL : OGRCreateCoordinateTransformation(src, dst);
		}


		void GetASTERTiles(vector<ASTERTile*>& asterTilesTouched, const BBox& asterBBox, int& startX, int& startY, int& numTilesX, int& numTilesY)
		{
			startX = (int)floor(asterBBox.minX) - AsterTileStartLongitude;
			startY = (int)floor(asterBBox.minY) - asterTileStartLatitude;
			int endX = (int)floor(asterBBox.maxX - 0.000001) - AsterTileStartLongitude; // we fudge the max values because each
			int endY = (int)floor(asterBBox.maxY - 0.000001) - asterTileStartLatitude; // tile overlaps by one pixel with next tile

			numTilesX = endX - startX + 1;
			numTilesY = endY - startY + 1;

			int clampedStartY = max(startY, 0);
			endY = min(endY, asterTileEndLatitude - asterTileStartLatitude);

			for (int y = clampedStartY; y <= endY; y++)
			{
				int vIndex = y * NumASTERTilesX;
				for (int x = startX; x <= endX; x++)
				{
					ASTERTile* asterTile = &asterTiles[vIndex + ((x + NumASTERTilesX) % NumASTERTilesX)]; // wrap around date border

					if (asterTile->latitude != MissingTileCoordinate)
					{
						asterTilesTouched.push_back(asterTile);
					}
				}
			}

			startX = (startX + NumASTERTilesX) % NumASTERTilesX;

			if (asterBBox.minX < 0.0 && startX > AsterTileEndLongitude)
			{
				startX -= NumASTERTilesX;
			}
		}
	};

	IMPLEMENT_WEBMAPSERVICE_LAYER(QualityElevation, LayerName, LayerTitle);
	}
}
