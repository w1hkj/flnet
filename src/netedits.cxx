// Editor dialogs for both 16 & 24 field database structures

#include "netedits.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "netshared.h"
#include "netutils.h"
#include "date.h"
#include "calendar.h"

static void cb_mnuSortByAPS(Fl_Menu_*, void*) {
  SortByAPS ();
cbGoFirstRec (NULL,NULL);
}

static void cb_mnuSortBySAP(Fl_Menu_*, void*) {
  SortBySAP ();
cbGoFirstRec (NULL,NULL);
}

static void cb_mnuSortByNetNbr(Fl_Menu_*, void*) {
  SortByNetNbr ();
cbGoFirstRec (NULL,NULL);
}

Fl_Menu_Item menu_mbarMain[] = {
 {"&Sort", 0,  0, 0, 64, 0, 0, 14, 56},
 {"&Area/Prefix/Suffix", 0,  (Fl_Callback*)cb_mnuSortByAPS, 0, 0, 0, 0, 14, 56},
 {"&Suffix/Area/Prefix", 0,  (Fl_Callback*)cb_mnuSortBySAP, 0, 0, 0, 0, 14, 56},
 {"&Net Number", 0,  (Fl_Callback*)cb_mnuSortByNetNbr, 0, 0, 0, 0, 14, 56},
 {0},
 {"Sea&rch", 0,  0, 0, 64, 0, 0, 14, 56},
 {"&Callsign", 0,  (Fl_Callback*)cb_mnuSearchCallsign, 0, 0, 0, 0, 14, 56},
 {"&NetNbr", 0,  (Fl_Callback*)cb_mnuSearchNetNbr, 0, 0, 0, 0, 14, 56},
 {0},
 {"&Browse", 0,  0, 0, 64, 0, 0, 14, 56},
 {"&Callsign", 0,  (Fl_Callback*)cb_mnuBrowseCallsign, 0, 0, 0, 0, 14, 56},
 {"&NetNbr", 0,  (Fl_Callback*)cb_mnuBrowseNetNbr, 0, 0, 0, 0, 14, 56},
 {0},
 {"QRZ", 0,  (Fl_Callback*)cb_mnuQRZ, 0, 0, 0, 0, 14, 56},
 {0}
};

Fl_Menu_Bar *mbarMain = (Fl_Menu_Bar *)0;
Fl_Input *inpPrefix=(Fl_Input *)0;
Fl_Input *inpArea=(Fl_Input *)0;
Fl_Input *inpSuffix=(Fl_Input *)0;
Fl_Input *inpNickname=(Fl_Input *)0;
Fl_Input *inpNetNbr=(Fl_Input *)0;
Fl_Input *inpFname=(Fl_Input *)0;
Fl_Input *inpLname=(Fl_Input *)0;
Fl_Input *inpAddress=(Fl_Input *)0;
Fl_Input *inpCity=(Fl_Input *)0;
Fl_Input *inpState=(Fl_Input *)0;
Fl_Input *inpZip=(Fl_Input *)0;
Fl_Input *inpPhone=(Fl_Input *)0;
Fl_DateInput *inpBirthday=(Fl_DateInput *)0;
Fl_Input *inpSpouse=(Fl_Input *)0;
Fl_Input *inpSpBirthday=(Fl_Input *)0;
Fl_DateInput *txtLogDate=(Fl_DateInput *)0;
Fl_Button *btnFirst=(Fl_Button *)0;
Fl_Button *btnPrev=(Fl_Button *)0;
Fl_Button *btnNext=(Fl_Button *)0;
Fl_Button *btnLast=(Fl_Button *)0;
Fl_Button *btnNewSave=(Fl_Button *)0;
Fl_Button *btnDelete=(Fl_Button *)0;
Fl_Button *btnUpdateCancel=(Fl_Button *)0;
Fl_Button *btnClose=(Fl_Button *)0;
Fl_Output *lblFileName=(Fl_Output *)0;
Fl_Output *lblNumRecs=(Fl_Output *)0;
Fl_Input *inpCallsign=(Fl_Input *)0;
Fl_DateInput *inpPrevDate=(Fl_DateInput *)0;
Fl_Input *inpNbrLogins=(Fl_Input *)0;
Fl_Input *inpStatus=(Fl_Input *)0;
Fl_DateInput *inpJoined=(Fl_DateInput *)0;
Fl_Input *inpEmail=(Fl_Input *)0;
Fl_Input *inpComment1=(Fl_Input *)0;
Fl_Input *inpComment2=(Fl_Input *)0;

Fl_Window * newEditWindow() 
{
  Fl_Window *dlgNetEdit = 
		new Fl_Window(	main_window->x() + main_window->w() + 10,
								main_window->y(),
								535, 460, "Database Editor");
  dlgNetEdit->color(52);

  mbarMain = new Fl_Menu_Bar(0, 0, 535, 25);
  mbarMain->menu(menu_mbarMain);

  inpPrefix = new Fl_Input(35, 75, 35, 25, "Prefix");
  inpPrefix->align(FL_ALIGN_TOP_LEFT);

  inpArea = new Fl_Input(80, 75, 30, 25, "Area");
  inpArea->align(FL_ALIGN_TOP_LEFT);

  inpSuffix = new Fl_Input(125, 75, 45, 25, "Suffix");
  inpSuffix->align(FL_ALIGN_TOP_LEFT);

  inpNickname = new Fl_Input(230, 75, 125, 25, "Nickname");
  inpNickname->align(FL_ALIGN_TOP_LEFT);

  inpNetNbr = new Fl_Input(385, 75, 60, 25, "Net #");
  inpNetNbr->align(FL_ALIGN_TOP_LEFT);

  inpFname = new Fl_Input(80, 205, 105, 25, "Name:");
  inpLname = new Fl_Input(190, 205, 170, 25);
  inpAddress = new Fl_Input(80, 235, 280, 25, "Address:");
  inpCity = new Fl_Input(80, 265, 190, 25, "City/St/Zip:");
  inpState = new Fl_Input(275, 265, 35, 25);
  inpZip = new Fl_Input(320, 265, 50, 25);
  inpPhone = new Fl_Input(385, 265, 135, 25, "Phone:");
  inpPhone->align(FL_ALIGN_TOP_LEFT);

  inpBirthday = new Fl_DateInput(385, 205, 110, 25, (char*)"Birthday:");
  inpBirthday->align(FL_ALIGN_TOP_LEFT);
  inpBirthday->format(1);
    
  inpSpouse = new Fl_Input(80, 325, 85, 25, "Spouse:");
  inpSpBirthday = new Fl_Input(235, 325, 75, 25, "Birthday:");
  txtLogDate = new Fl_DateInput(80, 140, 110, 25, (char*)"Log Date:");
  txtLogDate->format(2);
 
  btnFirst = new Fl_Button(40, 420, 25, 25, "@|<");
  btnFirst->callback((Fl_Callback*)cbGoFirstRec);

  btnPrev = new Fl_Button(65, 420, 25, 25, "@<");
  btnPrev->callback((Fl_Callback*)cbGoPrevRec);

  btnNext = new Fl_Button(90, 420, 25, 25, "@>");
  btnNext->callback((Fl_Callback*)cbGoNextRec);

  btnLast = new Fl_Button(115, 420, 25, 25, "@>|");
  btnLast->callback((Fl_Callback*)cbGoLastRec);

  btnNewSave = new Fl_Button(185, 420, 50, 25, "New");
  btnNewSave->callback((Fl_Callback*)cb_btnNewSave);

  btnDelete = new Fl_Button(305, 420, 50, 25, "Delete");
  btnDelete->callback((Fl_Callback*)cb_btnDelete);

  btnUpdateCancel = new Fl_Button(245, 420, 50, 25, "Update");
  btnUpdateCancel->callback((Fl_Callback*)cb_btnUpdateCancel);

  btnClose = new Fl_Button(420, 420, 50, 25, "Close");
  btnClose->callback((Fl_Callback*)cb_CloseEditor);

  lblFileName = new Fl_Output(25, 30, 410, 25);
  lblFileName->box(FL_BORDER_BOX);
  lblFileName->color(52);

  lblNumRecs = new Fl_Output(440, 30, 85, 25);
  lblNumRecs->box(FL_BORDER_BOX);
  lblNumRecs->color(53);

  inpCallsign = new Fl_Input(80, 105, 85, 25, "Callsign:");

  inpPrevDate = new Fl_DateInput(200, 140, 110, 25, (char*)"Prev Date:");
  inpPrevDate->format(2);
  inpPrevDate->align(FL_ALIGN_TOP_LEFT);

  inpNbrLogins = new Fl_Input(315, 140, 85, 25, "Nbr Logins:");
  inpNbrLogins->align(FL_ALIGN_TOP_LEFT);

  inpStatus = new Fl_Input(80, 175, 25, 25, "Status:");
  inpJoined = new Fl_DateInput(180, 175, 110, 25, (char*)"Joined:");
  inpJoined->format(2);
  inpEmail = new Fl_Input(80, 295, 440, 25, "Email:");
  inpComment1 = new Fl_Input(80, 355, 440, 25, "Comments");
  inpComment2 = new Fl_Input(80, 385, 440, 25);
  dlgNetEdit->set_modal();
  dlgNetEdit->end();

  return dlgNetEdit;
}

