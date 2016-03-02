#pragma once

#include "../../dwcore.h"

namespace dw
{
	enum HTTPStatusCode
	{
		HTTP_OK = 200,
		HTTP_BadRequest = 400
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

		virtual size ReadBody(u8* destDataBuffer, const size dataSizeToRead) const = 0;

		virtual HTTPStatusCode GetStatusCode() const = 0;
	};

	struct IHTTPClient
	{
		virtual ~IHTTPClient() {};

		virtual std::shared_ptr<IHTTPResponse> Request(const string& arguments) = 0;

		static IHTTPClient* Create(const string& baseUri);
	};
}