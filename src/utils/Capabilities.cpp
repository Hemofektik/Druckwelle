
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h> //typedef to Poco::AutoPtr
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>

using namespace Poco::XML;

#include "../dwcore.h"
#include "../WebMapService.h"
#include "../WebMapTileService.h"

namespace dw
{
	template<ServiceType ST, typename Layer>
	class ServiceCapabilities
	{

	public:
		ServiceCapabilities(const std::map<astring, Layer*>& availableLayers)
		{
			AutoPtr<Document> pDoc = new Document;
			AutoPtr<Element> pRoot = pDoc->createElement("root");
			pDoc->appendChild(pRoot);
			AutoPtr<Element> pChild1 = pDoc->createElement("child1");
			AutoPtr<Text> pText1 = pDoc->createTextNode("text1");
			pChild1->appendChild(pText1);
			pRoot->appendChild(pChild1);
			AutoPtr<Element> pChild2 = pDoc->createElement("child2");
			AutoPtr<Text> pText2 = pDoc->createTextNode("text2");
			pChild2->appendChild(pText2);
			pRoot->appendChild(pChild2);
			DOMWriter writer;
			writer.setNewLine("\n");
			writer.setOptions(XMLWriter::PRETTY_PRINT);

			writer.writeNode(std::cout, pDoc);
		}

		virtual ~ServiceCapabilities()
		{
		}

	};

	class WMSCapabilities : ServiceCapabilities<WMS, WebMapService::Layer>
	{
	};
	class WMTSCapabilities : ServiceCapabilities<WMTS, WebMapTileService::Layer>
	{
	};
}