
#include <iostream>
#include <iomanip>
#include <chrono>

#include <string.h>

#include "WebMapService.h"

#include <ogr_api.h>
#include <ogr_spatialref.h>

#include "utils/ImageProcessor.h"
#include "utils/Capabilities.h"


using namespace std;
using namespace std::chrono;
using namespace libconfig;

static const char* wmsCapabilites =
#include "GetCapabilities.xml"
;

namespace dw
{
	WebMapService::WebMapService()
	{
	}

	int WebMapService::Start(ChainedSetting& config)
	{
		// TODO: provide option to list all available layers and propose detailed config info for each layer (e.g. --help <layerName>)

		cout << "WebMapService: Reading Config" << endl;

		// TODO: read config
		/*string name = cs["name"].defaultValue("<name>").isMandatory();
		string abstract = cs["abstract"].defaultValue("<unknown>");
		double longitude = cs["longitude"].min(-180.0).max(180.0).isMandatory();
		double latitude = cs["latitude"].min(-90.0).max(90.0).isMandatory();*/

		cout << "WebMapService: Creating Layers" << endl;

		auto layersConfig = config["layers"];
		LayerFactory::CreateLayers(availableLayers, layersConfig);

		if (availableLayers.size() == 0)
		{
			cout << "WebMapService: not a single layer was configured... shutting down" << endl;

			return -1;
		}

		WMSCapabilities caps(availableLayers);
		cout << caps.GetXML();
		// TODO: get whole caps string and replace current static solution

#if _DEBUG
		//cout << endl << "WebMapService: example request: " << endl << "http://localhost:8282/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&BBOX=-14607737,2378356,-6201882,7104700.22959910612553358&CRS=EPSG:3857&WIDTH=1905&HEIGHT=1071&LAYERS=" << availableLayers.begin()->first << "&STYLES=&FORMAT=image/png&DPI=192&MAP_RESOLUTION=192&FORMAT_OPTIONS=dpi:192&TRANSPARENT=TRUE" << endl;
#endif

		return 0;
	}

	void WebMapService::Stop()
	{
	}

	void WebMapService::LayerFactory::CreateLayers(std::map<string, Layer*>& layers, ChainedSetting& config)
	{
		for (const auto& layerDesc : LayerFactory::GetStaticLayers())
		{
			auto layerName = layerDesc.name;
			auto layerConfig = config[layerName];
			if (!layerConfig.exists()) continue;

			Layer* newLayer = layerDesc.createLayer();

			cout << "WebMapService: Loading layer: " << layerName << '\r';
			if (newLayer->InitBase(layerConfig) && newLayer->Init(layerConfig))
			{
				layers[layerDesc.name] = newLayer;
				wcout << "WebMapService: Activated layer: " << newLayer->GetTitle() << endl;
			}
			else
			{
				cout << "WebMapService: Discarded layer: " << layerName << endl;
				delete newLayer;
			}
		}
	}

	static void HandleGetCapabilities(IHTTPRequest& request)
	{
		request.Reply(HTTP_OK, wmsCapabilites);
	}

	static void HandleServiceException(IHTTPRequest& request, const string& exeptionCode)
	{
		// TODO implement service exception according to WMS 1.3.0 Specs (XML)
		request.Reply(HTTP_BadRequest, exeptionCode);
	}

	void WebMapService::HandleGetMapRequest(IHTTPRequest& request, const string& layers, ContentType contentType, GetMapRequest& gmr)
	{
		auto availableLayer = availableLayers.find(layers); // TODO: support multiple comma separated layers (incl. alpha blended composite as result)
		if (availableLayer == availableLayers.end())
		{
			return HandleServiceException(request, "LayerNotDefined");
		}

		auto& layer = *availableLayer->second;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		auto supportedFormats = layer.GetSuppordetFormats();
		gmr.dataType = FindCompatibleDataType(contentType, supportedFormats);

		if (gmr.dataType == DT_Unknown)
		{
			return HandleServiceException(request, "InvalidFormat");
		}

		Image image(gmr.width, gmr.height, gmr.dataType);

		Layer::HandleGetMapRequestResult result = layer.HandleGetMapRequest(gmr, image);
		if (result != Layer::HGMRR_OK)
		{
			switch (result)
			{
			case dw::WebMapService::Layer::HGMRR_InvalidStyle:
				return HandleServiceException(request, "StyleNotDefined");
			case dw::WebMapService::Layer::HGMRR_InvalidFormat:
				return HandleServiceException(request, "InvalidFormat");
			case dw::WebMapService::Layer::HGMRR_InvalidSRS:
				return HandleServiceException(request, "InvalidCRS");
			case dw::WebMapService::Layer::HGMRR_InvalidBBox:
				return HandleServiceException(request, "InvalidBBox");
			case dw::WebMapService::Layer::HGMRR_InternalError:
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

	void WebMapService::HandleRequest(IHTTPRequest& request)
	{
		auto requestType = request.GetArgumentValue("request");
		if (requestType == "GetCapabilities")
		{
			return HandleGetCapabilities(request);
		}
		if (requestType != "GetMap")
		{
			return HandleServiceException(request, "unsupported request type");
		}

		// mandatory arguments
		const auto layers = request.GetArgumentValue("layers");
		const auto styles = request.GetArgumentValue("styles");
		const auto crs = request.GetArgumentValue("crs");
		const auto bbox = request.GetArgumentValue("bbox");
		const auto width = request.GetArgumentValue("width");
		const auto height = request.GetArgumentValue("height");
		const auto format = request.GetArgumentValue("format");

		// optional arguments
		//const char* time = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");

		if (!layers.size() || !crs.size() || !bbox.size() || !width.size() || !height.size() || !format.size())
		{
			return HandleServiceException(request, "missing mandatory argument");
		}

		GetMapRequest gmr;
		ContentType contentType = GetContentType(format);
		if (contentType == CT_Unknown)
		{
			return HandleServiceException(request, "InvalidFormat");
		}

		gmr.crs = crs;
		gmr.width = stoi(width); // TODO: should be limited by size given in config/GetCapabilities.xml
		gmr.height = stoi(height);

		if (gmr.width == 0|| gmr.height == 0)
		{
			return HandleServiceException(request, "InvalidSize");
		}
		
		size_t bboxValueLocalOffset = 0;
		size_t bboxValueOffset = 0;
		const size_t bboxEnd = bbox.length();
		gmr.bbox.minX = stod(bbox, &bboxValueLocalOffset); bboxValueOffset += bboxValueLocalOffset;
		if (bboxValueOffset == bboxEnd || bbox[bboxValueOffset] != ',') return HandleServiceException(request, "InvalidBBOX");
		gmr.bbox.minY = stod(bbox.substr(bboxValueOffset + 1), &bboxValueLocalOffset); bboxValueOffset += bboxValueLocalOffset + 1;
		if (bboxValueOffset == bboxEnd || bbox[bboxValueOffset] != ',') return HandleServiceException(request, "InvalidBBOX");
		gmr.bbox.maxX = stod(bbox.substr(bboxValueOffset + 1), &bboxValueLocalOffset); bboxValueOffset += bboxValueLocalOffset + 1;
		if (bboxValueOffset == bboxEnd || bbox[bboxValueOffset] != ',') return HandleServiceException(request, "InvalidBBOX");
		gmr.bbox.maxY = stod(bbox.substr(bboxValueOffset + 1), &bboxValueLocalOffset); bboxValueOffset += bboxValueLocalOffset + 1;

		if (gmr.bbox.minX > gmr.bbox.maxX) return HandleServiceException(request, "InvalidBBOX");
		if (gmr.bbox.minY > gmr.bbox.maxY) return HandleServiceException(request, "InvalidBBOX");

		return HandleGetMapRequest(request, layers, contentType, gmr);
	}

	bool WebMapService::Layer::InitBase(libconfig::ChainedSetting& config)
	{
		auto crs = config["CRS"];
		const int numCRS = crs.getLength();
		if (numCRS > 0)
		{
			for (int c = 0; c < numCRS; c++)
			{
				supportedCRS[crs[c]] = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
			}
		}
		else
		{
			cout << "WebMapService::Layer: " << "At least one CRS must be defined" << endl;
			return false;
		}

		for (auto it = supportedCRS.begin(); it != supportedCRS.end(); it++)
		{
			OGRErr err = OGRERR_UNSUPPORTED_SRS;
			string crsString = it->first;
			if (crsString.length() > 5)
			{
				int epsgCode = stoi(crsString.c_str() + 5); // TODO: support non-EPSG codes
				err = it->second->importFromEPSG(epsgCode);
			}
			if (err == OGRERR_UNSUPPORTED_SRS)
			{
				cout << "WebMapService::Layer: " << "requested CRS is unsupported:" << crsString << endl;
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