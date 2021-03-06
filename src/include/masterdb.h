//======================================================================
//
// masterdb.h
//
// Authors:
//
// Copyright (C) 2021, Dave Freese, W1HKJ
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

#ifndef _MASTERDB
#define _MASTERDB

#include <string>

#include "net_ui.h"
#include "netutils.h"
#include "netedits.h"

#include "csvdb.h"

struct SEARCH_REC {
	int  recN;
	char prefix[4];
	char area[2];
	char suffix[4];
};

extern std::string uppercase (std::string s);
extern std::string trim (std::string s);

extern csvdb *masterdb;
extern long is_in_masterdb(const char *p, const char *a, const char *s);

extern void save_to_masterdb ();

extern int  binary_search_masterdb(int l, int r, std::string &p, std::string &a, std::string &s);
extern bool from_masterdb(const char *p, const char *a, const char *s, csvRecord *mrec);
extern void add_to_masterdb(csvRecord &mrec);
extern void replace_masterdb(size_t recn, csvRecord &mrec);
extern void sort_master_recs ();

extern bool open_masterdb();
extern void close_masterdb();

#endif

