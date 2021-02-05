// =====================================================================
//
// netshared.cxx
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


#include <string>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/filename.H>

#include "netshared.h"
#include "my_UI.h"
#include "loglist.h"
#include "lookupcall.h"
#include "netsupport.h"
#include "config.h"
#include "net_config.h"
#include "xml_io.h"
#include "csvdb.h"
#include "status.h"
#include "icons.h"
#include "masterdb.h"

csvdb *masterdb = (csvdb *)0;

csvRecord master_rec;

SEARCH_REC *master_recs = (SEARCH_REC *)0;

// l = left element in search interval
// r = right element in search interval
// p = search prefix
// a = search area
// s = search suffix
int  binary_search_masterdb(int l, int r, std::string &p, std::string &a, std::string &s) 
{ 
	std::string p2, a2, s2;
	if (r >= l) { 
		int mid = l + (r - l) / 2; 
		p2 = master_recs[mid].prefix;
		a2 = master_recs[mid].area;
		s2 = master_recs[mid].suffix;
		if (s2 > s) return binary_search_masterdb(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_masterdb(mid + 1, r, p, a, s);
		if (a2 > a) return binary_search_masterdb(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_masterdb(mid + 1, r, p, a, s);
		if (p2 > p) return binary_search_masterdb(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_masterdb(mid + 1, r, p, a, s);
		return mid;
	} 
	return -1; 
} 

int mrec_compare (const void *p1, const void *p2)
{
	SEARCH_REC *s1 = (SEARCH_REC *)p1;
	SEARCH_REC *s2 = (SEARCH_REC *)p2;
	int cmp;
	if ((cmp = strcmp(s1->suffix, s2->suffix)) != 0) return cmp;
	if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
	return strcmp(s1->prefix, s2->prefix);
}

void sort_master_recs()
{
	if (!master_recs || !masterdb->numrecs()) return;
	qsort ( &(master_recs[0]), masterdb->numrecs(), sizeof(SEARCH_REC), mrec_compare);
}

void load_master_recs()
{
	csvRecord rec;
	if (master_recs)
		delete [] master_recs;
	master_recs = new SEARCH_REC[masterdb->numrecs()];
	for (size_t n = 0; n < masterdb->numrecs(); n++) {
		masterdb->get(n, rec);
		memset(master_recs[n].area, 0, sizeof(master_recs[n].area));
		strncpy(master_recs[n].area, rec.area.c_str(), 1);
		memset(master_recs[n].prefix, 0, sizeof(master_recs[n].prefix));
		strncpy(master_recs[n].prefix, rec.prefix.c_str(), 2);
		memset(master_recs[n].suffix, 0, sizeof(master_recs[n].suffix));
		strncpy(master_recs[n].suffix, rec.suffix.c_str(), 3);
		master_recs[n].recN = n;
	}
	sort_master_recs();
}

bool open_masterdb()
{
	if (progStatus.masterdb == netdb.filename()) {
		fl_alert2("You are editing the %s master DB", netdb.filename().c_str());
		return false;
	}
	masterdb = new csvdb();
	masterdb->filename(progStatus.masterdb);
	if (masterdb->load()) {
		delete masterdb;
		masterdb = (csvdb *)0;
		return false;
	}
	load_master_recs();
	return true;
}

bool from_masterdb(const char *p, const char *a, const char *s, csvRecord *mrec)
{
	if (!masterdb) {
		if (!open_masterdb() )
			return false;
	}
	std::string sp = p;
	std::string sa = a;
	std::string ss = s; 

	int recn = binary_search_masterdb(0, masterdb->numrecs() - 1, sp, sa, ss);

	if (recn < 0) {
		fl_alert2("Not in Master DB");
		return false;
	}

	csvRecord rec;

	masterdb->get(master_recs[recn].recN, rec);
	mrec->name = rec.name;
	mrec->fname = rec.fname;
	mrec->lname = rec.lname;
	mrec->addr = rec.addr;
	mrec->city = rec.city;
	mrec->state = rec.state;
	mrec->zip = rec.zip;
	mrec->phone = rec.phone;
	mrec->birthdate = rec.birthdate;
	mrec->spouse = rec.spouse;
	mrec->sp_birth = rec.sp_birth;
	mrec->comment1 = rec.comment1;
	mrec->comment2 = rec.comment2;
	mrec->email = rec.email;
	mrec->locator = rec.locator;
	mrec->country = rec.country;
	if (progStatus.mdb_netnbr)
		mrec->netnbr = rec.netnbr;

	return true;
}

void to_masterdb(csvRecord &mrec)
{
	if (!masterdb) return;
	masterdb->add(mrec);
}

void close_masterdb()
{
	if (!masterdb) return;
	masterdb->save();
	delete masterdb;
	masterdb = (csvdb *)0;
}
