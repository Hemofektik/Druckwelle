#pragma once

#include "../dwcore.h"
#include "../WebMapService.h"
#include "../WebMapTileService.h"

#include <map>

namespace dw
{
	template<ServiceType ST, typename Layer>
	class ServiceCapabilities
	{
	public:
		ServiceCapabilities(const std::map<string, Layer*>& availableLayers);

		virtual ~ServiceCapabilities();

		const string& GetXML() const;

	private:
		string xml;
	};

	class WMSCapabilities : public ServiceCapabilities<WMS, WebMapService::Layer>
	{
	public:
		WMSCapabilities(const std::map<string, WebMapService::Layer*>& availableLayers);
	};
	class WMTSCapabilities : public ServiceCapabilities<WMTS, WebMapTileService::Layer>
	{
	public:
		WMTSCapabilities(const std::map<string, WebMapTileService::Layer*>& availableLayers);
	};
}