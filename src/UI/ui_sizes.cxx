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

#include "net_ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "netshared.h"

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
	{ 0, 0, 390, 455, 0, 12}, // window  0
	{ 0, 0, 390, 25, 0, 12}, // menu_ bar 1
	{ 0, 25, 390, 430, 0, 12}, // myUI 2
	{ 125, 25, 74, 20, 1, 14}, // txtNcallins  3
	{ 0, 45, 225, 255, 1, 14}, // net_grp1  4
	{ 5, 125, 15, 25, 0, 12}, // ptr_left  4
	{ 210, 125, 15, 25, 0, 12}, // ptr_right  6
	{ 17, 45, 196, 256, 0, 12}, // net_grp2  7
	{ 20, 47, 190, 26, 4, 13}, // txtTitles  8
	{ 20, 70, 190, 16, 4, 13}, // txtLine[0]  9
	{ 20, 85, 190, 16, 4, 13}, // txtLine[1]  10
	{ 20, 100, 190, 16, 4, 13}, // txtLine[2]  11
	{ 20, 115, 190, 16, 4, 13}, // txtLine[3]  12
	{ 20, 130, 190, 16, 4, 13}, // txtLine[4]  13
	{ 20, 145, 190, 16, 4, 13}, // txtLine[5]  14
	{ 20, 160, 190, 16, 4, 13}, // txtLine[6]  15
	{ 20, 175, 190, 16, 4, 13}, // txtLine[7]  16
	{ 20, 190, 190, 16, 4, 13}, // txtLine[8]  17
	{ 20, 205, 190, 16, 4, 13}, // txtLine[9]  18
	{ 20, 220, 190, 16, 4, 13}, // txtLine[10]  19
	{ 20, 235, 190, 16, 4, 13}, // txtLine[11]  20
	{ 20, 250, 190, 16, 4, 13}, // txtLine[12]  21
	{ 20, 265, 190, 16, 4, 13}, // txtLine[13]  22
	{ 20, 280, 190, 16, 4, 13}, // txtLine[14]  23
	{ 230, 45, 155, 250, 13, 12}, // dbSelectGroup  24
	{ 240, 75, 40, 25, 13, 12}, // inpLoginSuffix  25
	{ 285, 75, 30, 25, 13, 13}, // inpLoginPrefix  26
	{ 320, 75, 40, 25, 13, 13}, // inpLoginArea  27
	{ 240, 115, 75, 160, 0, 12}, // net_grp3  28
	{ 240, 115, 75, 16, 13, 12}, // txtPick[0]   29
	{ 240, 131, 75, 16, 13, 12}, // txtPick[1]  30
	{ 240, 147, 75, 16, 13, 12}, // txtPick[2]  31
	{ 240, 163, 75, 16, 13, 12}, // txtPick[3]  32
	{ 240, 179, 75, 16, 13, 12}, // txtPick[4]  33
	{ 240, 195, 75, 16, 13, 12}, // txtPick[5]  34
	{ 240, 211, 75, 16, 13, 12}, // txtPick[6]  35
	{ 240, 227, 75, 16, 13, 12}, // txtPick[7]  36
	{ 240, 243, 75, 16, 13, 12}, // txtPick[8]  37
	{ 240, 259, 75, 16, 13, 12}, // txtPick[9]  38
	{ 320, 135, 25, 120, 0, 12}, // txtPickArrows  39
	{ 235, 45, 40, 30, 0, 12}, // bx_suffix  40
	{ 280, 45, 40, 30, 0, 12}, // bx_prefix  41
	{ 320, 45, 40, 30, 0, 12}, // bx_area  42
	{ 5, 301, 379, 150, 5, 14}, // txtInfo  43
};

UI_SIZES UI_big[] = {
	{ 0, 0, 475, 635, 0, 16}, // window
	{ 0, 0, 475, 25, 0, 12}, // menu_ bar
	{ 0, 25, 475, 414, 0, 16}, // myUI
	{ 140, 30, 115, 20, 1, 16}, // txtNcallins
	{ 0, 49, 275, 396, 1, 16}, // net_grp1
	{ 2, 180, 15, 24, 0, 16}, // ptr_left
	{ 258, 180, 15, 24, 0, 16}, // ptr_right
	{ 17, 60, 240, 384, 0, 16}, // net_grp2
	{ 17, 60, 240, 24, 4, 16}, // txtTitles
	{ 17, 84, 240, 24, 4, 16}, // txtLine[0]
	{ 17, 108, 240, 24, 4, 16}, // txtLine[1]
	{ 17, 132, 240, 24, 4, 16}, // txtLine[2]
	{ 17, 156, 240, 24, 4, 16}, // txtLine[3]
	{ 17, 180, 240, 24, 4, 16}, // txtLine[4]
	{ 17, 204, 240, 24, 4, 16}, // txtLine[5]
	{ 17, 228, 240, 24, 4, 16}, // txtLine[6]
	{ 17, 252, 240, 24, 4, 16}, // txtLine[7]
	{ 17, 276, 240, 24, 4, 16}, // txtLine[8]
	{ 17, 300, 240, 24, 4, 16}, // txtLine[9]
	{ 17, 324, 240, 24, 4, 16}, // txtLine[10]
	{ 17, 348, 240, 24, 4, 16}, // txtLine[11]
	{ 17, 372, 240, 24, 4, 16}, // txtLine[12]
	{ 17, 396, 240, 24, 4, 16}, // txtLine[13]
	{ 17, 420, 240, 24, 4, 16}, // txtLine[14]
	{ 284, 60, 185, 380, 13, 18}, // dbSelectGroup
	{ 291, 87, 60, 30, 13, 18}, // inpLoginSuffix
	{ 355, 87, 60, 30, 13, 18}, // inpLoginPrefix
	{ 419, 87, 40, 30, 13, 18}, // inpLoginArea
	{ 291, 125, 170, 300, 0, 18}, // net_grp3
	{ 291, 125, 170, 30, 4, 18}, //  txtPick[0]
	{ 291, 155, 170, 30, 4, 18}, //  txtPick[1]
	{ 291, 185, 170, 30, 4, 18}, //  txtPick[2]
	{ 291, 215, 170, 30, 4, 18}, //  txtPick[3]
	{ 291, 245, 170, 30, 4, 18}, //  txtPick[4]
	{ 291, 275, 170, 30, 4, 18}, //  txtPick[5]
	{ 291, 305, 170, 30, 4, 18}, //  txtPick[6]
	{ 291, 335, 170, 30, 4, 18}, //  txtPick[7]
	{ 291, 365, 170, 30, 4, 18}, //  txtPick[8]
	{ 291, 395, 170, 30, 4, 18}, //  txtPick[9]
	{ 374, 195, 25, 180, 0, 18}, // txtPickArrows
	{ 291, 60, 60, 25, 0, 16}, // bx_suffix
	{ 355, 60, 61, 25, 0, 16}, // bx_prefix
	{ 419, 60, 40, 25, 0, 16}, // bx_area
	{ 5, 449, 465, 180, 5, 16}, // txtInfo
};

UI_SIZES *ui_last = UI_small;

void change_size()
{
	UI_SIZES *ui = ui_last;

	if (ui == UI_small)
		ui = ui_last = UI_big;
	else
		ui = ui_last = UI_small;

	int X = main_window->x();
	int Y = main_window->y();

	main_window->resize(X, Y, ui->w, ui->h);
	ui++;
	mnu_bar->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	myUI->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	txtNcallins->resize(ui->x, ui->y, ui->w, ui->h);
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
	dbSelectGroup->resize(ui->x, ui->y, ui->w, ui->h);
	ui++;
	inpLoginSuffix->resize(ui->x, ui->y, ui->w, ui->h);
	inpLoginSuffix->labelfont(ui->fnt);
	inpLoginSuffix->labelsize(ui->size);
	ui++;
	inpLoginPrefix->resize(ui->x, ui->y, ui->w, ui->h);
	inpLoginPrefix->labelfont(ui->fnt);
	inpLoginPrefix->labelsize(ui->size);
	ui++;
	inpLoginArea->resize(ui->x, ui->y, ui->w, ui->h);
	inpLoginArea->labelfont(ui->fnt);
	inpLoginArea->labelsize(ui->size);
	ui++;
	net_grp3->resize(ui->x, ui->y, ui->w, ui->h);
	net_grp3->labelsize(ui->size);
	ui++;
	txtPick[0]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[0]->labelfont(ui->fnt);
	txtPick[0]->labelsize(ui->size);
	ui++;
	txtPick[1]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[1]->labelfont(ui->fnt);
	txtPick[1]->labelsize(ui->size);
	ui++;
	txtPick[2]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[2]->labelfont(ui->fnt);
	txtPick[2]->labelsize(ui->size);
	ui++;
	txtPick[3]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[3]->labelfont(ui->fnt);
	txtPick[3]->labelsize(ui->size);
	ui++;
	txtPick[4]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[4]->labelfont(ui->fnt);
	txtPick[4]->labelsize(ui->size);
	ui++;
	txtPick[5]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[5]->labelfont(ui->fnt);
	txtPick[5]->labelsize(ui->size);
	ui++;
	txtPick[6]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[6]->labelfont(ui->fnt);
	txtPick[6]->labelsize(ui->size);
	ui++;
	txtPick[7]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[7]->labelfont(ui->fnt);
	txtPick[7]->labelsize(ui->size);
	ui++;
	txtPick[8]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[8]->labelfont(ui->fnt);
	txtPick[8]->labelsize(ui->size);
	ui++;
	txtPick[9]->resize(ui->x, ui->y, ui->w, ui->h);
	txtPick[9]->labelfont(ui->fnt);
	txtPick[9]->labelsize(ui->size);
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

