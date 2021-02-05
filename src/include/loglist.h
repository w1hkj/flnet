//======================================================================
// combo.cxx
//
// Authors:
//
// Copyright (C) 2012, Dave Freese, W1HKJ
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
//
//======================================================================

#ifndef _LOGLIST
#define _LOGLIST

#include <stdlib.h>
#include <string.h>

enum logStatus{EMPTY,LOGIN,FIRST,SECOND,LOGOUT};

#define LISTINCR 500
#define DLINESIZE 30
#define BLANKS 4

struct _logged {
	size_t    recN;
	logStatus status;
	char szPrefix[3];
	char szArea[2];
	char szSuffix[4];
	char szName[15];
	char szTime[6];
	char chPriority;
	char displine[DLINESIZE + 1];
};

class loglist {
private:
	_logged *llist;
	int nlist;
	int lsize;
	int iLastup;
	char cP1;
	char cP2;
	char cP3;
	int  iAutoPriority;
	void CreateDispLine (int n);
public:
	loglist ();

	int  numlist (void) { return nlist - BLANKS; }
	long recN (int n);
	void recN (int n, size_t N);
	logStatus status (int n);
	void status(int n, logStatus st);
	int add (long N,
			 const char *prefix, const char *area, const char *suffix,
			 const char *name, const char *szTime, char flg = ' ');
	void modify (int n,
				 long N,
				 const char *prefix, const char *area, const char *suffix,
				 const char *name);
	int del (int n);
	void clear (void);
	char *displine (int n);
	const char *report_line(int n);
	char *prefix (int n);
	char *area (int n);
	char *suffix (long n);
	int inList (size_t N);
	int Pri_0 (int n);
	int Pri_1 (int n);
	int Pri_2 (int n);
	int Pri_3 (int n);
	int AutoPriority () {return iAutoPriority;}
	void AutoPriority (int n) {iAutoPriority = n;}
	void setPri_1 (char c) {cP1 = c;}
	void setPri_2 (char c) {cP2 = c;}
	void setPri_3 (char c) {cP3 = c;}
	int nextup (void);
	int MoveEarlier (int);
	int MoveLater (int);
	void lastup (int);
	int  lastup ();
};

#endif

