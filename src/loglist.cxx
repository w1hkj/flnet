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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "loglist.h"
#include "netshared.h"

void loglist::clear (void)
{
	for (int i = 0; i < lsize; i++) {
		llist[i].recN = -1;
		llist[i].status = LOGIN;
		llist[i].displine[0] = 0;
		llist[i].chPriority = ' ';
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

void loglist::CreateDispLine (int n)
{
	char dline[25], call[10];
	strcpy (call, trim(llist[n].szPrefix));
	strcat (call, trim(llist[n].szArea));
	strcat (call, trim(llist[n].szSuffix));
	snprintf (dline, sizeof(dline), "%-8s%-8s%5s %c",
			  call,
			  llist[n].szName,
			  llist[n].szTime,
			  llist[n].chPriority);
	memset (llist[n].displine, 0, DLINESIZE + 1);
	strncpy (llist[n].displine, dline, DLINESIZE);

	return;
}

char *loglist::report_line(int n)
{
	int nn = n + BLANKS;
	static char dline[50], call[10];
	strcpy (call, trim(llist[nn].szPrefix));
	strcat (call, trim(llist[nn].szArea));
	strcat (call, trim(llist[nn].szSuffix));
	snprintf (dline, sizeof(dline), "%s\t%s\t%s",
			  call,
			  llist[nn].szName,
			  llist[nn].szTime);
	return dline;
}

int loglist::add (long N,
				  const char *p, const char *a, const char *s,
				  const char *name, const char *szTime, char flg)
{
	if (nlist >= lsize) {
		_logged *temp = new _logged[lsize + LISTINCR];
		if (!temp) {
			printf("out of memory!\n"); fflush (stdout);
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

	strcpy(llist[nlist].szPrefix, p);
	strcpy(llist[nlist].szArea, a);
	strcpy(llist[nlist].szSuffix, s);

	strncpy (llist[nlist].szName, name, 7);
	strcpy (llist[nlist].szTime, szTime);

	CreateDispLine (nlist);
	nlist++;
	if (flg == cP1)
		Pri_1 (nlist - 1 - BLANKS);
	else if (flg == cP2)
		Pri_2 (nlist - 1 - BLANKS);
	else if (flg == cP3)
		Pri_3 (nlist - 1 - BLANKS);
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
	strncpy (llist[nn].szName, name, 7);

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

int loglist::inList (long N)
{
	for (int i = BLANKS; i <= nlist; i++)
		if (llist[i].recN == N) return 1;
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

int loglist::Pri_1 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (cP1 == ' ') return n;
	llist[nn].chPriority = cP1;
	CreateDispLine (nn);
	if (nlist == BLANKS + 1) return n;
	if (n == 0 || iAutoPriority == 0) return n;
	_logged thiscall = llist[nn];
	del (n);
	for (int i = BLANKS; i < nlist; i++) {
		if (llist[i].chPriority != cP1) {
			newpos = i;
			for (int j = nlist; j > newpos; j--)
				llist[j] = llist[j-1];
			llist[newpos] = thiscall;
			nlist++;
			return newpos - BLANKS;
		}
	}
	llist[nn] = thiscall;
	return n;
}

int loglist::Pri_2 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (cP2 == ' ') return n;
	llist[nn].chPriority = cP2;
	CreateDispLine (nn);
	if (nlist == BLANKS + 1 || iAutoPriority == 0) return n;
	_logged thiscall = llist[nn];
	del (n);
	char ch;
	for (int i = BLANKS; i < nlist; i++) {
		ch = llist[i].chPriority;
		if (cP1 != ' ' && ch == cP1) continue;
		if (ch == cP2) continue;
		newpos = i;
		for (int j = nlist; j > newpos; j--)
			llist[j] = llist[j-1];
		llist[newpos] = thiscall;
		nlist++;
		return newpos - BLANKS;
	}
	llist[nn] = thiscall;
	return n;
}

int loglist::Pri_3 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (cP3 == ' ') return n;
	llist[nn].chPriority = cP3;
	CreateDispLine (nn);
	if (nlist == BLANKS + 1 || iAutoPriority == 0) return n;
	_logged thiscall = llist[nn];
	del (n);
	char ch;
	for (int i = BLANKS; i < nlist; i++) {
		ch = llist[i].chPriority;
		if (cP1 != ' ' && ch == cP1) continue;
		if (cP2 != ' ' && ch == cP2) continue;
		newpos = i;
		for (int j = nlist; j > newpos; j--)
			llist[j] = llist[j-1];
		llist[newpos] = thiscall;
		nlist++;
		return newpos - BLANKS;
	}
	llist[nn] = thiscall;
	return n;
}

int loglist::Pri_0 (int n)
{
	int nn = n + BLANKS;
	int newpos;
	if (llist[nn].chPriority == ' ') return n;
	llist[nn].chPriority = ' ';
	CreateDispLine (nn);

	if (nn == nlist - 1) return n;
	if (iAutoPriority == 0) return n;
	_logged thiscall = llist[nn];
	del (n);
	char ch;
	for (int i = BLANKS; i < nlist; i++) {
		ch = llist[i].chPriority;
		if (cP1 != ' ' && ch == cP1) continue;
		if (cP2 != ' ' && ch == cP2) continue;
		if (cP3 != ' ' && ch == cP3) continue;
		newpos = i;
		for (int j = nlist; j > newpos; j--)
			llist[j] = llist[j-1];
		llist[newpos] = thiscall;
		nlist++;
		return newpos - BLANKS;
	}
	llist[nn] = thiscall;
	return n;
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

