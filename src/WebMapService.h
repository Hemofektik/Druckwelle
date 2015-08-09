#pragma once

#include "dwcore.h"
#include <string>
#include <vector>

namespace dw
{
	class WebMapService
	{
	public:

		class Layer
		{
		public:
			virtual const char_t* GetName() const = 0;
			virtual const char_t* GetTitle() const = 0;
			virtual const char_t* GetAbstract()  const { return NULL; };
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

		int HandleGetMapRequest(struct MHD_Connection *connection, struct GetMapRequest& gmr);
	
		struct MHD_Daemon* daemon;
	};

	#define IMPLEMENT_WEBMAPSERVICE_LAYER(Class, Name, Title) \
		static WebMapService::Layer* Construct##LayerName() { return new Class(); } \
		WebMapService::LayerFactory Class##Factory(LayerName, LayerTitle, Construct##LayerName);
}