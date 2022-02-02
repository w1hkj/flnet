// =====================================================================
//
// sorting.cxx
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

#include <iostream>

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Check_Button.H>

#include "netsupport.h"
#include "sorting.h"
#include "status.h"

Fl_Scroll *column_select = (Fl_Scroll *)0;
Fl_Button *btn_row_up = (Fl_Button *)0;
Fl_Button *btn_row_dn = (Fl_Button *)0;
Fl_Button *btn_colsel_done = (Fl_Button *)0;

int numfields = 0;

SORTED_BY columns[] = {
	{1, "CALLSIGN", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{1, "NAME", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{1, "NETNBR", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "LOGDATE", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{1, "LOGTIME", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "PREVDATE", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "NBRLOGINS", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{1, "STATUS", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "PREFIX", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "AREA", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "SUFFIX", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "FNAME", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "LNAME", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "ADDR", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "CITY", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "STATE", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "ZIP", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "PHONE", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "BIRTHDATE", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "JOINED", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "EMAIL", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "LOCATOR", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "COUNTRY", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "COUNTY", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "TRAFFIC", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "SPOUSE", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "SP_BIRTH", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "COMMENT1", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "COMMENT2", (Fl_Check_Button *)0, (Fl_Light_Button *)0},
	{0, "", (Fl_Check_Button *)0, (Fl_Light_Button *)0}
};

void load_column_select()
{
	columns[0].checked = progStatus.chk0; columns[0].header = progStatus.header0;
	columns[1].checked = progStatus.chk1; columns[1].header = progStatus.header1;
	columns[2].checked = progStatus.chk2; columns[2].header = progStatus.header2;
	columns[3].checked = progStatus.chk3; columns[3].header = progStatus.header3;
	columns[4].checked = progStatus.chk4; columns[4].header = progStatus.header4;
	columns[5].checked = progStatus.chk5; columns[5].header = progStatus.header5;
	columns[6].checked = progStatus.chk6; columns[6].header = progStatus.header6;
	columns[7].checked = progStatus.chk7; columns[7].header = progStatus.header7;
	columns[8].checked = progStatus.chk8; columns[8].header = progStatus.header8;
	columns[9].checked = progStatus.chk9; columns[9].header = progStatus.header9;

	columns[10].checked = progStatus.chk10; columns[10].header = progStatus.header10;
	columns[11].checked = progStatus.chk11; columns[11].header = progStatus.header11;
	columns[12].checked = progStatus.chk12; columns[12].header = progStatus.header12;
	columns[13].checked = progStatus.chk13; columns[13].header = progStatus.header13;
	columns[14].checked = progStatus.chk14; columns[14].header = progStatus.header14;
	columns[15].checked = progStatus.chk15; columns[15].header = progStatus.header15;
	columns[16].checked = progStatus.chk16; columns[16].header = progStatus.header16;
	columns[17].checked = progStatus.chk17; columns[17].header = progStatus.header17;
	columns[18].checked = progStatus.chk18; columns[18].header = progStatus.header18;
	columns[19].checked = progStatus.chk19; columns[19].header = progStatus.header19;

	columns[20].checked = progStatus.chk20; columns[20].header = progStatus.header20;
	columns[21].checked = progStatus.chk21; columns[21].header = progStatus.header21;
	columns[22].checked = progStatus.chk22; columns[22].header = progStatus.header22;
	columns[23].checked = progStatus.chk23; columns[23].header = progStatus.header23;
	columns[24].checked = progStatus.chk24; columns[24].header = progStatus.header24;
	columns[25].checked = progStatus.chk25; columns[25].header = progStatus.header25;
	columns[26].checked = progStatus.chk26; columns[26].header = progStatus.header26;
	columns[27].checked = progStatus.chk27; columns[27].header = progStatus.header27;
	columns[28].checked = progStatus.chk28; columns[28].header = progStatus.header28;
}

void save_column_select()
{
	progStatus.chk0 = columns[0].checked; progStatus.header0 = columns[0].header;
	progStatus.chk1 = columns[1].checked; progStatus.header1 = columns[1].header;
	progStatus.chk2 = columns[2].checked; progStatus.header2 = columns[2].header;
	progStatus.chk3 = columns[3].checked; progStatus.header3 = columns[3].header;
	progStatus.chk4 = columns[4].checked; progStatus.header4 = columns[4].header;
	progStatus.chk5 = columns[5].checked; progStatus.header5 = columns[5].header;
	progStatus.chk6 = columns[6].checked; progStatus.header6 = columns[6].header;
	progStatus.chk7 = columns[7].checked; progStatus.header7 = columns[7].header;
	progStatus.chk8 = columns[8].checked; progStatus.header8 = columns[8].header;
	progStatus.chk9 = columns[9].checked; progStatus.header9 = columns[9].header;

	progStatus.chk10 = columns[10].checked; progStatus.header10 = columns[10].header;
	progStatus.chk11 = columns[11].checked; progStatus.header11 = columns[11].header;
	progStatus.chk12 = columns[12].checked; progStatus.header12 = columns[12].header;
	progStatus.chk13 = columns[13].checked; progStatus.header13 = columns[13].header;
	progStatus.chk14 = columns[14].checked; progStatus.header14 = columns[14].header;
	progStatus.chk15 = columns[15].checked; progStatus.header15 = columns[15].header;
	progStatus.chk16 = columns[16].checked; progStatus.header16 = columns[16].header;
	progStatus.chk17 = columns[17].checked; progStatus.header17 = columns[17].header;
	progStatus.chk18 = columns[18].checked; progStatus.header18 = columns[18].header;
	progStatus.chk19 = columns[19].checked; progStatus.header19 = columns[19].header;

	progStatus.chk20 = columns[20].checked; progStatus.header20 = columns[20].header;
	progStatus.chk21 = columns[21].checked; progStatus.header21 = columns[21].header;
	progStatus.chk22 = columns[22].checked; progStatus.header22 = columns[22].header;
	progStatus.chk23 = columns[23].checked; progStatus.header23 = columns[23].header;
	progStatus.chk24 = columns[24].checked; progStatus.header24 = columns[24].header;
	progStatus.chk25 = columns[25].checked; progStatus.header25 = columns[25].header;
	progStatus.chk26 = columns[26].checked; progStatus.header26 = columns[26].header;
	progStatus.chk27 = columns[27].checked; progStatus.header27 = columns[27].header;
	progStatus.chk28 = columns[28].checked; progStatus.header28 = columns[28].header;

}

void cb_chk (Fl_Check_Button *w, void *val)
{
	size_t n = 0;
	while (!columns[n].header.empty()) {
		if (w == columns[n].chk) {
			columns[n].checked = w->value();
			break;
		}
		n++;
	}
	update_log_ins();
}

int last_selected = -1;

void cb_btn (Fl_Light_Button *w, void *val)
{
	size_t n = 0;
	while (!columns[n].header.empty()) {
		if (w == columns[n].btn) {
			if (columns[n].btn->value() == 1) last_selected = n;
			else last_selected = -1;
		} else
			columns[n].btn->value(0);
		n++;
	}
}

void swap_rows (int a, int b)
{
	std::string s1 = columns[a].header;
	int  checked1  = columns[a].checked;
	int  ona       = columns[a].btn->value();
	int  onb       = columns[b].btn->value();

	columns[a].header = columns[b].header;
	columns[a].checked = columns[b].checked;

	columns[b].header = s1;
	columns[b].checked = checked1;

	columns[a].chk->value(columns[a].checked);
	columns[a].btn->label(columns[a].header.c_str());
	columns[a].btn->value(onb);
	columns[a].chk->redraw();
	columns[a].btn->redraw();
	columns[a].btn->redraw_label();

	columns[b].chk->value(columns[b].checked);
	columns[b].btn->label(columns[b].header.c_str());
	columns[b].btn->value(ona);
	columns[b].chk->redraw();
	columns[b].btn->redraw();
	columns[b].btn->redraw_label();
}

//void print_row(int a)
//{
//	std::cout << columns[a].checked << ", " << columns[a].header << ", " << columns[a].chk << ", " << columns[a].btn << std::endl;
//}

void cb_btn_row_up (Fl_Widget *, void *)
{
	if (last_selected < 1) return;

	swap_rows(last_selected, last_selected - 1);
	last_selected--;
	update_log_ins();
}

void cb_btn_row_dn (Fl_Widget *, void *)
{
	if (last_selected >= numfields) return;

	swap_rows( last_selected, last_selected + 1);
	last_selected++;
	update_log_ins();
}

void cb_btn_colsel_done (Fl_Widget *w, void *)
{
	int n = 0;
	while ( !columns[n].header.empty() ) {
		n++;
	}
	w->parent()->hide();
}

Fl_Double_Window* create_sorting_dialog() {
	Fl_Double_Window* w = new Fl_Double_Window(230, 8 + 20 * 15, "Report Sort");

		column_select = new Fl_Scroll (4, 4, 150, w->h() - 8, "");
		column_select->type(Fl_Scroll::VERTICAL);
		column_select->box(FL_DOWN_BOX);

		int n = 0;
		while ( !columns[n].header.empty() ) {

			columns[n].chk = new Fl_Check_Button  (
				column_select->x() + 2, column_select->y() + 2 + 20 * n,
				16, 16, "");
			columns[n].chk->value(columns[n].checked);
			columns[n].chk->callback( (Fl_Callback *)cb_chk, (void *) 0);

			columns[n].btn = new Fl_Light_Button (
				columns[n].chk->x() + columns[n].chk->w()  + 2, column_select->y() + 20 * n,
				column_select->w() - columns[n].chk->w(), 20, columns[n].header.c_str());
			columns[n].btn->box(FL_FLAT_BOX);
			columns[n].btn->callback( (Fl_Callback *)cb_btn, (void *)0);

			n++;

		}
		numfields = n;

		column_select->end();

		btn_row_up = new Fl_Button(180, w->h() / 2 - 35, 30, 30, "@8->");
		btn_row_up->callback( (Fl_Callback *)cb_btn_row_up, NULL );

		btn_row_dn = new Fl_Button(180, w->h() / 2 + 5, 30, 30, "@2->");
		btn_row_dn->callback( (Fl_Callback *)cb_btn_row_dn, NULL );

		btn_colsel_done = new Fl_Button(w->w() - 65, w->h() - 30, 60, 20, "Done");
		btn_colsel_done->callback( (Fl_Callback *)cb_btn_colsel_done, NULL );

	w->end();
	return w;
}

/*
int main (int argc, char *argv[]) {
	Fl_Double_Window *main_w = create_sorting_dialog();
	main_w->show();
	Fl::run();
}
*/
