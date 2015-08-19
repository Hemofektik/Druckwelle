#pragma once

#include "dwcore.h"

#include <string>
#include <vector>
#include <map>

namespace dw
{
	class WebServer
	{
	public:

		WebServer();

		int Start();
		void Stop();

		int HandleRequest(struct MHD_Connection* connection, const char* url, const char* method);

	private:
		struct MHD_Daemon* daemon;
		class WebMapService* wms;
		class WebMapTileService* wmts;
	};
}
