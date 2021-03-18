// =====================================================================
//
// loglist.h
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
// =====================================================================

#include <iostream>
#include <string>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "loglist.h"
#include "netshared.h"
#include "status.h"

void loglist::clear (void)
{
	for (int i = 0; i < lsize; i++) {
		llist[i].recN = -1;
		llist[i].status = LOGIN;
		llist[i].displine[0] = 0;
		llist[i].chPriority = ' ';
		llist[i].szPrefix[0] = 0;
		llist[i].szArea[0] = 0;
		llist[i].szSuffix[0] = 0;
		llist[i].szName[0] = 0;
		llist[i].szdt[0] = 0;
	}
	nlist = BLANKS;
	iLastup = BLANKS;
	cP1 = ' ';
	cP2 = ' ';
	cP3 = ' ';
	iAutoPriority = 0;
}

loglist::loglist ()
{
	llist = new _logged[LISTINCR];
	if (llist) {
		lsize = LISTINCR;
		clear ();
	} else
		exit (2);
}

void loglist::justify()
{
	for (int i = BLANKS; i < nlist; i++)
		CreateDispLine(i);
}

void loglist::CreateDispLine (int n)
{
	std::string prefix = trim(llist[n].szPrefix),
				area   = trim(llist[n].szArea),
				suffix = trim(llist[n].szSuffix);
	std::string call;
	static std::string dline;
	static std::string name;
	static std::string time;
	call.clear();
	dline.clear();
	name.clear();
	time.clear();

	if (progStatus.call_justify == RIGHT_JUSTIFY) {
		call.assign(prefix);
		call.append(area);
		call.append(suffix);
		if (call.length() < 8) call.insert(0, 8 - call.length(), ' ');
	} else if (progStatus.call_justify == LEFT_JUSTIFY) {
		call.assign(prefix);
		call.append(area);
		call.append(suffix);
		if (call.length() < 8) call.append(8 - call.length(), ' ');
	}
	else { // AREA_JUSTIFY
		call.assign(prefix);
		if (call.length() < 3) call.insert(0, 3 - call.length(), ' ');
		call.append(area);
		call.append(suffix);
		if (call.length() < 8) call.append(8 - call.length(), ' ');
	}

	dline.append(call);

	dline.append(" ");
	name = trim(llist[n].szName);
	if (name.length() > 11) name = name.erase(11);
	if (progStatus.name_justify == LEFT_JUSTIFY) {
		if (name.length() < 11) name.append(11 - name.length(), ' ');
	} else {
		if (name.length() < 11) name = std::string(11 - name.length(),' ').append(name);
	}
	dline.append(name);

	dline.append(" ");
	time = llist[n].szTime;
	time.erase(2,1);
	dline.append(time);

	dline.append(" ");
	dline += llist[n].chPriority;

	memset (llist[n].displine, 0, DLINESIZE + 1);
	strncpy (llist[n].displine, dline.c_str(), DLINESIZE);

	return;
}

const char *loglist::report_line(int n)
{
	int nn = n + BLANKS;
	static std::string rline;
	rline.assign(trim(llist[nn].szPrefix))
		 .append(trim(llist[nn].szArea))
		 .append(trim(llist[nn].szSuffix))
		 .append("\t")
		 .append(llist[nn].szName)
		 .append("\t")
		 .append(llist[nn].szTime);
	return rline.c_str();
}

int loglist::add (long N,
				  const char *p, const char *a, const char *s,
				  const char *name, 
				  char flg)
{
	if (nlist >= lsize) {
		_logged *temp = new _logged[lsize + LISTINCR];
		if (!temp) {
			return 0;
		}
		for (int i = 0; i < lsize + LISTINCR; i++) {
			llist[i].recN = -1;
			llist[i].status = EMPTY;
			llist[i].displine[0] = 0;
		}
		for (int i = 0; i < lsize; i++) temp[i] = llist[i];
		delete [] llist;
		llist = temp;
		lsize += LISTINCR;
	}

	llist[nlist].recN = N;
	llist[nlist].status = LOGIN;

	memset (llist[nlist].szPrefix, 0, 4);
	memset (llist[nlist].szArea, 0, 2);
	memset (llist[nlist].szSuffix, 0, 5);
	memset (llist[nlist].szName, 0, 15);

	strncpy (llist[nlist].szPrefix, p, 3);
	strncpy (llist[nlist].szArea, a, 1);
	strncpy (llist[nlist].szSuffix, s, 4);
	strncpy (llist[nlist].szName, name, 14);

	time_t the_time;
	struct tm *tm_ptr;
	time (&the_time);
	tm_ptr = localtime (&the_time);

	sprintf( llist[nlist].szTime, "%02d:%02d", tm_ptr->tm_hour, tm_ptr->tm_min);
	sprintf( llist[nlist].szdt,
			 "%04d%03d%02d%02d%02d",
			 tm_ptr->tm_year,
			 tm_ptr->tm_yday,
			 tm_ptr->tm_hour,
			 tm_ptr->tm_min,
			 tm_ptr->tm_sec);

	CreateDispLine (nlist);

	nlist++;

	if (flg == cP1)
		Pri_1 (nlist - BLANKS - 1);
	else if (flg == cP2)
		Pri_2 (nlist - BLANKS - 1);
	else if (flg == cP3)
		Pri_3 (nlist - BLANKS - 1);

	return 1;
}

void loglist::modify (int n,
					  long N,
					  const char *p, const char *a, const char *s,
					  const char *name)
{
	int nn = n + BLANKS;

	if (nn == nlist) return;

	llist[nn].recN = N;
	strcpy(llist[nn].szPrefix, p);
	strcpy(llist[nn].szArea, a);
	strcpy(llist[nn].szSuffix, s);
	strncpy (llist[nn].szName, name, 14);

	CreateDispLine (nn);

	return;
}

long loglist::recN (int n)
{
	int nn = n + BLANKS;
	if (nn <= nlist)
		return llist[nn].recN;
	return -1L;
}

void loglist::recN (int n, size_t N)
{
	llist[n + BLANKS].recN = N;
}

logStatus loglist::status (int n)
{
	int nn = n + BLANKS;
	if (nn <= nlist)
		return llist[nn].status;
	return LOGIN;
}

void loglist::status (int n, logStatus st)
{
	int nn = n + BLANKS;
	if (nn <= nlist)
		llist[nn].status = st;
	return;
}

char * loglist::displine (int n)
{
	int nn = n + BLANKS;
	if (nn <= nlist)
		return llist[nn].displine;
	return NULL;
}

int loglist::del (int n)
{
	int nn = n + BLANKS;
	if ( nn > nlist ) return 0;
	if (nn == iLastup) iLastup = BLANKS;
	if (nn < iLastup) {
		iLastup--;
		if (iLastup < BLANKS) iLastup = BLANKS;
	}
	for (int i = nn; i < nlist; i++)
		llist[i] = llist[i+1];
	nlist--;
	return 1;
}

int loglist::inList (size_t N)
{
	for (int i = BLANKS; i <= nlist; i++)
		if (llist[i].recN == N) return 1;
	return 0;
}

int loglist::inList (std::string call)
{
	std::string tcall;
	for (int i = BLANKS; i < nlist; i++) {
		tcall.assign(llist[i].szPrefix).append(llist[i].szArea).append(llist[i].szSuffix);
		if (tcall == call) return 1;
	}
	return 0;
}

char * loglist::prefix (int n)
{
	return llist[n + BLANKS].szPrefix;
}

char * loglist::area (int n)
{
	return llist[n + BLANKS].szArea;
}

char * loglist::suffix (long n)
{
	return llist[n + BLANKS].szSuffix;
}

int loglist::Pri_0 (int n)
{
	int nn = n + BLANKS;

	llist[nn].chPriority = ' ';
	CreateDispLine (nn);

	if (iAutoPriority == 0) return n;

	_logged thiscall = llist[nn];
	del(n);

	int newpos = BLANKS;
	for (newpos = BLANKS; newpos < nlist; newpos++) {
		if (llist[newpos].chPriority != ' ')
			continue;
		if (std::string(llist[newpos].szdt) > std::string(thiscall.szdt))
			break;
	}
	for (int n = nlist; n > newpos; n--)
		llist[n] = llist[n-1];

	llist[newpos] = thiscall;
	nlist++;

	return newpos - BLANKS;
}

int loglist::Pri_1 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (cP1 == ' ') return n;

	llist[nn].chPriority = cP1;
	CreateDispLine (nn);

	if (iAutoPriority == 0) return n;

	_logged thiscall = llist[nn];
	del(n);

	std::string dt = thiscall.szdt;
	for (int i = BLANKS; i < nlist; i++) {
		if ((llist[i].chPriority == cP1) && 
			(std::string(llist[i].szdt) < dt))
			continue;
		newpos = i;
		for (int j = nlist; j > newpos; j--)
			llist[j] = llist[j-1];
		llist[newpos] = thiscall;
		nlist++;
		return newpos - BLANKS;
	}
	llist[nlist++] = thiscall;
	return nlist -1 - BLANKS;
}

int loglist::Pri_2 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (cP2 == ' ') return n;

	llist[nn].chPriority = cP2;
	CreateDispLine (nn);

	if (iAutoPriority == 0) return n;

	_logged thiscall = llist[nn];
	del(n);

	std::string dt = thiscall.szdt;
	char ch;
	for (int i = BLANKS; i < nlist; i++) {
		ch = llist[i].chPriority;
		if (ch == cP1)
			continue;
		if ((ch == cP2) && 
			(std::string(llist[i].szdt) < dt))
			continue;
		newpos = i;
		for (int j = nlist; j > newpos; j--)
			llist[j] = llist[j-1];
		llist[newpos] = thiscall;
		nlist++;
		return newpos - BLANKS;
	}
	llist[nlist++] = thiscall;
	return nlist - 1 - BLANKS;
}

int loglist::Pri_3 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (cP3 == ' ') return n;

	llist[nn].chPriority = cP3;
	CreateDispLine (nn);

	if (iAutoPriority == 0) return n;

	_logged thiscall = llist[nn];
	del(n);

	std::string dt = thiscall.szdt;
	char ch;
	for (int i = BLANKS; i < nlist; i++) {
		ch = llist[i].chPriority;
		if (ch == cP1 || ch == cP2)
			continue;
		if ((ch == cP3) &&
			(std::string(llist[i].szdt) < dt))
			continue;
		newpos = i;
		for (int j = nlist; j > newpos; j--)
			llist[j] = llist[j-1];
		llist[newpos] = thiscall;
		nlist++;
		return newpos - BLANKS;
	}
	llist[nlist++] = thiscall;
	return nlist -1 - BLANKS;
}

int loglist::nextup (void)
{
	for (int i = BLANKS; i < nlist; i++)
		if (llist[i].status == LOGIN) return (i - BLANKS);
	return (nlist -1 - BLANKS);
}

int loglist::MoveEarlier (int n)
{
	int nn = n + BLANKS;
	if (n == 0) return n;
	if (nlist == BLANKS + 1) return n;
	_logged tmp = llist[nn];
	llist[nn] = llist[nn-1];
	llist[nn-1] = tmp;
	return (n - 1);
}

int loglist::MoveLater (int n)
{
	int nn = n + BLANKS;
	if (nn == nlist - 1) return n;
	_logged tmp = llist[nn+1];
	llist[nn+1] = llist[nn];
	llist[nn] = tmp;
	return (n + 1);
}

void loglist::lastup (int n)
{
	iLastup = n + BLANKS;
}


int loglist::lastup ()
{
	return iLastup - BLANKS;
}

int loglist::locate(std::string prefix, std::string area, std::string suffix)
{
	bool b1 = false, b2 = false, b3 = false;
	for (int i = BLANKS; i < nlist; i++) {
		b1 = prefix == llist[i].szPrefix;
		b2 = area == llist[i].szArea;
		b3 = suffix == llist[i].szSuffix;
		if ( b1 && b2 && b3 ) {
			return i - BLANKS;
		}
	} 
	return -1;
}

int loglist::locate(size_t refn)
{
	for (int i = BLANKS; i < nlist; i++) {
		if (llist[i].recN == refn) return i - BLANKS;
	}
	return -1;
}

static char _ch1, _ch2, _ch3;

int comp_dt( const void *p1, const void *p2)
{
	_logged *r1 = (_logged *)p1;
	_logged *r2 = (_logged *)p2;
	int comp;
	std::string s1 = r1->szdt;
	std::string s2 = r2->szdt;

	comp = s1.compare(s2);

	if (comp == 0) {
		int cp1 = 0, cp2 = 0;

		if (r1->chPriority == _ch1) cp1 = 3;
		else if (r1->chPriority == _ch2) cp1 = 2;
		else if (r1->chPriority == _ch3) cp1 = 1;

		if (r2->chPriority == _ch1) cp2 = 3;
		else if (r2->chPriority == _ch2) cp2 = 2;
		else if (r2->chPriority == _ch3) cp2 = 1;

		if ( cp1 < cp2 ) comp = 1;
		if ( cp1 == cp2 ) comp = 0;
		if ( cp1 > cp2 ) comp = -1;
	}

	return comp;
}

int comp_priority( const void *p1, const void *p2)
{
	_logged *r1 = (_logged *)p1;
	_logged *r2 = (_logged *)p2;

	int comp;
	int cp1 = 0, cp2 = 0;

	if (r1->chPriority == _ch1) cp1 = 3;
	else if (r1->chPriority == _ch2) cp1 = 2;
	else if (r1->chPriority == _ch3) cp1 = 1;

	if (r2->chPriority == _ch1) cp2 = 3;
	else if (r2->chPriority == _ch2) cp2 = 2;
	else if (r2->chPriority == _ch3) cp2 = 1;

	if ( cp1 < cp2 ) comp = 1;
	if ( cp1 == cp2 ) comp = 0;
	if ( cp1 > cp2 ) comp = -1;

	if (comp == 0) {
		std::string s1 = r1->szdt;
		std::string s2 = r2->szdt;
		comp = s1.compare(s2);
	}

	return comp;
}

void loglist::sort(int by)
{
	if (nlist - BLANKS == 0) return;
	_ch1 = cP1; _ch2 = cP2; _ch3 = cP3;

	if (by == BYDATETIME) {
		qsort (&llist[BLANKS], nlist - BLANKS, sizeof(*llist), comp_dt);
	} else {
		qsort (&llist[BLANKS], nlist - BLANKS, sizeof(*llist), comp_priority);
	}
}
