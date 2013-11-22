#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <mem.h>

#include "FL/Fl.H"

#include "config.h"
#include "net_ui.h"
#include "loglist.h"

extern loglist callinlist;

char chP1[] = "A";
char chP2[] = "B";
char chP3[] = "C";
char chAuto = 'n';
char szP1[256] = "";
char szP2[256] = "";
char szP3[256] = "";

Fl_Color  fgColors[] = {FL_BLACK, FL_BLACK, FL_WHITE, FL_YELLOW, FL_WHITE};
Fl_Color  bgColors[] = {FL_BLACK, FL_WHITE, FL_BLUE, FL_DARK_GREEN, FL_DARK_RED};

extern char sDbFileName[];

void readConfig ()
{
  char filename[120];
  FILE *cfgFile;
  int fg1,fg2,fg3,fg4, bg1,bg2,bg3,bg4;
  strcpy (filename, sDbFileName);
  strcat (filename, ".cfg");

  cfgFile = fopen (filename, "r");
  if (cfgFile) {
    fscanf (cfgFile,
            "%c%c%c%c\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
            chP1, chP2, chP3, &chAuto,
            szP1, szP2, szP3,
			&fg1, &fg2, &fg3, &fg4, &bg1, &bg2, &bg3, &bg4);
	fgColors[1] = (Fl_Color)fg1;
	fgColors[2] = (Fl_Color)fg2;
	fgColors[3] = (Fl_Color)fg3;
	fgColors[4] = (Fl_Color)fg4;
	bgColors[1] = (Fl_Color)bg1;
	bgColors[2] = (Fl_Color)bg2;
	bgColors[3] = (Fl_Color)bg3;
	bgColors[4] = (Fl_Color)bg4;
    fclose (cfgFile);
  }
}

void writeConfig ()
{
  char filename[120];
  FILE *cfgFile;
  strcpy (filename, sDbFileName);
  strcat (filename, ".cfg");
  cfgFile = fopen (filename, "w");
  if (!*szP1) strcpy(szP1, "none");
  if (!*szP2) strcpy(szP2, "none");
  if (!*szP3) strcpy(szP3, "none");
  if (cfgFile) {
    fprintf (cfgFile,
            "%c%c%c%c\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
            chP1[0], chP2[0], chP3[0], chAuto,
            szP1, szP2, szP3,
            fgColors[1], fgColors[2], fgColors[3], fgColors[4],
            bgColors[1], bgColors[2], bgColors[3], bgColors[4]);
    fclose(cfgFile);
  }
}

static Fl_Window *cfgDialog = 0;

void cbConfig ()
{
  if (!cfgDialog)
    cfgDialog = configDialog ();
  cfgP1->value (chP1);
  cfgP2->value (chP2);
  cfgP3->value (chP3);
  inpStatesList1->value(szP1);
  inpStatesList2->value(szP2);
  inpStatesList3->value(szP3);
  if (chAuto == 'y' || chAuto == 'Y')
    chkAutoPriority->value (1);
  else
    chkAutoPriority->value (0);
  cfgDialog->show();
}

void cbCloseConfig ()
{
  strncpy (chP1, cfgP1->value(), 1);
  if (strlen(chP1) == 0) chP1[0] = ' ';
  strncpy (chP2, cfgP2->value(), 1);
  if (strlen(chP2) == 0) chP2[0] = ' ';
  strncpy (chP3, cfgP3->value(), 1);
  if (strlen(chP3) == 0) chP3[0] = ' ';

  strncpy (szP1, inpStatesList1->value(), 255);
  strncpy (szP2, inpStatesList2->value(), 255);
  strncpy (szP3, inpStatesList3->value(), 255);

  callinlist.setPri_1 (chP1[0]);
  callinlist.setPri_2 (chP2[0]);
  callinlist.setPri_3 (chP3[0]);

  if (chkAutoPriority->value() == 1) {
    chAuto = 'y';
    callinlist.AutoPriority (1);
  } else {
    chAuto = ' ';
    callinlist.AutoPriority (0);
  }

  writeConfig ();

  cfgDialog->hide ();

}
