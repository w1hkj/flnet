//======================================================================
// cvsdb.cxx
//
// Authors:
//
// Copyright (C) 2013, Dave Freese, W1HKJ
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
//======================================================================


#ifndef CSVDB_H
#define CSVDB_H

#include <stdlib.h>

#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

using namespace std;


struct callsigns {
	char callsign[15];
	char netnbr[10];
	int nbr;
	callsigns() {
		memset(callsign, 0, 15);
		memset(netnbr, 0, 10);
		nbr = -1; }
};

struct csvRecord {
	string prefix;
	string area;
	string suffix;
	string callsign;
	string name;
	string netnbr;
	string logdate;
	string nbrlogins;
	string status;
	string joined;
	string fname;
	string lname;
	string addr;
	string city;
	string state;
	string zip;
	string phone;
	string birthdate;
	string spouse;
	string sp_birth;
	string comment1;
	string comment2;
	string email;
	string prevdate;
	string locator;
	string country;
	string county;
	string traffic;
	csvRecord() {
		prefix.clear(); area.clear(); suffix.clear(); callsign.clear();
		name.clear(); netnbr.clear(); logdate.clear(); nbrlogins.clear();
		status.clear(); joined.clear(); fname.clear(); lname.clear();
		addr.clear(); city.clear(); state.clear(); zip.clear();
		phone.clear(); birthdate.clear(); spouse.clear(); sp_birth.clear();
		comment1.clear(); comment2.clear(); email.clear(); prevdate.clear();
		locator.clear(); country.clear(); county.clear(); traffic.clear();
	}
	std::string print();

};

class csvdb {

enum FIELDS {
	PREFIX,AREA,SUFFIX,CALLSIGN,NAME,NETNBR,LOGDATE,NBRLOGINS,STATUS,JOINED,
	FNAME,LNAME,ADDR,CITY,STATE,ZIP,PHONE,BIRTHDATE,SPOUSE,SP_BIRTH,
	COMMENT1,COMMENT2,EMAIL,PREVDATE,LOCATOR,COUNTRY,COUNTY,TRAFFIC, LAST_FIELD
 };

 
public:
	callsigns *clist;
	int binary_search_callsign(callsigns *, int, int, std::string &);
	int binary_search_netnbr(callsigns *, int, int, std::string &);

private:
	static const char *csvFields;
	string dbfilename;
	vector<csvRecord> dbrecs;
	csvRecord record;
	int cur_recnbr;
	int fpos[LAST_FIELD];

	void update_clist();

public:
	csvdb() {
		dbfilename.assign("test.csv");
		cur_recnbr = 0;
		clist = 0;
	}
	~csvdb() { delete [] clist;}

	void filename(string fname) { dbfilename.assign(fname); }
	string filename() { return dbfilename; }
	int save();
	int load();
	size_t numrecs() { return dbrecs.size(); }
	int get(size_t n, csvRecord &rec);
	int put(size_t n, csvRecord &rec);
	int add(csvRecord &rec);
	int recnbr() { return cur_recnbr; }
	int find_netnbr(std::string);
	int find_callsign(std::string);
	int erase(size_t n);
	string delimit(string s);
	string trim(string s);
	void field(string &s, string &fld);
	bool split(string s, csvRecord &rec);
	void join(csvRecord &rec, string &s);
	void clearrec(csvRecord &rec);
	bool mapheader(string s);
	std::string print(csvRecord &rec);
	std::string print(int n);
};

#endif
