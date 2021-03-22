#include "netutils.h"
#include "icons.h"
#include "flinput2.h"

mybrowser *brwsCallsign = (mybrowser *)0;
Fl_Button *btnCancelCallsignSearch = (Fl_Button *)0;
Fl_Button *btn_OkCallsignSearch = (Fl_Button *)0;

Fl_Double_Window* winCallsignBrowse() {
	Fl_Double_Window* w = new Fl_Double_Window(304, 231, "Browse by Callsign");

		w->color(FL_LIGHT2);

		brwsCallsign = new mybrowser(0, 0, 215, 230);
		brwsCallsign->type(2);
		brwsCallsign->textfont(13);
		brwsCallsign->callback((Fl_Callback*)cb_CallsignSearch);
		brwsCallsign->when(FL_WHEN_ENTER_KEY_ALWAYS);

		btnCancelCallsignSearch = new Fl_Button(230, 35, 60, 40, "Cancel");
		btnCancelCallsignSearch->callback((Fl_Callback*)cb_btnCancelCallsignSearch);

		btn_OkCallsignSearch = new Fl_Button(230, 135, 60, 40, "OK");
		btn_OkCallsignSearch->callback((Fl_Callback*)cb_OkCallsignSearch);

		w->set_modal();
		w->end();

	return w;
}

mybrowser *brwsNetNbr = (mybrowser *)0;
Fl_Button *btnCancelNetNbrSearch = (Fl_Button *)0;
Fl_Button *btn_OkNetNbrSearch = (Fl_Button *)0;

Fl_Double_Window* winNetNbrBrowse() {
	Fl_Double_Window* w = new Fl_Double_Window(300, 230, "Browse by Net Nbr");
		w->color(FL_LIGHT2);

		brwsNetNbr = new mybrowser(0, 0, 215, 230);
		brwsNetNbr->type(2);
		brwsNetNbr->textfont(4);
		brwsNetNbr->callback((Fl_Callback*)cb_NetNbrSearch);
		brwsNetNbr->when(FL_WHEN_ENTER_KEY_ALWAYS);

		btnCancelNetNbrSearch = new Fl_Button(225, 45, 60, 40, "Cancel");
		btnCancelNetNbrSearch->callback((Fl_Callback*)cb_btnCancelNetNbrSearch);

		btn_OkNetNbrSearch = new Fl_Button(225, 120, 60, 40, "OK");
		btn_OkNetNbrSearch->callback((Fl_Callback*)cb_OkNetNbrSearch);

		w->set_modal();
		w->end();

	return w;
}

Fl_Double_Window *winSearchCallsignDialog = (Fl_Double_Window *)0;
Fl_Input2 *sSrchCall = (Fl_Input2 *)0;
Fl_Button *btnSearchCancel = (Fl_Button *)0;
Fl_Return_Button *btnSearchOK = (Fl_Return_Button *)0;

Fl_Double_Window* newSearchCallsignDialog() {
	winSearchCallsignDialog = new Fl_Double_Window(207, 107);
 
		sSrchCall = new Fl_Input2 (50, 35, 120, 25, "Call");
		sSrchCall->textfont(13);
		sSrchCall->align(Fl_Align(FL_ALIGN_LEFT));
		sSrchCall->callback((Fl_Callback *)cb_SrchCall);
		sSrchCall->when(FL_WHEN_ENTER_KEY_ALWAYS);

		btnSearchCancel = new Fl_Button(20, 70, 80, 25, "Cancel");
		btnSearchCancel->callback((Fl_Callback*)cb_btnSearchCancel);

		btnSearchOK = new Fl_Return_Button(105, 70, 80, 25, "Search");
		btnSearchOK->callback((Fl_Callback*)cb_btnSearchOK);

		new Fl_Box(0, 0, 210, 20, "Enter Full Call");

		winSearchCallsignDialog->set_modal();
		winSearchCallsignDialog->end();

	return winSearchCallsignDialog;
}

Fl_Double_Window *winSearchNetNbrDialog = (Fl_Double_Window *)0;
Fl_Input2 *sSrchNetNbr = (Fl_Input2 *)0;
Fl_Button *btnSearchNetNbrCancel = (Fl_Button *)0;
Fl_Return_Button *btnSearchNetNbrOK = (Fl_Return_Button *)0;

Fl_Double_Window* newSearchNetNbrDialog() {

	winSearchNetNbrDialog = new Fl_Double_Window(207, 107);

		sSrchNetNbr = new Fl_Input2(80, 30, 100, 25, "Enter Net Number");
		sSrchNetNbr->textfont(13);
		sSrchNetNbr->align(Fl_Align(FL_ALIGN_TOP));
		sSrchNetNbr->callback((Fl_Callback*)cb_SrchNetNbr);
		sSrchNetNbr->when(FL_WHEN_ENTER_KEY_ALWAYS);

		btnSearchNetNbrCancel = new Fl_Button(20, 70, 80, 25, "Cancel");
		btnSearchNetNbrCancel->callback((Fl_Callback*)cb_btnSearchNetNbrCancel);

		btnSearchNetNbrOK = new Fl_Return_Button(105, 70, 80, 25, "Search");
		btnSearchNetNbrOK->callback((Fl_Callback*)cb_btnSearchNetNbrOK);

		winSearchNetNbrDialog->set_modal();
		winSearchNetNbrDialog->end();

	return winSearchNetNbrDialog;
}

Fl_Double_Window *dlgAbout = (Fl_Double_Window *)0;
Fl_Group *lblVersion = (Fl_Group *)0;

static void cb_Thanks(Fl_Return_Button*, void*) {
	dlgAbout->hide ();
}

Fl_Double_Window* newAboutDialog() {
	dlgAbout = new Fl_Double_Window(320, 184);

		dlgAbout->color((Fl_Color)53);

		lblVersion = new Fl_Group(5, 5, 310, 105, "xxx:");
		lblVersion->box(FL_ENGRAVED_FRAME);
		lblVersion->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
		lblVersion->end();

		Fl_Return_Button* retbtn = new Fl_Return_Button(100, 150, 100, 25, "Thanks");
		retbtn->callback((Fl_Callback*)cb_Thanks);

		Fl_Group* info = new Fl_Group(5, 115, 310, 30, "w1hkj@@bellsouth.net");
		info->box(FL_ENGRAVED_FRAME);
		info->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
		info->end();

		dlgAbout->set_modal();
		dlgAbout->end();

	return dlgAbout;
}
