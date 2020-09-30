#pragma once

#ifndef SERVERJOBPROXY_H
#define SERVERJOBPROXY_H

class OSServer;
class ServerJobProxy
{
	public:
		static void callServerJobRunSingleMountTest(OSServer* in_serverRef);

};

#endif
