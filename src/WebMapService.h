#pragma once

#include "dwcore.h"

#include <string>
#include <cstring>
#include <vector>
#include <map>

class OGRSpatialReference;
class OGRCoordinateTransformation;

namespace dw
{
	class WebMapService
	{
	public:

		struct GetMapRequest
		{
			string styles;
			string crs;
			BBox bbox;
			int width;
			int height;

			DataType dataType;
		};

		class Layer
		{
		protected:
			
			struct SrcDestTransfromId
			{
				const OGRSpatialReference* src;
				const OGRSpatialReference* dst;

				bool operator< (const SrcDestTransfromId& other) const
				{
					return memcmp(this, &other, sizeof(SrcDestTransfromId)) < 0;
				}
			};

			std::map<string, OGRSpatialReference*> supportedCRS;
			std::map<SrcDestTransfromId, OGRCoordinateTransformation*> srsTransforms;

			bool TransformBBox(
				const BBox& srcBBox, BBox& dstBBox,
				const OGRSpatialReference* srcSRS, const OGRSpatialReference* dstSRS) const;

			OGRCoordinateTransformation* GetTransform(const OGRSpatialReference* src, const OGRSpatialReference* dst) const;
		private: 
			void CreateTransform(OGRSpatialReference* src, OGRSpatialReference* dst);

		public:

			enum HandleGetMapRequestResult
			{
				HGMRR_OK,
				HGMRR_InvalidStyle,
				HGMRR_InvalidFormat,
				HGMRR_InvalidBBox,
				HGMRR_InvalidSRS,
				HGMRR_InternalError, // e.g. file corrupt/missing
			};

			virtual ~Layer() {};

			virtual bool InitBase(/* layerconfig */);  // return true on successful init
			virtual bool Init(/* layerconfig */) { return true; }  // return true on successful init
 			virtual const char* GetName() const = 0;				// computer readable name (unique identification)
			virtual const char_t* GetTitle() const = 0;				// human readable name
			virtual const char_t* GetAbstract()  const { return NULL; };
			virtual const int GetFixedWidth() const { return 0; };
			virtual const int GetFixedHeight() const { return 0; };
			virtual const int GetMaxWidth() const { return 0; };
			virtual const int GetMaxHeight() const { return 0; };
			virtual const std::vector<DataType>& GetSuppordetFormats() const = 0;

			virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, class Image& img) = 0;
		};

		typedef Layer* (*CreateLayer)();

		class LayerFactory
		{
		public:
			LayerFactory(const string& layerName, const string& layerTitle, CreateLayer createLayer)
			{
				LayerDesc lDesc;
				lDesc.name = layerName;
				lDesc.title = layerTitle;
				lDesc.createLayer = createLayer;
				GetStaticLayers().push_back(lDesc);
			}

			static void CreateLayers(std::map<string, Layer*>& layers /*, config*/);

		private:

			struct LayerDesc
			{
				string name; // computer readable name (unique identification)
				string title; // human readable name
				CreateLayer createLayer;
			};

			static std::vector<LayerDesc>& GetStaticLayers()
			{
				static std::vector<LayerDesc>* layers = NULL;
				if (layers == NULL)
				{
					layers = new std::vector<LayerDesc>();
				}
				return *layers;
			}
		};

		WebMapService();

		int Start();
		void Stop();

		int HandleRequest(struct MHD_Connection* connection, const char* url, const char* method);
	private:

		int HandleGetMapRequest(struct MHD_Connection *connection, const string& layers, ContentType contentType, struct GetMapRequest& gmr);

		std::map<string, Layer*> availableLayers;
	};

	#define DECLARE_WEBMAPSERVICE_LAYER(Class, Name, Title) \
		friend WebMapService::Layer* Construct##Class();

	#define IMPLEMENT_WEBMAPSERVICE_LAYER(Class, Name, Title) \
		WebMapService::Layer* Construct##Class() { return new Class(); } \
		WebMapService::LayerFactory Class##Factory(Name, Title, Construct##Class);
}
