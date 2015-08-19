
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <microhttpd.h>

#include "utils/ImageProcessor.h"
#include "WebMapTileService.h"

using namespace std;
using namespace std::chrono;

static const char* wmsCapabilites =
#include "GetCapabilities.xml"
;

namespace dw
{

	WebMapTileService::WebMapTileService()
	{
	}

	int WebMapTileService::Start()
	{
		// TODO: provide option to list all available layers and propose detailed config info for each layer (e.g. --help <layerName>)

		cout << "WebMapTileService: Reading Config" << endl;

		// TODO: read config

		cout << "WebMapTileService: Creating Layers" << endl;
		LayerFactory::CreateLayers(availableLayers /*, config*/);

		if (availableLayers.size() == 0)
		{
			cout << "WebMapTileService: not a single layer was configured... shutting down" << endl;

			return -1;
		}

#if _DEBUG
		cout << endl << "WebMapTileService: example request: " << endl << "http://localhost:8282/?SERVICE=WMTS&VERSION=1.3.0&REQUEST=GetTile..." << endl;
#endif

		return 0;
	}

	void WebMapTileService::Stop()
	{
	}

	void WebMapTileService::LayerFactory::CreateLayers(std::map<astring, Layer*>& layers /*, config*/)
	{
		for each (const auto& layerDesc in LayerFactory::GetStaticLayers())
		{
			Layer* newLayer = layerDesc.createLayer();

			astring layerTitle(layerDesc.title.cbegin(), layerDesc.title.cend());
			cout << "WebMapTileService: Loading layer: " << layerTitle << '\r';
			if (newLayer->Init( /* layerconfig */ ))
			{
				layers[layerDesc.name] = newLayer;
				cout << "WebMapTileService: Enabled layer: " << layerTitle << endl;
			}
			else
			{
				cout << "WebMapTileService: Discarded layer: " << layerTitle << endl;
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

	int WebMapTileService::HandleGetTileRequest(struct MHD_Connection *connection, const astring& layers, ContentType contentType, struct GetTileRequest& gtr)
	{
		auto availableLayer = availableLayers.find(layers); // TODO: support multiple comma separated layers (incl. alpha blended composite as result)
		if (availableLayer == availableLayers.end())
		{
			return HandleServiceException(connection, "LayerNotDefined");
		}

		auto& layer = *availableLayer->second;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		auto supportedFormats = layer.GetSuppordetFormats();
		gtr.dataType = FindCompatibleDataType(contentType, supportedFormats);

		if (gtr.dataType == DT_Unknown)
		{
			return HandleServiceException(connection, "InvalidFormat");
		}

		int tileWidth = availableLayer->second->GetTileWidth();
		int tileHeight = availableLayer->second->GetTileHeight();

		Image image(tileWidth, tileHeight, gtr.dataType);

		Layer::HandleGetTileRequestResult result = layer.HandleGetTileRequest(gtr, image.rawData);
		if (result != Layer::HGTRR_OK)
		{
			switch (result)
			{
			case dw::WebMapTileService::Layer::HGTRR_InvalidStyle:
				return HandleServiceException(connection, "StyleNotDefined");
			case dw::WebMapTileService::Layer::HGTRR_InvalidFormat:
				return HandleServiceException(connection, "InvalidFormat");
			case dw::WebMapTileService::Layer::HGTRR_InternalError:
			default:
				return HandleServiceException(connection, "Internal Error");
				break;
			}
		}

		ConvertRawImageToContentType(image, contentType);
		struct MHD_Response* response = MHD_create_response_from_buffer(image.processedDataSize, image.processedData, MHD_RESPMEM_MUST_COPY);

		int success = MHD_add_response_header(response, "Content-Type", ContentTypeId[contentType].c_str());
		int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
		std::cout << "GetMapRequest was processed within " << std::setprecision(5) << time_span.count() << " ms" << endl;

		return ret;
	}

	int WebMapTileService::HandleRequest(MHD_Connection* connection, const char* url, const char* method)
	{
		const char* request = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "request");
		astring requestStr = request ? astring(request) : "";

		if (requestStr == "GetCapabilities")
		{
			return HandleGetCapabilities(connection);
		}
		if (requestStr != "GetTile")
		{
			return HandleServiceException(connection, "unsupported request type");
		}

		// mandatory arguments
		const char* layers = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "layers");
		const char* styles = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "styles");
		const char* format = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "format");
		const char* tileRow = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "tilerow");
		const char* tileCol = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "tilecol");

		// optional arguments
		//const char* time = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");

		if (!layers || !format)
		{
			return HandleServiceException(connection, "missing mandatory argument");
		}

		GetTileRequest gtr;
		ContentType contentType = GetContentType(format);
		if (contentType == CT_Unknown)
		{
			return HandleServiceException(connection, "InvalidFormat");
		}

		gtr.tileRow = atoi(tileRow);
		gtr.tileCol = atoi(tileCol);

		return HandleGetTileRequest(connection, layers, contentType, gtr);
	}

}