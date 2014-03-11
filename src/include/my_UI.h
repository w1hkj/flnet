#ifndef _MY_UI
#define _MY_UI

#include <FL/Fl_Group.H>
#include "loglist.h"

extern int WhoIsUp;
extern void updateCallins ();
extern loglist callinlist;

enum MYGUI_STATUS {LOGLIST, SUFFIX, PREFIX, AREA, PICKLIST};

class my_UI : public Fl_Group 
{  
  private:
    MYGUI_STATUS my_status;
//    int WhoIsUP;
    void clearSAP ();
    void fillPickList ();
    void PickedToCallins (int);
    void PickedColors ();
    void clearPickList ();
  public:
    my_UI (int x, int y, int w, int h, const char *l = 0);
    int handle (int);
    void dispCallIns ();
    void UpdateWhoIsUp (long L);
};

#endif

