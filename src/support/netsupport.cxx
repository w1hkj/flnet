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
#include <FL/fl_draw.H>

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

	if (editor) editor->hide();

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
extern void close_field_select(void);

void cleanExit(void)
{
	switch (fl_choice2("Exit program", 0, "Cancel", "Yes")) {
		case 0: return; // Esc pressed
		case 1: return; // Cancel button pressed, Enter pressed
		case 2: break;  // Yes button pressed
	}
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
	close_field_select();
	close_main_window();
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

void close_login_list(void)
{
	if(login_list)
		login_list->hide();
}

static int widths[30];

void update_log_ins()
{
	if (!log_in_view) return;
	log_in_view->clear();

	int N = callinlist.numlist();
	std::string lines[N + 1];

	fl_font(log_in_view->textfont(), log_in_view->textsize());

	for (int i = 0; i < 30; i++) widths[i] = 0;

	lines[0] = callinlist.header_line(widths);
	for (int i = 0; i < N; i++) {
		lines[i+1] = callinlist.report_line(i, widths);
	}

	int nuW = 0;
	for (int i = 0; i < 30; i++)  {
		nuW += widths[i];
	}

	nuW += 30;
	if (nuW < 300) nuW = 300;
	if (nuW > 800) nuW = 800;

	log_in_view->column_widths(widths); // assign array to widget
	log_in_view->resize(log_in_view->x(), log_in_view->y(), nuW, log_in_view->h());

	login_list->size(nuW + 8, login_list->h());
	login_list->redraw();

	btn_copy_to_clipboard->position(nuW + 8 - 150, 210);
	btn_copy_to_clipboard->redraw();

	btn_close_log_ins->position(nuW + 8 - 75, 210);
	btn_close_log_ins->redraw();

	for (int i = 0; i <= N; i++) {
		log_in_view->add(lines[i].c_str());
	}
	log_in_view->redraw();

	updateLogins(false);
}

void open_log_ins()
{
	if (!login_list) login_list = Log_ins_dialog();
	if (callinlist.numlist() == 0) return;

	update_log_ins();

	login_list->show();

}

void copy_to_clipboard()
{
	Fl::copy(ascii_copy.c_str(), ascii_copy.length(), 1);
}
