
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <microhttpd.h>

#include "WebMapService.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace std;
using namespace std::chrono;

static const char* wmsCapabilites =
#include "GetCapabilities.xml"
;

static int HandleRequestStatic(void *cls, struct MHD_Connection *connection,
	const char *url, const char *method,
	const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	auto wms = static_cast<dw::WebMapService*>(cls);
	return wms->HandleRequest(connection, url, method);
}

namespace dw
{

	WebMapService::WebMapService()
	{
	}

	static const int DefaultServerPort = 8282;
	int WebMapService::Start()
	{
		cout << "Reading Config" << endl;

		// TODO: read config

		cout << "Creating Layers" << endl;
		LayerFactory::CreateLayers(availableLayers /*, config*/);

		if (availableLayers.size() == 0)
		{
			cout << "not a single layer was configured... shutting down" << endl;

			// TODO: list all available layers and propose detailed config info for each layer (e.g. --help <layerName>)

			return -1;
		}

		cout << "Starting HTTP Server listening to port " << DefaultServerPort << endl;

		daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, DefaultServerPort, NULL, NULL, &HandleRequestStatic, this, MHD_OPTION_END);
		if (!daemon) {
			cout << "ERROR: unable to create HTTP server" << endl;
			return -1;
		}

		cout << "ready" << endl;

#if _DEBUG
		cout << endl << "example request: " << endl << "http://localhost:8282/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&BBOX=-14607737,2378356,-6201882,7104700.22959910612553358&CRS=EPSG:3857&WIDTH=1905&HEIGHT=1071&LAYERS=" << availableLayers.begin()->first << "&STYLES=&FORMAT=image/png&DPI=192&MAP_RESOLUTION=192&FORMAT_OPTIONS=dpi:192&TRANSPARENT=TRUE" << endl;
#endif

		return 0;
	}

	void WebMapService::Stop()
	{
		MHD_stop_daemon(daemon);
	}

	void WebMapService::LayerFactory::CreateLayers(std::map<astring, Layer*>& layers /*, config*/)
	{
		for each (const auto& layerDesc in LayerFactory::GetStaticLayers())
		{
			Layer* newLayer = layerDesc.createLayer();

			astring layerTitle(layerDesc.title.cbegin(), layerDesc.title.cend());
			cout << "Loading layer: " << layerTitle << '\r';
			if (newLayer->Init())
			{
				layers[layerDesc.name] = newLayer;
				cout << "Enabled layer: " << layerTitle << endl;
			}
			else
			{
				cout << "Discarded layer: " << layerTitle << endl;
				delete newLayer;
			}
		}
	}

	static int HandleGetCapabilities(struct MHD_Connection *connection)
	{
		struct MHD_Response* response = MHD_create_response_from_buffer(strlen(wmsCapabilites), (void*)wmsCapabilites, MHD_RESPMEM_PERSISTENT);
		int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);

		return ret;
	}

	static int HandleServiceException(struct MHD_Connection *connection, const astring& exeptionCode)
	{
		// TODO implement service exception according to WMS 1.3.0 Specs (XML)

		struct MHD_Response* response = MHD_create_response_from_buffer(exeptionCode.length(), (void*)exeptionCode.c_str(), MHD_RESPMEM_MUST_COPY);
		int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
		MHD_destroy_response(response);
		return ret;
	}

	int WebMapService::HandleGetMapRequest(struct MHD_Connection *connection, const astring& layers, const astring& format, GetMapRequest& gmr)
	{
		auto availableLayer = availableLayers.find(layers); // TODO: support multiple comma separated layers (incl. alpha blended composite as result)
		if (availableLayer == availableLayers.end())
		{
			return HandleServiceException(connection, "LayerNotDefined");
		}

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		if (format == "image/png")
		{
			gmr.valueFormat = CF_UINT32;
		}

		size_t pixelSize = ValueFormatSize[gmr.valueFormat];
		u8* data = new u8[pixelSize * gmr.width * gmr.height];

		Layer::HandleGetMapRequestResult result = availableLayer->second->HandleGetMapRequest(gmr, data);
		if (result != Layer::HGMRR_OK)
		{
			delete[] data;
			switch (result)
			{
			case dw::WebMapService::Layer::HGMRR_InvalidStyle:
				return HandleServiceException(connection, "StyleNotDefined");
			case dw::WebMapService::Layer::HGMRR_InvalidFormat:
				return HandleServiceException(connection, "InvalidFormat");
			case dw::WebMapService::Layer::HGMRR_InvalidSRS:
				return HandleServiceException(connection, "InvalidCRS");
			case dw::WebMapService::Layer::HGMRR_InvalidBBox:
				return HandleServiceException(connection, "InvalidBBox");
			case dw::WebMapService::Layer::HGMRR_OK:
			default:
				return HandleServiceException(connection, "Internal Error");
				break;
			}
		}

		int dataSize = 0; // TODO: replace stb library by faster lib (e.g. turbo version of libpng)
		unsigned char* renderingData = stbi_write_png_to_mem((u8*)data, gmr.width * sizeof(u32), gmr.width, gmr.height, 4, &dataSize);
		delete[] data;

		struct MHD_Response* response = MHD_create_response_from_buffer(dataSize, renderingData, MHD_RESPMEM_MUST_COPY);
		STBIW_FREE(renderingData);

		int success = MHD_add_response_header(response, "Content-Type", "image/png");
		int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
		std::cout << "GetMapRequest was processed within " << std::setprecision(2) << time_span.count() << " ms" << endl;

		return ret;
	}

	int WebMapService::HandleRequest(MHD_Connection* connection, const char* url, const char* method)
	{
		const char* request = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "request");
		astring requestStr = request ? astring(request) : "";

		if (requestStr == "GetCapabilities")
		{
			return HandleGetCapabilities(connection);
		}
		if (requestStr != "GetMap")
		{
			return HandleServiceException(connection, "unsupported request type");
		}

		// mandatory arguments
		const char* layers = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "layers");
		const char* styles = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "styles");
		const char* crs = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "crs");
		const char* bbox = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "bbox");
		const char* width = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "width");
		const char* height = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "height");
		const char* format = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "format");

		// optional arguments
		//const char* time = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");

		if (!layers || !crs || !bbox || !width || !height || !format)
		{
			return HandleServiceException(connection, "missing mandatory argument");
		}

		GetMapRequest gmr;
		gmr.crs = crs;

		gmr.width = atoi(width); // TODO: should be limited by size given in config/GetCapabilities.xml
		gmr.height = atoi(height);

		if (gmr.width == 0|| gmr.height == 0)
		{
			return HandleServiceException(connection, "InvalidSize");
		}

		char* bboxValue = NULL;
		const char* bboxEnd = bbox + strlen(bbox);
		gmr.bbox.minX = strtod(bbox, &bboxValue);
		if (bboxValue == bboxEnd || *bboxValue != ',') return HandleServiceException(connection, "InvalidBBOX");
		gmr.bbox.minY = strtod(bboxValue + 1, &bboxValue);
		if (bboxValue == bboxEnd || *bboxValue != ',') return HandleServiceException(connection, "InvalidBBOX");
		gmr.bbox.maxX = strtod(bboxValue + 1, &bboxValue);
		if (bboxValue == bboxEnd || *bboxValue != ',') return HandleServiceException(connection, "InvalidBBOX");
		gmr.bbox.maxY = strtod(bboxValue + 1, &bboxValue);

		return HandleGetMapRequest(connection, layers, format, gmr);
	}

}