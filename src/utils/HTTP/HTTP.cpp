
#include "HTTP.h"

#include <cpprest/http_client.h>

using namespace utility;
using namespace web::http::client;

namespace dw
{

struct HTTPResponse : public IHTTPResponse
{
	virtual ~HTTPResponse() override
	{

	};

	virtual HTTPStatusCode GetStatusCode() const override
	{

	}
};

struct HTTPClient : public IHTTPClient
{
	HTTPClient(const string& baseUri)
		: client(string_t(baseUri.begin(), baseUri.end()))
	{

	}

	virtual ~HTTPClient() override {};

private:
	http_client client;
};


IHTTPClient* IHTTPClient::Create(const string& baseUri)
{
	return new HTTPClient(baseUri);
}

}