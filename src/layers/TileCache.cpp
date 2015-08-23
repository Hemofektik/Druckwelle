
#include "../WebMapTileService.h"

#include <Poco/Net/HTTPRequest.h>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <istream>
#include <ostream>
#include <sstream>

#include <thread>

using namespace std;

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

			virtual HandleGetTileRequestResult HandleGetTileRequest(const WebMapTileService::GetTileRequest& gmr, u8* data) override
			{
				return HGTRR_OK;
			}

		private:

			void CreateTileCacheAsync()
			{
				// TODO: enumerate all top level tiles and create missing ones

				int width = 3601;
				int height = 3601;
				ContentType ct = CT_Image_Raw_S16;
				DataType dt = DT_S16;

				astring tileRequestUri = "/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&BBOX=10.0,46.0,11.0,47.0&CRS=EPSG:4326&LAYERS=QualityElevation&STYLES=";
				tileRequestUri += "&WIDTH=" + to_string(width); 
				tileRequestUri += "&HEIGHT=" + to_string(height);
				tileRequestUri += "&FORMAT=" + ContentTypeId[CT_Image_Raw_S16];

				HTTPClientSession s("localhost", 8282);
				HTTPRequest request(HTTPRequest::HTTP_GET, tileRequestUri);

				s.sendRequest(request);
				HTTPResponse response;
				istream& rs = s.receiveResponse(response);

				size expectedTileSize = width * height * DataTypePixelSize[dt];
				u8* data = new u8[expectedTileSize];

				std::streamsize len = 0;
				rs.read((char*)data, expectedTileSize);
				std::streamsize readSize = rs.gcount();
				if (readSize < expectedTileSize)
				{
					return; // TODO: handle error in a meaningful way
				}

				delete[] data;
			}
		};

		IMPLEMENT_WEBMAPTILESERVICE_LAYER(TileCache, "TileCache", dwTEXT("Can become a tile cache for any WMS layer"));
	}
}