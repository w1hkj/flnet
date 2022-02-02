// =====================================================================
//
// sorting.h
//
// Copyright (C) 2022, Dave Freese, W1HKJ
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

#ifndef SORTING_H
#define SORTING_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>

extern Fl_Button *btn_row_up;
extern Fl_Button *btn_row_dn;
extern Fl_Button *btn_colsel_done;

Fl_Double_Window* create_sorting_dialog();

struct SORTED_BY {
	int checked;
	std::string header;
	Fl_Check_Button *chk;
	Fl_Light_Button *btn;
};

extern SORTED_BY columns[];

void load_column_select();

void save_column_select();

#endif
