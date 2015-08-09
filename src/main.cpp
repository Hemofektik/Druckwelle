
#include "WebMapService.h"
#include <condition_variable>

int main(int argc, const char* argv[])
{
	dw::WebMapService wms;

	int result = wms.Start();
	if (result)
	{
		return result;
	}


	std::condition_variable cv; 
	std::mutex cv_m;
	std::unique_lock<std::mutex> lk(cv_m);
	cv.wait(lk);

	wms.Stop();

	return 0;
}
