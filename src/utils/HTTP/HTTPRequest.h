#pragma once

#include "../../dwcore.h"

namespace dw
{
	struct IHTTPRequest
	{
		enum StatusCode
		{
			StatusCode_OK,
			StatusCode_BadRequest,
			
			NumStatusCodes,
		};

		virtual ~IHTTPRequest() {};

		virtual string GetArgumentValue(const string& argument) const = 0;
		virtual void Reply(StatusCode statusCode, const string& message) = 0;
		virtual void Reply(StatusCode statusCode, const u8* data, const size dataSize, const ContentType contentType) = 0;
	};
}