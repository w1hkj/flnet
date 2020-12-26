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

static Fl_Window *cfgDialog = 0;

void close_config(void)
{
	if(cfgDialog)
		cfgDialog->hide();
}

void cbConfig ()
{
	if (!cfgDialog)
		cfgDialog = configDialog ();
	cfgP1->value (progStatus.chP1.c_str());
	cfgP2->value (progStatus.chP2.c_str());
	cfgP3->value (progStatus.chP3.c_str());
	inpStatesList1->value(progStatus.strP1.c_str());
	inpStatesList2->value(progStatus.strP2.c_str());
	inpStatesList3->value(progStatus.strP3.c_str());
	if (progStatus.chAuto == 'y' || progStatus.chAuto == 'Y')
		chkAutoPriority->value (1);
	else
		chkAutoPriority->value (0);
	cfgDialog->show();
}

void cbCloseConfig ()
{
	progStatus.chP1 = cfgP1->value();
	if (progStatus.chP1.empty()) progStatus.chP1 = " ";
	progStatus.chP2 = cfgP2->value();
	if (progStatus.chP2.empty()) progStatus.chP2 = " ";
	progStatus.chP3 = cfgP3->value();
	if (progStatus.chP3.empty()) progStatus.chP3 = " ";

	progStatus.strP1 = inpStatesList1->value();
	progStatus.strP2 = inpStatesList2->value();
	progStatus.strP3 = inpStatesList3->value();

	callinlist.setPri_1 (progStatus.chP1[0]);
	callinlist.setPri_2 (progStatus.chP2[0]);
	callinlist.setPri_3 (progStatus.chP3[0]);

	if (chkAutoPriority->value() == 1) {
		progStatus.chAuto = 'y';
		callinlist.AutoPriority (1);
	} else {
		progStatus.chAuto = ' ';
		callinlist.AutoPriority (0);
	}

	cfgDialog->hide ();

}
