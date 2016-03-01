#pragma once

#include "dwcore.h"
#include "utils\HTTP\HTTPRequest.h"

#include <string>
#include <vector>
#include <map>

namespace dw
{
	class WebMapTileService
	{
	public:
		struct GetTileRequest
		{
			int tileCol;
			int tileRow;

			DataType dataType;
		};

		class Layer
		{
		public:

			enum HandleGetTileRequestResult
			{
				HGTRR_OK,
				HGTRR_InvalidStyle,
				HGTRR_InvalidFormat,
				HGTRR_InternalError, // e.g. file corrupt/missing
			};

			virtual ~Layer() {};

			virtual bool Init(/* layerconfig */) { return true; };  // return true on successful init
 			virtual const char* GetIdentifier() const = 0;			// computer readable name (unique identification)
			virtual const char_t* GetTitle() const = 0;				// human readable name
			virtual const char_t* GetAbstract()  const { return NULL; };
			virtual int GetTileWidth() const = 0;
			virtual int GetTileHeight() const = 0;
			virtual const std::vector<DataType>& GetSuppordetFormats() const = 0;

			virtual HandleGetTileRequestResult HandleGetTileRequest(const WebMapTileService::GetTileRequest& gtr, class Image& img) = 0;
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

		WebMapTileService();

		int Start();
		void Stop();

		void HandleRequest(IHTTPRequest& request);
	private:
		void HandleGetTileRequest(IHTTPRequest& request, const string& layers, ContentType contentType, struct GetTileRequest& gtr);

		std::map<string, Layer*> availableLayers;
	};

	#define IMPLEMENT_WEBMAPTILESERVICE_LAYER(Class, Name, Title) \
		static WebMapTileService::Layer* Construct##LayerName() { return new Class(); } \
		WebMapTileService::LayerFactory Class##Factory(Name, Title, Construct##LayerName);
}
