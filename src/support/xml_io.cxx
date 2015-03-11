//======================================================================
// xml_io.cxx
//
// Authors:
//
// Copyright (C) 2012, Dave Freese, W1HKJ
// Copyright (C) 2014, Robert Stiles, KK5VD
//
// This file is part of FLNET.
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// xmlrpc interface to fldigi
//
// fetches current list of modem types from fldigi
// fetches current modem in use in fldigi
// sets fldigi modem-by-name when required
//
//======================================================================

#include <stdio.h>

#include <cstdlib>
#include <string>
#include <vector>
#include <queue>

#include <iostream>
#include <errno.h>

#include "XmlRpcClient.h"
#include "XmlRpcSocket.h"
#include "XmlRpcException.h"
#include "XmlRpcValue.h"
#include "XmlRpc.h"
#include "xml_io.h"
#include "threads.h"

using namespace std;
using XmlRpc::XmlRpcValue;

#define DEFAULT_XMLRPC_TIMEOUT 6.0

double xmlrpc_timeout = DEFAULT_XMLRPC_TIMEOUT;

// these are set only
static const char* fldigi_set_callsign  = "log.set_call";
static const char* fldigi_set_name      = "log.set_name";
static const char* fldigi_get_callsign  = "log.get_call";
static const char* fldigi_get_name      = "log.get_name";
static const char* fldigi_get_qth       = "log.get_qth";
static const char* fldigi_get_state     = "log.get_state";
static const char* fldigi_online_xmlrpc = "fldigi.name";

static XmlRpc::XmlRpcClient* client;

//#define XMLRPC_UPDATE_INTERVAL  250
#define XMLRPC_UPDATE_INTERVAL  500
#define XMLRPC_UPDATE_AFTER_WRITE 1000
#define XMLRPC_RETRY_INTERVAL 2000

extern int errno;

#define LOG_ERROR printf
#define LOG_DEBUG printf
#define LOG_INFO  printf

#define DEFAULT_XMLRPC_IP_ADDRESS "127.0.0.1"
#define DEFAULT_XMLRPC_PORT_NO "7362"

static std::string xmlrpc_ip_address = DEFAULT_XMLRPC_IP_ADDRESS;
static std::string xmlrpc_port_no    = DEFAULT_XMLRPC_PORT_NO;

pthread_t *xmlrpc_thread = 0;
pthread_mutex_t mutex_xmlrpc  = PTHREAD_MUTEX_INITIALIZER;

int  xmlrpc_errno     = 0;
bool xmlrpc_running   = false;
bool xmlrpc_kill_flag = false;
bool fldigi_online    = false;

bool numbers_and_dots_only(char *str, int expected_argc);
void * xmlrpc_loop(void *d);

//=====================================================================
// socket ops
//=====================================================================
int update_interval = XMLRPC_UPDATE_INTERVAL;

string xmlcall = "";

/*************************************************************
 *
 *************************************************************/
void MilliSleep(long msecs)
{
#ifndef __WIN32__
	struct timespec tv;
	tv.tv_sec = msecs / 1000;
	tv.tv_nsec = (msecs - tv.tv_sec * 1000) * 1000000L;
	nanosleep(&tv, NULL);
#else
	Sleep(msecs);
#endif

}

/*************************************************************
 *
 *************************************************************/
void set_xmlrpc_port_number(char *port)
{
	if(numbers_and_dots_only(port, 1)) {
		xmlrpc_port_no.assign(port);
		LOG_INFO("XMLRPC Port assignment:%s\n", port);
	}
}

/*************************************************************
 *
 *************************************************************/
void set_xmlrpc_ip_address(char *addr)
{
	if(numbers_and_dots_only(addr, 4)) {
		xmlrpc_ip_address.assign(addr);
		LOG_INFO("XMLRPC Address assignment:%s\n", addr);
	}
}

/*************************************************************
 *
 *************************************************************/
bool numbers_and_dots_only(char *str, int expected_argc)
{
	int f = 0, s = 0, t = 0, fo = 0;
	int argc = 0;

	if(str == (char *)0) return false;
	if(*str == (char) 0) return false;

	argc = sscanf((const char *)str, "%d.%d.%d.%d", &f, &s, &t, &fo);

	if(argc == expected_argc) return true;

	return false;
}

/*************************************************************
 *
 *************************************************************/
static inline void execute(const char* name, const XmlRpcValue& param, XmlRpcValue& result)
{
	if (client) {
		if (!client->execute(name, param, result, xmlrpc_timeout)) {
			xmlrpc_errno = errno;

			if(client->isFault())
				LOG_DEBUG("Server fault response!\n");

			throw XmlRpc::XmlRpcException(name);
		}
	}
	xmlrpc_errno = errno;
}

/*************************************************************
 *
 *************************************************************/
void set_xmlrpc_timeout(double value)
{
	pthread_mutex_lock(&mutex_xmlrpc);
	if(value < DEFAULT_XMLRPC_TIMEOUT) return;
	xmlrpc_timeout = value;
	pthread_mutex_unlock(&mutex_xmlrpc);
}

/*************************************************************
 *
 *************************************************************/
void set_xmlrpc_timeout_default(void)
{
	xmlrpc_timeout = DEFAULT_XMLRPC_TIMEOUT;
}


/*************************************************************
 *
 *************************************************************/
void send_callsign(std::string data)
{
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		XmlRpcValue call(data), res;
		execute(fldigi_set_callsign, call, res);
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
	}
	update_interval = XMLRPC_UPDATE_AFTER_WRITE;
	pthread_mutex_unlock(&mutex_xmlrpc);
}

/*************************************************************
 *
 *************************************************************/
void send_name(std::string data)
{
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		XmlRpcValue call(data), res;
		execute(fldigi_set_name, call, res);
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
	}
	update_interval = XMLRPC_UPDATE_AFTER_WRITE;
	pthread_mutex_unlock(&mutex_xmlrpc);
}

/*************************************************************
 * Returns comma delimited data.
 *************************************************************/
std::string get_callsign(void)
{
	XmlRpcValue status;
	XmlRpcValue query;
	static string response;

	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		execute(fldigi_get_callsign, query, status);
		string resp = status;
		response = resp;
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
	}
	pthread_mutex_unlock(&mutex_xmlrpc);

	return response;
}

/*************************************************************
 *
 *************************************************************/
std::string get_name(void)
{
	XmlRpcValue status;
	XmlRpcValue query;
	static string response;

	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		execute(fldigi_get_name, query, status);
		string resp = status;
		response = resp;
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
	}
	pthread_mutex_unlock(&mutex_xmlrpc);

	return response;
}

/*************************************************************
 *
 *************************************************************/
std::string get_qth(void)
{
	XmlRpcValue status;
	XmlRpcValue query;
	static string response;

	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		execute(fldigi_get_qth, query, status);
		string resp = status;
		response = resp;
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
	}
	pthread_mutex_unlock(&mutex_xmlrpc);

	return response;
}

/*************************************************************
 *
 *************************************************************/
std::string get_state(void)
{
	XmlRpcValue status;
	XmlRpcValue query;
	static string response;

	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		execute(fldigi_get_state, query, status);
		string resp = status;
		response = resp;
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
	}
	pthread_mutex_unlock(&mutex_xmlrpc);

	return response;
}

/*************************************************************
 * Return emply string if offline
 * Contains "T" if online.
 *************************************************************/
std::string fldigi_online_check(void)
{
	XmlRpcValue status;
	XmlRpcValue query;
	static string response;

	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		execute(fldigi_online_xmlrpc, query, status);
		string resp = status;
		response = resp;
	} catch (const XmlRpc::XmlRpcException& e) {
		LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
		response.clear();
	}
	pthread_mutex_unlock(&mutex_xmlrpc);

	return response;
}

/*************************************************************
 *
 *************************************************************/
void close_xmlrpc()
{
	void *vPtr = (void *)0;

	xmlrpc_kill_flag = true;
	pthread_join(*xmlrpc_thread, &vPtr);

	pthread_mutex_lock(&mutex_xmlrpc);
	delete client;
	client = NULL;
	pthread_mutex_unlock(&mutex_xmlrpc);
}

/*************************************************************
 *
 *************************************************************/
void open_xmlrpc(void)
{
	fldigi_online = false;
	xmlrpc_thread = new pthread_t;
	if (pthread_create(xmlrpc_thread, NULL, xmlrpc_loop, NULL)) {
		perror("pthread_create");
		return;
	}

	pthread_mutex_lock(&mutex_xmlrpc);

	int server_port = atoi(xmlrpc_port_no.c_str());

	client = new XmlRpc::XmlRpcClient( xmlrpc_ip_address.c_str(), server_port );

	pthread_mutex_unlock(&mutex_xmlrpc);
}


/*************************************************************
 * dos loop
 *************************************************************/
void * xmlrpc_loop(void *d)
{
	fldigi_online = false;
	std::string data = "";
	xmlrpc_running = true;
	xmlrpc_kill_flag = false;

	while(!xmlrpc_kill_flag) {

		try {
			data = fldigi_online_check();
		} catch (const XmlRpc::XmlRpcException& e) {
			LOG_ERROR("%s xmlrpc_errno = %d\n", e.getMessage().c_str(), xmlrpc_errno);
			fldigi_online = false;
		}

		MilliSleep(update_interval);

		pthread_mutex_lock(&mutex_xmlrpc);

		if(data.empty()) {
			fldigi_online = false;
		} else if(data[0] == 'F' || data[0] == 'f') {
			fldigi_online = true;
		}

		if (update_interval != XMLRPC_UPDATE_INTERVAL)
			update_interval = XMLRPC_UPDATE_INTERVAL;
		pthread_mutex_unlock(&mutex_xmlrpc);
	}
	return NULL;
}

/*************************************************************
 *
 *************************************************************/
void update_fldigi_callsign(std::string szFirstName, std::string szCallSign)
{
	if(szCallSign.empty() || !fldigi_online) return;

	send_callsign(szCallSign);
	send_name(szFirstName);
}

/*************************************************************
 *
 *************************************************************/
struct callsign_data *update_flnet_calldata(void)
{
	if(!fldigi_online) return (CALLSIGN_DATA *)0;

	CALLSIGN_DATA *data = (CALLSIGN_DATA *)0;
	data = (CALLSIGN_DATA *) new CALLSIGN_DATA;

	if(data) {
		data->name.assign(get_name());
		data->callsign.assign(get_callsign());
		data->qth.assign(get_qth());
		data->state.assign(get_state());
	}

	return data;
}
