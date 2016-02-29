#pragma once

#include "dwcore.h"

#include <string>
#include <vector>
#include <map>

namespace dw
{
	struct IWebServer
	{
		virtual int Start() = 0;
		virtual void Stop() = 0;

		virtual ~IWebServer() {};

		static IWebServer* Create();
	};
}
