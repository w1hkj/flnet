// =====================================================================
//
// net_ui.cxx
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

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Scroll.H>

#include "icons.h"
#include "flinput2.h"
#include "net_ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "netsupport.h"
#include "netshared.h"
#include "config.h"
#include "net_config.h"
#include "debug.h"
#include "lookupcall.h"
#include "status.h"
#include "combo.h"
#include "masterdb.h"

Fl_Menu_Bar *mnu_bar=(Fl_Menu_Bar *)0;
Fl_Group *tabGroupColors=(Fl_Group *)0;
Fl_Group *tabGroupPriority = (Fl_Group *)0;
Fl_Group *tabGroupLookup = (Fl_Group *)0;
Fl_Group *tabGroupUI = (Fl_Group *)0;
Fl_Group *tabGroupMasterDB = (Fl_Group *)0;
Fl_Group *tabGroupSharedFields = (Fl_Group *)0;

Fl_Input2 *cfgP1 = (Fl_Input2 *)0;
Fl_Input2 *inpStatesList1 = (Fl_Input2 *)0;
Fl_Input2 *cfgP2 = (Fl_Input2 *)0;
Fl_Input2 *inpStatesList2 = (Fl_Input2 *)0;
Fl_Input2 *cfgP3 = (Fl_Input2 *)0;
Fl_Input2 *inpStatesList3 = (Fl_Input2 *)0;
Fl_Check_Button *chkAutoPriority = (Fl_Check_Button *)0;

Fl_Input2 *inp_myLocator = (Fl_Input2 *)0;
Fl_Input2 *inp_user_name = (Fl_Input2 *)0;
Fl_Input2 *inp_user_password = (Fl_Input2 *)0;
Fl_Check_Button *chk_pwd = (Fl_Check_Button *)0;

Fl_Check_Button *chk_callook = (Fl_Check_Button *)0;
Fl_Input2 *inp_callookurl = (Fl_Input2 *)0;

Fl_Check_Button *chk_hamqth = (Fl_Check_Button *)0;
Fl_Input2 *inp_hamqthurl = (Fl_Input2 *)0;

Fl_Check_Button *chk_hamcall = (Fl_Check_Button *)0;
Fl_Input2 *inp_hamcallurl = (Fl_Input2 *)0;

Fl_Check_Button *chk_qrz = (Fl_Check_Button *)0;
Fl_Input2 *inp_qrzurl = (Fl_Input2 *)0;

Fl_Input2 *inp_masterdb = (Fl_Input2 *)0;
Fl_Box *box_mdb_isopen = (Fl_Box *)0;
Fl_Button *btn_masterdb = (Fl_Button *)0;
Fl_Button *btn_open_masterdb = (Fl_Button *)0;
Fl_Button *btn_close_masterdb = (Fl_Button *)0;
Fl_Button *mdb_color = (Fl_Button *)0;

Fl_Check_Button *chk_mdb_netnbr = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_prefix = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_area = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_suffix = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_callsign = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_name = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_fname = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_lname = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_addr = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_city = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_state = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_zip = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_phone = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_birthdate = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_spouse = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_sp_birth = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_comment1 = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_comment2 = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_email = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_locator = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_country = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_logdate = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_prevdate = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_nbrlogins = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_status = (Fl_Check_Button *)0;
Fl_Check_Button *chk_mdb_joined = (Fl_Check_Button *)0;

Fl_Choice     *cbo_call_justify = (Fl_Choice *)0;
Fl_Choice     *cbo_name_justify = (Fl_Choice *)0;

Fl_Choice *combo_arc_conversion = (Fl_Choice *)0;

Fl_Return_Button *btnConfigOK = (Fl_Return_Button *)0;

Fl_Group *grp_sort_order = (Fl_Group*)0;
Fl_Check_Button *btn_sort_by_PAS = (Fl_Check_Button*)0;
Fl_Check_Button *btn_sort_by_APS = (Fl_Check_Button*)0;
Fl_Check_Button *btn_sort_by_SAP = (Fl_Check_Button*)0;
Fl_Check_Button *btn_sort_by_NETNBR = (Fl_Check_Button*)0;

Fl_Tabs *tabsConfig=(Fl_Tabs *)0;

Fl_Check_Button *btn_new_login_is_up=(Fl_Check_Button *)0;
Fl_Check_Button *btnOpenEditor=(Fl_Check_Button *)0;
Fl_Check_Button *btn_current_call_in_is_up=(Fl_Check_Button *)0;
Fl_Output *txtSample[6]={(Fl_Output *)0};
Fl_Button *btnFg[6]={(Fl_Button *)0};
Fl_Button *btnBg[6]={(Fl_Button *)0};

//----------------------------------------------------------------------
// main dialog
//----------------------------------------------------------------------

//my_UI *myUI=(my_UI *)0;

my_UI *inp_focus=(my_UI *)0;

Fl_Group *myUI=(Fl_Group *)0;
Fl_Group *net_grp1=(Fl_Group *)0;
Fl_Box *box_callins=(Fl_Box *)0;
Fl_Box *ptr_left=(Fl_Box *)0;
Fl_Box *ptr_right=(Fl_Box *)0;
Fl_Group *net_grp2=(Fl_Group *)0;
Fl_Box *txtTitles=(Fl_Box *)0;
Fl_Box *txtLine[15]={(Fl_Box *)0};
Fl_Box *box_db_select=(Fl_Box *)0;
Fl_Group *dbSelectGroup=(Fl_Group *)0;
Fl_Box *boxLoginSuffix=(Fl_Box *)0;
Fl_Box *boxLoginPrefix=(Fl_Box *)0;

Fl_Box *boxLoginArea=(Fl_Box *)0;
Fl_Group *net_grp3=(Fl_Group *)0;
Fl_Box *txtPick[NPICKITEMS]={(Fl_Box *)0};
Fl_Box *txtPickArrows=(Fl_Box *)0;
Fl_Box *bx_suffix=(Fl_Box *)0;
Fl_Box *bx_prefix=(Fl_Box *)0;
Fl_Box *bx_area=(Fl_Box *)0;
Fl_Box *txtInfo=(Fl_Box *)0;

void cb_mnuLogIns(Fl_Menu_*, void*) {
	open_log_ins();
}

void cb_mnuSize(Fl_Menu_*, void*) {
	change_size();
}

Fl_Menu_Item menu_mnu_bar[] = {
	{"Files", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
		{"Open", 0,  (Fl_Callback*)cb_mnuOpen, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
		{"New", 0,   (Fl_Callback*)cb_mnuNew, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
		{"Exit", 0,  (Fl_Callback*)cb_mnuExit, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
	{"&Editor", 0,  (Fl_Callback*)cb_mnuEditor, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"&Config", 0,  (Fl_Callback*)cb_mnuConfig, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Log-&Ins", 0,  (Fl_Callback*)cb_mnuLogIns, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Si&ze", 0,  (Fl_Callback*)cb_mnuSize, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Help", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
		{"Content", 0,  (Fl_Callback*)cb_mnuHelpContent, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
		{"Event log", 0, (Fl_Callback*)cb_mnuEventLog, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
		{"About", 0,  (Fl_Callback*)cb_mnuAbout, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0}
};

Fl_Double_Window* newNetControl() {
	Fl_Double_Window* w = new Fl_Double_Window(390, 455, "flnet");
	w->box(FL_BORDER_BOX);

	mnu_bar = new Fl_Menu_Bar(0, 0, 390, 25);
	mnu_bar->menu(menu_mnu_bar);

//	myUI = new my_UI(0, 25, 390, 430);
	myUI = new Fl_Group(0, 25, 390, 430);
		myUI->box(FL_NO_BOX);
		myUI->color(FL_BACKGROUND_COLOR);
		myUI->selection_color(FL_BACKGROUND_COLOR);
		myUI->labeltype(FL_NORMAL_LABEL);
		myUI->labelfont(FL_SCREEN);
		myUI->labelsize(14);
		myUI->labelcolor(FL_FOREGROUND_COLOR);
		myUI->align(Fl_Align(FL_ALIGN_TOP));
		myUI->when(FL_WHEN_RELEASE);

		box_callins = new Fl_Box( 20, 26, 200, 22, "Check-ins:");
		box_callins->color(FL_BACKGROUND_COLOR);
		box_callins->box(FL_FLAT_BOX);
		box_callins->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

// hidden input widget to capture keyboard events
		inp_focus = new my_UI( 0, 25, 0, 0, "");

		net_grp1 = new Fl_Group(0, 45, 225, 255, "");
		net_grp1->tooltip("Use UP/DN arrow keys to scroll list");
		net_grp1->labelfont(FL_SCREEN);
		net_grp1->align(Fl_Align(FL_ALIGN_TOP_LEFT));
		net_grp1->box(FL_NO_BOX);

			ptr_left = new Fl_Box(5, 125, 15, 25, "@>");

			ptr_right = new Fl_Box(210, 125, 15, 25, "@<");

			net_grp2 = new Fl_Group(17, 45, 196, 255, "");
			net_grp2->box(FL_DOWN_BOX);
			net_grp2->color(FL_BACKGROUND2_COLOR);
			net_grp2->align(FL_ALIGN_CENTER);

				txtTitles = new Fl_Box(20, 47, 190, 22, " Call   |   Name    |Time|F");
				txtTitles->box(FL_FLAT_BOX);
				txtTitles->color((Fl_Color)23);
				txtTitles->labelfont(FL_SCREEN);
				txtTitles->labelsize(12);
				txtTitles->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

				for (int i = 0; i < 15; i++) {
					txtLine[i] = new Fl_Box(20, 70 + 15 * i, 190, 15);
					txtLine[i]->box(FL_FLAT_BOX);
					txtLine[i]->color(FL_BACKGROUND2_COLOR);
					txtLine[i]->labelfont(FL_SCREEN);
					txtLine[i]->labelsize(12);
					txtLine[i]->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
				}
			net_grp2->end();

		net_grp1->end();

		box_db_select = new Fl_Box(200, 26, 155, 18, "");
		box_db_select->color(FL_BACKGROUND_COLOR);
		box_db_select->box(FL_FLAT_BOX);
		box_db_select->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

		dbSelectGroup = new Fl_Group(230, 45, 155, 250);
		dbSelectGroup->box(FL_ENGRAVED_FRAME);
		dbSelectGroup->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			boxLoginSuffix = new Fl_Box(240, 65, 40, 22);
			boxLoginSuffix->tooltip("Enter SUFFIX, Esc to abort");
			boxLoginSuffix->box(FL_DOWN_BOX);
			boxLoginSuffix->color(FL_BACKGROUND2_COLOR);
			boxLoginSuffix->labelfont(FL_SCREEN);
			boxLoginSuffix->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			boxLoginPrefix = new Fl_Box(285, 65, 30, 22);
			boxLoginPrefix->tooltip("Enter PREFIX Esc to abort");
			boxLoginPrefix->box(FL_DOWN_BOX);
			boxLoginPrefix->color(FL_BACKGROUND2_COLOR);
			boxLoginPrefix->labelfont(FL_SCREEN);
			boxLoginPrefix->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			boxLoginArea = new Fl_Box(320, 65, 40, 22);
			boxLoginArea->tooltip("Enter AREA Esc to abort");
			boxLoginArea->box(FL_DOWN_BOX);
			boxLoginArea->color(FL_BACKGROUND2_COLOR);
			boxLoginArea->labelfont(FL_SCREEN);
			boxLoginArea->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			net_grp3 = new Fl_Group(235, 95, 146, 12*NPICKITEMS/2 + 4);
			net_grp3->tooltip("Use UP/DN arrow keys to select callsign");
			net_grp3->box(FL_DOWN_BOX);
			net_grp3->color(FL_BACKGROUND2_COLOR);

				for (int i = 0; i < NPICKITEMS / 2; i++) {
					txtPick[i] = new Fl_Box(237, 97 + 12*i, 72, 12);
					txtPick[i]->box(FL_FLAT_BOX);
					txtPick[i]->color(FL_BACKGROUND2_COLOR);
					txtPick[i]->labelfont(FL_SCREEN);
					txtPick[i]->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

					txtPick[i + NPICKITEMS/2] = new Fl_Box(308, 97 + 12*i, 72, 12);
					txtPick[i + NPICKITEMS/2]->box(FL_FLAT_BOX);
					txtPick[i + NPICKITEMS/2]->color(FL_BACKGROUND2_COLOR);
					txtPick[i + NPICKITEMS/2]->labelfont(FL_SCREEN);
					txtPick[i + NPICKITEMS/2]->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
				}

			net_grp3->end();

			txtPickArrows = new Fl_Box(320, 135, 25, 120);
			txtPickArrows->hide();

			bx_suffix = new Fl_Box(235, 40, 40, 30, "Suffix");
			bx_prefix = new Fl_Box(280, 40, 40, 30, "Prefix");
			bx_area = new Fl_Box(320, 40, 40, 30, "Area");

		dbSelectGroup->end();

	myUI->end();

	txtInfo = new Fl_Box(5, 301, 379, 150);
	txtInfo->box(FL_DOWN_BOX);
	txtInfo->color((Fl_Color)20);
	txtInfo->selection_color(FL_BACKGROUND2_COLOR);
	txtInfo->labelfont(FL_SCREEN);
	txtInfo->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

	w->end();

	inp_focus->take_focus();

	return w;
}

static void cb_btn_new_login_is_up(Fl_Check_Button* o, void*) {
	progStatus.disp_new_login = o->value();
}

static void cb_btnOpenEditor(Fl_Check_Button* o, void*) {
	progStatus.open_editor = o->value();
}

static void cb_btn_current_call_in_is_up(Fl_Check_Button* o, void*) {
	progStatus.callin_is_up = o->value();
}

static void cb_btnFg(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.fgColors[1], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.fgColors[1] = fl_rgb_color(r, g, b);
	}
	txtSample[1]->textcolor(progStatus.fgColors[1]);
	txtSample[1]->redraw();
}

static void cb_btnBg(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.bgColors[1], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.bgColors[1] = fl_rgb_color(r, g, b);
	}
	txtSample[1]->color(progStatus.bgColors[1]);
	txtSample[1]->redraw();
}

static void cb_btnFg1(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.fgColors[2], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.fgColors[2] = fl_rgb_color(r, g, b);
	}
	txtSample[2]->textcolor(progStatus.fgColors[2]);
	txtSample[2]->redraw();
}

static void cb_btnBg1(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.bgColors[2], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.bgColors[2] = fl_rgb_color(r, g, b);
	}
	txtSample[2]->color(progStatus.bgColors[2]);
	txtSample[2]->redraw();
}

static void cb_btnFg2(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.fgColors[3], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.fgColors[3] = fl_rgb_color(r, g, b);
	}
	txtSample[3]->textcolor(progStatus.fgColors[3]);
	txtSample[3]->redraw();
}

static void cb_btnBg2(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.bgColors[3], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.bgColors[3] = fl_rgb_color(r, g, b);
	}
	txtSample[3]->color(progStatus.bgColors[3]);
	txtSample[3]->redraw();
}

static void cb_btnFg3(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.fgColors[4], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.fgColors[4] = fl_rgb_color(r, g, b);
	}
	txtSample[4]->textcolor(progStatus.fgColors[4]);
	txtSample[4]->redraw();
}

static void cb_btnBg3(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.bgColors[4], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.bgColors[4] = fl_rgb_color(r, g, b);
	}
	txtSample[4]->color(progStatus.bgColors[4]);
	txtSample[4]->redraw();
}

static void cb_btnFg4(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.fgColors[5], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.fgColors[5] = fl_rgb_color(r, g, b);
	}
	txtSample[5]->textcolor(progStatus.fgColors[5]);
	txtSample[5]->redraw();
}

static void cb_btnBg4(Fl_Button*, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.bgColors[5], r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.bgColors[5] = fl_rgb_color(r, g, b);
	}
	txtSample[5]->color(progStatus.bgColors[5]);
	txtSample[5]->redraw();
}

static void cb_mdb_color(Fl_Button *, void*) {
	unsigned char r,g,b;
	Fl::get_color(progStatus.mdb_color, r, g, b);
	if (fl_color_chooser("Select Color", r, g, b)) {
		progStatus.mdb_color = fl_rgb_color(r, g, b);
	}
	mdb_color->color(progStatus.mdb_color);
}

static void cb_myLocator(Fl_Input2 *, void *)
{
	progStatus.myLocator = inp_myLocator->value();
}

static void cb_user_name(Fl_Input2 *, void *)
{
	progStatus.user_name = inp_user_name->value();
}

static void cb_user_password(Fl_Input2 *, void *)
{
	progStatus.user_password = inp_user_password->value();
}

static void cb_chk_pwd(Fl_Check_Button *btn, void *)
{
	if (btn->value())
		inp_user_password->type(FL_NORMAL_INPUT);
	else
		inp_user_password->type(FL_SECRET_INPUT);
	inp_user_password->redraw();
}

static void cb_chk_callook(Fl_Check_Button *, void *)
{
	chk_callook->value(1);
	chk_hamcall->value(0);
	chk_hamqth->value(0);
	chk_qrz->value(0);
	chk_callook->redraw();
	chk_hamcall->redraw();
	chk_hamqth->redraw();
	chk_qrz->redraw();
	progStatus.QRZXML = CALLOOK;
}

static void cb_callookurl(Fl_Input2 *, void *)
{
	progStatus.callookurl = inp_callookurl->value();
}

static void cb_chk_hamqth(Fl_Check_Button *, void *)
{
	chk_callook->value(0);
	chk_hamcall->value(0);
	chk_hamqth->value(1);
	chk_qrz->value(0);
	chk_callook->redraw();
	chk_hamcall->redraw();
	chk_hamqth->redraw();
	chk_qrz->redraw();
	progStatus.QRZXML = HAMQTH;
}

static void cb_hamqthurl(Fl_Input2 *, void *)
{
	progStatus.hamqthurl = inp_hamqthurl->value();
}

static void cb_chk_hamcall(Fl_Check_Button *, void *)
{
	chk_callook->value(0);
	chk_hamcall->value(1);
	chk_hamqth->value(0);
	chk_qrz->value(0);
	chk_callook->redraw();
	chk_hamcall->redraw();
	chk_hamqth->redraw();
	chk_qrz->redraw();
	progStatus.QRZXML = HAMCALLNET;
}

static void cb_hamcallurl(Fl_Input2 *, void *)
{
	progStatus.hamcallurl = inp_hamcallurl->value();
}

static void cb_chk_qrz(Fl_Check_Button *, void *)
{
	chk_callook->value(0);
	chk_hamcall->value(0);
	chk_hamqth->value(0);
	chk_qrz->value(1);
	chk_callook->redraw();
	chk_hamcall->redraw();
	chk_hamqth->redraw();
	chk_qrz->redraw();
	progStatus.QRZXML = QRZNET;
}

static void cb_hamqrzurl(Fl_Input2 *, void *)
{
	progStatus.qrzurl = inp_qrzurl->value();
}

static void cb_masterdb(Fl_Input2 *, void *)
{
	if (progStatus.masterdb != inp_masterdb->value()) {
		close_masterdb();
		progStatus.masterdb = inp_masterdb->value();
	}
}

static void cb_select_masterdb( Fl_Button *, void *)
{
	string open_dir = home_dir;
	char *p = fl_file_chooser (
		"Select master DB file", "*.csv", 
		open_dir.c_str(), 0);
	if (!p) return;
	inp_masterdb->value(p);
	if (progStatus.masterdb != inp_masterdb->value()) {
		close_masterdb();
		progStatus.masterdb = inp_masterdb->value();
	}
}

static void cb_open_masterdb( Fl_Button *, void *)
{
	close_masterdb(); // function will test for valid masterdb
	if (progStatus.masterdb.empty())
		return;
	open_masterdb();
}

static void cb_close_masterdb( Fl_Button *, void *)
{
	close_masterdb();
}

static void cb_call_justify(Fl_Check_Button *, void *)
{
	progStatus.call_justify = cbo_call_justify->value();
	callinlist.justify();
	updateCallins(false);
}

static void cb_name_justify(Fl_Check_Button *, void *)
{
	progStatus.name_justify = cbo_name_justify->value();
	callinlist.justify();
	updateCallins(false);
}

static void cb_arc_conversion (Fl_Choice *w, void *)
{
	progStatus.arc_conversion = combo_arc_conversion->value();
	compute();
	updateCallins(false);
}

static void cb_sort_by_PAS (Fl_Check_Button *, void *)
{
	if (btn_sort_by_PAS->value() == 0) {
		btn_sort_by_PAS->value(1);
		return;
	}
	progStatus.preferred_sort_order = 0;
	btn_sort_by_APS->value(0);
	btn_sort_by_SAP->value(0);
	btn_sort_by_NETNBR->value(0);
}

static void cb_sort_by_APS (Fl_Check_Button *, void *)
{
	if (btn_sort_by_APS->value() == 0) {
		btn_sort_by_APS->value(1);
		return;
	}
	progStatus.preferred_sort_order = 1;
	btn_sort_by_PAS->value(0);
	btn_sort_by_SAP->value(0);
	btn_sort_by_NETNBR->value(0);
}

static void cb_sort_by_SAP (Fl_Check_Button *, void *)
{
	if (btn_sort_by_SAP->value() == 0) {
		btn_sort_by_SAP->value(1);
		return;
	}
	progStatus.preferred_sort_order = 2;
	btn_sort_by_APS->value(0);
	btn_sort_by_PAS->value(0);
	btn_sort_by_NETNBR->value(0);
}

static void cb_sort_by_NETNBR (Fl_Check_Button *, void *)
{
	if (btn_sort_by_NETNBR->value() == 0) {
		btn_sort_by_NETNBR->value(1);
		return;
	}
	progStatus.preferred_sort_order = 3;
	btn_sort_by_APS->value(0);
	btn_sort_by_PAS->value(0);
	btn_sort_by_SAP->value(0);
}

void cb_cfgP1 (Fl_Input2 *w, void *)
{
	progStatus.chP1 = cfgP1->value();
}

void cb_cfgP2 (Fl_Input2 *w, void *)
{
	progStatus.chP2 = cfgP2->value();
}

void cb_cfgP3 (Fl_Input2 *w, void *)
{
	progStatus.chP3 = cfgP3->value();
}

void cb_inpStatesList1 (Fl_Input2 *w, void *)
{
	progStatus.strP1 = inpStatesList1->value();
}

void cb_inpStatesList2 (Fl_Input2 *w, void *)
{
	progStatus.strP2 = inpStatesList2->value();
}

void cb_inpStatesList3 (Fl_Input2 *w, void *)
{
	progStatus.strP3 = inpStatesList3->value();
}

void cb_chkAutoPriority (Fl_Check_Button *w, void *)
{
	progStatus.chAuto = chkAutoPriority->value();
	callinlist.AutoPriority (progStatus.chAuto);

	if (callinlist.numlist() <= 0)
		return;

	std::string prefix = callinlist.prefix(WhoIsUp);
	std::string area   = callinlist.area(WhoIsUp);
	std::string suffix = callinlist.suffix(WhoIsUp);

	callinlist.sort( (progStatus.chAuto ? loglist::BYPRIORITY : loglist::BYDATETIME) );

	getindexed_list ();
	refresh_logins ();

	updateCallins ();

	WhoIsUp = callinlist.locate(prefix, area, suffix);

	inp_focus->dispCallIns(false);

}

void cb_chk_mdb_netnbr (Fl_Check_Button *w, void *)
{
	progStatus.mdb_netnbr = chk_mdb_netnbr->value();
}

void cb_chk_mdb_prefix (Fl_Check_Button *w, void *)
{
	progStatus.mdb_prefix = chk_mdb_prefix->value();
}

void cb_chk_mdb_area (Fl_Check_Button *w, void *)
{
	progStatus.mdb_area = chk_mdb_area->value();
}

void cb_chk_mdb_suffix (Fl_Check_Button *w, void *)
{
	progStatus.mdb_suffix = chk_mdb_suffix->value();
}

void cb_chk_mdb_callsign (Fl_Check_Button *w, void *)
{
	progStatus.mdb_callsign = chk_mdb_callsign->value();
}

void cb_chk_mdb_name (Fl_Check_Button *w, void *)
{
	progStatus.mdb_name = chk_mdb_name->value();
}

void cb_chk_mdb_fname (Fl_Check_Button *w, void *)
{
	progStatus.mdb_fname = chk_mdb_fname->value();
}

void cb_chk_mdb_lname (Fl_Check_Button *w, void *)
{
	progStatus.mdb_lname = chk_mdb_lname->value();
}

void cb_chk_mdb_addr (Fl_Check_Button *w, void *)
{
	progStatus.mdb_addr = chk_mdb_addr->value();
}

void cb_chk_mdb_city (Fl_Check_Button *w, void *)
{
	progStatus.mdb_city = chk_mdb_city->value();
}

void cb_chk_mdb_state (Fl_Check_Button *w, void *)
{
	progStatus.mdb_state = chk_mdb_state->value();
}

void cb_chk_mdb_zip (Fl_Check_Button *w, void *)
{
	progStatus.mdb_zip = chk_mdb_zip->value();
}

void cb_chk_mdb_phone (Fl_Check_Button *w, void *)
{
	progStatus.mdb_phone = chk_mdb_phone->value();
}

void cb_chk_mdb_birthdate (Fl_Check_Button *w, void *)
{
	progStatus.mdb_birthdate = chk_mdb_birthdate->value();
}

void cb_chk_mdb_spouse (Fl_Check_Button *w, void *)
{
	progStatus.mdb_spouse = chk_mdb_spouse->value();
}

void cb_chk_mdb_sp_birth (Fl_Check_Button *w, void *)
{
	progStatus.mdb_sp_birth = chk_mdb_sp_birth->value();
}

void cb_chk_mdb_comment1 (Fl_Check_Button *w, void *)
{
	progStatus.mdb_comment1 = chk_mdb_comment1->value();
}

void cb_chk_mdb_comment2 (Fl_Check_Button *w, void *)
{
	progStatus.mdb_comment2 = chk_mdb_comment2->value();
}

void cb_chk_mdb_email (Fl_Check_Button *w, void *)
{
	progStatus.mdb_email = chk_mdb_email->value();
}

void cb_chk_mdb_locator (Fl_Check_Button *w, void *)
{
	progStatus.mdb_locator = chk_mdb_locator->value();
}

void cb_chk_mdb_country (Fl_Check_Button *w, void *)
{
	progStatus.mdb_country = chk_mdb_country->value();
}

void cb_chk_mdb_logdate (Fl_Check_Button *w, void *)
{
	progStatus.mdb_logdate = chk_mdb_logdate->value();
}

void cb_chk_mdb_prevdate (Fl_Check_Button *w, void *)
{
	progStatus.mdb_prevdate = chk_mdb_prevdate->value();
}

void cb_chk_mdb_nbrlogins (Fl_Check_Button *w, void *)
{
	progStatus.mdb_nbrlogins = chk_mdb_nbrlogins->value();
}

void cb_chk_mdb_status (Fl_Check_Button *w, void *)
{
	progStatus.mdb_status = chk_mdb_status->value();
}

void cb_chk_mdb_joined (Fl_Check_Button *w, void *)
{
	progStatus.mdb_joined = chk_mdb_joined->value();
}

void cb_btn_setall(Fl_Button*, void *)
{
	chk_mdb_netnbr->value(progStatus.mdb_netnbr = 1); chk_mdb_netnbr->redraw();
	chk_mdb_prefix->value(progStatus.mdb_prefix = 1); chk_mdb_prefix->redraw();
	chk_mdb_area->value(progStatus.mdb_area = 1); chk_mdb_area->redraw();
	chk_mdb_suffix->value(progStatus.mdb_suffix = 1); chk_mdb_suffix->redraw();
	chk_mdb_callsign->value(progStatus.mdb_callsign = 1); chk_mdb_callsign->redraw();
	chk_mdb_name->value(progStatus.mdb_name = 1); chk_mdb_name->redraw();
	chk_mdb_fname->value(progStatus.mdb_fname = 1); chk_mdb_fname->redraw();
	chk_mdb_lname->value(progStatus.mdb_lname = 1); chk_mdb_lname->redraw();
	chk_mdb_addr->value(progStatus.mdb_addr = 1); chk_mdb_addr->redraw();
	chk_mdb_city->value(progStatus.mdb_city = 1); chk_mdb_city->redraw();
	chk_mdb_state->value(progStatus.mdb_state = 1); chk_mdb_state->redraw();
	chk_mdb_zip->value(progStatus.mdb_zip = 1); chk_mdb_zip->redraw();
	chk_mdb_phone->value(progStatus.mdb_phone = 1); chk_mdb_phone->redraw();
	chk_mdb_birthdate->value(progStatus.mdb_birthdate = 1); chk_mdb_birthdate->redraw();
	chk_mdb_spouse->value(progStatus.mdb_spouse = 1); chk_mdb_spouse->redraw();
	chk_mdb_sp_birth->value(progStatus.mdb_sp_birth = 1); chk_mdb_sp_birth->redraw();
	chk_mdb_comment1->value(progStatus.mdb_comment1 = 1); chk_mdb_comment1->redraw();
	chk_mdb_comment2->value(progStatus.mdb_comment2 = 1); chk_mdb_comment2->redraw();
	chk_mdb_email->value(progStatus.mdb_email = 1); chk_mdb_email->redraw();
	chk_mdb_locator->value(progStatus.mdb_locator = 1); chk_mdb_locator->redraw();
	chk_mdb_country->value(progStatus.mdb_country = 1); chk_mdb_country->redraw();
	chk_mdb_logdate->value(progStatus.mdb_logdate = 1); chk_mdb_logdate->redraw();
	chk_mdb_prevdate->value(progStatus.mdb_prevdate = 1); chk_mdb_prevdate->redraw();
	chk_mdb_nbrlogins->value(progStatus.mdb_nbrlogins = 1); chk_mdb_nbrlogins->redraw();
	chk_mdb_status->value(progStatus.mdb_status = 1); chk_mdb_status->redraw();
	chk_mdb_joined->value(progStatus.mdb_joined = 1); chk_mdb_joined->redraw();
}

void cb_btn_clrall(Fl_Button*, void *)
{
	chk_mdb_netnbr->value(progStatus.mdb_netnbr = 0); chk_mdb_netnbr->redraw();
	chk_mdb_prefix->value(progStatus.mdb_prefix = 0); chk_mdb_prefix->redraw();
	chk_mdb_area->value(progStatus.mdb_area = 0); chk_mdb_area->redraw();
	chk_mdb_suffix->value(progStatus.mdb_suffix = 0); chk_mdb_suffix->redraw();
	chk_mdb_callsign->value(progStatus.mdb_callsign = 0); chk_mdb_callsign->redraw();
	chk_mdb_name->value(progStatus.mdb_name = 0); chk_mdb_name->redraw();
	chk_mdb_fname->value(progStatus.mdb_fname = 0); chk_mdb_fname->redraw();
	chk_mdb_lname->value(progStatus.mdb_lname = 0); chk_mdb_lname->redraw();
	chk_mdb_addr->value(progStatus.mdb_addr = 0); chk_mdb_addr->redraw();
	chk_mdb_city->value(progStatus.mdb_city = 0); chk_mdb_city->redraw();
	chk_mdb_state->value(progStatus.mdb_state = 0); chk_mdb_state->redraw();
	chk_mdb_zip->value(progStatus.mdb_zip = 0); chk_mdb_zip->redraw();
	chk_mdb_phone->value(progStatus.mdb_phone = 0); chk_mdb_phone->redraw();
	chk_mdb_birthdate->value(progStatus.mdb_birthdate = 0); chk_mdb_birthdate->redraw();
	chk_mdb_spouse->value(progStatus.mdb_spouse = 0); chk_mdb_spouse->redraw();
	chk_mdb_sp_birth->value(progStatus.mdb_sp_birth = 0); chk_mdb_sp_birth->redraw();
	chk_mdb_comment1->value(progStatus.mdb_comment1 = 0); chk_mdb_comment1->redraw();
	chk_mdb_comment2->value(progStatus.mdb_comment2 = 0); chk_mdb_comment2->redraw();
	chk_mdb_email->value(progStatus.mdb_email = 0); chk_mdb_email->redraw();
	chk_mdb_locator->value(progStatus.mdb_locator = 0); chk_mdb_locator->redraw();
	chk_mdb_country->value(progStatus.mdb_country = 0); chk_mdb_country->redraw();
	chk_mdb_logdate->value(progStatus.mdb_logdate = 0); chk_mdb_logdate->redraw();
	chk_mdb_prevdate->value(progStatus.mdb_prevdate = 0); chk_mdb_prevdate->redraw();
	chk_mdb_nbrlogins->value(progStatus.mdb_nbrlogins = 0); chk_mdb_nbrlogins->redraw();
	chk_mdb_status->value(progStatus.mdb_status = 0); chk_mdb_status->redraw();
	chk_mdb_joined->value(progStatus.mdb_joined = 0); chk_mdb_joined->redraw();
}

Fl_Double_Window* configDialog() {
	Fl_Double_Window* w = new Fl_Double_Window(440, 250, "Net Configuration");

	tabsConfig = new Fl_Tabs(0, 10, 440, 210);
	tabsConfig->color((Fl_Color)44);
		tabGroupUI = new Fl_Group(0, 35, 440, 185, "UI");
			btn_new_login_is_up = new Fl_Check_Button(15, 50, 70, 15, "New login is up");
			btn_new_login_is_up->tooltip("Move first time login to the >...<\nspot in the call in list");
			btn_new_login_is_up->down_box(FL_DOWN_BOX);
			btn_new_login_is_up->callback((Fl_Callback*)cb_btn_new_login_is_up);
			btn_new_login_is_up->value(progStatus.disp_new_login);

			btnOpenEditor = new Fl_Check_Button(15, 75, 70, 15, "Open editor for new login");
			btnOpenEditor->tooltip("Open editor for first time call in\nNew login is up must be enabled");
			btnOpenEditor->down_box(FL_DOWN_BOX);
			btnOpenEditor->callback((Fl_Callback*)cb_btnOpenEditor);
			btnOpenEditor->value(progStatus.open_editor);

			btn_current_call_in_is_up = new Fl_Check_Button(15, 100, 70, 15, "Current call in is up");
			btn_current_call_in_is_up->tooltip("Move last login to the >...<\nspot in the calll in list");
			btn_current_call_in_is_up->down_box(FL_DOWN_BOX);
			btn_current_call_in_is_up->callback((Fl_Callback*)cb_btn_current_call_in_is_up);
			btn_current_call_in_is_up->value(progStatus.callin_is_up);

			cbo_call_justify = new Fl_Choice(15, 125, 100, 24, "Justify callsign");
			cbo_call_justify->value(progStatus.call_justify);
			cbo_call_justify->add("RIGHT");
			cbo_call_justify->add("LEFT");
			cbo_call_justify->add("AREA");
			cbo_call_justify->align(FL_ALIGN_RIGHT);
			cbo_call_justify->callback((Fl_Callback*)cb_call_justify);

			cbo_name_justify = new Fl_Choice(15, 150, 100, 24, "Justify nickname");
			cbo_name_justify->value(progStatus.name_justify);
			cbo_name_justify->add("RIGHT");
			cbo_name_justify->add("LEFT");
			cbo_name_justify->align(FL_ALIGN_RIGHT);
			cbo_name_justify->callback((Fl_Callback*)cb_name_justify);

			combo_arc_conversion = new Fl_Choice (15, 180, 120, 24, "Arc Distance");
			combo_arc_conversion->add("Kilometers");
			combo_arc_conversion->add("Nautical Miles");
			combo_arc_conversion->add("Statute Miles");
			combo_arc_conversion->align(FL_ALIGN_RIGHT);
			combo_arc_conversion->value(progStatus.arc_conversion);
			combo_arc_conversion->callback((Fl_Callback*)cb_arc_conversion);

			grp_sort_order = new Fl_Group(240, 50, 180, 125, "Preferred Sort Order");
			grp_sort_order->box(FL_ENGRAVED_BOX);
			grp_sort_order->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

				btn_sort_by_PAS = new Fl_Check_Button(250, 75, 90, 15, "P/A/S");
				btn_sort_by_PAS->value(progStatus.preferred_sort_order == 0);
				btn_sort_by_PAS->callback((Fl_Callback*)cb_sort_by_PAS);

				btn_sort_by_APS = new Fl_Check_Button(250, 100, 90, 15, "A/P/S");
				btn_sort_by_APS->value(progStatus.preferred_sort_order == 1);
				btn_sort_by_APS->callback((Fl_Callback*)cb_sort_by_APS);

				btn_sort_by_SAP = new Fl_Check_Button(250, 125, 90, 15, "S/A/P");
				btn_sort_by_SAP->value(progStatus.preferred_sort_order == 2);
				btn_sort_by_SAP->callback((Fl_Callback*)cb_sort_by_SAP);

				btn_sort_by_NETNBR = new Fl_Check_Button(250, 150, 90, 15, "Net Nbr");
				btn_sort_by_NETNBR->value(progStatus.preferred_sort_order == 3);
				btn_sort_by_NETNBR->callback((Fl_Callback*)cb_sort_by_NETNBR);

		tabGroupUI->end();

		tabGroupColors = new Fl_Group(0, 35, 440, 185, "Colors");
		tabGroupColors->hide();
			Fl_Box *gcbox = new Fl_Box(0, 40, 440, 20, "Color coding for call-in & pick lists");
			gcbox->box(FL_FLAT_BOX);
			gcbox->align(FL_ALIGN_CENTER);

			txtSample[1] = new Fl_Output(145, 65, 45, 24, "Logged In");
			txtSample[1]->textfont(FL_SCREEN);
			txtSample[1]->value("Text");
			txtSample[1]->color(progStatus.bgColors[1]);
			txtSample[1]->textcolor(progStatus.fgColors[1]);

			btnFg[1] = new Fl_Button(205, 65, 45, 24, "Fg");
			btnFg[1]->callback((Fl_Callback*)cb_btnFg);

			btnBg[2] = new Fl_Button(270, 65, 45, 24, "Bg");
			btnBg[2]->callback((Fl_Callback*)cb_btnBg);

			txtSample[2] = new Fl_Output(145, 92, 45, 24, "First Response");
			txtSample[2]->textfont(FL_SCREEN);
			txtSample[2]->value("Text");
			txtSample[2]->color(progStatus.bgColors[2]);
			txtSample[2]->textcolor(progStatus.fgColors[2]);

			btnFg[2] = new Fl_Button(205, 92, 45, 24, "Fg");
			btnFg[2]->callback((Fl_Callback*)cb_btnFg1);

			btnBg[2] = new Fl_Button(270, 92, 45, 24, "Bg");
			btnBg[2]->callback((Fl_Callback*)cb_btnBg1);

			txtSample[3] = new Fl_Output(145, 119, 45, 24, "Second Response");
			txtSample[3]->textfont(FL_SCREEN);
			txtSample[3]->value("Text");
			txtSample[3]->color(progStatus.bgColors[3]);
			txtSample[3]->textcolor(progStatus.fgColors[3]);

			btnFg[3] = new Fl_Button(205, 119, 45, 24, "Fg");
			btnFg[3]->callback((Fl_Callback*)cb_btnFg2);

			btnBg[3] = new Fl_Button(270, 119, 45, 24, "Bg");
			btnBg[3]->callback((Fl_Callback*)cb_btnBg2);

			txtSample[4] = new Fl_Output(145, 146, 45, 24, "Logged Out");
			txtSample[4]->textfont(FL_SCREEN);
			txtSample[4]->value("Text");
			txtSample[4]->color(progStatus.bgColors[4]);
			txtSample[4]->textcolor(progStatus.fgColors[4]);

			btnFg[4] = new Fl_Button(205, 146, 45, 24, "Fg");
			btnFg[4]->callback((Fl_Callback*)cb_btnFg3);

			btnBg[4] = new Fl_Button(270, 145, 45, 24, "Bg");
			btnBg[4]->callback((Fl_Callback*)cb_btnBg3);

			txtSample[5] = new Fl_Output(145, 180, 45, 24, "Pick List Item");
			txtSample[5]->textfont(FL_SCREEN);
			txtSample[5]->value("Text");
			txtSample[5]->color(progStatus.bgColors[5]);
			txtSample[5]->textcolor(progStatus.fgColors[5]);

			btnFg[5] = new Fl_Button(205, 180, 45, 24, "Fg");
			btnFg[5]->callback((Fl_Callback*)cb_btnFg4);

			btnBg[5] = new Fl_Button(270, 180, 45, 24, "Bg");
			btnBg[5]->callback((Fl_Callback*)cb_btnBg4);

		tabGroupColors->end();

		tabGroupPriority = new Fl_Group(0, 35, 440, 185, "Priority");
		tabGroupPriority->hide();
			cfgP1 = new Fl_Input2(160, 70, 20, 25, "Priority 1 character");
			cfgP1->callback((Fl_Callback*)cb_cfgP1);
			cfgP1->when(FL_WHEN_CHANGED);
			cfgP1->tooltip("Character used to designate priority 1 call in");

			inpStatesList1 = new Fl_Input2(200, 70, 225, 25, "States List (ie: FL, AL, GA)");
			inpStatesList1->align(Fl_Align(FL_ALIGN_TOP_LEFT));
			inpStatesList1->callback((Fl_Callback*)cb_inpStatesList1);
			inpStatesList1->when(FL_WHEN_CHANGED);
			inpStatesList1->tooltip("Comma separated list of priority 1 states");

			cfgP2 = new Fl_Input2(160, 100, 20, 25, "Priority 2 character");
			cfgP2->callback((Fl_Callback*)cb_cfgP2);
			cfgP2->when(FL_WHEN_CHANGED);
			cfgP2->tooltip("Character used to designate priority 2 call in");

			inpStatesList2 = new Fl_Input2(200, 100, 225, 25);
			inpStatesList2->align(Fl_Align(FL_ALIGN_TOP_LEFT));
			inpStatesList2->callback((Fl_Callback*)cb_inpStatesList2);
			inpStatesList2->when(FL_WHEN_CHANGED);
			inpStatesList2->tooltip("Comma separated list of priority 2 states");

			cfgP3 = new Fl_Input2(160, 130, 20, 25, "Priority 3 character");
			cfgP3->callback((Fl_Callback*)cb_cfgP3);
			cfgP3->when(FL_WHEN_CHANGED);
			cfgP3->tooltip("Character used to designate priority 3 call in");

			inpStatesList3 = new Fl_Input2(200, 130, 225, 25);
			inpStatesList3->align(Fl_Align(FL_ALIGN_TOP_LEFT));
			inpStatesList3->callback((Fl_Callback*)cb_inpStatesList3);
			inpStatesList3->when(FL_WHEN_CHANGED);
			inpStatesList3->tooltip("Comma separated list of priority 3 states");

			chkAutoPriority = new Fl_Check_Button(160, 165, 25, 25, "Auto By Priority");
			chkAutoPriority->down_box(FL_DOWN_BOX);
			chkAutoPriority->align(Fl_Align(FL_ALIGN_LEFT));
			chkAutoPriority->callback((Fl_Callback*)cb_chkAutoPriority);
			chkAutoPriority->when(FL_WHEN_CHANGED);
			chkAutoPriority->tooltip("ON - Sort call list entries by priority\nOFF - Sort by entry date-time");

		tabGroupPriority->end();

		tabGroupLookup = new Fl_Group(0, 35, 440, 185, "Lookup");
		tabGroupLookup->hide();

			inp_myLocator = new Fl_Input2(15, 60, 80, 24, "My Loc:");
			inp_myLocator->value(progStatus.myLocator.c_str());
			inp_myLocator->callback((Fl_Callback*)cb_myLocator);
			inp_myLocator->align(FL_ALIGN_TOP_LEFT);
			inp_myLocator->tooltip("Maidenhead locator, i.e. EM64qv");

			inp_user_name = new Fl_Input2(100, 60, 110, 24, "Id:");
			inp_user_name->value(progStatus.user_name.c_str());
			inp_user_name->callback((Fl_Callback*)cb_user_name);
			inp_user_name->align(FL_ALIGN_TOP_LEFT);
			inp_user_name->tooltip("User ID for selected on line database");

			inp_user_password = new Fl_Input2(215, 60, 120, 24, "Pwd:");
			inp_user_password->value(progStatus.user_password.c_str());
			inp_user_password->callback((Fl_Callback*)cb_user_password);
			inp_user_password->type(FL_SECRET_INPUT);
			inp_user_password->align(FL_ALIGN_TOP_LEFT);
			inp_user_password->tooltip("Password for selected on line database");

			chk_pwd = new Fl_Check_Button(340, 60, 40, 18, "Show pwd");
			chk_pwd->value(false);
			chk_pwd->callback((Fl_Callback*)cb_chk_pwd);

			chk_callook = new Fl_Check_Button(23, 88, 50, 18, "callook.info:");
			chk_callook->value(progStatus.QRZXML == CALLOOK);
			chk_callook->callback((Fl_Callback*)cb_chk_callook);
			chk_callook->tooltip("Query callook on line database");

			inp_callookurl = new Fl_Input2(140, 85, 280, 24, "");
			inp_callookurl->value(progStatus.callookurl.c_str());
			inp_callookurl->callback((Fl_Callback*)cb_callookurl);
			inp_callookurl->tooltip("Url for callook on line database");

			chk_hamqth = new Fl_Check_Button(23, 113, 50, 18, "hamqth.com:");
			chk_hamqth->value(progStatus.QRZXML == HAMQTH);
			chk_hamqth->callback((Fl_Callback*)cb_chk_hamqth);
			chk_hamqth->tooltip("Query hamqth on line database");

			inp_hamqthurl = new Fl_Input2(140, 110, 280, 24, "");
			inp_hamqthurl->value(progStatus.hamqthurl.c_str());
			inp_hamqthurl->callback((Fl_Callback*)cb_hamqthurl);
			inp_hamqthurl->tooltip("Url for Hamqth.com on line database");

			chk_hamcall = new Fl_Check_Button(23, 138, 50, 18, "hamcall.net:");
			chk_hamcall->value(progStatus.QRZXML == HAMCALLNET);
			chk_hamcall->callback((Fl_Callback*)cb_chk_hamcall);
			chk_hamcall->tooltip("Query Hamcall on line database");

			inp_hamcallurl = new Fl_Input2(140, 135, 280, 24, "");
			inp_hamcallurl->value(progStatus.hamcallurl.c_str());
			inp_hamcallurl->callback((Fl_Callback*)cb_hamcallurl);
			inp_hamcallurl->tooltip("Url for Hamcall");

			chk_qrz = new Fl_Check_Button(23, 163, 50, 18, "qrz.com:");
			chk_qrz->value(progStatus.QRZXML == QRZNET);
			chk_qrz->callback((Fl_Callback*)cb_chk_qrz);
			chk_qrz->tooltip("Query QRZ.com on line database");

			inp_qrzurl = new Fl_Input2(140, 160, 280, 24, "");
			inp_qrzurl->value(progStatus.qrzurl.c_str());
			inp_qrzurl->callback((Fl_Callback*)cb_hamqrzurl);
			inp_qrzurl->tooltip("Url for QRZ.com");

		tabGroupLookup->end();

		tabGroupMasterDB = new Fl_Group(0, 35, 440, 185, "MasterDB");
		tabGroupMasterDB->hide();

			inp_masterdb = new Fl_Input2(10, 60, 360, 24, "Master DB");
			inp_masterdb->value(progStatus.masterdb.c_str());
			inp_masterdb->callback((Fl_Callback*)cb_masterdb);
			inp_masterdb->align(FL_ALIGN_TOP_LEFT);
			inp_masterdb->tooltip("Enter full pathname of master database");

			btn_masterdb = new Fl_Button(375, 60, 60, 24, "Select");
			btn_masterdb->callback((Fl_Callback*)cb_select_masterdb);

			box_mdb_isopen = new Fl_Box(20, 90, 14, 14, "Master DB is open");
			box_mdb_isopen->box(FL_DOWN_BOX);
			box_mdb_isopen->align(FL_ALIGN_RIGHT);
			box_mdb_isopen->color(progStatus.mdb_isopen ? progStatus.mdb_color : FL_BACKGROUND2_COLOR);

			btn_open_masterdb = new Fl_Button(375, 90, 60, 24, "Open");
			btn_open_masterdb->callback((Fl_Callback*)cb_open_masterdb);
			btn_open_masterdb->tooltip("Open the master database");

			btn_close_masterdb = new Fl_Button(376, 120, 60, 24, "Close");
			btn_close_masterdb->callback((Fl_Callback*)cb_close_masterdb);
			btn_close_masterdb->tooltip("Close the master database");

			mdb_color = new Fl_Button(20, 150, 24, 24, "Select Indicator Color");
			mdb_color->align(FL_ALIGN_RIGHT);
			mdb_color->color(progStatus.mdb_color);
			mdb_color->callback((Fl_Callback*)cb_mdb_color);
			mdb_color->tooltip("Select open indicator color");

		tabGroupMasterDB->end();

		tabGroupSharedFields = new Fl_Group(0, 35, 440, 185, "Shared Fields");

int gx = 5, gy = 45;
			chk_mdb_prefix = new Fl_Check_Button(gx, gy, 15, 15, "prefix");
			chk_mdb_prefix->value(progStatus.mdb_prefix);
			chk_mdb_prefix->align(FL_ALIGN_RIGHT);
			chk_mdb_prefix->callback((Fl_Callback*)cb_chk_mdb_prefix);
gy += 22;
			chk_mdb_area = new Fl_Check_Button(gx, gy, 15, 15, "area");
			chk_mdb_area->value(progStatus.mdb_area);
			chk_mdb_area->align(FL_ALIGN_RIGHT);
			chk_mdb_area->callback((Fl_Callback*)cb_chk_mdb_area);
gy += 22;
			chk_mdb_suffix = new Fl_Check_Button(gx, gy, 15, 15, "suffix");
			chk_mdb_suffix->value(progStatus.mdb_suffix);
			chk_mdb_suffix->align(FL_ALIGN_RIGHT);
			chk_mdb_suffix->callback((Fl_Callback*)cb_chk_mdb_suffix);
gy += 22;
			chk_mdb_callsign = new Fl_Check_Button(gx, gy, 15, 15, "callsign");
			chk_mdb_callsign->value(progStatus.mdb_callsign);
			chk_mdb_callsign->align(FL_ALIGN_RIGHT);
			chk_mdb_callsign->callback((Fl_Callback*)cb_chk_mdb_callsign);
gy += 22;
			chk_mdb_name = new Fl_Check_Button(gx, gy, 15, 15, "nickname");
			chk_mdb_name->value(progStatus.mdb_name);
			chk_mdb_name->align(FL_ALIGN_RIGHT);
			chk_mdb_name->callback((Fl_Callback*)cb_chk_mdb_name);
gy += 22;
			chk_mdb_fname = new Fl_Check_Button(gx, gy, 15, 15, "fname");
			chk_mdb_fname->value(progStatus.mdb_fname);
			chk_mdb_fname->align(FL_ALIGN_RIGHT);
			chk_mdb_fname->callback((Fl_Callback*)cb_chk_mdb_fname);
gy += 22;
			chk_mdb_lname = new Fl_Check_Button(gx, gy, 15, 15, "lname");
			chk_mdb_lname->value(progStatus.mdb_lname);
			chk_mdb_lname->align(FL_ALIGN_RIGHT);
			chk_mdb_lname->callback((Fl_Callback*)cb_chk_mdb_lname);
gx += 105; gy = 45;
			chk_mdb_addr = new Fl_Check_Button(gx, gy, 15, 15, "addr");
			chk_mdb_addr->value(progStatus.mdb_addr);
			chk_mdb_addr->align(FL_ALIGN_RIGHT);
			chk_mdb_addr->callback((Fl_Callback*)cb_chk_mdb_addr);
gy += 22;
			chk_mdb_city = new Fl_Check_Button(gx, gy, 15, 15, "city");
			chk_mdb_city->value(progStatus.mdb_city);
			chk_mdb_city->align(FL_ALIGN_RIGHT);
			chk_mdb_city->callback((Fl_Callback*)cb_chk_mdb_city);
gy += 22;
			chk_mdb_state = new Fl_Check_Button(gx, gy, 15, 15, "state");
			chk_mdb_state->value(progStatus.mdb_state);
			chk_mdb_state->align(FL_ALIGN_RIGHT);
			chk_mdb_state->callback((Fl_Callback*)cb_chk_mdb_state);
gy += 22;
			chk_mdb_zip = new Fl_Check_Button(gx, gy, 15, 15, "zip");
			chk_mdb_zip->value(progStatus.mdb_zip);
			chk_mdb_zip->align(FL_ALIGN_RIGHT);
			chk_mdb_zip->callback((Fl_Callback*)cb_chk_mdb_zip);
gy += 22;
			chk_mdb_phone = new Fl_Check_Button(gx, gy, 15, 15, "phone");
			chk_mdb_phone->value(progStatus.mdb_phone);
			chk_mdb_phone->align(FL_ALIGN_RIGHT);
			chk_mdb_phone->callback((Fl_Callback*)cb_chk_mdb_phone);
gy += 22;
			chk_mdb_birthdate = new Fl_Check_Button(gx, gy, 15, 15, "birthdate");
			chk_mdb_birthdate->value(progStatus.mdb_birthdate);
			chk_mdb_birthdate->align(FL_ALIGN_RIGHT);
			chk_mdb_birthdate->callback((Fl_Callback*)cb_chk_mdb_birthdate);
gy += 22;
			chk_mdb_spouse = new Fl_Check_Button(gx, gy, 15, 15, "spouse");
			chk_mdb_spouse->value(progStatus.mdb_spouse);
			chk_mdb_spouse->align(FL_ALIGN_RIGHT);
			chk_mdb_spouse->callback((Fl_Callback*)cb_chk_mdb_spouse);
gx += 105; gy = 45;
			chk_mdb_sp_birth = new Fl_Check_Button(gx, gy, 15, 15, "sp_birth");
			chk_mdb_sp_birth->value(progStatus.mdb_sp_birth);
			chk_mdb_sp_birth->align(FL_ALIGN_RIGHT);
			chk_mdb_sp_birth->callback((Fl_Callback*)cb_chk_mdb_sp_birth);
gy += 22;
			chk_mdb_comment1 = new Fl_Check_Button(gx, gy, 15, 15, "comment1");
			chk_mdb_comment1->value(progStatus.mdb_comment1);
			chk_mdb_comment1->align(FL_ALIGN_RIGHT);
			chk_mdb_comment1->callback((Fl_Callback*)cb_chk_mdb_comment1);
gy += 22;
			chk_mdb_comment2 = new Fl_Check_Button(gx, gy, 15, 15, "comment2");
			chk_mdb_comment2->value(progStatus.mdb_comment2);
			chk_mdb_comment2->align(FL_ALIGN_RIGHT);
			chk_mdb_comment2->callback((Fl_Callback*)cb_chk_mdb_comment2);
gy += 22;
			chk_mdb_email = new Fl_Check_Button(gx, gy, 15, 15, "email");
			chk_mdb_email->value(progStatus.mdb_email);
			chk_mdb_email->align(FL_ALIGN_RIGHT);
			chk_mdb_email->callback((Fl_Callback*)cb_chk_mdb_email);
gy += 22;
			chk_mdb_locator = new Fl_Check_Button(gx, gy, 15, 15, "locator");
			chk_mdb_locator->value(progStatus.mdb_locator);
			chk_mdb_locator->align(FL_ALIGN_RIGHT);
			chk_mdb_locator->callback((Fl_Callback*)cb_chk_mdb_locator);
gy += 22;
			chk_mdb_country = new Fl_Check_Button(gx, gy, 15, 15, "country");
			chk_mdb_country->value(progStatus.mdb_country);
			chk_mdb_country->align(FL_ALIGN_RIGHT);
			chk_mdb_country->callback((Fl_Callback*)cb_chk_mdb_country);
gy += 22;
			chk_mdb_logdate = new Fl_Check_Button(gx, gy, 15, 15, "logdate");
			chk_mdb_logdate->value(progStatus.mdb_logdate);
			chk_mdb_logdate->align(FL_ALIGN_RIGHT);
			chk_mdb_logdate->callback((Fl_Callback*)cb_chk_mdb_logdate);
gx += 105; gy = 45;
			chk_mdb_prevdate = new Fl_Check_Button(gx, gy, 15, 15, "prevdate");
			chk_mdb_prevdate->value(progStatus.mdb_prevdate);
			chk_mdb_prevdate->align(FL_ALIGN_RIGHT);
			chk_mdb_prevdate->callback((Fl_Callback*)cb_chk_mdb_prevdate);
gy += 22;
			chk_mdb_nbrlogins = new Fl_Check_Button(gx, gy, 15, 15, "nbrlogins");
			chk_mdb_nbrlogins->value(progStatus.mdb_nbrlogins);
			chk_mdb_nbrlogins->align(FL_ALIGN_RIGHT);
			chk_mdb_nbrlogins->callback((Fl_Callback*)cb_chk_mdb_nbrlogins);
gy += 22;
			chk_mdb_status = new Fl_Check_Button(gx, gy, 15, 15, "status");
			chk_mdb_status->value(progStatus.mdb_status);
			chk_mdb_status->align(FL_ALIGN_RIGHT);
			chk_mdb_status->callback((Fl_Callback*)cb_chk_mdb_status);
gy += 22;
			chk_mdb_joined = new Fl_Check_Button(gx, gy, 15, 15, "joined");
			chk_mdb_joined->value(progStatus.mdb_joined);
			chk_mdb_joined->align(FL_ALIGN_RIGHT);
			chk_mdb_joined->callback((Fl_Callback*)cb_chk_mdb_joined);
gy += 22;
			chk_mdb_netnbr = new Fl_Check_Button(gx, gy, 15, 15, "netnbr");
			chk_mdb_netnbr->value(progStatus.mdb_netnbr);
			chk_mdb_netnbr->align(FL_ALIGN_RIGHT);
			chk_mdb_netnbr->callback((Fl_Callback*)cb_chk_mdb_netnbr);

			Fl_Button *btn_setall = new Fl_Button(340, 164, 60, 24, "Set All");
			btn_setall->callback((Fl_Callback*)cb_btn_setall);

			Fl_Button *btn_clrall = new Fl_Button(340, 190, 60, 24, "Clr All");
			btn_clrall->callback((Fl_Callback*)cb_btn_clrall);

		tabGroupSharedFields->end();

	tabsConfig->end();

	btnConfigOK = new Fl_Return_Button(355, w->h() - 26, 75, 24, "OK");
	btnConfigOK->callback((Fl_Callback*)cb_btnCloseConfig);

	w->end();

	return w;
}

Fl_Browser *log_in_view=(Fl_Browser *)0;

Fl_Button *btn_copy_to_clipboard=(Fl_Button *)0;

static void cb_btn_copy_to_clipboard(Fl_Button*, void*) {
	copy_to_clipboard();
}

Fl_Button *btn_close_log_ins=(Fl_Button *)0;

static void cb_btn_close_log_ins(Fl_Button* o, void*) {
	o->parent()->hide();
}

Fl_Double_Window* Log_ins_dialog() {
	Fl_Double_Window* w = new Fl_Double_Window(300, 235, "Current Log Ins");

	log_in_view = new Fl_Browser(2, 2, 296, 205);
	log_in_view->align(Fl_Align(FL_ALIGN_TOP));
	log_in_view->has_scrollbar(Fl_Browser_::VERTICAL_ALWAYS);

	btn_copy_to_clipboard = new Fl_Button(40, 210, 70, 20, "Copy");
	btn_copy_to_clipboard->callback((Fl_Callback*)cb_btn_copy_to_clipboard);

	btn_close_log_ins = new Fl_Button(190, 210, 70, 20, "Close");
	btn_close_log_ins->callback((Fl_Callback*)cb_btn_close_log_ins);

	return w;
}
