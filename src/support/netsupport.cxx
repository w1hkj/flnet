// =====================================================================
//
// netsupport.cxx
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
#include <string>

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

#include "my_UI.h"
#include "net_ui.h"
#include "netshared.h"
#include "netversion.h"
#include "loglist.h"
#include "netsupport.h"
#include "xml_io.h"
#include "debug.h"
#include "masterdb.h"
#include "status.h"
#include "icons.h"

#include "config.h"

static Fl_Window *about = ( Fl_Window * )0;
//Fl_Window *editor = NULL;
//Fl_Window *content = ( Fl_Window * )0;

extern void updateLogins (bool closing);
extern void clear_outfilename();

void cb_mnuOpen(Fl_Menu_*mnu, void *d)
{
	string open_dir = home_dir;
	char *p = fl_file_chooser (
		"Select .csv file", "*.csv", 
		open_dir.c_str(), 0);
	if (!p) return;

	updateLogins (true);
	closeDB();

	selected_file = p;
	clear_outfilename();
	callinlist.clear ();
	getindexed_list ();
	WhoIsUp = 0;
	updateCallins ();

	LOG_INFO("Open database %s", selected_file.c_str());

	if (progStatus.masterdb == selected_file) {
		fl_alert2("You are using the master DB\n\n%s\n\nMaster db will be closed!", netdb.filename().c_str());
		close_masterdb();
	}

	openDB (selected_file.c_str());
}

void cb_mnuNew(Fl_Menu_*mnu, void *d)
{
	selected_file = home_dir;
	selected_file.append("newdb.csv");
	char *p = fl_file_chooser ("New .csv file", "*.csv", selected_file.c_str(), 0);
	if (!p) return;
	selected_file = p;

	clear_outfilename();
	updateLogins (true);
	callinlist.clear ();
	getindexed_list ();
	WhoIsUp = 0;
	updateCallins ();

	LOG_INFO("New database %s", selected_file.c_str());
	openDB (selected_file.c_str());
}

void cb_CloseEditor (Fl_Button *b, void *d)
{
	cbCloseEditor ();
}


void cb_mnuEditor(Fl_Menu_ *mnu, void *d)
{
	cbEditor ();
}

void close_about(void)
{
	if(about)
		about->hide();
}

extern void	close_main_window(void);
extern void	close_config(void);
extern void close_editor(void);
extern void close_login_list(void);
extern void close_misc_dialogs(void);

void cleanExit(void)
{
	updateLogins (true);
	progStatus.saveLastState();

	closeDB();
	close_masterdb();
	close_xmlrpc();

	debug::stop();

	close_config();
	close_about();
	close_editor();
	close_login_list();
	close_misc_dialogs();
	close_main_window();

	exit(0);
}

void cb_mnuExit (Fl_Menu_ *m, void *d)
{
	cleanExit();
}

void cb_mnuConfig (Fl_Menu_ *m, void *d)
{
	cbConfig ();
}

void cb_btnCloseConfig (Fl_Return_Button *b, void *d)
{
	cbCloseConfig ();
}

void cb_mnuMigrate (Fl_Menu_ *m, void *d)
{
}

static char szVersion[128];

void cb_mnuAbout(Fl_Menu_ *mnu, void *d)
{
	if (!about) {
		memset(szVersion, 0, sizeof(szVersion));
		snprintf (szVersion, sizeof(szVersion) - 1,
					"Net control program\n"
					"Version %s\n"
					"Free Hamware From\n"
					"W1HKJ\n\n"
					"Report problems to:", FLNET_VERSION);
		about = newAboutDialog ();
		lblVersion->label (szVersion);
		lblVersion->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
	}
	about->show ();
}

void cb_mnuHelpContent (Fl_Menu_ *mnu, void *d)
{
	visit_URL((void*)"http://www.w1hkj.com/Net-help/index.html");
	return;
}

void cb_mnuEventLog (Fl_Menu_ *mnu, void *d)
{
	debug::show();
}

//------------------------------------------------------------------------------
// support of log-in list viewer
//------------------------------------------------------------------------------
Fl_Double_Window *login_list = (Fl_Double_Window *)0;
string copy_list;

void close_login_list(void)
{
	if(login_list)
		login_list->hide();
}

void open_log_ins()
{
	if (!login_list) login_list = Log_ins_dialog();
	if (callinlist.numlist() == 0) return;

	char szLine[40];
	log_in_view->clear();
	copy_list.clear();

	static int widths[] = { 75, 165, 0 };  // widths for each column
	log_in_view->column_widths(widths); // assign array to widget

	for (int i = 0; i < callinlist.numlist(); i++) {
		strcpy(szLine, callinlist.report_line(i));
		log_in_view->add(szLine);
		copy_list.append(szLine).append("\n");
	}
	login_list->show();
}

void copy_to_clipboard()
{
	// copy list to clipboard
	Fl::copy(copy_list.c_str(), copy_list.length(), 1);
}
