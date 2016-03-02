
#include "HTTP.h"

#include <cpprest/http_client.h>

using namespace std;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

namespace dw
{

struct HTTPResponse : public IHTTPResponse
{
	HTTPResponse(http_response&& response)
		: response(response)
	{
	}

	virtual ~HTTPResponse() override {};

	virtual size ReadBody(u8* destDataBuffer, const size dataSizeToRead) const override
	{
		return response.body().streambuf().getn(destDataBuffer, dataSizeToRead).get();
	}

	virtual HTTPStatusCode GetStatusCode() const override
	{
		return (HTTPStatusCode)response.status_code();
	}

private:
	http_response response;
};

struct HTTPClient : public IHTTPClient
{
	HTTPClient(const string& baseUri)
		: client(string_t(baseUri.begin(), baseUri.end()))
	{
	}

	virtual ~HTTPClient() override {};

	virtual shared_ptr<IHTTPResponse> Request(const string& arguments) override
	{
		return shared_ptr<IHTTPResponse>(new HTTPResponse(move(client.request(methods::GET, string_t(arguments.begin(), arguments.end())).get())));
	}

private:
	http_client client;
};


IHTTPClient* IHTTPClient::Create(const string& baseUri)
{
	return new HTTPClient(baseUri);
}

}