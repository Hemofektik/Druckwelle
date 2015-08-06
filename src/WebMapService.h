#pragma once


struct WebMapService
{
	WebMapService();

	int Start();
	void Stop();

	int HandleRequest(struct MHD_Connection* connection, const char* url, const char* method);
private:

	int HandleGetMapRequest(struct MHD_Connection *connection, struct GetMapRequest& gmr);
	
	struct MHD_Daemon* daemon;
};

