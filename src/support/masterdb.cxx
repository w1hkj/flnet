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
#include "icons.h"

csvdb *masterdb = (csvdb *)0;

csvRecord master_rec;

SEARCH_REC *master_recs = (SEARCH_REC *)0;

//----------------------------------------------------------------------
// l = left element in search interval
// r = right element in search interval
// p = search prefix
// a = search area
// s = search suffix
//----------------------------------------------------------------------

int  binary_search_masterdb(int l, int r, std::string &p, std::string &a, std::string &s) 
{ 
	if (r < l) return -1;

	int mid = l + (r - l) / 2;

	std::string call1, call2;
	call1.assign(p).append(a).append(s);
	call2.assign(master_recs[mid].prefix).append(master_recs[mid].area).append(master_recs[mid].suffix);

	int cmp = call2.compare(call1);
	if (cmp > 0) return binary_search_masterdb(l, mid - 1, p, a, s);
	if (cmp < 0) return binary_search_masterdb(mid + 1, r, p, a, s);
	return mid;
} 

// sort by area / prefix / suffix

int mrec_compare (const void *p1, const void *p2)
{
	SEARCH_REC *s1 = (SEARCH_REC *)p1;
	SEARCH_REC *s2 = (SEARCH_REC *)p2;

	std::string call1, call2;
	call1.assign(s1->prefix).append(s1->area).append(s1->suffix);
	call2.assign(s2->prefix).append(s2->area).append(s2->suffix);

	return call2.compare(call1);

}

void sort_master_recs()
{
	return;
	if (!master_recs || !masterdb->numrecs()) return;
	qsort ( &(master_recs[0]), masterdb->numrecs(), sizeof(SEARCH_REC), mrec_compare);
}

void load_master_recs()
{
	if (masterdb->load()) {
		delete masterdb;
		masterdb = (csvdb *)0;
		if (box_mdb_isopen) {
			box_mdb_isopen->color(FL_BACKGROUND2_COLOR);
			box_mdb_isopen->redraw();
		}
		return;
	}

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
		fl_alert2("Master db is already opened for editing\n\n%s", netdb.filename().c_str());
		if (box_mdb_isopen) {
			box_mdb_isopen->color(FL_BACKGROUND2_COLOR);
			box_mdb_isopen->redraw();
		}
		return false;
	}
	masterdb = new csvdb();
	masterdb->filename(progStatus.masterdb);
	load_master_recs();
	if (box_mdb_isopen) {
		box_mdb_isopen->color(progStatus.mdb_color);
		box_mdb_isopen->redraw();
	}
	progStatus.mdb_isopen = true;
	return true;
}

bool from_masterdb(const char *p, const char *a, const char *s, csvRecord *mrec)
{
	if (!masterdb) {
		fl_alert2("Master db is not open");
			return false;
	}
	std::string sp = p;
	std::string sa = a;
	std::string ss = s; 

	int recn = binary_search_masterdb(
		0, masterdb->numrecs() - 1, sp, sa, ss);

	if (recn < 0) {
		fl_alert2("Not in Master DB");
		return false;
	}

	csvRecord rec;

	masterdb->get(master_recs[recn].recN, rec);

	mrec->prefix = rec.prefix;
	mrec->area = rec.area;
	mrec->suffix = rec.suffix;
	mrec->callsign = rec.callsign;
	mrec->prevdate = rec.prevdate;
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
	mrec->logdate = rec.logdate;
	mrec->prevdate = rec.prevdate;
	mrec->nbrlogins = rec.nbrlogins;
	mrec->status = rec.status;
	mrec->joined = rec.joined;
	mrec->netnbr = rec.netnbr;

	return true;
}

void add_to_masterdb(csvRecord &mrec)
{
	if (!masterdb) {
		fl_alert2("Master db is not open");
		return;
	}
	masterdb->add(mrec);
	masterdb->save();
	load_master_recs();
}

void replace_masterdb(size_t recn, csvRecord &mrec)
{
	if (!masterdb) {
		fl_alert2("Master db is not open");
		return;
	}
	masterdb->erase(recn);
	masterdb->add(mrec);
	masterdb->save();
	load_master_recs();
}

void close_masterdb()
{
	if (!masterdb) return;
	masterdb->save();
	delete masterdb;
	masterdb = (csvdb *)0;
	progStatus.mdb_isopen = false;
	if (box_mdb_isopen) {
		box_mdb_isopen->color(FL_BACKGROUND2_COLOR);
		box_mdb_isopen->redraw();
	}
}
