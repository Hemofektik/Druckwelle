
#include <sstream>


#include "Capabilities.h"

namespace dw
{
	/*static AutoPtr<Element> AddSimpleElement(Document* doc, Element* root, string elementName, string elementValue)
	{
		AutoPtr<Element> element = doc->createElement(elementName);
		if(elementValue.length() > 0)
		{
			AutoPtr<Text> nameValue = doc->createTextNode(elementValue);
			element->appendChild(nameValue);
		}
		root->appendChild(element);
		return element;
	}*/

	template<ServiceType ST, typename Layer>
	ServiceCapabilities<ST, Layer>::ServiceCapabilities(const std::map<string, Layer*>& availableLayers)
	{
		/*AutoPtr<Document> doc = new Document;
		AutoPtr<Element> root = doc->createElement((ST == WMS) ? "WMS_Capabilities" : "Capabilities");
		doc->appendChild(root);
		root->setAttribute("version", "1.3.0");
		root->setAttribute("xmlns", "http://www.opengis.net/wms");
		root->setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");

		AutoPtr<Element> service = doc->createElement("Service");
		{
			AddSimpleElement(doc, service, "Name", "WMS");
			AddSimpleElement(doc, service, "Title", "Druckwelle Example WMS");
		}
		root->appendChild(service);


		DOMWriter writer;
		writer.setNewLine("\n");
		writer.setOptions(XMLWriter::PRETTY_PRINT | XMLWriter::WRITE_XML_DECLARATION);
		writer.setEncoding("UTF-8", Poco::TextEncoding::byName("UTF-8"));

		std::stringstream xmlStream;
		writer.writeNode(xmlStream, doc);

		xml = xmlStream.str();*/
	}

	template<ServiceType ST, typename Layer>
	ServiceCapabilities<ST, Layer>::~ServiceCapabilities()
	{
	}

	template<>
	const string& ServiceCapabilities<WMS, WebMapService::Layer>::GetXML() const
	{
		return xml;
	}

	template<>
	const string& ServiceCapabilities<WMTS, WebMapTileService::Layer>::GetXML() const
	{
		return xml;
	}

	WMSCapabilities::WMSCapabilities(const std::map<string, WebMapService::Layer*>& availableLayers) 
		: ServiceCapabilities<WMS, WebMapService::Layer>(availableLayers)
	{
	}

	WMTSCapabilities::WMTSCapabilities(const std::map<string, WebMapTileService::Layer*>& availableLayers)
		: ServiceCapabilities<WMTS, WebMapTileService::Layer>(availableLayers)
	{
	}
}