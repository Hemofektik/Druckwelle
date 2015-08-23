
#include "../WebMapTileService.h"

#include <Poco/Net/HTTPRequest.h>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include <istream>
#include <ostream>
#include <sstream>

#include <thread>

using namespace std;

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::StreamCopier;

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
				astring tileRequestUrl = "/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&BBOX=10.0,46.0,11.0,47.0&CRS=EPSG:4326&WIDTH=128&HEIGHT=128&LAYERS=QualityElevation&STYLES=&FORMAT=image/png";
				HTTPClientSession s("localhost", 8282);
				HTTPRequest request(HTTPRequest::HTTP_GET, tileRequestUrl);
				s.sendRequest(request);
				HTTPResponse response;
				istream& rs = s.receiveResponse(response);

				ostringstream ostr;
				streamsize dataLength = StreamCopier::copyStream(rs, ostr);
				//ostr.str();
				
				while (true)
				{
					this_thread::sleep_for(chrono::seconds(1));
				}
			}
		};

		IMPLEMENT_WEBMAPTILESERVICE_LAYER(TileCache, "TileCache", dwTEXT("Can become a tile cache for any WMS layer"));
	}
}