#include <filesystem>
#include <vector>
#include <mutex>
#include <iostream>
#include <memory>

#include <ogr_api.h>
#include <ogr_spatialref.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#pragma warning (pop)
#include <cpl_vsi.h>

#include <ZFXMath.h>

#include "../WebMapService.h"

using namespace std;
using namespace std::experimental::filesystem::v1;
using namespace dw;
using namespace ZFXMath;

namespace dw
{
namespace Layers
{
	const astring LayerName = "QualityElevation";
	const string LayerTitle = dwTEXT("ASTER + SRTMv3 High Quality Elevation Service");
	const string LayerAbstract =
		dwTEXT("The native resolution of this layer is 1 arc second per pixel.")
		dwTEXT("A request's bounding box must not exceed 1° in width and height.")
		dwTEXT("It will throw an InvalidBBox Service Exception otherwise.");

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
			astring filename_dem;
			astring filename_num;
		};

		struct ASTERTileContent
		{
			s16* elevation;
			s16* quality;

			int width;
			int height;

			double geoTransform[6];

			~ASTERTileContent()
			{
				delete[] elevation;
				delete[] quality;
			}
		};

		map<astring, OGRSpatialReference*> supportedCRS;
		OGRSpatialReference* ASTER_EPSG;
		map<SrcDestTransfromId, OGRCoordinateTransformation*> srsTransforms;

		ASTERTile* asterTiles;
		int asterTileStartLatitude;
		int asterTileEndLatitude;
		const int AsterTileStartLongitude = -180;
		const int AsterTileEndLongitude = 179;
		const int NumASTERTilesX = AsterTileEndLongitude - AsterTileStartLongitude + 1;
		const int MissingTileCoordinate = -1000;

	public:

		virtual bool Init(/* layerconfig */) override
		{
			GDALRegister_GTiff();

			supportedCRS =
			{
				//pair<astring, OGRSpatialReference*>("EPSG:3857", (OGRSpatialReference*)OSRNewSpatialReference(NULL)),
				pair<astring, OGRSpatialReference*>("EPSG:4326",  (OGRSpatialReference*)OSRNewSpatialReference(NULL))
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

			ASTER_EPSG = supportedCRS["EPSG:4326"];

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

			// load TOC from disk
			//const path ASTERSourceDir("D:/ASTER/");
			const path ASTERSourceDir("C:/Dev/temp/ASTER/");

			unique_ptr<u8> fileExists(new u8[NumASTERTilesX * 180]);
			memset(fileExists.get(), 0, NumASTERTilesX * 180);
			asterTileStartLatitude = 90;
			asterTileEndLatitude = -90;
			for (directory_iterator di(ASTERSourceDir); di != end(di); di++)
			{
				const auto& entity = *di;
				const auto extension = entity.path().extension();
				if (is_regular_file(entity.status()) && entity.path().extension().generic_string() == ".zip")
				{
					astring filepath = entity.path().string();
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
			memset(asterTiles, 0, sizeof(ASTERTile) * NumASTERTilesX * NumASTERTilesY);
			for (int y = asterTileStartLatitude; y <= asterTileEndLatitude; y++)
			{
				for (int x = AsterTileStartLongitude; x <= AsterTileEndLongitude; x++)
				{
					auto& tile = asterTiles[(y - asterTileStartLatitude) * NumASTERTilesX + (x - AsterTileStartLongitude)];
					tile.longitude = x;
					tile.latitude = y;

					int absX = abs(x);
					int absY = abs(y);

					astring filename = ((y < 0) ? "S" : "N");
					if (absY < 10) filename += "0";
					filename += to_string(absY) + ((x < 0) ? "W" : "E");
					if (absX < 10) filename += "00";
					else if (absX < 100) filename += "0";
					filename += to_string(absX);

					astring zipFilename = ASTERSourceDir.string() + "ASTGTM2_" + filename + ".zip";

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

		virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, u8* data) override
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
					return HandleGetMapRequest(gmr, crs->second, (s16*)data);
				}
				case DT_U8:
				{
					unique_ptr<s16> elevationData(new s16[gmr.width * gmr.height]);
					auto result = HandleGetMapRequest(gmr, crs->second, elevationData.get());
					if (result != HGMRR_OK) return result;
					u8* dstGreyScaleEnd = data + gmr.width * gmr.height;
					u8* dstGreyScale = data;
					s16* elevation = elevationData.get();
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

			for each (auto crs in supportedCRS)
			{
				OGRSpatialReference::DestroySpatialReference(crs.second);
			}

			for each (auto trans in srsTransforms)
			{
				OGRCoordinateTransformation::DestroyCT(trans.second);
			}
		}

		bool LoadASTERTileContent(const ASTERTile* tile, ASTERTileContent& content)
		{
			content.elevation = NULL;
			content.quality = NULL;

			GDALDataset* demDS = NULL;
			GDALDataset* numDS = NULL;

			demDS = (GDALDataset*)GDALOpen(tile->filename_dem.c_str(), GA_ReadOnly);
			if (!demDS) goto err;

			numDS = (GDALDataset*)GDALOpen(tile->filename_num.c_str(), GA_ReadOnly);
			if (!numDS) goto err;

			if (demDS->GetGeoTransform(content.geoTransform) != CE_None) goto err;

			auto demRasterBand = demDS->GetRasterBand(1);
			if (!demRasterBand || demRasterBand->GetRasterDataType() != GDT_Int16) goto err;

			content.width = demRasterBand->GetXSize();
			content.height = demRasterBand->GetYSize();

			content.elevation = new s16[content.width * content.height];
			content.quality = new s16[content.width * content.height];

			int blockSizeX, blockSizeY;
			demRasterBand->GetBlockSize(&blockSizeX, &blockSizeY);
			if (blockSizeX != content.width && blockSizeY != 1) goto err;

			for (int y = 0; y < content.height; y++)
			{
				if (demRasterBand->ReadBlock(0, y, &content.elevation[y * content.width]) != CE_None)
				{
					goto err;
				}
			}

			auto numRasterBand = numDS->GetRasterBand(1);
			if (!numRasterBand || numRasterBand->GetRasterDataType() != GDT_Int16) goto err;

			for (int y = 0; y < content.height; y++)
			{
				if (numRasterBand->ReadBlock(0, y, &content.quality[y * content.width]) != CE_None)
				{
					goto err;
				}
			}

			return true;
		err:
			GDALClose(demDS);
			GDALClose(numDS);
			return false;
		}


		HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, s16* data)
		{
			WebMapService::GetMapRequest::BBox asterBBox;
			if (!TransformBBox(gmr.bbox, asterBBox, requestSRS, ASTER_EPSG))
			{
				return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
			}

			// TODO: compute BBox padding to load tiles for in order to provide all data the used filter needs

			vector<ASTERTile*> asterTilesTouched;
			GetASTERTiles(asterTilesTouched, asterBBox);

			const s16 InvalidValueASTER = -9999;

			static_assert(sizeof(wchar_t) == sizeof(s16), "Need explicit two byte memset");
			wmemset((wchar_t*)data, InvalidValueASTER, gmr.width * gmr.height);

			for each (const auto& tile in asterTilesTouched)
			{
				ASTERTileContent tileContent;

				if (!LoadASTERTileContent(tile, tileContent))
				{
					return HGMRR_InternalError;
				}

				// TODO: implement proper sampling method here (e.g. lanczos)
				const int stepX = tileContent.width / gmr.width;
				const int stepY = tileContent.height / gmr.height;

				s16* dest = data;
				for (int y = 0; y < gmr.height; y++)
				{
					auto elevation = &tileContent.elevation[y * stepY * tileContent.width];
					for (int x = 0; x < gmr.width; x++)
					{
						*dest = *elevation;

						elevation += stepX;
						dest++;
					}
				}
			}

			return HGMRR_OK;
		}

		bool TransformBBox(
			const WebMapService::GetMapRequest::BBox& srcBBox, WebMapService::GetMapRequest::BBox& dstBBox, 
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
				memcpy(&dstBBox, &srcBBox, sizeof(WebMapService::GetMapRequest::BBox));
			}
			return success;
		}

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


		void GetASTERTiles(vector<ASTERTile*>& asterTilesTouched, const WebMapService::GetMapRequest::BBox& asterBBox)
		{
			int startX = Clamp((int)floor(asterBBox.minX), AsterTileStartLongitude, AsterTileEndLongitude) - AsterTileStartLongitude;
			int startY = Clamp((int)floor(asterBBox.minY), asterTileStartLatitude, asterTileEndLatitude) - asterTileStartLatitude;
			int endX = Clamp((int)floor(asterBBox.maxX - 0.000001), AsterTileStartLongitude, AsterTileEndLongitude) - AsterTileStartLongitude; // we fudge the max values because each 
			int endY = Clamp((int)floor(asterBBox.maxY - 0.000001), asterTileStartLatitude, asterTileEndLatitude) - asterTileStartLatitude; // tile overlaps by one pixel with next tile

			for (int y = startY; y <= endY; y++)
			{
				int vIndex = y * NumASTERTilesX;
				ASTERTile* asterTile = &asterTiles[vIndex + startX];
				for (int x = startX; x <= endX; x++)
				{
					if (asterTile->latitude != MissingTileCoordinate)
					{
						asterTilesTouched.push_back(asterTile);
					}

					asterTile++;
				}
			}
		}
	};

	IMPLEMENT_WEBMAPSERVICE_LAYER(QualityElevation, LayerName, LayerTitle);
	}
}