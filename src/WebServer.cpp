
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

#include <libconfig_chained.h>

#include "WebServer.h"
#include "WebMapService.h"
#include "WebMapTileService.h"

using namespace std;
using namespace std::chrono;
using namespace libconfig;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

namespace dw
{
	class WebServer : public IWebServer
	{
	public:
		WebServer();
		WebServer(WebServer& other) = delete;

		virtual int Start() override;
		virtual void Stop() override;

		virtual ~WebServer() override;

	private:
		void HandleGetRequest(http_request message);

		class http_listener* listener;
		class WebMapService* wms;
		class WebMapTileService* wmts;
	};

	IWebServer* IWebServer::Create()
	{
		return new WebServer();
	}

	// Read the config file. If there is an error, report it and exit.
	int ReadConfig(libconfig::Config& cfg, const char* filename)
	{
		try
		{
			cfg.readFile(filename);
		}
		catch (const libconfig::FileIOException&)
		{
			std::cerr << "I/O error while reading config file: " << filename << std::endl;
			return (EXIT_FAILURE);
		}
		catch (const libconfig::ParseException& pex)
		{
			std::cerr	<< "Parse error at " << pex.getFile() << ":" << pex.getLine()
						<< " - " << pex.getError() << std::endl;
			return (EXIT_FAILURE);
		}
		return EXIT_SUCCESS;
	}

	WebServer::WebServer()
		: listener(NULL)
		, wms(NULL)
		, wmts(NULL)
	{
	}

	int WebServer::Start()
	{
		cout << "Reading Config" << endl;

		libconfig::Config cfg;
		int result = ReadConfig(cfg, "webserver.cfg");
		if (result != EXIT_SUCCESS)
		{
			return result;
		}

		ChainedSetting config(cfg.getRoot());

		// read config
		int port = config["port"].min(0).max(65535).defaultValue(43113);

		auto wmsConfig = config["wms"];
		if (wmsConfig.exists())
		{
			wms = new WebMapService();
			const auto wmsStartResult = wms->Start(wmsConfig);
			if (wmsStartResult)
			{
				delete wms;
				wms = NULL;
			}
		}

		wmts = new WebMapTileService();
		const auto wmtsStartResult = wmts->Start();
		if (wmtsStartResult)
		{
			delete wmts;
			wmts = NULL;
		}

		cout << "Starting HTTP Server listening to port " << port << endl;

		utility::string_t address = U("http://localhost:");
		address.append(to_wstring(port));

		listener = new http_listener(address);
		if (!listener) 
		{
			cout << "ERROR: unable to create HTTP server" << endl;
			return -1;
		}

		listener->support(methods::GET, std::bind(&WebServer::HandleGetRequest, this, std::placeholders::_1));

		Concurrency::task_status status = listener->open().wait();
		if (status == Concurrency::completed)
		{
			cout << "ready" << endl;
			return 0;
		}

		return -1;
	}

	void WebServer::Stop()
	{
		if (wms)
		{
			wms->Stop();
			delete wms;
			wms = NULL;
		}

		if (listener)
		{
			listener->close().wait();
			delete listener;
			listener = NULL;
		}
	}

	WebServer::~WebServer()
	{
		Stop();
	}

	static void HandleServiceException(http_request message, const string& exeptionCode)
	{
		// TODO implement service exception according to WMS 1.3.0 Specs (XML)

		auto& response = message.get_response().get();
		response.set_status_code(400);
		response.set_reason_phrase(string_t(exeptionCode.begin(), exeptionCode.end()));
	}

	void WebServer::HandleGetRequest(http_request message)
	{
		/*const char* service = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "service");
		string serviceStr = service ? string(service) : "";

		if (wms && serviceStr == "WMS")
		{
			return wms->HandleRequest(connection, url, method);
		}
		else if (wmts && serviceStr == "WMTS")
		{
			return wmts->HandleRequest(connection, url, method);
		}
		
		return HandleServiceException(connection, "unknown service request");*/
	}

}