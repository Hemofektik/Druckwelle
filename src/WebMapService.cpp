
#include <vector>
#include <iostream>
#include <chrono>

#include <microhttpd.h>

#include "WebMapService.h"

using namespace std;
using namespace std::chrono;

const char* wmsCapabilites =
#include "GetCapabilities.xml"
;

static int HandleRequestStatic(void *cls, struct MHD_Connection *connection,
	const char *url, const char *method,
	const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	auto wms = (WebMapService*)cls;
	return wms->HandleRequest(connection, url, method);
}

WebMapService::WebMapService()
{
}

static const int DefaultServerPort = 8282;
int WebMapService::Start()
{
	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, DefaultServerPort, NULL, NULL, &HandleRequestStatic, this, MHD_OPTION_END);
	if (!daemon) {
		cout << "ERROR: unable to create HTTP server" << endl;
		return -1;
	}

	cout << "Started HTTP Server listening to port " << DefaultServerPort << endl;

	return 0;
}

void WebMapService::Stop()
{
	MHD_stop_daemon(daemon);
}

static int HandleGetCapabilities(struct MHD_Connection *connection)
{
	struct MHD_Response* response = MHD_create_response_from_buffer(strlen(wmsCapabilites), (void*)wmsCapabilites, MHD_RESPMEM_PERSISTENT);
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}

static int HandleBadRequest(struct MHD_Connection *connection, const string& message)
{
	const char ErrorMessage[] = "unknown request";
	struct MHD_Response* response = MHD_create_response_from_buffer(strlen(ErrorMessage), (void*)ErrorMessage, MHD_RESPMEM_MUST_COPY);
	int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
	MHD_destroy_response(response);
	return ret;
}

struct GetMapRequest
{
	struct BBox
	{
		double minX;
		double minY;
		double maxX;
		double maxY;
	};
	string crs;
	BBox bbox;
	int width;
	int height;
};

int WebMapService::HandleGetMapRequest(struct MHD_Connection *connection, GetMapRequest& gmr)
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	struct MHD_Response* response = MHD_create_response_from_buffer(6, "naaak!", MHD_RESPMEM_MUST_COPY);

	int success = MHD_add_response_header(response, "Content-Type", "image/png");
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
	std::cout << "GetMapRequest was processed within " << round(time_span.count()) << " ms" << endl;

	return MHD_YES;
}

int WebMapService::HandleRequest(MHD_Connection* connection, const char* url, const char* method)
{
	const char* request = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "request");
	string requestStr = request ? string(request) : "";

	if (requestStr == "GetCapabilities")
	{
		return HandleGetCapabilities(connection);
	}
	if (requestStr != "GetMap")
	{
		return HandleBadRequest(connection, "unsupported request type");
	}

	const char* layers = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "layers");
	const char* styles = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "styles");
	const char* crs = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "crs");
	const char* bbox = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "bbox");
	const char* width = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "width");
	const char* height = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "height");

	const char* time = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");

	if (!layers || !crs || !bbox || !width || !height)
	{
		return HandleBadRequest(connection, "missing mandatory argument");
	}
	if (string(layers) != "nexrad_wms")
	{
		return HandleBadRequest(connection, "unknown layer requested");
	}

	vector<string> supportedCRS = { "EPSG:3857", "EPSG:4326" }; // TODO: should be in sync with GetCapabilities.xml
	if (find(supportedCRS.begin(), supportedCRS.end(), crs) == supportedCRS.end())
	{
		return HandleBadRequest(connection, "unsupported CRS");
	}

	GetMapRequest gmr;
	gmr.width = atoi(width); // TODO: should be limited by size given in GetCapabilities.xml
	gmr.height = atoi(height);

	gmr.crs = crs;

	char* bboxValue = NULL;
	const char* bboxEnd = bbox + strlen(bbox);
	gmr.bbox.minX = strtod(bbox, &bboxValue);
	if (bboxValue == bboxEnd || *bboxValue != ',') return HandleBadRequest(connection, "unknown bbox syntax");
	gmr.bbox.minY = strtod(bboxValue + 1, &bboxValue);
	if (bboxValue == bboxEnd || *bboxValue != ',') return HandleBadRequest(connection, "unknown bbox syntax");
	gmr.bbox.maxX = strtod(bboxValue + 1, &bboxValue);
	if (bboxValue == bboxEnd || *bboxValue != ',') return HandleBadRequest(connection, "unknown bbox syntax");
	gmr.bbox.maxY = strtod(bboxValue + 1, &bboxValue);

	return HandleGetMapRequest(connection, gmr);
}