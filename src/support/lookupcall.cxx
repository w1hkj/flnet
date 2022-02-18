// ----------------------------------------------------------------------------
// lookupcall.cxx  -- a part of flnet
//
// Copyright (C) 2006-2019
//		Dave Freese, W1HKJ
// Copyright (C) 2006-2007
//		Leigh Klotz, WA5ZNU
// Copyright (C) 2008-2009
//              Stelios Bounanos, M0GLD
//
// This file is part of flnet.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with flnet.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <config.h>

#ifdef __MINGW32__
#  include "compat.h"
#endif

#include <sys/time.h>
#include "signal.h"
#include <string>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cctype>

#include "threads.h"
#include "util.h"
#include "lookupcall.h"
#include "locator.h"
#include "xmlreader.h"
#include "network.h"
#include "netedits.h"
#include "status.h"

#define DISP_DEBUG 0

std::string qrzhost = "xmldata.qrz.com";
std::string qrzSessionKey;
std::string qrzalert;
std::string qrzerror;

std::string callsign;

struct LOOKUP {
	std::string name;
	std::string addr1;
	std::string addr2;
	std::string state;
	std::string province;
	std::string zip;
	std::string country;
	std::string born;
	std::string fname;
	std::string qth;
	std::string locator;
	std::string latd;
	std::string lond;
	std::string email;
	std::string notes;
} query;

qrz_xmlquery_t DB_XML_query = QRZXMLNONE;

enum TAG {
	QRZ_IGNORE,	QRZ_KEY,	QRZ_ALERT,	QRZ_ERROR,	QRZ_CALL,
	QRZ_FNAME,	QRZ_NAME,	QRZ_ADDR1,	QRZ_ADDR2,	QRZ_STATE,
	QRZ_ZIP,	QRZ_COUNTRY,	QRZ_LATD,	QRZ_LOND,	QRZ_GRID,
	QRZ_DOB, QRZ_EMAIL
};

pthread_t* QRZ_thread = 0;
pthread_mutex_t qrz_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t qrz_cond = PTHREAD_COND_INITIALIZER;

static void *lookup_loop(void *args);

bool parseSessionKey();
bool parse_xml();

bool getSessionKey(std::string& sessionpage);
bool QRZGetXML(std::string& xmlpage);
int  bearing(const char *, const char *);
void qra(const char *, double &, double &);
void lookup_init(void);
void QRZclose(void);
void qthappend(std::string &qth, std::string &datum);
void QRZAlert();
bool QRZLogin(std::string& sessionpage);
void QRZquery();
void parse_html(const std::string& htmlpage);
bool HAMCALLget(std::string& htmlpage);
void HAMCALLquery();

void QRZ_DETAILS_query();

//static notify_dialog *announce = 0;

void print_query(const std::string &name, const std::string &s)
{
	if (DISP_DEBUG)
		printf("%s query:\n%s\n", name.c_str(), s.c_str());
}

void print_data(const std::string &name, const std::string &s) {
	if (DISP_DEBUG)
		printf("%s data:\n%s\n", name.c_str(), s.c_str());
}

void camel_case(std::string &s)
{
	bool first_letter = true;
	for (size_t n = 0; n < s.length(); n++) {
		if (s[n] == ' ') first_letter = true;
		else if (first_letter) {
			s[n] = toupper(s[n]);
			first_letter = false;
		} else s[n] = tolower(s[n]);
	}
}

struct ENTITIES {
	std::string c; std::string str1; std::string str2;} entities[17] = {
		{" ",  "&#160;", "&nbsp;"},
		{"<",  "&#60;",  "&lt;"},
		{"<",  "&#060;", "&lt;"},
		{">",  "&#62;",  "&gt;"},
		{">",  "&#062;", "&gt;"},
		{"&",  "&#38;",  "&amp;"},
		{"&",  "&#038;", "&amp;"},
		{"\"", "&#34;",  "&quot;"},
		{"\"", "&#034;", "&quot;"},
		{"'",  "&#39;",  "&apos;"},
		{"'",  "&#039;", "&apos;"},
		{"¢",  "&#162;", "&cent;"},
		{"£",  "&#163;", "&pound;"},
		{"¥",  "&#165;", "&yen;"},
		{"€",  "&#8364;","&euro;"},
		{"©",  "&#169;", "&copy;"},
		{"®",  "&#174;", "&reg;"}
	};

void subst_chars(std::string &s)
{
	size_t p;
	for (int n = 0; n < 17; n++) {
		while ((p = s.find(entities[n].str1)) != std::string::npos)
			s.replace(p, entities[n].str1.length(), entities[n].c);
		while ((p = s.find(entities[n].str2)) != std::string::npos)
			s.replace(p, entities[n].str2.length(), entities[n].c);
	}
}

void clear_lookup()
{
	query.name.clear();
	query.addr1.clear();
	query.addr2.clear();
	query.state.clear();
	query.province.clear();
	query.zip.clear();
	query.born.clear();
	query.fname.clear();
	query.qth.clear();
	query.locator.clear();
	query.latd.clear();
	query.lond.clear();
	query.notes.clear();
	query.country.clear();
	query.email.clear();
}

// ---------------------------------------------------------------------
// compute azimute and distance between to grid locations
// ---------------------------------------------------------------------

void compute()
{
	if (!inpLocator) return;

	std::string remote = inpLocator->value();
	if (remote.empty()) {
		outAzimuth->value("");
		outDistance->value("");
		return;
	}
	char az[10], dist[20];
	az[0] = '\0';
	dist[0] = '\0';
	double distance, azimuth, lon[2], lat[2];
	if ( QRB::locator2longlat(
			&lon[0], &lat[0],
			progStatus.myLocator.c_str()) != QRB::QRB_OK ) {
		outAzimuth->value("");
		outDistance->value("");
		if (DISP_DEBUG) std::cout << "myLocator conversion failed" << std::endl;
		return;
	}
	if ( QRB::locator2longlat(
			&lon[1], &lat[1],
			remote.c_str()) != QRB::QRB_OK ) {
		outAzimuth->value("");
		outDistance->value("");
		if (DISP_DEBUG) std::cout << "remote conversion failed" << std::endl;
		return;
	}
	if ( QRB::qrb(
			lon[0], lat[0], lon[1], lat[1],
			&distance, &azimuth) != QRB::QRB_OK) {
		outAzimuth->value("");
		outDistance->value("");
		if (DISP_DEBUG) std::cout << "geoidesic conversion failed" << std::endl;
		return;
	}
	snprintf(az, sizeof(az), "%03.0f", round(azimuth));
	snprintf(dist, sizeof(dist), "%.0f %s",
			 distance,
			 (progStatus.arc_conversion == 0 ? " km" :
			 (progStatus.arc_conversion == 1 ? " nm" : " mi") ) );
	outAzimuth->value(az);
	outDistance->value(dist);
	if (DISP_DEBUG) std::cout << "Locator:  " << remote << std::endl;
	if (DISP_DEBUG) std::cout << "Azimuth:  " << az << std::endl;
	if (DISP_DEBUG) std::cout << "Distance: " << dist << std::endl;
}

void display_lookup_result(void *)
{
	if (query.fname.length() > 0) {
		camel_case(query.fname);
		std::string::size_type spacePos = query.fname.find(" ");
		//    if fname is "ABC" then display "ABC"
		// or if fname is "A BCD" then display "A BCD"
		if (spacePos == std::string::npos || (spacePos == 1)) {
			if (DISP_DEBUG) std::cout << "Name: " << query.fname << std::endl;
			inpFname->value(query.fname.c_str());
			inpFname->redraw();
		}
		// if fname is "ABC Y" then display "ABC"
		else if (spacePos > 2) {
			std::string fname;
			fname.assign(query.fname, 0, spacePos);
			if (DISP_DEBUG) std::cout << "Name: " << query.fname << std::endl;
			inpFname->value(fname.c_str());
			inpFname->redraw();
		}
		// fname must be "ABC DEF" so display "ABC DEF"
		else {
			if (DISP_DEBUG) std::cout << "Name: " << query.fname << std::endl;
			inpFname->value(query.fname.c_str());
			inpFname->redraw();
		}
	}
	if (query.name.length() > 0) {
		// only name is set; don't know first/last, so just show all
		if (DISP_DEBUG) std::cout << "Name: " << query.name << std::endl;
		inpLname->value(query.name.c_str());
		inpLname->redraw();
	}
	if (inpNickname->value()[0] == 0) {
		inpNickname->value(query.fname.c_str());
		inpNickname->redraw();
	}

		if (DISP_DEBUG) std::cout << "Qth: " << query.qth << std::endl;
	if (!query.qth.empty()) {
		inpCity->value(query.qth.c_str());
		inpCity->position (0);
		inpCity->redraw();
	}

//	inpQth->value(query.qth.c_str());
//	inpQth->position (0);

		if (DISP_DEBUG) std::cout << "State: " << query.state << std::endl;
	if (!query.state.empty()) {
		inpState->value(query.state.c_str());
		inpState->position (0);
		inpState->redraw();
	} else if (!query.province.empty()) {
		if (DISP_DEBUG) std::cout << "Prov: " << query.province << std::endl;
		inpState->value(query.province.c_str());
		inpState->position (0);
		inpState->redraw();
	}

	std::string comment1 = inpComment1->value();

	if (!query.country.empty()) {
		if (DISP_DEBUG) std::cout << "Country: " << query.country << std::endl;
		inpCountry->value (query.country.c_str());
	}

	if (!query.addr1.empty()) {
		if (DISP_DEBUG) std::cout << "Address-1: " << query.addr1 << std::endl;
		inpAddress->value (query.addr1.c_str());
		inpAddress->redraw();
	}
	if (!query.addr2.empty()) {
		if (DISP_DEBUG) std::cout << "Address-2: " << query.addr2 << std::endl;
		inpCity->value (query.addr2.c_str());
		inpCity->redraw();
	}
	if (!query.zip.empty()) {
		if (DISP_DEBUG) std::cout << "Zip: " << query.zip << std::endl;
		inpZip->value (query.zip.c_str());
		inpZip->redraw();
	}
	if (inpBirthday->value()[0] == 0) {
		if (DISP_DEBUG) std::cout << "Born: " << query.born << std::endl;
		inpBirthday->value(query.born.c_str());
		inpBirthday->redraw();
	}
	if (inpEmail->value()[0] == 0) {
		if (DISP_DEBUG) std::cout << "Email: " << query.email << std::endl;
		inpEmail->value(query.email.c_str());
		inpEmail->redraw();
	}

	if (!query.locator.empty()) {

		if (inpLocator->value()[0] == 0) {
//			if (DISP_DEBUG) std::cout << "Locator: " << query.locator << std::endl;
			inpLocator->value(query.locator.c_str());
			inpLocator->redraw();
		}

		if (!progStatus.myLocator.empty())
			compute();

		inpComment1->value(comment1.c_str());
	}

	std::string comment2 = inpComment2->value();
	if (!query.notes.empty()) {

		if (DISP_DEBUG) std::cout << "Notes: " << query.notes << std::endl;
		if (!comment2.empty()) comment2.append("; ");
		comment2.append(query.notes);
	}
	inpComment2->value(comment2.c_str());

	inpComment1->redraw();
	inpComment2->redraw();
	Fl::flush();
}

// ----------------------------------------------------------------------------
// QRZ subscription query
// ----------------------------------------------------------------------------

bool parseSessionKey(const std::string& sessionpage)
{
	if (sessionpage.find("Bad Request") != std::string::npos) {
		return false;
	}

	IrrXMLReader* xml = createIrrXMLReader(new IIrrXMLStringReader(sessionpage));
	TAG tag=QRZ_IGNORE;
	while(xml && xml->read()) {
		switch(xml->getNodeType())
		{
		case EXN_TEXT:
		case EXN_CDATA:
			switch (tag)
			{
			default:
				break;
			case QRZ_KEY:
				qrzSessionKey = xml->getNodeData();
				break;
			case QRZ_ALERT:
				qrzalert = xml->getNodeData();
				break;
			case QRZ_ERROR:
				qrzerror = xml->getNodeData();
				break;
			}
			break;

		case EXN_ELEMENT_END:
			tag=QRZ_IGNORE;
			break;

		case EXN_ELEMENT:
		{
			const char *nodeName = xml->getNodeName();
			if (!strcmp("Key", nodeName)) tag=QRZ_KEY;
			else if (!strcmp("Alert", nodeName)) tag=QRZ_ALERT;
			else if (!strcmp("Error", nodeName)) tag=QRZ_ERROR;
			else tag=QRZ_IGNORE;
			break;
		}

		case EXN_NONE:
		case EXN_COMMENT:
		case EXN_UNKNOWN:
			break;
		}
	}
	delete xml;
	return true;
}


bool parse_xml(const std::string& xmlpage)
{
	print_data("xmldata.qrz.com", xmlpage);

	if (xmlpage.find("<Error>Not found") != std::string::npos) {
//		if (!announce) announce = new notify_dialog;
//		announce->notify("Not found", 2.0);
//		show_notifier, announce();
		return false;
	}

	IrrXMLReader* xml = createIrrXMLReader(new IIrrXMLStringReader(xmlpage));

// If we got any result back, clear the session key so that it will be
// refreshed by this response, or if not present, will be removed and we'll
// know to log in next time.
	if (xml) {
		qrzSessionKey.clear();
		qrzalert.clear();
		qrzerror.clear();
		clear_lookup();
	}

	TAG tag = QRZ_IGNORE;

// parse the file until end reached
	while(xml && xml->read()) {
		switch(xml->getNodeType()) {
			case EXN_TEXT:
			case EXN_CDATA:
				switch (tag) {
					default:
					case QRZ_IGNORE:
						break;
					case QRZ_CALL:
						break;
					case QRZ_FNAME:
						query.fname =  xml->getNodeData();
						break;
					case QRZ_NAME:
						query.name =  xml->getNodeData();
						break;
					case QRZ_ADDR1:
						{
						query.addr1 =  xml->getNodeData();
						size_t apt = query.addr1.find("#");
						if (apt != std::string::npos)
							query.addr1.erase(apt, query.addr1.length() - apt);
						break;
						}
					case QRZ_ADDR2:
						query.addr2 =  xml->getNodeData();
						break;
					case QRZ_STATE:
						query.state =  xml->getNodeData();
						break;
					case QRZ_ZIP:
						query.zip =  xml->getNodeData();
						break;
					case QRZ_COUNTRY:
						query.country =  xml->getNodeData();
						break;
					case QRZ_LATD:
						query.latd =  xml->getNodeData();
						break;
					case QRZ_LOND:
						query.lond =  xml->getNodeData();
						break;
					case QRZ_GRID:
						query.locator =  xml->getNodeData();
						break;
					case QRZ_EMAIL:
						query.email = xml->getNodeData();
						break;
					case QRZ_DOB:
						query.born = xml->getNodeData();
						break;
					case QRZ_ALERT:
						qrzalert = xml->getNodeData();
						break;
					case QRZ_ERROR:
						qrzerror = xml->getNodeData();
						break;
					case QRZ_KEY:
						qrzSessionKey = xml->getNodeData();
						break;
				}
				break;

			case EXN_ELEMENT_END:
				tag=QRZ_IGNORE;
				break;

			case EXN_ELEMENT:
				{
				const char *nodeName = xml->getNodeName();
				if (!strcmp("call", nodeName)) 			tag = QRZ_CALL;
				else if (!strcmp("fname", nodeName)) 	tag = QRZ_FNAME;
				else if (!strcmp("name", nodeName)) 	tag = QRZ_NAME;
				else if (!strcmp("addr1", nodeName)) 	tag = QRZ_ADDR1;
				else if (!strcmp("addr2", nodeName)) 	tag = QRZ_ADDR2;
				else if (!strcmp("state", nodeName)) 	tag = QRZ_STATE;
				else if (!strcmp("zip", nodeName)) 		tag = QRZ_ZIP;
				else if (!strcmp("country", nodeName))	tag = QRZ_COUNTRY;
				else if (!strcmp("lat", nodeName)) 		tag = QRZ_LATD;
				else if (!strcmp("lon", nodeName)) 		tag = QRZ_LOND;
				else if (!strcmp("grid", nodeName)) 	tag = QRZ_GRID;
				else if (!strcmp("dob", nodeName)) 		tag = QRZ_DOB;
				else if (!strcmp("email", nodeName))	tag = QRZ_EMAIL;
				else if (!strcmp("born", nodeName))		tag = QRZ_DOB;
				else if (!strcmp("Alert", nodeName)) 	tag = QRZ_ALERT;
				else if (!strcmp("Error", nodeName)) 	tag = QRZ_ERROR;
				else if (!strcmp("Key", nodeName)) 		tag = QRZ_KEY;
				else tag = QRZ_IGNORE;
				}
				break;

			case EXN_NONE:
			case EXN_COMMENT:
			case EXN_UNKNOWN:
				break;
		}
	}

// delete the xml parser after usage
	delete xml;
	return true;
}

bool getSessionKey(std::string& sessionpage)
{
	std::string html = "http://";
	html.append(qrzhost);
	html.append(" /xml/current/?username=");
	html.append(progStatus.user_name);
	html.append(";password=");
	html.append(progStatus.user_password);
	html.append(";agent=");
	html.append(PACKAGE_NAME);
	html.append("-");
	html.append(PACKAGE_VERSION);
	return get_http(html, sessionpage, 5.0);
}

bool QRZGetXML(std::string& xmlpage)
{
	std::string html;
	html.assign("http://").append(qrzhost);
	html.append(" /bin/xml?s=");
	html.append(qrzSessionKey);
	html.append(";callsign=");
	html.append(callsign);
	return get_http(html, xmlpage, 5.0);
}

void lookup_init(void)
{
	if (QRZ_thread)
		return;
	QRZ_thread = new pthread_t;
	if (pthread_create(QRZ_thread, NULL, lookup_loop, NULL) != 0) {
		printf("pthread_create\n");
		return;
	}
	MilliSleep(10);
}

void QRZclose(void)
{
	if (!QRZ_thread)
		return;

	DB_XML_query = QRZXML_EXIT;

	pthread_mutex_lock(&qrz_mutex);
	pthread_cond_signal(&qrz_cond);
	pthread_mutex_unlock(&qrz_mutex);

	pthread_join(*QRZ_thread, NULL);
	delete QRZ_thread;
	QRZ_thread = 0;
}

void qthappend(std::string &qth, std::string &datum) {
	if (datum.empty()) return;
	if (!qth.empty()) qth += ", ";
	qth += datum;
}

void QRZAlert()
{

	std::string qrznote;
	if (!qrzalert.empty()) {
		qrznote.append("QRZ alert:\n");
		qrznote.append(qrzalert);
		qrznote.append("\n");
		qrzalert.clear();
	}
	if (!qrzerror.empty()) {
		qrznote.append("QRZ error:\n");
		qrznote.append(qrzerror);
		qrzerror.clear();
	}
	std::string notes;
	notes.clear();

	if (!qrznote.empty()) {
		if (!notes.empty()) notes.append("\n");
		notes.append(qrznote);
//		inpNotes->value(notes.c_str());
	}
}

bool QRZLogin(std::string& sessionpage)
{
	bool ok = true;
	if (qrzSessionKey.empty()) {
		ok = getSessionKey(sessionpage);
		if (ok) ok = parseSessionKey(sessionpage);
	}
	if (!ok) {
		printf("QRZ login failed\n");
//		QRZAlert();
	}

	return ok;
}

void QRZquery()
{
	bool ok = true;

	std::string qrzpage;

	if (qrzSessionKey.empty())
		ok = QRZLogin(qrzpage);
	if (ok)
		ok = QRZGetXML(qrzpage);
	if (ok) {
		subst_chars(qrzpage);
		parse_xml(qrzpage);
		if (qrzalert.empty() && qrzerror.empty()) {
			query.qth = query.addr2;
			if (query.country.find("Canada") != std::string::npos) {
				query.province = query.state;
				query.state.clear();
			}
			Fl::awake(display_lookup_result);
		}
	}
	else {
		qrzerror = qrzpage;
//		QRZAlert();
	}
}

// ------------------------------ END QRZ.COM --------------------------
/*
<?xml version="1.0" encoding="UTF-8"?>
<callook>
	<status>VALID</status>
	<type>PERSON</type>
	<current>
		<callsign>AE5ZA</callsign>
		<class>EXTRA</class>
	</current>
	<previous>
		<callsign>KE6KZJ</callsign>
		<class>GENERAL</class>
	</previous>
	<trustee>
		<callsign></callsign>
		<name></name>
	</trustee>
	<name>CLIFFORD K SCOTT</name>
	<address>
		<line1>2439 COUNTY ROAD 1240</line1>
		<line2>PITTSBURG, TX 75686</line2>
		<attn></attn>
	</address>
	<location>
		<latitude>32.92178</latitude>
		<longitude>-94.9185</longitude>
		<gridsquare>EM22mw</grodsquare>
	</location>
	<otherinfo>
		<grantdate>04/24/2014</grantdate>
		<expirydate>07/22/2024</expirydate>
		<lastactiondate>04/24/2014</lastactiondate>
		<frn>0003840618</frn>
		<ulsurl>http://wireless2.fcc.gov/UlsApp/UlsSearch/license.jsp?licKey=3305874</ulsurl>
	</otherinfo>
</callook>
*/

// ---------------------------------------------------------------------
// HTTP:://callook.info queries
// ---------------------------------------------------------------------

std::string node_data(const std::string &xmlpage, const std::string nodename)
{
	size_t pos1, pos2;
	std::string test;
	test.assign("<").append(nodename).append(">");
	pos1 = xmlpage.find(test);
	if (pos1 == std::string::npos) return "";
	pos1 += test.length();
	test.assign("</").append(nodename).append(">");
	pos2 = xmlpage.find(test);
	if (pos2 == std::string::npos) return "";
	return xmlpage.substr(pos1, pos2 - pos1);
}

void parse_callook(std::string& xmlpage)
{
	print_data("Callook info", xmlpage);

	if (xmlpage.find("INVALID") != std::string::npos) {
//		if (!announce) announce = new notify_dialog;
//		announce->notify("Call not found", 2.0);
//		show_notifier(announce);
		return;
	}

	std::string nodestr = node_data(xmlpage, "current");

	if (nodestr.empty()) {
//		if (!announce) announce = new notify_dialog;
//		announce->notify("no data from callook.info", 2.0);
//		show_notifier(announce);
		return;
	}

	size_t start_pos = xmlpage.find("</trustee>");
	if (start_pos == std::string::npos) return;

	start_pos += 10;
	xmlpage = xmlpage.substr(start_pos);
	query.name = node_data(xmlpage, "name");
	camel_case(query.name);
	size_t p = query.name.find(" ");
	if (p != std::string::npos) {
		query.fname = query.name.substr(0, p);
		query.name.erase(0, p+1);
	}

	nodestr = node_data(xmlpage, "address");
	if (!nodestr.empty()) {
		query.addr1 = node_data(nodestr, "line1");
		query.addr2 = node_data(nodestr, "line2");
	}

	nodestr = node_data(xmlpage, "location");
	if (!nodestr.empty()) {
		query.lond = node_data(nodestr, "longitude");
		query.latd = node_data(nodestr, "latitude");
		query.locator = node_data(nodestr, "gridsquare");
	}

	p = query.addr2.find(",");
	if (p != std::string::npos) {
		query.qth = query.addr2.substr(0, p);
		camel_case(query.qth);
		query.addr2.erase(0, p+2);
		p = query.addr2.find(" ");
		if (p != std::string::npos) {
			query.state = query.addr2.substr(0, p);
			query.addr2.erase(0, p+1);
			if (!query.addr2.empty()) query.zip = query.addr2;
		}
		query.addr2 = query.qth;
	}
	query.country = "United States";
}

bool CALLOOKGetXML(std::string& xmlpage)
{
	std::string url = progStatus.callookurl;
	size_t p = 0;
	if ((p = url.find("https")) != std::string::npos)
		url.erase(p+4,1);
	if ((p = url.find("www.")) != std::string::npos)
		url.erase(p, 4);
	url.append(callsign).append("/xml");
	bool res = get_http(url, xmlpage, 5.0);
//	bool res = get_http_debug(url, xmlpage, 5.0);
	return res;
}

void CALLOOKquery()
{
	bool ok = true;

	std::string CALLOOKpage;

	clear_lookup();
	ok = CALLOOKGetXML(CALLOOKpage);
	if (ok) {
		subst_chars(CALLOOKpage);
		parse_callook(CALLOOKpage);
		Fl::awake(display_lookup_result);
	}
}

//------------------------ END CALLOOK ---------------------------------

// ---------------------------------------------------------------------
// Hamcall specific functions
// ---------------------------------------------------------------------

#define HAMCALL_CALL   181
#define HAMCALL_FIRST  184
#define HAMCALL_CITY   191
#define HAMCALL_STATE  192
#define HAMCALL_LOCATOR 202
#define HAMCALL_DOB    194

void parse_html(const std::string& htmlpage)
{
	print_data("Hamcall data", htmlpage);

	size_t p;

	clear_lookup();

	if ((p = htmlpage.find(HAMCALL_FIRST)) != std::string::npos) {
		p++;
		while ((unsigned char)htmlpage[p] < 128 && p < htmlpage.length() )
			query.fname += htmlpage[p++];
		camel_case(query.fname);
	}
	if ((p = htmlpage.find(HAMCALL_CITY)) != std::string::npos) {
		p++;
		while ((unsigned char)htmlpage[p] < 128 && p < htmlpage.length())
			query.qth += htmlpage[p++];
	}
	if ((p = htmlpage.find(HAMCALL_STATE)) != std::string::npos) {
		p++;
		while ((unsigned char)htmlpage[p] < 128 && p < htmlpage.length())
			query.state += htmlpage[p++];
	}
	if ((p = htmlpage.find(HAMCALL_LOCATOR)) != std::string::npos) {
		p++;
		while ((unsigned char)htmlpage[p] < 128 && p < htmlpage.length())
			query.locator += htmlpage[p++];
	}
	if ((p = htmlpage.find(HAMCALL_DOB)) != std::string::npos) {
		p++;
		query.notes = "DOB: ";
		while ((unsigned char)htmlpage[p] < 128 && p < htmlpage.length())
			query.notes += htmlpage[p++];
	}
}

bool HAMCALLget(std::string& htmlpage)
{
        std::string url = progStatus.hamcallurl;
        size_t p = url.find("//");
        std::string service = url.substr(0, p);
        url.erase(0, p+2);
        size_t len = url.length();
        if (url[len-1]!='/') url.append("/");
        url.append("call?username=");
        url.append(progStatus.user_name);
        url.append("&password=");
        url.append(progStatus.user_password);
        url.append("&rawlookup=1&callsign=");
        url.append(callsign);
        url.append("&program=flnet-");
        url.append(VERSION);
        return get_http(url, htmlpage, 5.0);
}

void HAMCALLquery()
{
	std::string htmlpage;

	if (HAMCALLget(htmlpage)) {
		subst_chars(htmlpage);
		parse_html(htmlpage);
	} else
		query.notes = htmlpage;
	Fl::awake(display_lookup_result);
}

// ----------------------- END HAMCALL ---------------------------------

// ---------------------------------------------------------------------
// hamqth specific functions
// ---------------------------------------------------------------------

static std::string HAMQTH_session_id = "";
static std::string HAMQTH_reply = "";

#define HAMQTH_DEBUG 1
#undef HAMQTH_DEBUG
/*
 * send:
     https://www.hamqth.com/xml.php?u=username&p=password
 * response:
     <?xml version="1.0"?>
     <HamQTH version="2.7" xmlns="https://www.hamqth.com">
     <session>
     <session_id>09b0ae90050be03c452ad235a1f2915ad684393c</session_id>
     </session>
     </HamQTH>
 *
 * send:
     https://www.hamqth.com/xml.php?id=09b0ae90050be03c452ad235a1f2915ad684393c\
             &callsign=ok7an&prg=YOUR_PROGRAM_NAME
 * response:
	<?xml version="1.0"?>
	<HamQTH version="2.7" xmlns="https://www.hamqth.com">
	<search>
	<callsign>ok7an</callsign>
	<nick>Petr</nick>
	<qth>Neratovice</qth>
	<country>Czech Republic</country>
	<adif>503</adif>
	<itu>28</itu>
	<cq>15</cq>
	<locator>jo70gg</locator>
	<adr_name>Petr Hlozek</adr_name>
	<adr_street1>17. listopadu 1065</adr_street1>
	<adr_city>Neratovice</adr_city>
	<adr_zip>27711</adr_zip>
	<adr_country>Czech Republic</adr_country>
	<adr_adif>503</adr_adif>
	<district>GZL</district>
	<lotw>Y</lotw>
	<qsl>Y</qsl>
	<qsldirect>Y</qsldirect>
	<eqsl>Y</eqsl>
	<email>petr@ok7an.com</email>
	<jabber>petr@ok7an.com</jabber>
	<skype>PetrHH</skype>
	<birth_year>1982</birth_year>
	<lic_year>1998</lic_year>
	<web>https://www.ok7an.com</web>
	<latitude>50.07</latitude>
	<longitude>14.42</longitude>
	<continent>EU</continent>
	<utc_offset>-1</utc_offset>
	<picture>https://www.hamqth.com/userfiles/o/ok/ok7an/_profile/ok7an_nove.jpg</picture>
	</search>
	</HamQTH>
*/
bool HAMQTH_get_session_id()
{
	std::string url;
	std::string retstr = "";
	size_t p1 = std::string::npos;
	size_t p2 = std::string::npos;

	url.assign(progStatus.hamqthurl);
	if ((p1 = url.find("https")) != std::string::npos)
		url.erase(p1+4,1);
	if (url[url.length()-1] != '/') url += '/';
	url.append("xml.php?u=").append(progStatus.user_name);
	url.append("&p=").append(progStatus.user_password);

	HAMQTH_session_id.clear();

	int ret = get_http(url, retstr, 5.0);

	if (ret == 0 ) {
		printf("get_http( %s, retstr, 5.0) failed\n", url.c_str());
		return false;
	}

	p1 = retstr.find("<error>");
	if (p1 != std::string::npos) {
		p2 = retstr.find("</error>");
		if (p2 != std::string::npos) {
			p1 += 7;
			query.notes = retstr.substr(p1, p2 - p1);
		}
		return false;
	}
	p1 = retstr.find("<session_id>");
	if (p1 == std::string::npos) {
		query.notes = "HamQTH not available";
		return false;
	}
	p2 = retstr.find("</session_id>");
	HAMQTH_session_id = retstr.substr(p1 + 12, p2 - p1 - 12);
	print_data("HamQTH session id", HAMQTH_session_id);
	return true;
}

void parse_HAMQTH_html(const std::string& htmlpage)
{
	print_data("HamQth html", htmlpage);

	size_t p = std::string::npos;
	size_t p1 = std::string::npos;
	std::string tempstr;

	clear_lookup();

	if ((p = htmlpage.find("<error>")) != std::string::npos) {
		p += 7;
		p1 = htmlpage.find("</error>", p);
//		if (p1 != std::string::npos) {
//			std::string errstr = htmlpage.substr(p, p1 - p);
//		}
		return;
	}
	if ((p = htmlpage.find("<nick>")) != std::string::npos) {
		p += 6;
		p1 = htmlpage.find("</nick>", p);
		if (p1 != std::string::npos) {
			query.fname = htmlpage.substr(p, p1 - p);
			camel_case(query.fname);
		}
	}
	if ((p = htmlpage.find("<adr_name>")) != std::string::npos) {
		p += strlen("<adr_name>");
		p1 = htmlpage.find("</adr_name>");
		if (p1 != std::string::npos) {
			query.name = htmlpage.substr(p, p1 - p);
			p = query.name.find(" ");
			if (p != std::string::npos) {
				if (query.fname.empty())
					query.fname = query.name.substr(0, p);
				query.name.erase(0, p+1);
			}
		}
	}

	if ((p = htmlpage.find("<adr_street1>")) != std::string::npos) {
		p += strlen("<adr_street1>");
		p1 = htmlpage.find("</adr_street1>", p);
		if (p1 != std::string::npos) {
			query.addr1 = htmlpage.substr(p, p1 - p);
			camel_case(query.addr1);
		}
	}

	if ((p = htmlpage.find("<adr_city>")) != std::string::npos) {
		p += strlen("<adr_city>");
		p1 = htmlpage.find("</adr_city>", p);
		if (p1 != std::string::npos) {
			query.addr2 = htmlpage.substr(p, p1 - p);
			camel_case(query.addr2);
		}
	}

	if ((p = htmlpage.find("<qth>")) != std::string::npos) {
		p += 5;
		p1 = htmlpage.find("</qth>", p);
		if (p1 != std::string::npos)
			query.qth = htmlpage.substr(p, p1 - p);
	}

	if ((p = htmlpage.find("<adr_zip>")) != std::string::npos) {
		p += 9;
		p1 = htmlpage.find("</adr_zip>", p);
		if (p1 != std::string::npos)
			query.zip = htmlpage.substr(p, p1 - p);
	}

	if ((p = htmlpage.find("<adr_country>")) != std::string::npos) {
		p += 13;
		p1 = htmlpage.find("</adr_country>", p);
		if (p1 != std::string::npos)
			query.country = htmlpage.substr(p, p1 - p);
	}

	if ((p = htmlpage.find("<us_state>")) != std::string::npos) {
		p += 10;
		p1 = htmlpage.find("</us_state>", p);
		if (p1 != std::string::npos)
			query.state = htmlpage.substr(p, p1 - p);
	}

	if ((p = htmlpage.find("<grid>")) != std::string::npos) {
		p += 6;
		p1 = htmlpage.find("</grid>", p);
		if (p1 != std::string::npos)
			query.locator = htmlpage.substr(p, p1 - p);
	}

	if ((p = htmlpage.find("<email>")) != std::string::npos) {
		p += 7;
		p1 = htmlpage.find("</email>", p);
		if (p1 != std::string::npos)
			query.email = htmlpage.substr(p, p1 - p);
	}

	std::string szlat = "";
	std::string szlon = "";
	if ((p = htmlpage.find("<latitude>")) != std::string::npos) {
		p += 10;
		p1 = htmlpage.find("</latitude>", p);
		if (p1 != std::string::npos)
			szlat = htmlpage.substr(p, p1 - p);
	}
	if ((p = htmlpage.find("<longitude>")) != std::string::npos) {
		p += 11;
		p1 = htmlpage.find("</longitude>", p);
		if (p1 != std::string::npos)
			szlon = htmlpage.substr(p, p1 - p);
	}
	if (!szlat.empty() && !szlon.empty()) {
		double lat = atof(szlat.c_str());
		double lon = atof(szlon.c_str());
		char qthloc[10];
		QRB::longlat2locator(lon,lat,qthloc,3);
		qthloc[4] = tolower(qthloc[4]);
		qthloc[5] = tolower(qthloc[5]);
		query.locator = qthloc;
	}
}

bool HAMQTHget(std::string& htmlpage)
{
	std::string url;
	bool ret;

	if (HAMQTH_session_id.empty()) {
		if (!HAMQTH_get_session_id()) {
			printf("HAMQTH session id failed!\n");
			query.notes = "Get session id failed!\n";
			return false;
		}
	}

	size_t p1;
	url.assign(progStatus.hamqthurl);
	if ((p1 = url.find("https")) != std::string::npos)
		url.erase(p1+4,1);
	if (url[url.length()-1] != '/') url += '/';
	url.append("xml.php?id=").append(HAMQTH_session_id);
	url.append("&callsign=").append(callsign);
	url.append("&prg=FLDIGI");

	print_query("HamQTH", url);

	ret = get_http(url, htmlpage, 5.0);

	size_t p = htmlpage.find("Session does not exist or expired");
	if (p != std::string::npos) {
		htmlpage.clear();
		printf("HAMQTH session id expired!\n");
		HAMQTH_session_id.clear();
		if (!HAMQTH_get_session_id()) {
			printf("HAMQTH get session id failed!\n");
			query.notes = "Get session id failed!\n";
			htmlpage.clear();
			return false;
		}
		ret = get_http(url, htmlpage, 5.0);
	}
#ifdef HAMQTH_DEBUG
	FILE *fetchit = fopen("fetchit.txt", "a");
	fprintf(fetchit, "%s\n", htmlpage.c_str());
	fclose(fetchit);
#endif
	return ret;
}

void HAMQTHquery()
{
	std::string htmlpage;

	if (!HAMQTHget(htmlpage)) return;

	subst_chars(htmlpage);
	parse_HAMQTH_html(htmlpage);

	Fl::awake(display_lookup_result);

}

// ----------------------------------------------------------------------------

static void *lookup_loop(void *args)
{
	for (;;) {
		pthread_mutex_lock(&qrz_mutex);
		pthread_cond_wait(&qrz_cond, &qrz_mutex);
		pthread_mutex_unlock(&qrz_mutex);

		switch (DB_XML_query) {
		case QRZNET :
			QRZquery();
			break;
		case HAMCALLNET :
			HAMCALLquery();
			break;
		case CALLOOK:
			CALLOOKquery();
			break;
		case HAMQTH:
			HAMQTHquery();
			break;
		case QRZXML_EXIT:
			return NULL;
		default:
			break;
		}
	}

	return NULL;
}

void CALLSIGNquery(std::string inpCall)
{
	if (!QRZ_thread)
		lookup_init();

	// Filter callsign for nonsense characters (remove all but [A-Za-z0-9/])
	callsign = inpCall;

	size_t slash;
	while ((slash = callsign.rfind('/')) != std::string::npos) {
		if (((slash+1) * 2) < callsign.length())
			callsign.erase(0, slash + 1);
		else
			callsign.erase(slash);
	}

	switch (DB_XML_query = static_cast<qrz_xmlquery_t>(progStatus.QRZXML)) {
	case QRZNET:
//		printf("%s","Request sent to qrz.com...\n");
		break;
	case HAMCALLNET:
//		printf("%s","Request sent to www.hamcall.net...\n");
		break;
	case CALLOOK:
//		printf("Request sent to %s\n", progStatus.callookurl.c_str());
		break;
	case HAMQTH:
//		printf("Request sent to %s\n", progStatus.hamqthurl.c_str());
		break;
	case QRZXMLNONE:
		break;
	default:
//		printf("Bad query type %d\n", DB_XML_query);
		return;
	}

	pthread_mutex_lock(&qrz_mutex);
	pthread_cond_signal(&qrz_cond);
	pthread_mutex_unlock(&qrz_mutex);
}

