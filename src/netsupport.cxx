#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

#include "net_ui.h"
#include "netshared.h"
#include "netversion.h"
#include "loglist.h"
#include "netsupport.h"

#include "config.h"

Fl_Window *editor = NULL;

Fl_Window *about = NULL;
Fl_Window *content = NULL;

extern void updateLogins ();

void cb_mnuOpen(Fl_Menu_*mnu, void *d)
{
	char *p = fl_file_chooser ("Select .csv file", "*.csv", selected_file.c_str(), 0);
	if (!p) return;
	selected_file = p;
	updateLogins ();
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

void cleanExit()
{
  updateLogins ();
  closeDB();
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

char szVersion[80];

void cb_mnuAbout(Fl_Menu_ *mnu, void *d)
{
  if (!about) {
    sprintf (szVersion, "\
Net control program\n\
Version %s\n\
Free Hamware From\n\
W1HKJ\n\n\
Report problems to:", flnet_VERSION);
    about = newAboutDialog ();
    lblVersion->label (szVersion);
  }
  about->show ();
}

void cb_mnuHelpContent (Fl_Menu_ *mnu, void *d)
{
	FILE *tfile = fopen("Net-help/index.html", "r");
	if (tfile) {
		fclose(tfile);
		visit_URL((void*)"Net-help/index.html");
	} else
		visit_URL((void*)"http://www.w1hkj.com/Net-help/index.html");
	return;
}

//------------------------------------------------------------------------------
// support of log-in list viewer
//------------------------------------------------------------------------------
Fl_Double_Window *login_list = (Fl_Double_Window *)0;
string copy_list;

void open_log_ins()
{
	if (!login_list) login_list = Log_ins_dialog();
	if (callinlist.numlist() == 0) return;

	char szLine[40];
	log_in_view->clear();
	copy_list.clear();

	static int widths[] = { 80, 80, 80, 0 };  // widths for each column
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
