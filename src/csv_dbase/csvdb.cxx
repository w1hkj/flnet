//======================================================================
// csvdb.cxx
//
// Authors:
//
// Copyright (C) 2013, Dave Freese, W1HKJ
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
//======================================================================

#include "csvdb.h"
#include "util.h"
#include "threads.h"

std::string csvRecord::print()
{
	static std::string retstr;
	retstr.assign("call ......... ").append(callsign);
	retstr.append("\nname ......... ").append(name);
	retstr.append("\nnet nbr ...... ").append(netnbr);
	retstr.append("\nlog date ..... ").append(logdate);
	retstr.append("\nprev data .... ").append(prevdate);
	retstr.append("\nnbr logins ... ").append(nbrlogins);
	retstr.append("\nstatus ....... ").append(status);
	retstr.append("\njoined ....... ").append(joined);
	retstr.append("\nfirst name ... ").append(fname);
	retstr.append("\nlast name .... ").append(lname);
	retstr.append("\naddress ...... ").append(addr);
	retstr.append("\ncity ......... ").append(city);
	retstr.append("\nstate ........ ").append(state);
	retstr.append("\nzip code ..... ").append(zip);
	retstr.append("\nlocator ...... ").append(locator);
	retstr.append("\ncountry ...... ").append(country);
	retstr.append("\ncounty ....... ").append(county);
	retstr.append("\ntraffic ...... ").append(traffic);
	retstr.append("\nphone ........ ").append(phone);
	retstr.append("\nemail ........ ").append(email);
	retstr.append("\nbirth date ... ").append(birthdate);
	retstr.append("\nspouse name .. ").append(spouse);
	retstr.append("\nsp birthdate . ").append(sp_birth);
	retstr.append("\ncomments (1) . ").append(comment1);
	retstr.append("\ncomments (2) . ").append(comment2);
	return retstr;
}

//szFields on 10 char spacing
static string szFields = "\
PREFIX    AREA      SUFFIX    CALLSIGN  \
NAME      NETNBR    LOGDATE   NBRLOGINS \
STATUS    JOINED    FNAME     LNAME     \
ADDR      CITY      STATE     ZIP       \
PHONE     BIRTHDATE SPOUSE    SP_BIRTH  \
COMMENT1  COMMENT2  EMAIL     PREVDATE  \
LOCATOR   COUNTRY   COUNTY    TRAFFIC";

const char *csvdb::csvFields = "\
PREFIX,AREA,SUFFIX,CALLSIGN,NAME,NETNBR,LOGDATE,NBRLOGINS,STATUS,JOINED,\
FNAME,LNAME,ADDR,CITY,STATE,ZIP,PHONE,BIRTHDATE,SPOUSE,SP_BIRTH,\
COMMENT1,COMMENT2,EMAIL,PREVDATE,LOCATOR,COUNTRY,COUNTY,TRAFFIC";

bool csvdb::mapheader(string header)
{
	bool ok = false;
	size_t p;
	string hfield;
	for (int i = PREFIX; i < LAST_FIELD; fpos[i++] = -1);
	for (int i = PREFIX; i < LAST_FIELD; i++) {
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

static int callsign_comp(const void *r1, const void *r2) {
	callsigns *p1 = (callsigns *)r1;
	callsigns *p2 = (callsigns *)r2;
	return strcmp(p1->callsign, p2->callsign);
}

static int netnbr_comp(const void *r1, const void *r2) {
	callsigns *p1 = (callsigns *)r1;
	callsigns *p2 = (callsigns *)r2;
	return strcmp(p1->netnbr, p2->netnbr);
}

int  csvdb::binary_search_netnbr(callsigns *list, int l, int r, std::string &nbr)
{
	if (r >= l) {
		int mid = l + (r - l) / 2;
		long nl = atol(list[mid].netnbr);
		long nr = atol(nbr.c_str());
		if (nl == nr) return mid;
		if (nl > nr) return binary_search_netnbr(list, l, mid - 1, nbr);
		return binary_search_netnbr(list, mid + 1, r, nbr);
	}
	return -1;
}

int csvdb::find_netnbr(std::string netnbr)
{
	qsort ( clist, dbrecs.size(), sizeof(callsigns), netnbr_comp);
	int index = binary_search_netnbr( clist, 0, dbrecs.size() - 1, netnbr);
	if (index == -1) return -1;
	return clist[index].nbr;
}

int  csvdb::binary_search_callsign(callsigns *list, int l, int r, std::string &call)
{
	int cmp = -1;
	if (r >= l) {
		int mid = l + (r - l) / 2;
		cmp = strcmp(list[mid].callsign, call.c_str());
		if (cmp == 0) return mid;
		if (cmp > 0) return binary_search_callsign(list, l, mid - 1, call);
		return binary_search_callsign(list, mid + 1, r, call);
	}
	return -1;
}

int csvdb::find_callsign(std::string callsign)
{
	qsort ( clist, dbrecs.size(), sizeof(callsigns), callsign_comp);
	int index = binary_search_callsign( clist, 0, dbrecs.size() - 1, callsign);
	if (index == -1) return -1;
	return clist[index].nbr;
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

	for (int i = PREFIX; i < LAST_FIELD; i++) {
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
			case PREVDATE :		field(s, rec.prevdate); break;
			case LOCATOR :		field(s, rec.locator); break;
			case COUNTRY :		field(s, rec.country); break;
			case COUNTY :		field(s, rec.county); break;
			case TRAFFIC :		field(s, rec.traffic); break;
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
#define LINESIZE 2048
	char buff[LINESIZE + 1];

	backup(dbfilename);
	fstream dbfile(dbfilename.c_str(), ios::in | ios::binary);
	if (!dbfile) {
		dbrecs.clear();
		return 0;
	}

	csvRecord rec;
	dbrecs.clear();

	// read & map header line
	memset(buff, 0, LINESIZE + 1);
	dbfile.getline(buff, LINESIZE);
	if (!mapheader(buff)) return -1;

	// header passes test, read rest of file
	string sbuff;
	while (!dbfile.eof()) {
		memset(buff, 0, LINESIZE + 1);
		dbfile.getline(buff, LINESIZE);
		if (strlen(buff)) {
			sbuff = buff;
			if (split(buff, rec)) dbrecs.push_back(rec);
		}
	}
	dbfile.close();

	update_clist();

	return 0;
}

string csvdb::trim(string s)
{
	static string trimmed;
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
	str.append(delimit(rec.prevdate)).append(",");
	str.append(delimit(rec.locator)).append(",");
	str.append(delimit(rec.country)).append(",");
	str.append(delimit(rec.county)).append(",");
	str.append(delimit(rec.traffic));
}

void csvdb::update_clist()
{
	if (clist) delete [] clist;

	clist = new callsigns[dbrecs.size()];

	for (size_t n = 0; n < dbrecs.size(); n++) {
		strncpy(clist[n].callsign, dbrecs[n].callsign.c_str(), 14);
		strncpy(clist[n].netnbr, dbrecs[n].netnbr.c_str(), 9);
		clist[n].nbr = n;
	}
}

int csvdb::save()
{
	fstream dbfile(dbfilename.c_str(), ios::out | ios::binary);
	if (!dbfile) return -1;

	// csv file header line
	dbfile << csvFields << "\n";

	if (dbrecs.size() > 0) {
		qsort ( clist, dbrecs.size(), sizeof(callsigns), callsign_comp);
		// records
		string line;
		size_t n;
		for (n = 0; n < dbrecs.size(); n++) {
			join(dbrecs[clist[n].nbr], line);
			dbfile << line << "\n";
		}
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
	rec.locator.clear();
	rec.country.clear();
	rec.county.clear();
	rec.traffic.clear();
}

int csvdb::get(size_t n, csvRecord &rec)
{
	rec.prefix = rec.area = rec.suffix = rec.callsign = rec.name =
	rec.netnbr = rec.logdate = rec.nbrlogins = rec.status = rec.joined =
	rec.fname = rec.lname = rec.addr = rec.city = rec.state =
	rec.zip = rec.phone = rec.birthdate = rec.spouse = rec.sp_birth =
	rec.comment1 = rec.comment2 = rec.email = rec.prevdate =
	rec.locator = rec.country = rec.county = rec.traffic = "";

	if (n < 0 || n >= dbrecs.size()) return -1;

	if (!dbrecs.size()) return -2;

	rec = dbrecs[n];

	cur_recnbr = n;

	return 0;
}


int csvdb::put(size_t recnbr, csvRecord &rec)
{
	dbrecs[recnbr] = rec;
	update_clist();
	return 0;
}


int csvdb::add(csvRecord &rec)
{
	dbrecs.push_back(rec);
	update_clist();
	return 0;
}

int csvdb::erase(size_t n)
{
	vector<csvRecord>::iterator p = dbrecs.begin();
	size_t i = 0;
	while (p <= dbrecs.end() && i != n) {p++; i++;}
	if (i != n) return 1;
	dbrecs.erase(p);
	update_clist();
	return 0;
}

std::string csvdb::print(csvRecord &rec)
{
	return rec.print();
}

std::string csvdb::print(int n)
{
	return dbrecs[n].print();
}
