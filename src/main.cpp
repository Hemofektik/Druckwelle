
#include "WebMapService.h"

int main(int argc, const char* argv[])
{
	dw::WebMapService wms;

	// TODO: create wms config based on external settings

	int result = wms.Start();
	if (result)
	{
		return result;
	}

	wms.Stop();

	return 0;
}
