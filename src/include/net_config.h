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
