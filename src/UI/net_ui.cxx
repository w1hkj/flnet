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
#include "net_ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FL/fl_show_colormap.H>
#include "netsupport.h"
#include "netshared.h"
#include "config.h"
#include "net_config.h"
#include "debug.h"
#include "lookupcall.h"

Fl_Menu_Bar *mnu_bar=(Fl_Menu_Bar *)0;

static void cb_mnuLogIns(Fl_Menu_*, void*) {
	open_log_ins();
}

static void cb_mnuSize(Fl_Menu_*, void*) {
	change_size();
}

Fl_Menu_Item menu_mnu_bar[] = {
	{"Files", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
	{"Open", 0,  (Fl_Callback*)cb_mnuOpen, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"New", 0,   (Fl_Callback*)cb_mnuNew, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"Exit", 0,  (Fl_Callback*)cb_mnuExit, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
	{"Editor", 0,  (Fl_Callback*)cb_mnuEditor, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Config", 0,  (Fl_Callback*)cb_mnuConfig, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Log-Ins", 0,  (Fl_Callback*)cb_mnuLogIns, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Size", 0,  (Fl_Callback*)cb_mnuSize, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Help", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
	{"Content", 0,  (Fl_Callback*)cb_mnuHelpContent, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"Event log", 0, (Fl_Callback*)cb_mnuEventLog, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"About", 0,  (Fl_Callback*)cb_mnuAbout, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}
};

my_UI *myUI=(my_UI *)0;
Fl_Box *txtNcallins=(Fl_Box *)0;
Fl_Group *net_grp1=(Fl_Group *)0;
Fl_Box *ptr_left=(Fl_Box *)0;
Fl_Box *ptr_right=(Fl_Box *)0;
Fl_Group *net_grp2=(Fl_Group *)0;
Fl_Box *txtTitles=(Fl_Box *)0;
Fl_Box *txtLine[15]={(Fl_Box *)0};
Fl_Output *dbSelectLabel=(Fl_Output *)0;
Fl_Group *dbSelectGroup=(Fl_Group *)0;
Fl_Box *inpLoginSuffix=(Fl_Box *)0;
Fl_Box *inpLoginPrefix=(Fl_Box *)0;
Fl_Box *inpLoginArea=(Fl_Box *)0;
Fl_Group *net_grp3=(Fl_Group *)0;
Fl_Box *txtPick[NPICKITEMS]={(Fl_Box *)0};
Fl_Box *txtPickArrows=(Fl_Box *)0;
Fl_Box *bx_suffix=(Fl_Box *)0;
Fl_Box *bx_prefix=(Fl_Box *)0;
Fl_Box *bx_area=(Fl_Box *)0;
Fl_Box *txtInfo=(Fl_Box *)0;

Fl_Double_Window* newNetControl() {
	Fl_Double_Window* w = new Fl_Double_Window(390, 455, "flnet");
		w->box(FL_BORDER_BOX);

		mnu_bar = new Fl_Menu_Bar(0, 0, 390, 25);
		mnu_bar->menu(menu_mnu_bar);

		myUI = new my_UI(0, 25, 390, 430);
		myUI->box(FL_NO_BOX);
		myUI->color(FL_BACKGROUND_COLOR);
		myUI->selection_color(FL_BACKGROUND_COLOR);
		myUI->labeltype(FL_NORMAL_LABEL);
		myUI->labelfont(0);
		myUI->labelsize(14);
		myUI->labelcolor(FL_FOREGROUND_COLOR);
		myUI->align(Fl_Align(FL_ALIGN_TOP));
		myUI->when(FL_WHEN_RELEASE);

		txtNcallins = new Fl_Box(125, 25, 75, 20, "0 callins");
		txtNcallins->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

		net_grp1 = new Fl_Group(0, 45, 225, 255, "      Call In List");
		net_grp1->tooltip("Use UP/DN arrow keys to scroll list");
		net_grp1->labelfont(0);
		net_grp1->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			ptr_left = new Fl_Box(5, 125, 15, 25, "@>");

			ptr_right = new Fl_Box(210, 125, 15, 25, "@<");

			net_grp2 = new Fl_Group(17, 45, 196, 255);
			net_grp2->box(FL_DOWN_BOX);
			net_grp2->color(FL_BACKGROUND2_COLOR);

				txtTitles = new Fl_Box(20, 47, 190, 22, "Call    Name    Time  F");
				txtTitles->box(FL_FLAT_BOX);
				txtTitles->color((Fl_Color)23);
				txtTitles->labelfont(0);
				txtTitles->labelsize(13);
				txtTitles->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

				for (int i = 0; i < 15; i++) {
					txtLine[i] = new Fl_Box(20, 70 + 15 * i, 190, 15);
					txtLine[i]->box(FL_FLAT_BOX);
					txtLine[i]->color(FL_BACKGROUND2_COLOR);
					txtLine[i]->labelfont(0);
					txtLine[i]->labelsize(13);
					txtLine[i]->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
				}
			net_grp2->end();

		net_grp1->end();

		dbSelectLabel = new Fl_Output(230, 27, 155, 18);
		dbSelectLabel->color(FL_BACKGROUND_COLOR);
		dbSelectLabel->box(FL_FLAT_BOX);

		dbSelectGroup = new Fl_Group(230, 45, 155, 250);
		dbSelectGroup->box(FL_ENGRAVED_FRAME);
		dbSelectGroup->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			inpLoginSuffix = new Fl_Box(240, 65, 40, 22);
			inpLoginSuffix->tooltip("Enter SUFFIX, Esc to abort");
			inpLoginSuffix->box(FL_DOWN_BOX);
			inpLoginSuffix->color(FL_BACKGROUND2_COLOR);
			inpLoginSuffix->labelfont(0);
			inpLoginSuffix->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			inpLoginPrefix = new Fl_Box(285, 65, 30, 22);
			inpLoginPrefix->tooltip("Enter PREFIX Esc to abort");
			inpLoginPrefix->box(FL_DOWN_BOX);
			inpLoginPrefix->color(FL_BACKGROUND2_COLOR);
			inpLoginPrefix->labelfont(0);
			inpLoginPrefix->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			inpLoginArea = new Fl_Box(320, 65, 40, 22);
			inpLoginArea->tooltip("Enter AREA Esc to abort");
			inpLoginArea->box(FL_DOWN_BOX);
			inpLoginArea->color(FL_BACKGROUND2_COLOR);
			inpLoginArea->labelfont(0);
			inpLoginArea->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			net_grp3 = new Fl_Group(235, 95, 146, 12*NPICKITEMS/2 + 4);
			net_grp3->tooltip("Use UP/DN arrow keys to select callsign");
			net_grp3->box(FL_DOWN_BOX);
			net_grp3->color(FL_BACKGROUND2_COLOR);

				for (int i = 0; i < NPICKITEMS / 2; i++) {
					txtPick[i] = new Fl_Box(237, 97 + 12*i, 72, 12);
					txtPick[i]->color(FL_BACKGROUND2_COLOR);
					txtPick[i]->labelfont(0);
					txtPick[i]->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

					txtPick[i + NPICKITEMS/2] = new Fl_Box(308, 97 + 12*i, 72, 12);
					txtPick[i + NPICKITEMS/2]->color(FL_BACKGROUND2_COLOR);
					txtPick[i + NPICKITEMS/2]->labelfont(0);
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
	txtInfo->labelfont(0);
	txtInfo->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

	return w;
}

Fl_Tabs *tabsConfig=(Fl_Tabs *)0;

Fl_Check_Button *btn_new_login_is_up=(Fl_Check_Button *)0;

static void cb_btn_new_login_is_up(Fl_Check_Button* o, void*) {
	disp_new_login=o->value();
}

Fl_Check_Button *btnOpenEditor=(Fl_Check_Button *)0;

static void cb_btnOpenEditor(Fl_Check_Button* o, void*) {
	open_editor = o->value();
}

Fl_Check_Button *btn_current_call_in_is_up=(Fl_Check_Button *)0;

static void cb_btn_current_call_in_is_up(Fl_Check_Button* o, void*) {
	callin_is_up = o->value();
}

Fl_Group *tabGroupColors=(Fl_Group *)0;

static void cb_btnFg(Fl_Button*, void*) {
	fgColors[1] = fl_show_colormap(fgColors[1]);
	txtSample[1]->textcolor(fgColors[1]);
	txtSample[1]->redraw();
}

static void cb_btnBg(Fl_Button*, void*) {
	bgColors[1] = fl_show_colormap(bgColors[1]);
	txtSample[1]->color(bgColors[1]);
	txtSample[1]->redraw();
}

static void cb_btnFg1(Fl_Button*, void*) {
	fgColors[2] = fl_show_colormap(fgColors[2]);
	txtSample[2]->textcolor(fgColors[2]);
	txtSample[2]->redraw();
}

static void cb_btnBg1(Fl_Button*, void*) {
	bgColors[2] = fl_show_colormap(bgColors[2]);
	txtSample[2]->color(bgColors[2]);
	txtSample[2]->redraw();
}

static void cb_btnFg2(Fl_Button*, void*) {
	fgColors[3] = fl_show_colormap(fgColors[3]);
	txtSample[3]->textcolor(fgColors[3]);
	txtSample[3]->redraw();
}

static void cb_btnBg2(Fl_Button*, void*) {
	bgColors[3] = fl_show_colormap(bgColors[3]);
	txtSample[3]->color(bgColors[3]);
	txtSample[3]->redraw();
}

Fl_Output *txtSample[5]={(Fl_Output *)0};

Fl_Button *btnFg[5]={(Fl_Button *)0};

static void cb_btnFg3(Fl_Button*, void*) {
	fgColors[4] = fl_show_colormap(fgColors[4]);
	txtSample[4]->textcolor(fgColors[4]);
	txtSample[4]->redraw();
}

Fl_Button *btnBg[5]={(Fl_Button *)0};

static void cb_btnBg3(Fl_Button*, void*) {
	bgColors[4] = fl_show_colormap(bgColors[4]);
	txtSample[4]->color(bgColors[4]);
	txtSample[4]->redraw();
}

Fl_Group *tabGroupPriority = (Fl_Group *)0;
Fl_Input *cfgP1 = (Fl_Input *)0;
Fl_Input *inpStatesList1 = (Fl_Input *)0;
Fl_Input *cfgP2 = (Fl_Input *)0;
Fl_Input *inpStatesList2 = (Fl_Input *)0;
Fl_Input *cfgP3 = (Fl_Input *)0;
Fl_Input *inpStatesList3 = (Fl_Input *)0;
Fl_Check_Button *chkAutoPriority = (Fl_Check_Button *)0;

Fl_Group *tabGroupLookup = (Fl_Group *)0;
Fl_Input *inp_myLocator = (Fl_Input *)0;
Fl_Input *inp_user_name = (Fl_Input *)0;
Fl_Input *inp_user_password = (Fl_Input *)0;
Fl_Check_Button *chk_pwd = (Fl_Check_Button *)0;

Fl_Check_Button *chk_callook = (Fl_Check_Button *)0;
Fl_Input *inp_callookurl = (Fl_Input *)0;

Fl_Check_Button *chk_hamqth = (Fl_Check_Button *)0;
Fl_Input *inp_hamqthurl = (Fl_Input *)0;

Fl_Check_Button *chk_hamcall = (Fl_Check_Button *)0;
Fl_Input *inp_hamcallurl = (Fl_Input *)0;

Fl_Check_Button *chk_qrz = (Fl_Check_Button *)0;
Fl_Input *inp_qrzurl = (Fl_Input *)0;

Fl_Return_Button *btnConfigOK = (Fl_Return_Button *)0;

static void cb_myLocator(Fl_Input *, void *)
{
	progdefaults.myLocator = inp_myLocator->value();
}

static void cb_user_name(Fl_Input *, void *)
{
	progdefaults.user_name = inp_user_name->value();
}

static void cb_user_password(Fl_Input *, void *)
{
	progdefaults.user_password = inp_user_password->value();
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
	progdefaults.QRZXML = CALLOOK;
}

static void cb_callookurl(Fl_Input *, void *)
{
	progdefaults.callookurl = inp_callookurl->value();
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
	progdefaults.QRZXML = HAMQTH;
}

static void cb_hamqthurl(Fl_Input *, void *)
{
	progdefaults.hamqthurl = inp_hamcallurl->value();
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
	progdefaults.QRZXML = HAMCALLNET;
}

static void cb_hamcallurl(Fl_Input *, void *)
{
	progdefaults.hamcallurl = inp_hamcallurl->value();
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
	progdefaults.QRZXML = QRZNET;
}

static void cb_hamqrzurl(Fl_Input *, void *)
{
	progdefaults.qrzurl = inp_qrzurl->value();
}

Fl_Double_Window* configDialog() {
	Fl_Double_Window* w = new Fl_Double_Window(440, 250, "Net Configuration");

	tabsConfig = new Fl_Tabs(5, 10, 430, 210);
	tabsConfig->color((Fl_Color)44);
		Fl_Group* g1 = new Fl_Group(5, 35, 430, 185, "UI behavior");
			btn_new_login_is_up = new Fl_Check_Button(30, 60, 70, 15, "New login is up");
			btn_new_login_is_up->tooltip("Move new login to the >...<\nspot in the calll in list");
			btn_new_login_is_up->down_box(FL_DOWN_BOX);
			btn_new_login_is_up->callback((Fl_Callback*)cb_btn_new_login_is_up);
			btn_new_login_is_up->value(disp_new_login);

			btnOpenEditor = new Fl_Check_Button(30, 102, 70, 15, "Open editor for new login");
			btnOpenEditor->tooltip("Open editor for new call in\nNew login is up must be enabled");
			btnOpenEditor->down_box(FL_DOWN_BOX);
			btnOpenEditor->callback((Fl_Callback*)cb_btnOpenEditor);
			btnOpenEditor->value(open_editor);

			btn_current_call_in_is_up = new Fl_Check_Button(30, 145, 70, 15, "Current call in is up");
			btn_current_call_in_is_up->tooltip("Move last login to the >...<\nspot in the calll in list");
			btn_current_call_in_is_up->down_box(FL_DOWN_BOX);
			btn_current_call_in_is_up->callback((Fl_Callback*)cb_btn_current_call_in_is_up);
			btn_current_call_in_is_up->value(callin_is_up);

		g1->end();

		tabGroupColors = new Fl_Group(5, 35, 430, 185, "Colors");
		tabGroupColors->hide();
			txtSample[1] = new Fl_Output(145, 60, 45, 25, "Logged In");
			txtSample[1]->textfont(0);
			txtSample[1]->value("Text");
			txtSample[1]->color(bgColors[1]);
			txtSample[1]->textcolor(fgColors[1]);

			btnFg[1] = new Fl_Button(205, 60, 45, 25, "Fg");
			btnFg[1]->callback((Fl_Callback*)cb_btnFg);

			btnBg[2] = new Fl_Button(270, 60, 45, 25, "Bg");
			btnBg[2]->callback((Fl_Callback*)cb_btnBg);

			txtSample[2] = new Fl_Output(145, 95, 45, 25, "First Response");
			txtSample[2]->textfont(0);
			txtSample[2]->value("Text");
			txtSample[2]->color(bgColors[2]);
			txtSample[2]->textcolor(fgColors[2]);

			btnFg[2] = new Fl_Button(205, 95, 45, 25, "Fg");
			btnFg[2]->callback((Fl_Callback*)cb_btnFg1);

			btnBg[2] = new Fl_Button(270, 95, 45, 25, "Bg");
			btnBg[2]->callback((Fl_Callback*)cb_btnBg1);

			txtSample[3] = new Fl_Output(145, 130, 45, 25, "Second Response");
			txtSample[3]->textfont(0);
			txtSample[3]->value("Text");
			txtSample[3]->color(bgColors[3]);
			txtSample[3]->textcolor(fgColors[3]);

			btnFg[3] = new Fl_Button(205, 130, 45, 25, "Fg");
			btnFg[3]->callback((Fl_Callback*)cb_btnFg2);

			btnBg[3] = new Fl_Button(270, 130, 45, 25, "Bg");
			btnBg[3]->callback((Fl_Callback*)cb_btnBg2);

			txtSample[4] = new Fl_Output(145, 165, 45, 25, "Logged Out");
			txtSample[4]->textfont(0);
			txtSample[4]->value("Text");
			txtSample[4]->color(bgColors[4]);
			txtSample[4]->textcolor(fgColors[4]);

			btnFg[4] = new Fl_Button(205, 165, 45, 25, "Fg");
			btnFg[4]->callback((Fl_Callback*)cb_btnFg3);

			btnBg[4] = new Fl_Button(270, 165, 45, 25, "Bg");
			btnBg[4]->callback((Fl_Callback*)cb_btnBg3);

		tabGroupColors->end();

		tabGroupPriority = new Fl_Group(5, 35, 430, 185, "Priority");
		tabGroupPriority->hide();
			cfgP1 = new Fl_Input(160, 70, 20, 25, "Priority 1 character");

			inpStatesList1 = new Fl_Input(200, 70, 225, 25, "States List (ie: FL, AL, GA)");
			inpStatesList1->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			cfgP2 = new Fl_Input(160, 100, 20, 25, "Priority 2 character");

			inpStatesList2 = new Fl_Input(200, 100, 225, 25);
			inpStatesList2->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			cfgP3 = new Fl_Input(160, 130, 20, 25, "Priority 3 character");

			inpStatesList3 = new Fl_Input(200, 130, 225, 25);
			inpStatesList3->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			chkAutoPriority = new Fl_Check_Button(160, 165, 25, 25, "Auto By Priority");
			chkAutoPriority->down_box(FL_DOWN_BOX);
			chkAutoPriority->align(Fl_Align(FL_ALIGN_LEFT));

		tabGroupPriority->end();

		tabGroupLookup = new Fl_Group(5, 35, 430, 185, "Lookup");
		tabGroupLookup->hide();

			inp_myLocator = new Fl_Input(70, 50, 120, 24, "My Loc:");
			inp_myLocator->value(progdefaults.myLocator.c_str());
			inp_myLocator->callback((Fl_Callback*)cb_myLocator);

			inp_user_name = new Fl_Input(70, 80, 120, 24, "Id:");
			inp_user_name->value(progdefaults.user_name.c_str());
			inp_user_name->callback((Fl_Callback*)cb_user_name);

			inp_user_password = new Fl_Input(230, 80, 120, 24, "Pwd:");
			inp_user_password->value(progdefaults.user_password.c_str());
			inp_user_password->callback((Fl_Callback*)cb_user_password);
			inp_user_password->type(FL_SECRET_INPUT);

			chk_pwd = new Fl_Check_Button(363, 80, 40, 18, "Show");
			chk_pwd->value(false);
			chk_pwd->callback((Fl_Callback*)cb_chk_pwd);

			chk_callook = new Fl_Check_Button(23, 115, 50, 18, "callook.info:");
			chk_callook->value(progdefaults.QRZXML == CALLOOK);
			chk_callook->callback((Fl_Callback*)cb_chk_callook);

			inp_callookurl = new Fl_Input(140, 115, 280, 24, "");
			inp_callookurl->value(progdefaults.callookurl.c_str());
			inp_callookurl->callback((Fl_Callback*)cb_callookurl);

			chk_hamqth = new Fl_Check_Button(23, 140, 50, 18, "hamqth.com:");
			chk_hamqth->value(progdefaults.QRZXML == HAMQTH);
			chk_hamqth->callback((Fl_Callback*)cb_chk_hamqth);

			inp_hamqthurl = new Fl_Input(140, 140, 280, 24, "");
			inp_hamqthurl->value(progdefaults.hamqthurl.c_str());
			inp_hamqthurl->callback((Fl_Callback*)cb_hamqthurl);

			chk_hamcall = new Fl_Check_Button(23, 165, 50, 18, "hamcall.net:");
			chk_hamcall->value(progdefaults.QRZXML == HAMCALLNET);
			chk_hamcall->callback((Fl_Callback*)cb_chk_hamcall);

			inp_hamcallurl = new Fl_Input(140, 165, 280, 24, "");
			inp_hamcallurl->value(progdefaults.hamcallurl.c_str());
			inp_hamcallurl->callback((Fl_Callback*)cb_hamcallurl);

			chk_qrz = new Fl_Check_Button(23, 190, 50, 18, "qrz.com:");
			chk_qrz->value(progdefaults.QRZXML == QRZNET);
			chk_qrz->callback((Fl_Callback*)cb_chk_qrz);

			inp_qrzurl = new Fl_Input(140, 190, 180, 24, "");
			inp_qrzurl->value(progdefaults.qrzurl.c_str());
			inp_qrzurl->callback((Fl_Callback*)cb_hamqrzurl);

		tabGroupLookup->end();

	tabsConfig->end();

	btnConfigOK = new Fl_Return_Button(355, w->h() - 26, 75, 24, "OK");
	btnConfigOK->callback((Fl_Callback*)cb_btnCloseConfig);

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
	Fl_Double_Window* w = new Fl_Double_Window(240, 235, "Current Log Ins");

	log_in_view = new Fl_Browser(0, 0, 240, 205);
	log_in_view->align(Fl_Align(FL_ALIGN_TOP));

	btn_copy_to_clipboard = new Fl_Button(30, 210, 70, 20, "Copy");
	btn_copy_to_clipboard->callback((Fl_Callback*)cb_btn_copy_to_clipboard);

	btn_close_log_ins = new Fl_Button(140, 210, 70, 20, "Close");
	btn_close_log_ins->callback((Fl_Callback*)cb_btn_close_log_ins);

	return w;
}
