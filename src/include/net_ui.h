//======================================================================
// net_ui.h
//
// Authors:
//
// Copyright (C) 2020, Dave Freese, W1HKJ
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
//======================================================================

#ifndef net_ui_h
#define net_ui_h

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_show_colormap.H>
#include <FL/Fl_Choice.H>

#include "flinput2.h"
#include "my_UI.h"

extern Fl_Menu_Bar *mnu_bar;
extern void cb_mnuOpen(Fl_Menu_*, void*);
extern void cb_mnuNew(Fl_Menu_*, void*);
extern void cb_mnuExit(Fl_Menu_*, void*);
extern void cb_mnuEditor(Fl_Menu_*, void*);
extern void cb_mnuConfig(Fl_Menu_*, void*);
extern void cb_mnuLogIns(Fl_Menu_*, void*);
extern void cb_mnuSize(Fl_Menu_*, void*);
extern void cb_mnuHelpContent(Fl_Menu_*, void*);
extern void cb_mnuEventLog(Fl_Menu_*, void*);
extern void cb_mnuAbout(Fl_Menu_*, void*);

//extern my_UI *myUI;
extern Fl_Group *myUI;

//extern Fl_Input *inp_focus;
extern my_UI *inp_focus;

extern Fl_Group *net_grp1;
extern Fl_Box *box_callins;
extern Fl_Box *ptr_left;
extern Fl_Box *ptr_right;
extern Fl_Group *net_grp2;
extern Fl_Box *txtTitles;
extern Fl_Box *txtLine[15];
extern Fl_Box *box_db_select;
extern Fl_Group *dbSelectGroup;
extern Fl_Box *boxLoginSuffix;
extern Fl_Box *boxLoginPrefix;
extern Fl_Box *boxLoginArea;
extern Fl_Group *net_grp3;
extern Fl_Box *txtPick[NPICKITEMS];
extern Fl_Box *txtPickArrows;
extern Fl_Box *bx_suffix;
extern Fl_Box *bx_prefix;
extern Fl_Box *bx_area;
extern Fl_Box *txtInfo;

Fl_Double_Window* newNetControl();

extern Fl_Menu_Item menu_mnu_bar[];
#define mnuOpen (menu_mnu_bar+1)
#define mnuExit (menu_mnu_bar+2)
#define mnuEditor (menu_mnu_bar+4)
#define mnuConfig (menu_mnu_bar+5)
#define mnuLogIns (menu_mnu_bar+6)
#define mnuSize (menu_mnu_bar+7)
#define mnu_Content (menu_mnu_bar+9)
#define mnu_About (menu_mnu_bar+10)

extern void cb_btnCloseConfig(Fl_Return_Button*, void*);
extern Fl_Return_Button *btnConfigOK;

extern Fl_Double_Window* configDialog();

extern Fl_Browser *log_in_view;
extern Fl_Button *btn_copy_to_clipboard;
extern Fl_Button *btn_close_log_ins;
extern Fl_Double_Window* Log_ins_dialog();

extern Fl_Group *tabGroupColors;
extern Fl_Group *tabGroupPriority;
extern Fl_Group *tabGroupLookup;
extern Fl_Group *tabGroupUI;
extern Fl_Group *tabGroupMasterDB;
extern Fl_Group *grp_sort_order;

extern Fl_Input2 *cfgP1;
extern Fl_Input2 *inpStatesList1;
extern Fl_Input2 *cfgP2;
extern Fl_Input2 *inpStatesList2;
extern Fl_Input2 *cfgP3;
extern Fl_Input2 *inpStatesList3;
extern Fl_Input2 *inp_myLocator;
extern Fl_Input2 *inp_user_name;
extern Fl_Input2 *inp_user_password;
extern Fl_Input2 *inp_callookurl;
extern Fl_Input2 *inp_hamqthurl;
extern Fl_Input2 *inp_hamcallurl;
extern Fl_Input2 *inp_qrzurl;
extern Fl_Input2 *inp_masterdb;

extern Fl_Button *btn_masterdb;
extern Fl_Box    *box_mdb_isopen;
extern Fl_Button *btnFg[];
extern Fl_Button *btnBg[];

extern Fl_Check_Button *chkAutoPriority;
extern Fl_Check_Button *chk_pwd;
extern Fl_Check_Button *chk_callook;
extern Fl_Check_Button *chk_hamqth;
extern Fl_Check_Button *chk_hamcall;
extern Fl_Check_Button *chk_qrz;
extern Fl_Check_Button *chk_mdb_netnbr;
extern Fl_Check_Button *chk_call_left_justify;
extern Fl_Check_Button *chk_name_left_justify;
extern Fl_Check_Button *btn_sort_by_PAS;
extern Fl_Check_Button *btn_sort_by_APS;
extern Fl_Check_Button *btn_sort_by_SAP;
extern Fl_Check_Button *btn_sort_by_NETNBR;
extern Fl_Check_Button *btn_new_login_is_up;
extern Fl_Check_Button *btnOpenEditor;
extern Fl_Check_Button *btn_current_call_in_is_up;

extern Fl_Return_Button *btnConfigOK;

extern Fl_Choice *combo_arc_conversion;

extern Fl_Output *txtSample[];

#endif
