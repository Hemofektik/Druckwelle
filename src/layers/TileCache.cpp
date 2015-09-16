
#include "../WebMapTileService.h"
#include "../utils/ImageProcessor.h"

#include <Poco/Net/HTTPRequest.h>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <istream>
#include <ostream>
#include <sstream>

#include <thread>

#include <ZFXMath.h>

using namespace std;
using namespace ZFXMath;

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

namespace dw
{
	namespace Layers
	{
		class TileCache : public WebMapTileService::Layer
		{
			virtual ~TileCache() override {};

			thread* createTileCacheThread;

		public:

			virtual const char* GetIdentifier() const override
			{
				return "TileCache"; // TODO: make this dynamically configurable
			}

			virtual const char_t* GetTitle() const override
			{
				return dwTEXT("Tile Cache"); // TODO: make this dynamically configurable
			}

			virtual int GetTileWidth() const override
			{
				return 3600;
			}

			virtual int GetTileHeight() const override
			{
				return 3600;
			}

			virtual const vector<DataType>& GetSuppordetFormats() const override
			{
				static const vector<DataType> SuppordetFormats = { DT_RGBA8 };
				return SuppordetFormats;
			}

			virtual bool Init(/* layerconfig */) override
			{
				createTileCacheThread = new thread([this] { CreateTileCacheAsync(); });

				return true; 
			};

			virtual HandleGetTileRequestResult HandleGetTileRequest(const WebMapTileService::GetTileRequest& gmr, Image& img) override
			{
				return HGTRR_OK;
			}

		private:

			struct TileCacheDescription
			{
				astring srcHost;
				u16 srcPort;
				astring srcLayerName;

				u32 tileWidth;
				u32 tileHeight;
				ContentType contentType;
				DataType dataType;
				utils::InvalidValue invalidValue;

				u32 numTilesX;
				u32 numTilesY;

				u32 numLevels;

			};

			void CreateTileCacheAsync()
			{
				const int AsterPixelsPerDegree = 3600;
				
				const int NumSrcPixelsAlongLongitude = 360 * AsterPixelsPerDegree;
				const int NumSrcPixelsAlongLatitude = 180 * AsterPixelsPerDegree;

				const u32 NumDstPixelsAlongLongitude = NextPoweOfTwo(NumSrcPixelsAlongLongitude);
				const u32 NumDstPixelsAlongLatitude = NextPoweOfTwo(NumSrcPixelsAlongLatitude);

				const s16 InvalidValueASTER = -9999;

				TileCacheDescription desc;
				desc.srcHost = "localhost";
				desc.srcPort = 8282;
				desc.srcLayerName = "QualityElevation";

				desc.tileWidth = 2048;
				desc.tileHeight = 2048;
				desc.contentType = CT_Image_Raw_S16;
				desc.dataType = DT_S16;
				desc.invalidValue = utils::InvalidValue(InvalidValueASTER);

				desc.numTilesX = NumDstPixelsAlongLongitude / desc.tileWidth;
				desc.numTilesY = NumDstPixelsAlongLatitude / desc.tileHeight;
				desc.numLevels = Min(PowerOfTwoLog2(desc.numTilesX), PowerOfTwoLog2(desc.numTilesY)) + 1;

				Image tileImg(desc.tileWidth, desc.tileHeight, desc.dataType);
				const size expectedTileSize = tileImg.rawDataSize;

				// TODO: create only missing tiles
				bool runCacheCreation = true;
				for (int y = 0; y < desc.numTilesY && runCacheCreation; y++)
				{
					for (int x = 0; x < desc.numTilesX; x++)
					{
						astring tileRequestUri = "/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&CRS=EPSG:4326&LAYERS=" + desc.srcLayerName + "&STYLES=";
						tileRequestUri += "&WIDTH=" + to_string(desc.tileWidth);
						tileRequestUri += "&HEIGHT=" + to_string(desc.tileHeight);
						tileRequestUri += "&FORMAT=" + ContentTypeId[desc.contentType];
						tileRequestUri += "&BBOX=" + to_string(x) + "," + to_string(y) + "," + to_string(x + 0.5) + "," + to_string(y + 0.5);

						HTTPClientSession s(desc.srcHost, desc.srcPort);
						HTTPRequest request(HTTPRequest::HTTP_GET, tileRequestUri);

						s.sendRequest(request);
						HTTPResponse response;
						istream& rs = s.receiveResponse(response);

						std::streamsize len = 0;
						rs.read((char*)tileImg.rawData, expectedTileSize);
						std::streamsize readSize = rs.gcount();
						if (readSize < expectedTileSize)
						{
							runCacheCreation = false;
							cout << "Error: Cache Creation failed! Failed to receive valid tile (" << x << "," << y << ")!";
							break;
						}

						if (desc.invalidValue.IsSet())
						{
							if (utils::IsImageCompletelyInvalid(tileImg, desc.invalidValue))
							{
								continue;
							}
						}

						// TODO: pass tile to cache creator thingy
					}
				}
			}
		};

		IMPLEMENT_WEBMAPTILESERVICE_LAYER(TileCache, "TileCache", dwTEXT("Can become a tile cache for any WMS layer"));
	}
}