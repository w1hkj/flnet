// =====================================================================
//
// ui_sizes.cxx
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
// =====================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "icons.h"
#include "net_ui.h"
#include "netshared.h"
#include "status.h"

extern Fl_Window *main_window;

struct UI_SIZES {
	int			x;
	int			y;
	int			w;
	int			h;
	Fl_Font		fnt;
	int			size;
};

UI_SIZES UI_small[] = {
	{   0,   0, 390, 460, FL_COURIER, 12}, // window  0
	{   0,   0, 390,  25, FL_COURIER, 12}, // menu_ bar 1
	{   0,  25, 390, 430, FL_COURIER, 12}, // myUI 2
	{  15,  26, 140,  18, FL_COURIER_BOLD, 12}, // box_callins
	{   0,  45, 230, 255, FL_COURIER, 12}, // net_grp1  4
	{   2, 125,  15,  25, FL_COURIER, 12}, // ptr_left  4
	{ 215, 125,  15,  25, FL_COURIER, 12}, // ptr_right  6
	{  15,  45, 198, 255, FL_COURIER_BOLD, 14}, // net_grp2  7
	{  17,  47, 194,  26, FL_COURIER_BOLD, 12}, // txtTitles  8
	{  17,  70, 194,  16, FL_COURIER, 12}, // txtLine[0]  9
	{  17,  85, 194,  16, FL_COURIER, 12}, // txtLine[1]  10
	{  17, 100, 194,  16, FL_COURIER, 12}, // txtLine[2]  11
	{  17, 115, 194,  16, FL_COURIER, 12}, // txtLine[3]  12
	{  17, 130, 194,  16, FL_COURIER, 12}, // txtLine[4]  13
	{  17, 145, 194,  16, FL_COURIER, 12}, // txtLine[5]  14
	{  17, 160, 194,  16, FL_COURIER, 12}, // txtLine[6]  15
	{  17, 175, 194,  16, FL_COURIER, 12}, // txtLine[7]  16
	{  17, 190, 194,  16, FL_COURIER, 12}, // txtLine[8]  17
	{  17, 205, 194,  16, FL_COURIER, 12}, // txtLine[9]  18
	{  17, 220, 194,  16, FL_COURIER, 12}, // txtLine[10]  19
	{  17, 235, 194,  16, FL_COURIER, 12}, // txtLine[11]  20
	{  17, 250, 194,  16, FL_COURIER, 12}, // txtLine[12]  21
	{  17, 265, 194,  16, FL_COURIER, 12}, // txtLine[13]  22
	{  17, 280, 194,  16, FL_COURIER, 12}, // txtLine[14]  23

	{ 160,  26, 290,  18, FL_COURIER_BOLD, 12}, // box_db_select
	{ 230,  45, 155, 255, FL_COURIER, 12}, // dbSelectGroup  24
	{ 240,  65,  40,  25, FL_COURIER, 12}, // boxLoginSuffix  25
	{ 285,  65,  30,  25, FL_COURIER, 12}, // boxLoginPrefix  26
	{ 320,  65,  40,  25, FL_COURIER, 12}, // boxLoginArea  27

	{ 235,  95, 146, (NPICKITEMS/2)*12+8, FL_COURIER, 12}, // net_grp3  28
	{ 238,  98,  70,  12, FL_COURIER, 12}, // txtPick[0]   29
	{ 308,  98,  70,  12, FL_COURIER, 12}, // txtPick[12]  39

	{ 320, 135,  25, 120, FL_COURIER, 12}, // txtPickArrows  49
	{ 235,  40,  40,  30, FL_COURIER, 12}, // bx_suffix  50
	{ 280,  40,  40,  30, FL_COURIER, 12}, // bx_prefix  51
	{ 320,  40,  40,  30, FL_COURIER, 12}, // bx_area  52
	{   2, 301, 386, 155, FL_COURIER, 13}, // txtInfo  53
};

UI_SIZES UI_big[] = {
	{   0,   0, 475, 635, FL_COURIER, 16}, // window
	{   0,   0, 475,  25, FL_COURIER, 12}, // menu_ bar
	{   0,  25, 475, 414, FL_COURIER, 16}, // myUI
	{  17,  28, 163,  28, FL_COURIER_BOLD, 15}, // box_callins
	{   0,  49, 275, 396, FL_COURIER, 14}, // net_grp1
	{   2, 180,  15,  24, FL_COURIER, 16}, // ptr_left
	{ 270, 180,  15,  24, FL_COURIER, 16}, // ptr_right
	{  15,  58, 255, 388, FL_COURIER_BOLD, 16}, // net_grp2
	{  17,  60, 251,  24, FL_COURIER_BOLD, 15}, // txtTitles
	{  17,  84, 251,  24, FL_COURIER, 15}, // txtLine[0]
	{  17, 108, 251,  24, FL_COURIER, 15}, // txtLine[1]
	{  17, 132, 251,  24, FL_COURIER, 15}, // txtLine[2]
	{  17, 156, 251,  24, FL_COURIER, 15}, // txtLine[3]
	{  17, 180, 251,  24, FL_COURIER, 15}, // txtLine[4]
	{  17, 204, 251,  24, FL_COURIER, 15}, // txtLine[5]
	{  17, 228, 251,  24, FL_COURIER, 15}, // txtLine[6]
	{  17, 252, 251,  24, FL_COURIER, 15}, // txtLine[7]
	{  17, 276, 251,  24, FL_COURIER, 15}, // txtLine[8]
	{  17, 300, 251,  24, FL_COURIER, 15}, // txtLine[9]
	{  17, 324, 251,  24, FL_COURIER, 15}, // txtLine[10]
	{  17, 348, 251,  24, FL_COURIER, 15}, // txtLine[11]
	{  17, 372, 251,  24, FL_COURIER, 15}, // txtLine[12]
	{  17, 396, 251,  24, FL_COURIER, 15}, // txtLine[13]
	{  17, 420, 251,  24, FL_COURIER, 15}, // txtLine[14]

	{ 190,  28, 435,  28, FL_COURIER_BOLD, 15}, // box_db_select
	{ 284,  60, 185, 380, FL_COURIER, 18}, // dbSelectGroup
	{ 291,  87,  60,  30, FL_COURIER, 18}, // boxLoginSuffix
	{ 355,  87,  60,  30, FL_COURIER, 18}, // boxLoginPrefix
	{ 419,  87,  40,  30, FL_COURIER, 18}, // boxLoginArea

	{ 291, 125, 170, (NPICKITEMS/2)*16+4, FL_COURIER, 16}, // net_grp3  28
	{ 293, 127,  83,  16, FL_COURIER, 16}, // txtPick[0]   29
	{ 376, 127,  83,  16, FL_COURIER, 16}, // txtPick[12]  39

	{ 374, 195,  25, 180, FL_COURIER, 18}, // txtPickArrows
	{ 291,  60,  60,  25, FL_COURIER, 16}, // bx_suffix
	{ 355,  60,  61,  25, FL_COURIER, 16}, // bx_prefix
	{ 419,  60,  40,  25, FL_COURIER, 16}, // bx_area
	{   5, 449, 465, 180, FL_COURIER, 16}, // txtInfo
};

int last_size = SMALL;

void ui_size(int sz, int X, int Y)
{
	UI_SIZES *ui = UI_small;
	if (sz == BIG) ui = UI_big;
	last_size = progStatus.ui_size = sz;

	main_window->resize(X, Y, ui->w, ui->h);

	ui++;
	mnu_bar->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	myUI->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	box_callins->resize(ui->x, ui->y, ui->w, ui->h);
	box_callins->labelfont(ui->fnt);
	box_callins->labelsize(ui->size);
	inp_focus->resize(ui->x, ui->y, 0, 0);//ui->w, ui->h);
	ui++;
	net_grp1->resize(ui->x, ui->y, ui->w, ui->h);
	net_grp1->labelfont(ui->fnt);
	net_grp1->labelsize(ui->size);
	ui++;
	ptr_left->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	ptr_right->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	net_grp2->resize(ui->x, ui->y, ui->w, ui->h);
	net_grp2->labelfont(ui->fnt);
	net_grp2->labelsize(ui->size);
	ui++;
	txtTitles->resize(ui->x, ui->y, ui->w, ui->h);
	txtTitles->labelfont(ui->fnt);
	txtTitles->labelsize(ui->size);
	ui++;
	txtLine[0]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[0]->labelfont(ui->fnt);
	txtLine[0]->labelsize(ui->size);
	ui++;
	txtLine[1]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[1]->labelfont(ui->fnt);
	txtLine[1]->labelsize(ui->size);
	ui++;
	txtLine[2]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[2]->labelfont(ui->fnt);
	txtLine[2]->labelsize(ui->size);
	ui++;
	txtLine[3]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[3]->labelfont(ui->fnt);
	txtLine[3]->labelsize(ui->size);
	ui++;
	txtLine[4]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[4]->labelfont(ui->fnt);
	txtLine[4]->labelsize(ui->size);
	ui++;
	txtLine[5]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[5]->labelfont(ui->fnt);
	txtLine[5]->labelsize(ui->size);
	ui++;
	txtLine[6]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[6]->labelfont(ui->fnt);
	txtLine[6]->labelsize(ui->size);
	ui++;
	txtLine[7]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[7]->labelfont(ui->fnt);
	txtLine[7]->labelsize(ui->size);
	ui++;
	txtLine[8]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[8]->labelfont(ui->fnt);
	txtLine[8]->labelsize(ui->size);
	ui++;
	txtLine[9]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[9]->labelfont(ui->fnt);
	txtLine[9]->labelsize(ui->size);
	ui++;
	txtLine[10]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[10]->labelfont(ui->fnt);
	txtLine[10]->labelsize(ui->size);
	ui++;
	txtLine[11]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[11]->labelfont(ui->fnt);
	txtLine[11]->labelsize(ui->size);
	ui++;
	txtLine[12]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[12]->labelfont(ui->fnt);
	txtLine[12]->labelsize(ui->size);
	ui++;
	txtLine[13]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[13]->labelfont(ui->fnt);
	txtLine[13]->labelsize(ui->size);
	ui++;
	txtLine[14]->resize(ui->x, ui->y, ui->w, ui->h);
	txtLine[14]->labelfont(ui->fnt);
	txtLine[14]->labelsize(ui->size);
	ui++;
	box_db_select->resize(ui->x, ui->y, ui->w, ui->h);
	box_db_select->labelfont(ui->fnt);
	box_db_select->labelsize(ui->size);
	ui++;
	dbSelectGroup->resize(ui->x, ui->y, ui->w, ui->h);
	dbSelectGroup->labelfont(ui->fnt);
	dbSelectGroup->labelsize(ui->size);
	ui++;
	boxLoginSuffix->resize(ui->x, ui->y, ui->w, ui->h);
	boxLoginSuffix->labelfont(ui->fnt);
	boxLoginSuffix->labelsize(ui->size);
	ui++;
	boxLoginPrefix->resize(ui->x, ui->y, ui->w, ui->h);
	boxLoginPrefix->labelfont(ui->fnt);
	boxLoginPrefix->labelsize(ui->size);
	ui++;
	boxLoginArea->resize(ui->x, ui->y, ui->w, ui->h);
	boxLoginArea->labelfont(ui->fnt);
	boxLoginArea->labelsize(ui->size);
	ui++;
	net_grp3->resize(ui->x, ui->y, ui->w, ui->h);
	net_grp3->labelsize(ui->size);
	ui++;
	for (int i = 0; i < NPICKITEMS/2; i++) {
		txtPick[i]->resize(ui->x, ui->y + i * ui->size, ui->w, ui->h);
		txtPick[i]->labelfont(ui->fnt);
		txtPick[i]->labelsize(ui->size);
	}
	ui++;
	for (int i = 0; i < NPICKITEMS/2; i++) {
		txtPick[i + NPICKITEMS/2]->resize(ui->x, ui->y + i * ui->size, ui->w, ui->h);
		txtPick[i + NPICKITEMS/2]->labelfont(ui->fnt);
		txtPick[i + NPICKITEMS/2]->labelsize(ui->size);
	}
	ui++;
	txtPickArrows->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	bx_suffix->resize(ui->x, ui->y, ui->w, ui->h);
	bx_suffix->labelsize(ui->size);
	ui++;
	bx_prefix->resize(ui->x, ui->y, ui->w, ui->h);
	bx_prefix->labelsize(ui->size);
	ui++;
	bx_area->resize(ui->x, ui->y, ui->w, ui->h);
	bx_area->labelsize(ui->size);
	ui++;
	txtInfo->resize(ui->x, ui->y, ui->w, ui->h);
	txtInfo->labelfont(ui->fnt);
	txtInfo->labelsize(ui->size);
	
	main_window->redraw();
}

void change_size()
{
	if (last_size == BIG) {
		ui_size(SMALL, main_window->x(), main_window->y());
	} else {
		ui_size(BIG, main_window->x(), main_window->y());
	}
}
