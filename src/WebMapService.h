#pragma once

#include "dwcore.h"
#include <string>
#include <vector>
#include <map>

namespace dw
{
	class WebMapService
	{
	public:

		enum ValueFormat
		{
			CF_INT8,
			CF_INT16,
			CF_INT32,
			CF_INT64,
			CF_UINT8,
			CF_UINT16,
			CF_UINT32,
			CF_UINT64,
			CF_FLOAT32,
			CF_FLOAT64,
			CF_NumFormats // must be last entry
		};

		const size_t ValueFormatSize[CF_NumFormats] = {
			sizeof(s8),
			sizeof(s16),
			sizeof(s32),
			sizeof(s64),
			sizeof(u8),
			sizeof(u16),
			sizeof(u32),
			sizeof(u64),
			sizeof(f32),
			sizeof(f64)
		};

		struct GetMapRequest
		{
			struct BBox
			{
				double minX;
				double minY;
				double maxX;
				double maxY;
			};
			astring styles;
			astring crs;
			BBox bbox;
			int width;
			int height;

			ValueFormat valueFormat;
		};

		class Layer
		{
		public:

			enum HandleGetMapRequestResult
			{
				HGMRR_OK,
				HGMRR_InvalidStyle,
				HGMRR_InvalidFormat,
			};

			virtual const char_t* GetName() const = 0;
			virtual const char_t* GetTitle() const = 0;
			virtual const char_t* GetAbstract()  const { return NULL; };

			virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, u8* data) = 0;
		};

		typedef Layer* (*CreateLayer)();

		class LayerFactory
		{
		public:
			LayerFactory(const string& layerName, const string& layerTitle, CreateLayer createLayer)
			{
				LayerDesc lDesc;
				lDesc.layerName = layerName;
				lDesc.layerTitle = layerTitle;
				lDesc.createLayer = createLayer;
				layers.push_back(std::move(lDesc));
			}

			static void CreateLayers( std::map<astring, Layer*> layers /*, config*/)
			{
				// TODO: implement
			}

		private:

			struct LayerDesc
			{
				string layerName;
				string layerTitle;
				CreateLayer createLayer;
			};

			static std::vector<LayerDesc> layers;
		};

		WebMapService();

		int Start();
		void Stop();

		int HandleRequest(struct MHD_Connection* connection, const char* url, const char* method);
	private:

		int HandleGetMapRequest(struct MHD_Connection *connection, const astring& layers, const astring& format, struct GetMapRequest& gmr);

		struct MHD_Daemon* daemon;
		std::map<astring, Layer*> availableLayers;
	};

	#define IMPLEMENT_WEBMAPSERVICE_LAYER(Class, Name, Title) \
		static WebMapService::Layer* Construct##LayerName() { return new Class(); } \
		WebMapService::LayerFactory Class##Factory(LayerName, LayerTitle, Construct##LayerName);
}