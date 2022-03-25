//======================================================================
// config.cxx
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
//======================================================================

#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <mem.h>

#include "FL/Fl.H"

#include "config.h"
#include "net_ui.h"
#include "loglist.h"
#include "netshared.h"
#include "lookupcall.h"

#include "status.h"

extern loglist callinlist;

extern Fl_Output *txt_selected_font;

static Fl_Window *cfgDialog = 0;

void close_config(void)
{
	if(cfgDialog)
		cfgDialog->hide();
	myUI->set_visible_focus();
}

void create_config()
{
	if (!cfgDialog)
		cfgDialog = configDialog ();
}

#include "font_browser.h"

void cbConfig ()
{
	create_config();

	btn_new_login_is_up->value(progStatus.disp_new_login);
	btnOpenEditor->value(progStatus.open_editor);
	btn_current_call_in_is_up->value(progStatus.callin_is_up);

	txt_selected_font->value( font_browser->fontName(progStatus.ui_font) );

	cbo_call_justify->value(progStatus.call_justify);
	cbo_name_justify->value(progStatus.name_justify);

	combo_arc_conversion->value(progStatus.arc_conversion);

	btn_sort_by_PAS->value(progStatus.preferred_sort_order == 0);
	btn_sort_by_APS->value(progStatus.preferred_sort_order == 1);
	btn_sort_by_SAP->value(progStatus.preferred_sort_order == 2);
	btn_sort_by_NETNBR->value(progStatus.preferred_sort_order == 3);

	f1_text->value(progStatus.f1_text.c_str());
	f2_text->value(progStatus.f2_text.c_str());
	f3_text->value(progStatus.f3_text.c_str());
	f4_text->value(progStatus.f4_text.c_str());

	txt_sample->value("sample text");

	cfgP1->value (progStatus.chP1.c_str());
	cfgP2->value (progStatus.chP2.c_str());
	cfgP3->value (progStatus.chP3.c_str());
	inpStatesList1->value(progStatus.strP1.c_str());
	inpStatesList2->value(progStatus.strP2.c_str());
	inpStatesList3->value(progStatus.strP3.c_str());
	chkAutoPriority->value (progStatus.chAuto);

	inp_myLocator->value(progStatus.myLocator.c_str());
	inp_user_name->value(progStatus.user_name.c_str());
	inp_user_password->value(progStatus.user_password.c_str());

	chk_callook->value(progStatus.QRZXML == CALLOOK);
	inp_callookurl->value(progStatus.callookurl.c_str());
	chk_hamqth->value(progStatus.QRZXML == HAMQTH);
	inp_hamqthurl->value(progStatus.hamqthurl.c_str());
	chk_hamcall->value(progStatus.QRZXML == HAMCALLNET);
	inp_hamcallurl->value(progStatus.hamcallurl.c_str());
	chk_qrz->value(progStatus.QRZXML == QRZNET);
	inp_qrzurl->value(progStatus.qrzurl.c_str());

	inp_masterdb->value(progStatus.masterdb.c_str());
	chk_mdb_netnbr->value(progStatus.mdb_netnbr);
	box_mdb_isopen->color(progStatus.mdb_isopen ? progStatus.mdb_color : FL_BACKGROUND2_COLOR);
	cfgDialog->show();
}

void cbCloseConfig ()
{
	if(cfgDialog)
		cfgDialog->hide();
}
