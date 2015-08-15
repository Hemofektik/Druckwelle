#include <filesystem>
#include <vector>
#include <mutex>
#include <iostream>

#include <ogr_api.h>
#include <ogr_spatialref.h>

#include <ZFXMath.h>

#include "../utils/ReadZIP.h"
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
			path filename;
		};

		struct ASTERTileContent
		{
			uint8_t* elevation;
			uint64_t elevationSize;
			uint8_t* quality;
			uint64_t qualitySize;

			ZIPFile sourceFile;

			~ASTERTileContent()
			{
				DestroyZIPFile(sourceFile);
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
			supportedCRS =
			{
				pair<astring, OGRSpatialReference*>("EPSG:3857", new OGRSpatialReference()),
				pair<astring, OGRSpatialReference*>("EPSG:4326", new OGRSpatialReference())
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
					size_t offset = filepath.find_last_of('N');
					if (offset == std::string::npos)
					{
						offset = filepath.find_last_of('S');
						latitudeSign = -1;
					}
					if (offset != std::string::npos)
					{
						int latitude = atoi(filepath.c_str() + offset + 1) * latitudeSign;
						asterTileStartLatitude = min(asterTileStartLatitude, latitude);
						asterTileEndLatitude = max(asterTileEndLatitude, latitude);
					}
				}
			}

			const int NumASTERTilesX = 360;
			const int NumASTERTilesY = max(0, asterTileEndLatitude - asterTileStartLatitude + 1);

			if (NumASTERTilesY == 0)
			{
				cout << "ASTER data not found" << endl;
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

					astring filename = ((y < 0) ? "S" : "N");
					if (absY < 10) filename += "0";
					filename += to_string(absY) + ((x < 0) ? "W" : "E");
					if (absX < 10) filename += "00";
					else if (absX < 100) filename += "0";
					filename += to_string(absX);

					tile.filename = ASTERSourceDir.string() + "ASTGTM2_" + filename + ".zip";

					if (!exists(tile.filename))
					{
						tile.filename = "";
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

		virtual const int GetFixedWidth() const override
		{
			return 3600 * 360; // number of arc seconds around longitude
		}

		virtual const int GetFixedHeight() const override
		{
			return 3600 * 180; // number of arc seconds around latitude
		}


		virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, u8* data) override
		{
			auto crs = supportedCRS.find(gmr.crs);
			if (crs == supportedCRS.end())
			{
				return HGMRR_InvalidSRS;
			}

			switch (gmr.valueFormat)
			{
			case WebMapService::CF_UINT32:
				return HandleGetMapRequest<u32>(gmr, crs->second, (u32*)data);
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
				delete crs.second;
			}

			for each (auto trans in srsTransforms)
			{
				OGRCoordinateTransformation::DestroyCT(trans.second);
			}
		}

		bool GetFileFromZip(const ZIPFile& file, astring endsWidth, uint8_t*& data, uint64_t& dataSize)
		{
			for (uint64_t i = 0; i < file.numItems; i++)
			{
				auto& item = file.items[i];
				size_t pos = item.name.rfind(endsWidth);
				if (pos != astring::npos)
				{
					data = item.data;
					dataSize = item.dataSize;
					return true;
				}
			}

			return false;
		}

		bool LoadASTERTileContent(const ASTERTile* tile, ASTERTileContent& content)
		{
			if (!ReadZIPFile(tile->filename.string(), content.sourceFile, true))
			{
				return false;
			}

			if (!GetFileFromZip(content.sourceFile, "_dem.tif", content.elevation, content.elevationSize))
			{
				return false;
			}

			if (!GetFileFromZip(content.sourceFile, "_num.tif", content.quality, content.qualitySize))
			{
				return false;
			}

			return true;
		}

		template<typename T>
		HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, T* data)
		{
			WebMapService::GetMapRequest::BBox asterBBox;
			if (!TransformBBox(gmr.bbox, asterBBox, requestSRS, ASTER_EPSG))
			{
				return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
			}

			vector<ASTERTile*> asterTilesTouched;
			GetASTERTiles(asterTilesTouched, asterBBox);

			for each (const auto& tile in asterTilesTouched)
			{
				ASTERTileContent tileContent;

				if (!LoadASTERTileContent(tile, tileContent))
				{
					return HGMRR_InternalError;
				}


			}

			const int numPixels = gmr.width * gmr.height;
			for (size_t p = 0; p < numPixels; p++)
			{
				data[p] = 0xFF0000FF;
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