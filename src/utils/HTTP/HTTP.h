#pragma once

#include "../../dwcore.h"

namespace dw
{
	enum HTTPStatusCode
	{
		HTTP_OK,
		HTTP_BadRequest,

		NumHTTPStatusCodes,
	};

	struct IHTTPRequest
	{
		
		virtual ~IHTTPRequest() {};

		virtual string GetArgumentValue(const string& argument) const = 0;
		virtual void Reply(HTTPStatusCode statusCode, const string& message) = 0;
		virtual void Reply(HTTPStatusCode statusCode, const u8* data, const size dataSize, const ContentType contentType) = 0;
	};

	struct IHTTPResponse
	{
		virtual ~IHTTPResponse() {};

		virtual HTTPStatusCode GetStatusCode() const = 0;
	};

	struct IHTTPClient
	{
		virtual ~IHTTPClient() {};

		static IHTTPClient* Create(const string& baseUri);
	};
}