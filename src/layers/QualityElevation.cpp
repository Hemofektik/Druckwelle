#include <filesystem>
#include <vector>
#include <mutex>

#include <ogr_api.h>
#include <ogr_spatialref.h>

#include "../WebMapService.h"

using namespace std;
using namespace std::experimental::filesystem::v1;
using namespace dw;

namespace dw
{
namespace Layers
{
	const astring LayerName = "QualityElevation";
	const string LayerTitle = dwTEXT("ASTER + SRTMv3 High Quality Elevation Service");

	class QualityElevation : public WebMapService::Layer
	{
		struct SrcDestTransfromId
		{
			const OGRSpatialReference* src;
			const OGRSpatialReference* dst;

			bool operator< (const QualityElevation::SrcDestTransfromId& other) const
			{
				return memcmp(this, &other, sizeof(QualityElevation::SrcDestTransfromId)) < 0;
			}
		};

		vector<path> filePaths;
		map<astring, OGRSpatialReference*> supportedCRS;
		OGRSpatialReference* ASTER_EPSG;
		map<SrcDestTransfromId, OGRCoordinateTransformation*> srsTransforms;


	public:

		virtual bool Init(/* layerconfig */) override
		{
			supportedCRS =
			{
				pair<astring, OGRSpatialReference*>("EPSG:3857", new OGRSpatialReference()),
				pair<astring, OGRSpatialReference*>("EPSG:4326", new OGRSpatialReference())
			};

			for (auto it = supportedCRS.begin(); it != supportedCRS.end(); it++)
			{
				int epsgCode = atoi(it->first.c_str() + 5);
				OGRErr err = it->second->importFromEPSG(epsgCode);
				if (err == OGRERR_UNSUPPORTED_SRS)
				{
					return false;
				}
			}

			ASTER_EPSG = supportedCRS["EPSG:4326"];

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

			// load TOC from disk
			const path ASTERSourceDir("D:/ASTER/");

			filePaths.reserve(250000);

			for (directory_iterator di(ASTERSourceDir); di != end(di); di++)
			{
				const auto& entity = *di;
				const auto extension = entity.path().extension();
				if (is_regular_file(entity.status()) && entity.path().extension().generic_string() == ".zip")
				{
					filePaths.push_back(entity.path());
				}
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


		virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, u8* data) override
		{
			auto crs = supportedCRS.find(gmr.crs);
			if (crs == supportedCRS.end())
			{
				return HGMRR_InvalidSRS;
			}

			switch (gmr.valueFormat)
			{
			case WebMapService::CF_UINT32:
				return HandleGetMapRequest<u32>(gmr, crs->second, (u32*)data);
			default:
				return HGMRR_InvalidFormat;
			}
		}

	private:

		template<typename T>
		HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, T* data)
		{
			WebMapService::GetMapRequest::BBox asterBBox;
			if (!TransformBBox(gmr.bbox, asterBBox, requestSRS, ASTER_EPSG))
			{
				return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
			}

			memset(data, 0, sizeof(T) * gmr.width * gmr.height);

			const int numPixels = gmr.width * gmr.height;
			for (size_t p = 0; p < numPixels; p++)
			{
				data[p] = 0xFF0000FF;
			}

			return HGMRR_OK;
		}


		virtual ~QualityElevation() override
		{
			for each (auto crs in supportedCRS)
			{
				delete crs.second;
			}

			for each (auto trans in srsTransforms)
			{
				OGRCoordinateTransformation::DestroyCT(trans.second);
			}
		}


		bool TransformBBox(
			const WebMapService::GetMapRequest::BBox& srcBBox, WebMapService::GetMapRequest::BBox& dstBBox, 
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
				memcpy(&dstBBox, &srcBBox, sizeof(WebMapService::GetMapRequest::BBox));
			}
			return success;
		}

		OGRCoordinateTransformation* GetTransform(const OGRSpatialReference* src, const OGRSpatialReference* dst) const
		{
			SrcDestTransfromId transId;
			transId.src = src;
			transId.dst = dst;

			auto transform = srsTransforms.find(transId);
			return transform->second;
		}

		void CreateTransform(OGRSpatialReference* src, OGRSpatialReference* dst)
		{
			SrcDestTransfromId transId;
			transId.src = src;
			transId.dst = dst;

			auto existingTransform = srsTransforms.find(transId);
			if (existingTransform != srsTransforms.end()) return;

			srsTransforms[transId] = (src == dst) ? NULL : OGRCreateCoordinateTransformation(src, dst);
		}
	};




	IMPLEMENT_WEBMAPSERVICE_LAYER(QualityElevation, LayerName, LayerTitle);
	}
}