#pragma once

#include "dwcore.h"

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
