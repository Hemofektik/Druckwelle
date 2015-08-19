
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <microhttpd.h>

#include "WebServer.h"
#include "WebMapService.h"
#include "WebMapTileService.h"

using namespace std;
using namespace std::chrono;

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

	WebServer::WebServer()
		: daemon(NULL)
		, wms(NULL)
		, wmts(NULL)
	{
	}

	static const int DefaultServerPort = 8282;
	int WebServer::Start()
	{
		cout << "Reading Config" << endl;

		// TODO: read config

		wms = new WebMapService();
		const auto wmsStartResult = wms->Start();
		if(wmsStartResult)
		{
			delete wms;
			wms = NULL;
		}

		wmts = new WebMapTileService();
		const auto wmtsStartResult = wmts->Start();
		if (wmtsStartResult)
		{
			delete wmts;
			wmts = NULL;
		}

		cout << "Starting HTTP Server listening to port " << DefaultServerPort << endl;

		daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, DefaultServerPort, NULL, NULL, &HandleRequestStatic, this, MHD_OPTION_END);
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

	static int HandleServiceException(struct MHD_Connection *connection, const astring& exeptionCode)
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
		astring serviceStr = service ? astring(service) : "";

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