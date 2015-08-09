
#include "../WebMapService.h"

namespace dw
{
	const string LayerName = dwTEXT("CircleTestLayer");
	const string LayerTitle = dwTEXT("Circle Rendering Test Layer");

	class CircleTestLayer : public WebMapService::Layer
	{
	public:

		virtual const char_t* GetName() const override
		{
			return LayerName.c_str();
		}

		virtual const char_t* GetTitle() const override
		{
			return LayerTitle.c_str();
		}
	};

	IMPLEMENT_WEBMAPSERVICE_LAYER(CircleTestLayer, LayerName, LayerTitle);
}