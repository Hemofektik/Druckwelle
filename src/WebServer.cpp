
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <microhttpd.h>
#include <libconfig_chained.h>

#include "WebServer.h"
#include "WebMapService.h"
#include "WebMapTileService.h"

using namespace std;
using namespace std::chrono;
using namespace libconfig;

static int HandleRequestStatic(void *cls, struct MHD_Connection *connection,
	const char *url, const char *method,
	const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	auto ws = static_cast<dw::WebServer*>(cls);
	return ws->HandleRequest(connection, url, method);
}

namespace dw
{
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
		: daemon(NULL)
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

		daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL, &HandleRequestStatic, this, MHD_OPTION_END);
		if (!daemon) {
			cout << "ERROR: unable to create HTTP server" << endl;
			return -1;
		}

		cout << "ready" << endl;

		return 0;
	}

	void WebServer::Stop()
	{
		wms->Stop();
		delete wms;

		MHD_stop_daemon(daemon);
	}

	static int HandleServiceException(struct MHD_Connection *connection, const string& exeptionCode)
	{
		// TODO implement service exception according to WMS 1.3.0 Specs (XML)

		struct MHD_Response* response = MHD_create_response_from_buffer(exeptionCode.length(), (void*)exeptionCode.c_str(), MHD_RESPMEM_MUST_COPY);
		int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
		MHD_destroy_response(response);
		return ret;
	}

	int WebServer::HandleRequest(MHD_Connection* connection, const char* url, const char* method)
	{
		const char* service = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "service");
		string serviceStr = service ? string(service) : "";

		if (wms && serviceStr == "WMS")
		{
			return wms->HandleRequest(connection, url, method);
		}
		else if (wmts && serviceStr == "WMTS")
		{
			return wmts->HandleRequest(connection, url, method);
		}
		
		return HandleServiceException(connection, "unknown service request");
	}

}