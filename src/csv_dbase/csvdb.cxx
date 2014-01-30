// =====================================================================
//
// csvdb.cxx
//
// comma separated value database file support for flnet
//
// Copyright 2013, Dave Freese, W1HKJ
//
// =====================================================================

#include "csvdb.h"

enum FIELDS {
PREFIX,AREA,SUFFIX,CALLSIGN,NAME,NETNBR,LOGDATE,NBRLOGINS,STATUS,JOINED,
FNAME,LNAME,ADDR,CITY,STATE,ZIP,PHONE,BIRTHDATE,SPOUSE,SP_BIRTH,
COMMENT1,COMMENT2,EMAIL,PREVDATE };

//szFields on 10 char spacing
static string szFields = "\
PREFIX    AREA      SUFFIX    CALLSIGN  \
NAME      NETNBR    LOGDATE   NBRLOGINS \
STATUS    JOINED    FNAME     LNAME     \
ADDR      CITY      STATE     ZIP       \
PHONE     BIRTHDATE SPOUSE    SP_BIRTH  \
COMMENT1  COMMENT2  EMAIL     PREVDATE  ";

const char *csvdb::csvFields = "\
PREFIX,AREA,SUFFIX,CALLSIGN,NAME,NETNBR,LOGDATE,NBRLOGINS,STATUS,JOINED,\
FNAME,LNAME,ADDR,CITY,STATE,ZIP,PHONE,BIRTHDATE,SPOUSE,SP_BIRTH,\
COMMENT1,COMMENT2,EMAIL,PREVDATE";

bool csvdb::mapheader(string header)
{
	bool ok = false;
	size_t p;
	string hfield;
	for (int i = 0; i < 24; fpos[i++] = -1);
	for (int i = 0; i < 24; i++) {
		if (header.empty()) break;
		field( header, hfield);
		p = szFields.find(hfield);
		if (p != string::npos) {
			fpos[i] = p/10;
			ok = true;
		}
	}
	return ok;
}

static int compfunc(const void *r1, const void *r2) {
	callsigns *p1 = (callsigns *)r1;
	callsigns *p2 = (callsigns *)r2;
// sort by area / prefix / suffix
	int comp = p1->area.compare(p2->area);
	if (comp == 0) {
		comp = p1->prefix.compare(p2->prefix);
		if (comp == 0)
			comp = p1->suffix.compare(p2->suffix);
	}
	return comp;
}

//----------------------------------------------------------------------
// split separates a comma delimited string into the record components
// string may be simple comma separated, quoted-comma delimited or a
// mix of the two.  The csv file may be opened by a 3rd party program
// such as gnumeric or Excel and then resaved as a csv file.
// field may appear as:
// field,    first field unquoted
// ,field    last field unquoted
// ,"field a b c",
// ,"field ""a"" b c",
// ,"field a, b, c",
// "field",  first field quoted
// ,"field"   last field quoted
//
// field extracts the leading field in the referenced string
// the leading field is removed from the string
//----------------------------------------------------------------------
void csvdb::field(string &s, string &fld)
{
	// empty reference string ==> empty field
	if (s.empty()) {
		fld.clear();
		return;
	}
	size_t p;
	// remove leading comma
	if (s[0] == ',') {
		s.erase(0, 1);
	}
	if (s[0] == '"') {
	// field is quoted
		s.erase(0, 1);
		p = s.find("\","); // find end of field
		if (p == string::npos) {
			p = s.find("\""); // possibly last record
			if ( p != string::npos) {
				fld = s.substr(0, p);
				s.clear();
				return;
			} else {
				s.clear(); // error in reference line
				fld.clear();
				return;
			}
		}
		if (p == 0) {
			fld.clear();
			s.erase(0, 1);
		} else {
			fld = s.substr(0, p);
			s.erase(0, p+1);
		}
	} else {
	// field is only comma delimited
		if (s[0] == ',') {
			fld.clear();
		} else {
			p = s.find(",");
			if (p == string::npos) {
				fld = s;
				s.clear();
			} else {
				fld = s.substr(0, p);
				s.erase(0, p);
			}
		}
	}
	// change double quotes to single
	while ((p = fld.find("\"\"")) != string::npos)
		fld.erase(p, 1);
}

bool csvdb::split(string s, csvRecord &rec)
{
	clearrec(rec);

	if (s[0] == ',')
		s.insert(0,","); // first field is empty !

	for (int i = 0; i < 24; i++) {
		switch (fpos[i]) {
			case -1 : break;
			case PREFIX :		field(s, rec.prefix); break;
			case AREA :			field(s, rec.area); break;
			case SUFFIX :		field(s, rec.suffix); break;
			case CALLSIGN :		field(s, rec.callsign); break;
			case NAME :			field(s, rec.name); break;
			case NETNBR :		field(s, rec.netnbr); break;
			case LOGDATE :		field(s, rec.logdate); break;
			case NBRLOGINS :	field(s, rec.nbrlogins); break;
			case STATUS :		field(s, rec.status); break;
			case JOINED :		field(s, rec.joined); break;
			case FNAME :		field(s, rec.fname); break;
			case LNAME :		field(s, rec.lname); break;
			case ADDR :			field(s, rec.addr); break;
			case CITY :			field(s, rec.city); break;
			case STATE :		field(s, rec.state); break;
			case ZIP :			field(s, rec.zip); break;
			case PHONE :		field(s, rec.phone); break;
			case BIRTHDATE :	field(s, rec.birthdate); break;
			case SPOUSE :		field(s, rec.spouse); break;
			case SP_BIRTH :		field(s, rec.sp_birth); break;
			case COMMENT1 :		field(s, rec.comment1); break;
			case COMMENT2 :		field(s, rec.comment2); break;
			case EMAIL :		field(s, rec.email); break;
			case PREVDATE :		field(s, rec.prevdate);
		}
	}
	if (rec.prefix.empty() && rec.area.empty() && rec.suffix.empty())
		return false;
	if (rec.callsign.empty())
		rec.callsign.assign(trim(rec.prefix)).append(trim(rec.area)).append(trim(rec.suffix));
	return true;
}

int csvdb::load()
{
#define LINESIZE 1024
	char buff[LINESIZE + 1];
	fstream dbfile(dbfilename.c_str(), ios::in | ios::binary);
	if (!dbfile) return 0;

	csvRecord rec;
	dbrecs.clear();

// read & map header line
	memset(buff, 0, LINESIZE + 1);
	dbfile.getline(buff, LINESIZE);
	if (!mapheader(buff)) return -1;

// header passes test, read rest of file
	string sbuff;
	while (dbfile) {
		memset(buff, 0, LINESIZE + 1);
		dbfile.getline(buff, LINESIZE);
		if (dbfile && strlen(buff)) {
			sbuff = buff;
			if (split(buff, rec)) dbrecs.push_back(rec);
		}
	}
	dbfile.close();
	return 0;
}

string csvdb::trim(string s)
{
	string trimmed;
	trimmed.assign(s);
	while (trimmed.length() && trimmed[0] == ' ') trimmed.erase(0);
	while (trimmed.length() && trimmed[trimmed.length()-1] == ' ')
		trimmed.erase(trimmed.length() - 1);
	return trimmed;
}

string csvdb::delimit(string s)
{
	static string delimited;
	bool quoted = false;
	delimited.assign(trim(s));
	size_t p = delimited.find("\"");
	while (p != string::npos) {
		delimited.insert(p, "\"");
		quoted = true;
		p = delimited.find("\"", p+2);
	}
	if (delimited.find(" ") != string::npos ||
		delimited.find(",") != string::npos ||
		delimited.find(";") != string::npos ||
		delimited.find("'") != string::npos) quoted = true;
	if (quoted) {
		delimited.insert(0, "\"");
		delimited.append("\"");
	}
	return delimited;
}

//----------------------------------------------------------------------
// join combines the separate elements of a record into a comma
// separated string.  Quotes are used to delimit a field if that field
// contains an embedded comma or a ".  Quotes within a field are saved
// as a double quote.  All fields which contain white space including the
// quote, space, comma etc are quoted.  Trailing and leading spaces are
// removed to conserve file size
//----------------------------------------------------------------------
void csvdb::join(csvRecord &rec, string &str)
{
	str.assign(delimit(rec.prefix)).append(",");
	str.append(delimit(rec.area)).append(",");
	str.append(delimit(rec.suffix)).append(",");
	str.append(delimit(rec.callsign)).append(",");
	str.append(delimit(rec.name)).append(",");
	str.append(delimit(rec.netnbr)).append(",");
	str.append(delimit(rec.logdate)).append(",");
	str.append(delimit(rec.nbrlogins)).append(",");
	str.append(delimit(rec.status)).append(",");
	str.append(delimit(rec.joined)).append(",");
	str.append(delimit(rec.fname)).append(",");
	str.append(delimit(rec.lname)).append(",");
	str.append(delimit(rec.addr)).append(",");
	str.append(delimit(rec.city)).append(",");
	str.append(delimit(rec.state)).append(",");
	str.append(delimit(rec.zip)).append(",");
	str.append(delimit(rec.phone)).append(",");
	str.append(delimit(rec.birthdate)).append(",");
	str.append(delimit(rec.spouse)).append(",");
	str.append(delimit(rec.sp_birth)).append(",");
	str.append(delimit(rec.comment1)).append(",");
	str.append(delimit(rec.comment2)).append(",");
	str.append(delimit(rec.email)).append(",");
	str.append(delimit(rec.prevdate));
}

int csvdb::save()
{
	fstream dbfile(dbfilename.c_str(), ios::out | ios::binary);
	if (!dbfile) return -1;

	struct callsigns clist[dbrecs.size()];
	for (size_t n = 0; n < dbrecs.size(); n++) {
		clist[n].prefix = dbrecs[n].prefix;
		clist[n].area   = dbrecs[n].area;
		clist[n].suffix = dbrecs[n].suffix;
		clist[n].nbr = n;
	}
	qsort ( &(clist[0]), dbrecs.size(), sizeof(callsigns), compfunc);

// csv file header line
	dbfile << csvFields << "\n";
// records
	string line;
	for (size_t n = 0; n < dbrecs.size(); n++) {
		join(dbrecs[clist[n].nbr], line);
		dbfile << line << "\n";
	}

	dbfile.close();

	return 0;
}

void csvdb::clearrec(csvRecord &rec)
{
	rec.prefix.clear();
	rec.area.clear();
	rec.suffix.clear();
	rec.callsign.clear();
	rec.name.clear();
	rec.netnbr.clear();
	rec.logdate.clear();
	rec.nbrlogins.clear();
	rec.status.clear();
	rec.joined.clear();
	rec.fname.clear();
	rec.lname.clear();
	rec.addr.clear();
	rec.city.clear();
	rec.state.clear();
	rec.zip.clear();
	rec.phone.clear();
	rec.birthdate.clear();
	rec.spouse.clear();
	rec.sp_birth.clear();
	rec.comment1.clear();
	rec.comment2.clear();
	rec.email.clear();
	rec.prevdate.clear();
}

int csvdb::get(size_t n, csvRecord &rec)
{
	rec.prefix = rec.area = rec.suffix = rec.callsign = rec.name =
	rec.netnbr = rec.logdate = rec.nbrlogins = rec.status = rec.joined =
	rec.fname = rec.lname = rec.addr = rec.city = rec.state =
	rec.zip = rec.phone = rec.birthdate = rec.spouse = rec.sp_birth =
	rec.comment1 = rec.comment2 = rec.email = rec.prevdate = "";

	if (n < 0 || n >= dbrecs.size()) return -1;

	if (!dbrecs.size()) return -2;

	rec = dbrecs[n];

	cur_recnbr = n;

	return 0;
}


int csvdb::put(size_t recnbr, csvRecord &rec)
{
	dbrecs[recnbr] = rec;
	return 0;
}


int csvdb::add(csvRecord &rec)
{
	dbrecs.push_back(rec);
	return 0;
}


int csvdb::erase(size_t n)
{
	csvRecord rec;
	get(n, rec);
	vector<csvRecord>::iterator p = dbrecs.begin();
	while (p != dbrecs.end()) {
		if (p->callsign == rec.callsign) {
			dbrecs.erase(p);
			return 0;
		}
		p++;
	}
	return 1;
}
