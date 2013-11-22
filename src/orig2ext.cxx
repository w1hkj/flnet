#include <stdlib.h>
#include <stdio.h>

#include "orig2ext.h"

short rc;
long  fc;
xbXBase x1;
xbDbf db1( &x1 );
xbXBase x2;
xbDbf db2( &x2 );
long     numrecs;

char sDbFileName[120];

xbSchema dbNetSchema[] = {
  {"PREFIX",    XB_CHAR_FLD,  2, 0}, // 0
  {"AREA",      XB_CHAR_FLD,  1, 0}, // 1
  {"SUFFIX",    XB_CHAR_FLD,  3, 0}, // 2
  {"NAME",      XB_CHAR_FLD, 10, 0}, // 3
  {"NETNBR",    XB_CHAR_FLD,  4, 0}, // 4
  {"LOGDATE",   XB_DATE_FLD, 10, 0}, // 5
  {"FNAME",     XB_CHAR_FLD, 10, 0}, // 6
  {"LNAME",     XB_CHAR_FLD, 20, 0}, // 7
  {"ADDRESS",   XB_CHAR_FLD, 30, 0}, // 8
  {"CITY",      XB_CHAR_FLD, 25, 0}, // 9
  {"STATE",     XB_CHAR_FLD,  2, 0}, // 10
  {"ZIP",       XB_CHAR_FLD,  5, 0}, // 11
  {"PHONE",     XB_CHAR_FLD, 12, 0}, // 12
  {"BIRTHDATE", XB_DATE_FLD, 10, 0}, // 13
  {"SPOUSE",    XB_CHAR_FLD, 10, 0}, // 14
  {"SP_BIRTH",  XB_CHAR_FLD,  5, 0}, // 15
  {"",          0,            0, 0}
};

xbSchema dbNetExtendedSchema[] = {
  {"PREFIX",    XB_CHAR_FLD,  2, 0}, // 0
  {"AREA",      XB_CHAR_FLD,  1, 0}, // 1
  {"SUFFIX",    XB_CHAR_FLD,  3, 0}, // 2
  {"CALLSIGN",  XB_CHAR_FLD,  6, 0}, // 3
  {"NAME",      XB_CHAR_FLD, 10, 0}, // 4
  {"NETNBR",    XB_CHAR_FLD,  4, 0}, // 5
  {"LOGDATE",   XB_DATE_FLD,  8, 0}, // 6
  {"NBRLOGINS", XB_CHAR_FLD,  4, 0}, // 7
  {"STATUS",    XB_CHAR_FLD,  1, 0}, // 8
  {"JOINED",    XB_DATE_FLD,  8, 0}, // 9
  {"FNAME",     XB_CHAR_FLD, 10, 0}, // 10
  {"LNAME",     XB_CHAR_FLD, 20, 0}, // 11
  {"ADDR",      XB_CHAR_FLD, 30, 0}, // 12
  {"CITY",      XB_CHAR_FLD, 25, 0}, // 13
  {"STATE",     XB_CHAR_FLD,  2, 0}, // 14
  {"ZIP",       XB_CHAR_FLD,  5, 0}, // 15
  {"PHONE",     XB_CHAR_FLD, 12, 0}, // 16
  {"BIRTHDATE", XB_CHAR_FLD, 10, 0}, // 17
  {"SPOUSE",    XB_CHAR_FLD, 10, 0}, // 18
  {"SP_BIRTH",  XB_CHAR_FLD,  5, 0}, // 19
  {"COMMENT1",  XB_CHAR_FLD, 50, 0}, // 20
  {"COMMENT2",  XB_CHAR_FLD, 50, 0}, // 21
  {"EMAIL",     XB_CHAR_FLD, 30, 0}, // 22
  {"PREVDATE",  XB_DATE_FLD,  8, 0}, // 23
  {"",          0,            0, 0}
};


void openOrig (char *fname)
{
  if ((rc = db1.OpenDatabase( fname )) != XB_NO_ERROR ) {
    x1.DisplayError(rc);
    exit(1);
  }
}

void openExt (char *fname)
{
  if ((rc = db2.CreateDatabase (fname, dbNetExtendedSchema, XB_DONTOVERLAY)) 
       != XB_NO_ERROR ) {
    x2.DisplayError(rc);
    exit(2);
  }
}

void closeDbs ()
{
  db1.CloseDatabase();
  db2.CloseDatabase();
}

void copyDb ()
{
  char bdate[12], buf[12];
  numrecs = db1.NoOfRecords();

  for (long L = 1; L <= numrecs; L++) {
    db1.GetRecord (L);
    db2.BlankRecord ();

    db2.PutField ( (short) 0, 
                   db1.GetField ( (short) 0) );
    db2.PutField ( (short) 1, 
                   db1.GetField ( (short) 1) );
    db2.PutField ( (short) 2, 
                   db1.GetField ( (short) 2) );
    db2.PutField ( (short) 4, 
                   db1.GetField ( (short) 3) );
    db2.PutField ( (short) 5, 
                   db1.GetField ( (short) 4) );
    db2.PutField ( (short) 6, 
                   db1.GetField ( (short) 5) );
    db2.PutField ( (short) 10, 
                   db1.GetField ( (short) 6) );
    db2.PutField ( (short) 11, 
                   db1.GetField ( (short) 7) );
    db2.PutField ( (short) 12, 
                   db1.GetField ( (short) 8) );
    db2.PutField ( (short) 13, 
                   db1.GetField ( (short) 9) );
    db2.PutField ( (short) 14, 
                   db1.GetField ( (short) 10) );
    db2.PutField ( (short) 15, 
                   db1.GetField ( (short) 11) );
    db2.PutField ( (short) 16, 
                   db1.GetField ( (short) 12) );
    strcpy (buf, db1.GetField ( (short) 13));
    if (buf[0] != ' ') {
      if (buf[0] == '2') { buf[0] = '1'; buf[1] = '9';}
      strcpy (bdate, "00/00/0000");
      bdate[0] = buf[4];
      bdate[1] = buf[5];
      bdate[3] = buf[6];
      bdate[4] = buf[7];
      bdate[6] = buf[0];
      bdate[7] = buf[1];
      bdate[8] = buf[2];
      bdate[9] = buf[3];
      db2.PutField ( (short) 17, bdate );
    }
    db2.PutField ( (short) 18, 
                   db1.GetField ( (short) 14) );
    db2.PutField ( (short) 19, 
                   db1.GetField ( (short) 15) );
    db2.AppendRecord ();
  }
}

int main (int argc, char *argv[])
{
  if (argc != 3) {
    printf("Usage: orig2ext oldfile.dbf newfile.dbf\n");
    exit(0);
  }
  openOrig (argv[1]);
  openExt  (argv[2]);
  copyDb ();
  printf ("Created new database with %ld records\n", numrecs);
  closeDbs ();
  return 0;
}
