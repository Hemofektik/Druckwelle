
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <string.h>

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

	void WebMapTileService::LayerFactory::CreateLayers(std::map<string, Layer*>& layers /*, config*/)
	{
		for (const auto& layerDesc : LayerFactory::GetStaticLayers())
		{
			Layer* newLayer = layerDesc.createLayer();

			cout << "WebMapTileService: Loading layer: " << layerDesc.name << '\r';
			if (newLayer->Init( /* layerconfig */ ))
			{
				layers[layerDesc.name] = newLayer;
				wcout << "WebMapTileService: Activated layer: " << newLayer->GetTitle() << endl;
			}
			else
			{
				cout << "WebMapTileService: Discarded layer: " << layerDesc.name << endl;
				delete newLayer;
			}
		}
	}

	static void HandleGetCapabilities(IHTTPRequest& request)
	{
		return request.Reply(HTTP_OK, wmsCapabilites);
	}

	static void HandleServiceException(IHTTPRequest& request, const string& exeptionCode)
	{
		// TODO implement service exception according to WMTS Specs (XML)
		request.Reply(HTTP_BadRequest, exeptionCode);
	}

	void WebMapTileService::HandleGetTileRequest(IHTTPRequest& request, const string& layers, ContentType contentType, struct GetTileRequest& gtr)
	{
		auto availableLayer = availableLayers.find(layers); // TODO: support multiple comma separated layers (incl. alpha blended composite as result)
		if (availableLayer == availableLayers.end())
		{
			return HandleServiceException(request, "LayerNotDefined");
		}

		auto& layer = *availableLayer->second;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		auto supportedFormats = layer.GetSuppordetFormats();
		gtr.dataType = FindCompatibleDataType(contentType, supportedFormats);

		if (gtr.dataType == DT_Unknown)
		{
			return HandleServiceException(request, "InvalidFormat");
		}

		int tileWidth = availableLayer->second->GetTileWidth();
		int tileHeight = availableLayer->second->GetTileHeight();

		Image image(tileWidth, tileHeight, gtr.dataType);

		Layer::HandleGetTileRequestResult result = layer.HandleGetTileRequest(gtr, image);
		if (result != Layer::HGTRR_OK)
		{
			switch (result)
			{
			case dw::WebMapTileService::Layer::HGTRR_InvalidStyle:
				return HandleServiceException(request, "StyleNotDefined");
			case dw::WebMapTileService::Layer::HGTRR_InvalidFormat:
				return HandleServiceException(request, "InvalidFormat");
			case dw::WebMapTileService::Layer::HGTRR_InternalError:
			default:
				return HandleServiceException(request, "Internal Error");
				break;
			}
		}

		utils::ConvertRawImageToContentType(image, contentType);

		request.Reply(HTTP_OK, image.processedData, image.processedDataSize, contentType);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
		std::cout << "GetMapRequest was processed within " << std::setprecision(5) << time_span.count() << " ms" << endl;
	}

	void WebMapTileService::HandleRequest(IHTTPRequest& request)
	{
		string requestType = request.GetArgumentValue("request");

		if (requestType == "GetCapabilities")
		{
			return HandleGetCapabilities(request);
		}
		if (requestType != "GetTile")
		{
			return HandleServiceException(request, "unsupported request type");
		}

		// mandatory arguments
		const auto layers = request.GetArgumentValue("layers");
		const auto styles = request.GetArgumentValue("styles");
		const auto format = request.GetArgumentValue("format");
		const auto tileRow = request.GetArgumentValue("tilerow");
		const auto tileCol = request.GetArgumentValue("tilecol");
		const auto tileMatrixSet = request.GetArgumentValue("tilematrixset");
		const auto tileMatrix = request.GetArgumentValue("tilematrix");

		// optional arguments
		//const char* time = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");

		if (!layers.size() || !styles.size() || !format.size() || !tileRow.size() || !tileCol.size() || !tileMatrixSet.size() || !tileMatrix.size())
		{
			return HandleServiceException(request, "MissingParameterValue");
		}

		GetTileRequest gtr;
		ContentType contentType = GetContentType(format);
		if (contentType == CT_Unknown)
		{
			return HandleServiceException(request, "InvalidFormat");
		}

		gtr.tileRow = stoi(tileRow);
		gtr.tileCol = stoi(tileCol);

		return HandleGetTileRequest(request, layers, contentType, gtr);
	}

}