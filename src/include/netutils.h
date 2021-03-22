#ifndef netutils_h
#define netutils_h

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>

#include "flinput2.h"

class mybrowser : public Fl_Select_Browser {
public:
	mybrowser(int x, int y, int w, int h, const char* l = 0)
		: Fl_Select_Browser(x, y , w, h, l) {}
	int handle(int event) {
		if (event == FL_KEYBOARD) {
			int b = Fl::event_key();
			if (b == FL_Enter || b == FL_KP_Enter) {
				do_callback();
				return 1;
			}
		}
		return Fl_Select_Browser::handle(event);
	}
	~mybrowser() {}
};

extern mybrowser *brwsCallsign;
extern void cb_CallsignSearch(Fl_Select_Browser *, void *);

extern void cb_btnCancelCallsignSearch(Fl_Button*, void*);
extern Fl_Button *btnCancelCallsignSearch;

extern void cb_OkCallsignSearch(Fl_Button*, void*);
extern Fl_Button *btn_OkCallsignSearch;

extern mybrowser *brwsNetNbr;
extern void cb_NetNbrSearch(Fl_Select_Browser *, void *);

extern Fl_Button *btnCancelNetNbrSearch;
extern void cb_btnCancelNetNbrSearch(Fl_Button*, void*);

extern Fl_Button *btn_OkNetNbrSearch;
extern void cb_OkNetNbrSearch(Fl_Button*, void*);

extern Fl_Double_Window* winNetNbrBrowse();
extern Fl_Double_Window *winSearchCallsignDialog;

extern Fl_Double_Window* newSearchCallsignDialog();

extern Fl_Input2 *sSrchCall;
extern void cb_SrchCall(Fl_Input2 *c, void *d);

extern Fl_Button *btnSearchCancel;
extern void cb_btnSearchCancel(Fl_Button*, void*);

extern Fl_Return_Button *btnSearchOK;
extern void cb_btnSearchOK(Fl_Return_Button*, void*);

extern Fl_Double_Window* winCallsignBrowse();

extern Fl_Double_Window* newSearchNetNbrDialog();

extern Fl_Double_Window *winSearchNetNbrDialog;

extern Fl_Input2 *sSrchNetNbr;
extern void cb_SrchNetNbr(Fl_Input2 *c, void *d);

extern Fl_Button *btnSearchNetNbrCancel;
extern void cb_btnSearchNetNbrCancel(Fl_Button*, void*);

extern Fl_Return_Button *btnSearchNetNbrOK;
extern void cb_btnSearchNetNbrOK(Fl_Return_Button*, void*);

extern Fl_Double_Window* newAboutDialog();
extern Fl_Double_Window *dlgAbout;
extern Fl_Group *lblVersion;

#endif


