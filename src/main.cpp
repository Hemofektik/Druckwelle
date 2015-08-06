
#include "WebMapService.h"

int main(int argc, const char* argv[])
{
	WebMapService wms;

	int result = wms.Start();
	if (result)
	{
		return result;
	}

	wms.Stop();

	return 0;
}
