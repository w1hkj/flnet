#ifndef _NETSUPPORT
#define _NETSUPPORT

extern void openDB(char *);
extern void dispRec();
extern void closeDB();

extern void cleanExit();
extern void cb_btnNewSave (Fl_Button *, void *);
extern void cb_btnDelete (Fl_Button *, void *);
extern void cb_btnUpdateCancel (Fl_Button *, void *);
extern void cb_mnuConfig (Fl_Menu_ *, void *);
extern void cb_btnCloseConfig (Fl_Return_Button *, void *);

extern void cb_mnuMigrate (Fl_Menu_ *, void*);

extern void visit_URL(void *);

#endif

