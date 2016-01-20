
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <string.h>
#include <microhttpd.h>

#include "WebMapService.h"

#include <ogr_api.h>
#include <ogr_spatialref.h>

#include "utils/ImageProcessor.h"
#include "utils/Capabilities.h"


using namespace std;
using namespace std::chrono;

static const char* wmsCapabilites =
#include "GetCapabilities.xml"
;

namespace dw
{
	WebMapService::WebMapService()
	{
	}

	int WebMapService::Start()
	{
		// TODO: provide option to list all available layers and propose detailed config info for each layer (e.g. --help <layerName>)

		cout << "WebMapService: Reading Config" << endl;

		// TODO: read config

		cout << "WebMapService: Creating Layers" << endl;
		LayerFactory::CreateLayers(availableLayers /*, config*/);

		if (availableLayers.size() == 0)
		{
			cout << "WebMapService: not a single layer was configured... shutting down" << endl;

			return -1;
		}

		WMSCapabilities caps(availableLayers);
		cout << caps.GetXML();
		// TODO: get whole caps string and replace current static solution

#if _DEBUG
		cout << endl << "WebMapService: example request: " << endl << "http://localhost:8282/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&BBOX=-14607737,2378356,-6201882,7104700.22959910612553358&CRS=EPSG:3857&WIDTH=1905&HEIGHT=1071&LAYERS=" << availableLayers.begin()->first << "&STYLES=&FORMAT=image/png&DPI=192&MAP_RESOLUTION=192&FORMAT_OPTIONS=dpi:192&TRANSPARENT=TRUE" << endl;
#endif

		return 0;
	}

	void WebMapService::Stop()
	{
	}

	void WebMapService::LayerFactory::CreateLayers(std::map<string, Layer*>& layers /*, config*/)
	{
		for (const auto& layerDesc : LayerFactory::GetStaticLayers())
		{
			Layer* newLayer = layerDesc.createLayer();

			cout << "WebMapService: Loading layer: " << layerDesc.name << '\r';
			if (newLayer->InitBase(/* layerconfig */) && newLayer->Init(/* layerconfig */))
			{
				layers[layerDesc.name] = newLayer;
				wcout << "WebMapService: Activated layer: " << newLayer->GetTitle() << endl;
			}
			else
			{
				cout << "WebMapService: Discarded layer: " << layerDesc.name << endl;
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

	static int HandleServiceException(struct MHD_Connection *connection, const string& exeptionCode)
	{
		// TODO implement service exception according to WMS 1.3.0 Specs (XML)

		struct MHD_Response* response = MHD_create_response_from_buffer(exeptionCode.length(), (void*)exeptionCode.c_str(), MHD_RESPMEM_MUST_COPY);
		int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
		MHD_destroy_response(response);
		return ret;
	}

	int WebMapService::HandleGetMapRequest(struct MHD_Connection *connection, const string& layers, ContentType contentType, GetMapRequest& gmr)
	{
		auto availableLayer = availableLayers.find(layers); // TODO: support multiple comma separated layers (incl. alpha blended composite as result)
		if (availableLayer == availableLayers.end())
		{
			return HandleServiceException(connection, "LayerNotDefined");
		}

		auto& layer = *availableLayer->second;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		auto supportedFormats = layer.GetSuppordetFormats();
		gmr.dataType = FindCompatibleDataType(contentType, supportedFormats);

		if (gmr.dataType == DT_Unknown)
		{
			return HandleServiceException(connection, "InvalidFormat");
		}

		Image image(gmr.width, gmr.height, gmr.dataType);

		Layer::HandleGetMapRequestResult result = layer.HandleGetMapRequest(gmr, image);
		if (result != Layer::HGMRR_OK)
		{
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
			case dw::WebMapService::Layer::HGMRR_InternalError:
			default:
				return HandleServiceException(connection, "Internal Error");
				break;
			}
		}

		utils::ConvertRawImageToContentType(image, contentType);
		struct MHD_Response* response = MHD_create_response_from_buffer(image.processedDataSize, image.processedData, MHD_RESPMEM_MUST_COPY);
		
		int success = MHD_add_response_header(response, "Content-Type", ContentTypeId[contentType].c_str());
		int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
		std::cout << "GetMapRequest was processed within " << std::setprecision(5) << time_span.count() << " ms" << endl;

		return ret;
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
		ContentType contentType = GetContentType(format);
		if (contentType == CT_Unknown)
		{
			return HandleServiceException(connection, "InvalidFormat");
		}

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

		if (gmr.bbox.minX > gmr.bbox.maxX) return HandleServiceException(connection, "InvalidBBOX");
		if (gmr.bbox.minY > gmr.bbox.maxY) return HandleServiceException(connection, "InvalidBBOX");

		return HandleGetMapRequest(connection, layers, contentType, gmr);
	}

	bool WebMapService::Layer::InitBase(/* layerconfig */)
	{
		supportedCRS =
		{
			//pair<string, OGRSpatialReference*>("EPSG:3857", (OGRSpatialReference*)OSRNewSpatialReference(NULL)),
			pair<string, OGRSpatialReference*>("EPSG:4326",  (OGRSpatialReference*)OSRNewSpatialReference(NULL))
		};

		for (auto it = supportedCRS.begin(); it != supportedCRS.end(); it++)
		{
			int epsgCode = atoi(it->first.c_str() + 5);
			OGRErr err = it->second->importFromEPSG(epsgCode);
			if (err == OGRERR_UNSUPPORTED_SRS)
			{
				cout << "WebMapService::Layer: " << "requested EPSG Code is unsupported:" << epsgCode << endl;
				return false;
			}
		}

		// create all possible SRS transformation permutations
		for (auto it1 = supportedCRS.begin(); it1 != supportedCRS.end(); it1++)
		{
			for (auto it2 = it1; it2 != supportedCRS.end(); it2++)
			{
				if (it2 == supportedCRS.end()) break;

				CreateTransform(it1->second, it2->second);
				CreateTransform(it2->second, it1->second);
			}
		}

		return true;
	}

	bool WebMapService::Layer::TransformBBox(
		const BBox& srcBBox, BBox& dstBBox,
		const OGRSpatialReference* srcSRS, const OGRSpatialReference* dstSRS) const
	{
		bool success = true;
		auto transform = GetTransform(srcSRS, dstSRS);
		if (transform)
		{
			double x[2] = { srcBBox.minX, srcBBox.maxX };
			double y[2] = { srcBBox.minY, srcBBox.maxY };
			success = transform->Transform(2, x, y) == TRUE;
			dstBBox.minX = x[0];
			dstBBox.minY = y[0];
			dstBBox.maxX = x[1];
			dstBBox.maxY = y[1];
		}
		else
		{
			memcpy(&dstBBox, &srcBBox, sizeof(BBox));
		}
		return success;
	}

	OGRCoordinateTransformation* WebMapService::Layer::GetTransform(const OGRSpatialReference* src, const OGRSpatialReference* dst) const
	{
		SrcDestTransfromId transId;
		transId.src = src;
		transId.dst = dst;

		auto transform = srsTransforms.find(transId);
		return transform->second;
	}

	void WebMapService::Layer::CreateTransform(OGRSpatialReference* src, OGRSpatialReference* dst)
	{
		SrcDestTransfromId transId;
		transId.src = src;
		transId.dst = dst;

		auto existingTransform = srsTransforms.find(transId);
		if (existingTransform != srsTransforms.end()) return;

		srsTransforms[transId] = (src == dst) ? NULL : OGRCreateCoordinateTransformation(src, dst);
	}
}