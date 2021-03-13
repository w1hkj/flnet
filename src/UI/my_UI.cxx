 // =====================================================================
//
// my_UI.cxx
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include <string>
#include <iostream>
#include <sstream>
#include <cmath>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_ask.H>

#include "icons.h"
#include "flinput2.h"
#include "my_UI.h"
#include "net_ui.h"
#include "loglist.h"
#include "netshared.h"
#include "netsupport.h"
#include "config.h"
#include "net_config.h"
#include "net_ui.h"
#include "timeops.h"
#include "xml_io.h"
#include "debug.h"
#include "csvdb.h"
#include "status.h"

#include "locator.h"

char emptyline[] = "";

loglist callinlist;
int	WhoIsUp = 0;
static int lastUp;

char keyval[2] = " ";
string szPrefix = "";
string szArea = "";
string szSuffix = "";
string szFullName;
string szLogDate;
string szPrevDate;
string szBirthday;
string szNetNbr;
string szSpouse;
string szPhone;
string szQTH;
string szEmail;
string szComment1;
string szComment2;
string szFirstName;
string szCallSign;
string szCountry;
string szAzimuth;
string szDistance;

bool updateFldigi = true;

struct _pick {
	long recN;
	char	callsign[10];
};

static _pick Pick[NPICKITEMS];
static int nbrPicked;
static int whoPicked;

char *fmtDate(char *d)
{
	static char date[11];
	int i;
	strcpy (date, "  /  /    ");
	for (i = 0; i < 2; i++) date[i] = d[i+4];
	for (i = 0; i < 2; i++) date[i+3] = d[i+6];
	for (i = 0; i < 4; i++) date[i+6] = d[i];
	return date;
}

void updateCallins (bool fldigi_flag)
{
	int i,j;
	Fl_Color fg, bg;

	for (i = WhoIsUp - 4, j = 0; i < WhoIsUp + 11; i++, j++) {
		fg = progStatus.fgColors[callinlist.status(i)];
		bg = progStatus.bgColors[callinlist.status(i)];
		txtLine[j]->labelcolor (fg);
		txtLine[j]->color (bg);
		txtLine[j]->label (callinlist.displine(i));
		txtLine[j]->redraw_label();
	}

	szFullName.clear();
	szLogDate.clear();
	szPrevDate.clear();
	szBirthday.clear();
	szNetNbr.clear();
	szSpouse.clear();
	szPhone.clear();
	szEmail.clear();
	szQTH.clear();
	szFirstName.clear();
	szCallSign.clear();
	szCountry.clear();
	szAzimuth.clear();
	szDistance.clear();

	static char lbl[50];
	snprintf(lbl, sizeof(lbl), "Check-ins: %d", callinlist.numlist());
	while (strlen(lbl) < 15) strcat(lbl, " ");
	box_callins->label(lbl);
	box_callins->redraw_label();

	inp_focus->take_focus();

	long rc = callinlist.recN(WhoIsUp);

	if (rc >= 0 && rc < (long)netdb.numrecs()) {
		csvRecord rec;
		netdb.get(rc, rec);

		if(fldigi_flag) {
			if(rec.name.empty())
				szFirstName.assign(trim(rec.fname.c_str()));
			else
				szFirstName.assign(trim(rec.name.c_str()));

			szCallSign.assign(trim(rec.callsign.c_str()));
			update_fldigi_callsign(szFirstName, szCallSign);
		}

		szFullName.assign(trim(rec.fname.c_str())).append(" ");
		szFullName.append(trim(rec.lname.c_str()));
		szLogDate.assign(rec.logdate);
		szPrevDate.assign(rec.prevdate);
		szNetNbr.assign(rec.netnbr);
		szSpouse.assign(trim(rec.spouse.c_str()));
		szPhone.assign(trim(rec.phone.c_str()));

		szQTH.assign(trim(rec.city.c_str()));
		szQTH.append(", ").append(trim(rec.state.c_str()));

		szBirthday.assign(trim(rec.birthdate.c_str()));

		szEmail.assign(trim(rec.email.c_str()));
		size_t pat = szEmail.find("@");
		if (pat != string::npos) szEmail.insert(pat, "@");

		szCountry = trim(rec.country.c_str());

		{
		char az[10], dist[20];
		az[0] = '\0';
		dist[0] = '\0';
		double distance, azimuth, lon[2], lat[2];
		if (QRB::locator2longlat(
				&lon[0], &lat[0],
				progStatus.myLocator.c_str()) == QRB::QRB_OK &&
			QRB::locator2longlat(
				&lon[1], &lat[1],
				rec.locator.c_str()) == QRB::QRB_OK &&
			QRB::qrb(
				lon[0], lat[0], lon[1], lat[1],
				&distance, &azimuth) == QRB::QRB_OK) {

				snprintf(az, sizeof(az), "%03.0f", round(azimuth));
				snprintf(dist, sizeof(dist), "%.0f %s",
					distance,
					(progStatus.arc_conversion == 0 ? " km" :
					(progStatus.arc_conversion == 1 ? " nm" : " mi") ) );

				szAzimuth = az;
				szDistance = dist;
			}
		}

//----------------------------------------------------------------------
		static std::string ssInfo;
		std::string line;
//line 1
		line.assign("Name:  ").append(szFullName);
		if (line.length() < 30) line.append(30 - line.length(), ' ');
		line.append("Nbr:  ").append(szNetNbr);
		ssInfo.assign(line);
//line 2
		line.assign("Birth: ").append(szBirthday);
		if (line.length() < 30) line.append(30 - line.length(), ' ');
		line.append("Last: ").append(szLogDate);
		ssInfo.append("\n").append(line);
//line 3
		line.assign("QTH:   ").append(szQTH);
		if (line.length() > 30) line.erase(30);
		if (line.length() < 30) line.append(30 - line.length(), ' ');
		line.append("Loc:  ").append(rec.locator);
		ssInfo.append("\n").append(line);
//line 4
		line.assign("Cntry: ").append(szCountry);
		if (line.length() > 30) line.erase(30);
		if (line.length() < 30) line.append(30 - line.length(), ' ');
		line.append("Az:   ").append(szAzimuth);
		ssInfo.append("\n").append(line);
//line 5
		line.assign("Phone: ").append(szPhone);
		if (line.length() > 30) line.erase(30);
		if (line.length() < 30) line.append(30 - line.length(), ' ');
		line.append("Dist: ").append(szDistance);
		ssInfo.append("\n").append(line);
//line 6
		line.assign("Spouse: ").append(szSpouse);
		ssInfo.append("\n").append(line);
//line 7
		line.assign("Email: ").append(szEmail);
		ssInfo.append("\n").append(line);

		line.assign("Info:  ").append(rec.comment1);
		std::string temp = rec.comment2;
		if (!temp.empty()) {
			line.append("; ").append(temp);
		}
		if (line.length() > 46) {
			bool ok = false;
			for (size_t n = 46; n > 30; n--) {
				if (line[n] == ' ') {
					line[n] = '\n';
					ok = true;
					break;
				}
			}
			if (!ok) line.insert(46, "\n");
			size_t p = line.find('\n');
			std::string line2 = line.substr(p+1);
			line.erase(p+1);
			if (line2.length() > 46) line2.erase(46);
			line.append(line2);
		}
		ssInfo.append("\n").append(line);

		txtInfo->label (ssInfo.c_str());
	} else if (callinlist.numlist() == 0)
		txtInfo->label ("");
	else
		txtInfo->label ("NOT IN DATABASE");

}

static string outfilename;

void clear_outfilename()
{
	outfilename.clear();
}

void updateLogins (bool closing = false)
{
	int i, n;
	long rc;
	FILE *fToday;
	char today[80];
	char ftoday[80];
	char szLine[40];
	char sztemp[40];
	csvRecord rec;
	if (callinlist.numlist() == 0)
		return;

	struct tm tim;
	time_t t;
	time(&t);
	gmtime_r(&t, &tim);
	strftime(today, sizeof(today), "%Y%m%d", &tim);
	strftime(ftoday, sizeof(ftoday), "%Y.%m.%d.%H%M", &tim);

	if (outfilename.empty()) {
		outfilename = selected_file;
		size_t p = outfilename.find(".csv");
		if (p != string::npos) outfilename.erase(p);
		p = outfilename.find(".CSV");
		if (p != string::npos) outfilename.erase(p);
		outfilename.append("-").append(ftoday).append(".log");
		fToday = fopen(outfilename.c_str(), "w");
	} else
		fToday = fopen(outfilename.c_str(), "w");

	for (i = 0; i < callinlist.numlist(); i++) {
		strcpy (szLine, callinlist.displine(i));
		fprintf (fToday, "%s\n", szLine);
		if (closing) {
			rc = callinlist.recN (i);
			if (rc >= 0 && rc < (long)netdb.numrecs()) {
				netdb.get(rc, rec);
				rec.prevdate.assign(rec.logdate);
				n = atoi (rec.nbrlogins.c_str());
				n += 1;
				sprintf (sztemp, "%d", n);
				rec.nbrlogins.assign(sztemp);
			rec.logdate.assign(today);
			netdb.put(rc, rec);
			}
		}
	}

	fclose (fToday);
}

void my_UI::UpdateWhoIsUp (long L)
{
	string pr, ar, su, nm;
	csvRecord rec;
	netdb.get(L, rec);
	callinlist.modify (WhoIsUp, L,
					   rec.prefix.c_str(),
					   rec.area.c_str(),
					   rec.suffix.c_str(),
					   rec.name.c_str());
	dispCallIns(false);
}

void my_UI::clearPickList ()
{
	for (int i = 0; i < NPICKITEMS; i++ ) {
		Pick[i].callsign[0] = 0;
		txtPick[i]->label ("");
		txtPick[i]->labelcolor (FL_BLACK);
		txtPick[i]->color (FL_BACKGROUND2_COLOR);
		txtPick[i]->redraw_label ();
		txtPick[i]->redraw ();
	}
	nbrPicked = 0;
}

void my_UI::fillPickList ()
{
	int i;
	int cmp;
	size_t rc;
	char padded[4];

	if (!indexed_list) getindexed_list ();
	SortBySAP ();

	clearPickList ();
	strcpy (padded, szSuffix.c_str());
	while (strlen(padded) < 3) strcat (padded, " ");

	i = 0; rc = 0;
	while (rc < netdb.numrecs() && padded[0] > indexed_list[rc].suffix[0]) rc++;

	while (rc < netdb.numrecs()) {
		cmp = strcmp(padded, indexed_list[rc].suffix);
		if (cmp < 0) break;
		if (cmp == 0 && i < NPICKITEMS) {
			Pick[i].recN = indexed_list[rc].recN;
			strcpy (Pick[i].callsign, indexed_list[rc].prefix);
			strcat (Pick[i].callsign, indexed_list[rc].area);
			strcat (Pick[i].callsign, indexed_list[rc].suffix);
			txtPick[i]->label (Pick[i].callsign);
			if (i == 0) {
				txtPick[i]->labelcolor (progStatus.fgColors[5]);
				txtPick[i]->color (progStatus.bgColors[5]);
			} else {
				txtPick[i]->labelcolor (FL_BLACK);
				txtPick[i]->color (FL_BACKGROUND2_COLOR);
			}
			txtPick[i]->redraw_label ();
			txtPick[i]->redraw ();
			i++;
		}
		rc++;
	}
	nbrPicked = i;
}

void my_UI::PickedColors ()
{
	int i;
	for (i = 0; i < NPICKITEMS; i++) {
		if (i == whoPicked) {
			txtPick[i]->labelcolor (progStatus.fgColors[5]);
			txtPick[i]->color (progStatus.bgColors[5]);
		} else {
			txtPick[i]->labelcolor (FL_BLACK);
			txtPick[i]->color (FL_BACKGROUND2_COLOR);
		}
		txtPick[i]->redraw_label ();
		txtPick[i]->redraw ();
	}
}

void my_UI::dispCallIns (bool flag)
{
	updateCallins (flag);
}

int my_UI::PickedToCallins (int n)
{
	return PickedToCallinsDB((size_t) Pick[n].recN);
}

int my_UI::PickedToCallinsDB (size_t record_number)
{

	if (callinlist.inList(record_number)) {
		clearPickList ();
		clearSAP ();
		return -1;
	}

	csvRecord rec;
	netdb.get(record_number, rec);
	std::string pr, ar, su, nm, st;
	pr = uppercase (trim(rec.prefix));
	ar = uppercase (trim(rec.area));
	su = uppercase (trim(rec.suffix));
	nm = trim(rec.name);
	st = uppercase(trim(rec.state));

	std::string strP1, strP2, strP3;
	strP1 = uppercase(trim(progStatus.strP1));
	strP2 = uppercase(trim(progStatus.strP2));
	strP3 = uppercase(trim(progStatus.strP3));

	if (st.empty()) {
		callinlist.add (
			record_number,
			pr.c_str(), ar.c_str(), su.c_str(), nm.c_str() );
	} else {
		if (!strP1.empty() &&
			strP1.find(st) != std::string::npos) {
			callinlist.add (
				record_number,
				pr.c_str(), ar.c_str(), su.c_str(), nm.c_str(),
				progStatus.chP1[0]);
		} else if (!strP2.empty() &&
			strP2.find(st) != std::string::npos) {
			callinlist.add (
				record_number,
				pr.c_str(), ar.c_str(), su.c_str(), nm.c_str(),
				progStatus.chP2[0]);
		} else if (!strP3.empty() &&
			strP3.find(st) != std::string::npos) {
			callinlist.add (
				record_number,
				pr.c_str(), ar.c_str(), su.c_str(), nm.c_str(),
				progStatus.chP3[0]);
		} else {
			callinlist.add (
				record_number,
				pr.c_str(), ar.c_str(), su.c_str(), nm.c_str() );
		}
	}
	clearPickList ();
	clearSAP ();
	return record_number;
}

my_UI::my_UI (int x, int y, int w, int h, const char *l) : Fl_Input(x,y,w,h,"")
//Fl_Group(x,y,w,h)
{
	callinlist = loglist ();
	my_status = LOGLIST;
//	end();
}

void my_UI::clearSAP ()
{
	szPrefix.clear(); szArea.clear(); szSuffix.clear();
	boxLoginSuffix->labelcolor (FL_BLACK);
	boxLoginSuffix->color (FL_WHITE);
	boxLoginPrefix->labelcolor (FL_BLACK);
	boxLoginPrefix->color (FL_WHITE);
	boxLoginArea->labelcolor (FL_BLACK);
	boxLoginArea->color (FL_WHITE);
	boxLoginPrefix->label (szPrefix.c_str());
	boxLoginArea->label (szArea.c_str());
	boxLoginSuffix->label (szSuffix.c_str());
	my_status = LOGLIST;
}

static int oldkey = 0;
static int keywait = 0;

void debounce (void *d)
{
	keywait = 0;
}

/*
std::string binary(int val)
{
	static std::string ans;
	ans.assign("000000000000000000000000");
	int d = 2;
	for (size_t i = 0; i < ans.length(); i++) {
		if (d & val) ans[ans.length() -1 - i] = '1';
		d *= 2;
	}
	return ans;
}

std::string keycode(int val)
{
	static std::string ans;
	if (val >= '0' && val <= 'z') ans.assign(1,char(val));
	else {
		switch (val) {
			case 65307: ans.assign("ESC"); break;
			case 65470: ans.assign("F1"); break;
			case 65471: ans.assign("F2"); break;
			case 65472: ans.assign("F3"); break;
			case 65473: ans.assign("F4"); break;
			case 65474: ans.assign("F5"); break;
			case 65475: ans.assign("F6"); break;
			case 65476: ans.assign("F7"); break;
			case 65477: ans.assign("F8"); break;
			case 65478: ans.assign("F9"); break;
			case 65479: ans.assign("F10"); break;
			case 65480: ans.assign("F11"); break;
			case 65481: ans.assign("F12"); break;
			case 65482: ans.assign("F13"); break;
			case 65483: ans.assign("F14"); break;
			case 65484: ans.assign("F15"); break;
			case 65485: ans.assign("F16"); break;
			case 65486: ans.assign("F17"); break;
			case 65487: ans.assign("F18"); break;
			case 65488: ans.assign("F19"); break;
			case 65361: ans.assign("LEFT"); break;
			case 65362: ans.assign("UP"); break;
			case 65363: ans.assign("RIGHT"); break;
			case 65364: ans.assign("DOWN"); break;
			case 65360: ans.assign("HOME"); break;
			case 65367: ans.assign("END"); break;
			case 65293: ans.assign("Enter"); break;
			default: ans.assign("other"); break;
		}
	}
	return ans;
}
*/

static bool ignore_event = false;
int my_UI::handle (int e)
{
	int k = Fl::event_key();
	int state = Fl::event_state();

//	if (!k || !e) return 0;  // not keyboard events

	if (keywait && oldkey == k) { // debounce
		return 1;
	}

/*
std::cout << 
"key:       " << k << " " << keycode(k) <<
"\nevt:     " << binary(e) << 
"\ndwn:     " << binary(FL_KEYDOWN) <<
"\nup :     " << binary(FL_KEYUP) <<
"\ncut:     " << binary(FL_SHORTCUT) << 
"\nstate:   " << binary(state) <<
"\nFL_META: " << binary(FL_META) <<
"\nFL_ALT:  " << binary(FL_ALT) << 
"\nFL_CTRL: " << binary(FL_CTRL) << std::endl;
*/

#ifdef __APPLE__
// keydown event for Cmd-Q
	if ( (state & FL_META) == FL_META ) {
		if (k == 'Q' || k == 'q') { 
			oldkey = k;
			keywait = 1; // disable all future keyboard events
			cleanExit();
			return 1;
		} else
			return 0;
	}
#endif

	if ((state & FL_ALT) == FL_ALT) {
		if ( (e & FL_KEYDOWN) == FL_KEYDOWN)
			switch (k) {
				case 'E' : case 'e' :
				cbEditor ();
				break;
			case 'C' : case 'c' :
				cbConfig ();
				break;
			case 'I' : case 'i' :
				open_log_ins ();
				break;
			case 'Z' : case 'z' :
				change_size();
				break;
			case FL_F + 4 :
				cleanExit();
				break;
			default:
				break;
		}
		Fl::add_timeout (0.1, debounce);
		oldkey = k;
		keywait = 1;
		return 1;
	}

	if (k == FL_Up || k == FL_Down) {
		if (e == FL_SHORTCUT) {					// keydown event
			if (my_status == LOGLIST) {
				if (k == FL_Up) {
					WhoIsUp--;
					if (WhoIsUp < 0)
						WhoIsUp++;
					dispCallIns (false);
				} else {
					WhoIsUp++;
					if (WhoIsUp == callinlist.numlist ())
						WhoIsUp--;
					dispCallIns (false);
				}
			}
			else if (my_status == SUFFIX) {
				if (nbrPicked) {
					boxLoginSuffix->color (FL_DARK_RED);
					boxLoginSuffix->labelcolor (FL_WHITE);
					boxLoginSuffix->redraw ();
					if (nbrPicked > 1) {
						if (k == FL_Down)
							whoPicked = 1;
						else
							whoPicked = nbrPicked - 1;
					} else
						whoPicked = 0;
					PickedColors();
					my_status = PICKLIST;
				}
			} 
			else if (my_status == PICKLIST) {
				if (k == FL_Down) {
					whoPicked++;
					if (whoPicked > nbrPicked -1) whoPicked = 0;
					PickedColors();
				} else {
					whoPicked--;
					if (whoPicked < 0) whoPicked = nbrPicked -1;
					PickedColors();
				}
			}
			Fl::add_timeout (0.1, debounce);
			oldkey = k;
			keywait = 1;
		}
	} // keydown event processing
	else
		keywait = 0;

	if (e != FL_KEYDOWN && e != FL_SHORTCUT) {
		return 1;
	}

	if (k == FL_Escape && my_status != LOGLIST) {
		clearSAP ();
		clearPickList ();
		return 1;
	}

	if (my_status == LOGLIST) {
		if (k == FL_Home) {
			lastUp = WhoIsUp;
			WhoIsUp = 0;
			dispCallIns (false);
			return 1;
		}
		if (k == FL_End) {
			lastUp = WhoIsUp;
			WhoIsUp = callinlist.numlist () - 1;
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 1) {
			callinlist.status(WhoIsUp, LOGIN);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 2) {
			callinlist.status(WhoIsUp, FIRST);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 3) {
			callinlist.status(WhoIsUp, SECOND);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 4) {
			callinlist.status(WhoIsUp, LOGOUT);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 5) { // Priority 0 station
			WhoIsUp = callinlist.Pri_0 (WhoIsUp);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 6) { // Priority 1 station
			WhoIsUp = callinlist.Pri_1 (WhoIsUp);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 7) { // Priority 2 station
			WhoIsUp = callinlist.Pri_2 (WhoIsUp);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 8) { // Priority 2 station
			WhoIsUp = callinlist.Pri_3 (WhoIsUp);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 9) { // Move this call up in list
			WhoIsUp = callinlist.MoveEarlier (WhoIsUp);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 10) { // Move this call dn in list
			WhoIsUp = callinlist.MoveLater (WhoIsUp);
			dispCallIns (false);
			return 1;
		}
		if (k == FL_F + 12) {
			if((Fl::event_state() & FL_SHIFT) == FL_SHIFT)
				cb_ShiftF12();
			else
				cb_F12 (WhoIsUp);
			return 1;
		}
		if (k == FL_Delete) {
			if (!((WhoIsUp == 0) && (callinlist.status(WhoIsUp) == EMPTY))) {
				if (fl_choice("Confirm Delete", "cancel", "OK", NULL) == 1) {
					callinlist.del(WhoIsUp);
					WhoIsUp--;
					if (WhoIsUp < 0) WhoIsUp = 0;
					updateLogins();
					dispCallIns (false);
				}
			}
			inp_focus->take_focus();
			ignore_event = true;
			return 1;
		}
		if (k == FL_Enter || k == FL_KP_Enter) {
			if (!ignore_event) {
				dispCallIns(updateFldigi);
			}
			ignore_event = false;
			return 1;
		}
	}

	if (my_status == PICKLIST) {
		if (k == FL_Enter || k == FL_KP_Enter) {
			int recnbr = PickedToCallins (whoPicked);
			if (progStatus.callin_is_up && recnbr >=0) {
				csvRecord rec;
				netdb.get(recnbr, rec);
				WhoIsUp = callinlist.locate(rec.prefix, rec.area, rec.suffix);
			}
			updateLogins();
			dispCallIns(false);
			my_status = LOGLIST;
		}
	}

	if (my_status == SUFFIX && (k == FL_Enter || k == FL_KP_Enter)) {
		if (nbrPicked) {
			whoPicked = 0;
			int recnbr = PickedToCallins (whoPicked);
			if (progStatus.callin_is_up && recnbr >=0) {
				csvRecord rec;
				netdb.get(recnbr, rec);
				WhoIsUp = callinlist.locate(rec.prefix, rec.area, rec.suffix);
			}
			updateLogins();
			my_status = LOGLIST;
			dispCallIns (false);
		}
	}

	if ( (k >= 'A' && k <= 'z') || (k >= '0' && k <= '9') )	{
		if (isalpha (k))
			keyval[0] = toupper(k);
		else
			keyval[0] = k;

		switch (my_status) {
			case LOGLIST :
				szSuffix.clear();
				szSuffix = keyval;
				boxLoginSuffix->label (szSuffix.c_str());
				fillPickList ();
				my_status = SUFFIX;
				boxLoginSuffix->labelcolor (FL_WHITE);
				boxLoginSuffix->color (FL_DARK_BLUE);
				break;
			case SUFFIX :
				szSuffix.append(keyval);
				if (szSuffix.length() > 3) szSuffix.erase(0,1);
				boxLoginSuffix->label (szSuffix.c_str());
				fillPickList ();
				break;
			case PREFIX :
				szPrefix.append(keyval);
				if (szPrefix.length() > 2) szPrefix.erase(0,1);
				boxLoginPrefix->label (szPrefix.c_str());
				break;
			case AREA :
				szArea = keyval;
				boxLoginArea->label (szArea.c_str());
				break;
			default : ;
		}
		return 1;
	}

	if (k == FL_Tab) {
		switch (my_status) {
			case SUFFIX :
				boxLoginSuffix->labelcolor (FL_BLACK);
				boxLoginSuffix->color (FL_WHITE);
				boxLoginPrefix->labelcolor (FL_WHITE);
				boxLoginPrefix->color (FL_DARK_BLUE);
				boxLoginPrefix->label (szPrefix.c_str());
				my_status = PREFIX;
				break;
			case PREFIX :
				boxLoginPrefix->labelcolor (FL_BLACK);
				boxLoginPrefix->color (FL_WHITE);
				boxLoginArea->labelcolor (FL_WHITE);
				boxLoginArea->color (FL_DARK_BLUE);
				boxLoginArea->label (szArea.c_str());
				my_status = AREA;
				break;
			case AREA :
			default :
				break;
		}
		return 1;
	}

	if ((k == FL_Enter || k == FL_KP_Enter) && my_status == AREA) {
		int found = IsInDB(szPrefix.c_str(), szArea.c_str(), szSuffix.c_str());
		if (found >= 0) {
			PickedToCallinsDB(found);
			dispCallIns (false);
			return 1;
		}

		callinlist.add (-1, szPrefix.c_str(), szArea.c_str(), szSuffix.c_str(), "" );
		if (progStatus.disp_new_login) WhoIsUp = callinlist.numlist () - 1;
		dispCallIns (false);
		clearPickList ();
		clearSAP ();
		if (progStatus.disp_new_login && progStatus.open_editor) cb_F12 (WhoIsUp);
		updateLogins();
		return 1;
	}

	return 0;
}

void add_to_callins(int rnbr)
{
	int recnbr = inp_focus->PickedToCallinsDB(rnbr);
//	myUI->PickedToCallinsDB(rnbr);
	csvRecord rec;
	netdb.get(recnbr, rec);

	if (progStatus.callin_is_up && recnbr >=0) {
		WhoIsUp = callinlist.locate(rec.prefix, rec.area, rec.suffix);
	}

	updateLogins();
//	myUI->dispCallIns (false);
	inp_focus->dispCallIns (false);
}
