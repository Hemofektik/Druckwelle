
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <atomic>

#include <ogr_api.h>
#include <ogr_spatialref.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#pragma warning (pop)
#include <cpl_vsi.h>

#include <omp.h>

#include <ZFXMath.h>

#include "../WebMapService.h"
#include "../utils/ImageProcessor.h"
#include "../utils/Filesystem.h"

using namespace std;
using namespace std::chrono;

using namespace ZFXMath;

using namespace dw;
using namespace dw::utils;

namespace dw
{
	namespace Layers
	{
		const string LayerName = "OSM_SDF";
		const string LayerTitle = "Open Streetmap data as Signed Distance Field";
		const string LayerAbstract = "Rasterizes Open Streetmap data to signed distance fields.";

		class OSMSDF : public WebMapService::Layer
		{
			OGRSpatialReference* OSM_SpatRef;
		public:

			virtual bool Init(/* layerconfig */) override
			{
				OSM_SpatRef = supportedCRS["EPSG:4326"];

				if (!OSM_SpatRef)
				{
					cout << "OSMSDF: " << "CRS support for EPSG:4326 is mandatory" << endl;
					return false;
				}
				
				return true;
			}

			virtual const char* GetName() const override
			{
				return LayerName.c_str();
			}

			virtual const char_t* GetTitle() const override
			{
				return LayerTitle.c_str();
			}

			virtual const char_t* GetAbstract()  const override
			{
				return LayerAbstract.c_str();
			};

			virtual const vector<DataType>& GetSuppordetFormats() const override
			{
				static const vector<DataType> SuppordetFormats = { DT_U8 };
				return SuppordetFormats;
			}

			virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, Image& img) override
			{
				auto crs = supportedCRS.find(gmr.crs);
				if (crs == supportedCRS.end())
				{
					return HGMRR_InvalidSRS;
				}

				switch (gmr.dataType)
				{
				case DT_U8:
				{
					return HandleGetMapRequest(gmr, crs->second, img);
				}
				default:
					return HGMRR_InvalidFormat;
				}
			}

		private:

			virtual ~OSMSDF() override
			{
			}

			HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, Image& img)
			{
				assert(img.rawDataType == DT_U8);

				BBox asterBBox;
				if (!TransformBBox(gmr.bbox, asterBBox, requestSRS, OSM_SpatRef))
				{
					return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
				}

				HandleGetMapRequestResult result = HGMRR_OK;

				high_resolution_clock::time_point t1 = high_resolution_clock::now();

				// TODO: render to image
				memset(img.rawData, 0x7F, img.rawDataSize);

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
				std::cout << "OSM SDF computation took " << std::setprecision(5) << time_span.count() << " ms" << endl;

				if (false) // debug output of loaded region of ASTER tiles
				{
					img.SaveToPNG("osmsdf.png");
				}

				return result;
			}
		};

		IMPLEMENT_WEBMAPSERVICE_LAYER(OSMSDF, LayerName, LayerTitle);
	}
}
