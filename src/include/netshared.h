// netshared.h
#ifndef _NETSHARED
#define _NETSHARED

#include <string>

#include "net_ui.h"
#include "netutils.h"
#include "netedits.h"

#include "csvdb.h"

extern std::string home_dir;
extern std::string selected_file;

extern csvdb netdb;

struct brwsStruct {
  int  recN;
  char prefix[4];
  char area[2];
  char suffix[4];
  char netnbr[5];
};

enum State {NEW, UPDATE, ADD, MODIFY};
enum schema {ORIG, EXTENDED};

extern Fl_Window *main_window;
extern Fl_Window *editor;

extern brwsStruct *brwsData;
extern State editstate;

extern size_t currec;
extern size_t brwsnum;

extern char sSimpleName[];

extern Fl_Input *inpPrefix;

extern void saveCurRecord ();
extern void clearEditForm ();
extern void appendNewRecord ();

extern char *uppercase (const char *);
extern char *trim (const char *);

extern void toggleState ();

extern void getBrwsData ();
extern int NetNbrCompare (const void *, const void *);
extern int SAPCompare (const void *, const void *);
extern int APSCompare (const void *, const void *);
extern void AddNewRecord (char *prefix, char *area, char *suffix);
extern void setAddState ();

extern void SortBySAP ();
extern void SortByNetNbr ();
extern void SortByAPS ();

extern void closeDB ();
extern void openDB (char *);
extern void dispRec ();
extern void clearEditForm ();
extern void setFields ();
extern void saveCurRecord ();
extern void appendNewRecord ();

extern void cb_F12 (int);
extern void cbEditor ();
extern void cbCloseEditor ();
extern void cbConfig ();
extern void cbCloseConfig ();

extern void change_size();

#endif

