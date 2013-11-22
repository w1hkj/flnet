#include <stdlib.h>
#include <stdio.h>

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
  char *szChoosen =
    fl_file_chooser ("Select .csv file", "*.csv", "", 0);
  if (szChoosen == NULL)
    return;
  updateLogins ();
  openDB (szChoosen);
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
