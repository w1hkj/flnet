// =====================================================================
//
// xml_io.h
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
// =====================================================================

#ifndef XML_IO_H
#define XML_IO_H

#define DEFAULT_XMLRPC_IP_ADDRESS "127.0.0.1"
#define DEFAULT_XMLRPC_PORT_NO "7362"

typedef struct callsign_data {
	std::string name;
	std::string callsign;
	std::string qth;
	std::string state;
} CALLSIGN_DATA;

extern void open_xmlrpc(void);
extern void close_xmlrpc();

extern void *xmlrpc_loop(void *d);

extern void set_xmlrpc_timeout(double value);
extern void set_xmlrpc_timeout_default(void);

extern void set_xmlrpc_port_number(char *addr);
extern void set_xmlrpc_ip_address(char *addr);

extern bool fldigi_online;

extern void update_fldigi_callsign(std::string szFirstName, std::string szCallSign);
extern struct callsign_data *update_flnet_calldata(void);

#endif
