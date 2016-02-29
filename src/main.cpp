
#include "WebServer.h"
#include <memory>
#include <condition_variable>

using namespace std;

int main(int argc, const char* argv[])
{
	unique_ptr<dw::IWebServer> ws(dw::IWebServer::Create());

	int result = ws->Start();
	if (result != 0)
	{
		return result;
	}

	std::condition_variable cv;
	std::mutex cv_m;
	std::unique_lock<std::mutex> lk(cv_m);
	cv.wait(lk);

	ws->Stop();

	return 0;
}
