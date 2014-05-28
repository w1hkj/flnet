//======================================================================
// net_config.h
//
// Authors:
//
// Copyright (C) 2013, Dave Freese, W1HKJ
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

#ifndef CONFIG_H
#define CONFIG_H

#include <FL/Enumerations.H>

extern char chP1[], chP2[], chP3[];
extern char chAuto;
extern char szP1[], szP2[], szP3[];

extern Fl_Color  fgColors[];
extern Fl_Color  bgColors[];

extern int disp_new_login;
extern int open_editor;
extern int callin_is_up;

void readConfig ();
void writeConfig ();

#endif
