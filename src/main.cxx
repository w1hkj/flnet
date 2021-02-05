// =====================================================================
//
// main.cxx
//
// Authors:
//
// Copyright (C) 2012, Dave Freese, W1HKJ
// Copyright (C) 2014, Robert Stiles, KK5VD
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


#include <fstream>
#include <ostream>
#include <string>

#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/x.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Item.H>

#include "UI/net_icon.cxx"
#include "config.h"
#include "net_config.h"
#include "netshared.h"
#include "netversion.h"
#include "netsupport.h"
#include "xml_io.h"
#include "status.h"
#include "debug.h"
#include "masterdb.h"

#ifdef WIN32
#  include "flnetrc.h"
#  include "compat.h"
#  define dirent fl_dirent_no_thanks
#endif

#include <FL/filename.H>

#include "csvdb.h"

#ifdef __MINGW32__
#	if FLNET_FLTK_API_MAJOR == 1 && FLNET_FLTK_API_MINOR < 3
#		undef dirent
#		include <dirent.h>
#	endif
#	ifndef IDI_ICON
#		define IDI_ICON 101
#	endif
#else
#	include <dirent.h>
#endif

#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_File_Chooser.H>

Fl_Window *main_window = (Fl_Window *)0;

std::string home_dir = "";
std::string selected_file;

// Show an error dialog and print to cerr if available.
// On win32 Fl::fatal displays its own error window.
static void fatal_error(string sz_error)
{
	string s = "Fatal error!\n";
	s.append(sz_error).append("\n").append(strerror(errno));

	// Win32 will display a MessageBox error message
#if !defined(__WOE32__)
	fl_message_font(FL_HELVETICA, FL_NORMAL_SIZE);
	fl_alert("%s", s.c_str());
#endif
	Fl::fatal(s.c_str());
}

#define KNAME "net"
#if !defined(__WIN32__) && !defined(__APPLE__)
Pixmap	net_icon_pixmap;

void make_pixmap(Pixmap *xpm, const char **data)
{
	Fl_Window w(0,0, PACKAGE_NAME);
	w.xclass(PACKAGE_NAME);
	w.show();
	w.make_current();
	Fl_Pixmap icon(data);
	int maxd = (icon.w() > icon.h()) ? icon.w() : icon.h();
	*xpm = fl_create_offscreen(maxd, maxd);
	fl_begin_offscreen(*xpm);
	fl_color(FL_BACKGROUND_COLOR);
	fl_rectf(0, 0, maxd, maxd);
	icon.draw(maxd - icon.w(), maxd - icon.h());
	fl_end_offscreen();
}

#endif

int parse_args(int argc, char **argv, int& idx);

char title[50];

char progdir[80];

int handle (int e) {
	if (Fl::focus() == main_window)
		return (e == FL_SHORTCUT);
	return 0;
}

void visit_URL(void* arg)
{
	const char* url = reinterpret_cast<const char *>(arg);
#ifndef __WOE32__
	const char* browsers[] = {
#  ifdef __APPLE__
		getenv("FLDIGI_BROWSER"), // valid for any OS - set by user
		"open"                    // OS X
#  else
		"fl-xdg-open",            // Puppy Linux
		"xdg-open",               // other Unix-Linux distros
		getenv("FLDIGI_BROWSER"), // force use of spec'd browser
		getenv("BROWSER"),        // most Linux distributions
		"sensible-browser",
		"firefox",
		"mozilla"                 // must be something out there!
#  endif
	};
	switch (fork()) {
		case 0:
#  ifndef NDEBUG
			unsetenv("MALLOC_CHECK_");
			unsetenv("MALLOC_PERTURB_");
#  endif
			for (size_t i = 0; i < sizeof(browsers)/sizeof(browsers[0]); i++)
				if (browsers[i])
					execlp(browsers[i], browsers[i], url, (char*)0);
			exit(EXIT_FAILURE);
		case -1:
			fl_alert(
					 "Could not run a web browser:\n%s\n\n"
					 "Open this URL manually:\n%s",
					 strerror(errno), url);
	}
#else
	// gurgle... gurgle... HOWL
	// "The return value is cast as an HINSTANCE for backward
	// compatibility with 16-bit Windows applications. It is
	// not a true HINSTANCE, however. The only thing that can
	// be done with the returned HINSTANCE is to cast it to an
	// int and compare it with the value 32 or one of the error
	// codes below." (Error codes omitted to preserve sanity).
	if ((int)ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL) <= 32)
		fl_alert("Could not open url:\n%s\n", url);
#endif
}

void exit_main(Fl_Widget *w)
{
	if (Fl::event_key() == FL_Escape)
		return;
	cleanExit();
}


void close_main_window(void)
{
	progStatus.saveLastState();

	if(main_window)
		main_window->hide();
}

void check_home_directory(std::string home_dir)
{
	struct stat sb;

	memset(&sb, 0, sizeof(sb));
	int state = stat(home_dir.c_str(), &sb);

	if((state == 0) && S_ISDIR(sb.st_mode))
		return;

	mkdir(home_dir.c_str(), 0766);
}

int main(int argc, char **argv)
{
	Fl::lock();
	int arg_idx;

	{
		char dirbuf[FL_PATH_MAX + 1];
#ifdef __WOE32__
		fl_filename_expand(dirbuf, sizeof(dirbuf) -1, "$USERPROFILE/");
		home_dir = dirbuf;
#else
		fl_filename_expand(dirbuf, sizeof(dirbuf) -1, "$HOME/");
		home_dir = dirbuf;
#endif
	}

	home_dir.append("flnet.files/");

	check_home_directory(home_dir);

	Fl::args(argc, argv, arg_idx, parse_args);

	Fl::add_handler (handle);

	progStatus.loadLastState();

	std::string debug_file = home_dir;
	debug_file.append("flnet_debug_log.txt");
	debug::start(debug_file.c_str());

	main_window = newNetControl();

	Fl::visual (FL_DOUBLE|FL_INDEX);

	sprintf (title, "flnet %s", FLNET_VERSION);
	main_window->label(title);
	main_window->callback(exit_main);

	ui_size(progStatus.ui_size, progStatus.mainX, progStatus.mainY);

#ifdef WIN32
	main_window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON)));
	main_window->show(argc, argv);
#elif !defined(__APPLE__)
	make_pixmap(&net_icon_pixmap, net_icon);
	main_window->icon((char *)net_icon_pixmap);
	main_window->show(argc, argv);
#else
	main_window->show(argc, argv);
#endif

	LOG_INFO("flnet debug log");

	strcpy (progdir, argv[0]);
	char *p = strrchr(progdir,'/');
	if (!p) p= strrchr(progdir, '\\');
	if (p) *(p+1) = 0;

#if !defined(__APPLE__)
	if (argc == 2) {
		char absolute[500];
		fl_filename_absolute(absolute, sizeof(absolute) - 1, argv[1]);
		int len = strlen(absolute);
		if (absolute[len-1] == '\n') absolute[len-1] = 0;
		openDB (selected_file = absolute);
	} else
#endif
	{
		int r;
		if ((r = mkdir(home_dir.c_str(), 0777)) == -1 && errno != EEXIST) {
			string s = "Could not make directory ";
			fatal_error(s);
		}
		std::string last_filename = home_dir;
		last_filename.append("new_net.csv");

		std::string cfg_filename = home_dir;
		cfg_filename.append("flnet.cfg");

#define LINESIZE 1024
		char buff[LINESIZE + 1];
		fstream dbfile(cfg_filename.c_str(), ios::in | ios::binary);
		if (dbfile) {
			// read & map header line
			memset(buff, 0, LINESIZE + 1);
			dbfile.getline(buff, LINESIZE);
			selected_file = buff;
			size_t p = selected_file.find('\r');
			if (p != string::npos) selected_file.erase(p);
			p = selected_file.rfind('\n');
			if (p != string::npos) selected_file.erase(p);
			last_filename = selected_file;
			dbfile.close();
		} else {
			selected_file = last_filename;
			char *p = fl_file_chooser ("Select .csv file",
									   "*.csv",
									   selected_file.c_str(), 0);
			if (!p) exit(0);
			selected_file = p;
		}
		openDB (selected_file);
		LOG_INFO("Opened: %s", selected_file.c_str());
	}
	if (!progStatus.masterdb.empty())
		open_masterdb();

	open_xmlrpc();

	return Fl::run();
}

int parse_args(int argc, char **argv, int& idx)
{
	if (strcasecmp("--help", argv[idx]) == 0) {
		printf("\nUsage: \n" \
			   "\t--help this help text\n" \
			   "\t--version\n" \
			   "\tdatabase.csv\n" \
			   "\topen 'named' database file.\n"
			   "\t--xmlrpc-server-address <ip_addess> default: %s\n" \
			   "\t--xmlrpc-server-port <port> default: %s\n\n\n", \
			   DEFAULT_XMLRPC_IP_ADDRESS,
			   DEFAULT_XMLRPC_PORT_NO
			   );
		exit(0);
	}

	if (strcasecmp("--version", argv[idx]) == 0) {
		printf("Version: %s\n", VERSION);
		exit (0);
	}
	
	if (strcasecmp(argv[idx], "--xmlrpc-server-address") == 0) {
		idx++;
		if((idx < argc) && argv[idx]) {
			set_xmlrpc_ip_address(argv[idx]);
			idx++;
		}
		return 1;
	}
	
	if (strcasecmp(argv[idx], "--xmlrpc-server-port") == 0) {
		idx++;
		if((idx < argc) && argv[idx]) {
			set_xmlrpc_port_number(argv[idx]);
			idx++;
		}
		return 1;
	}
	
	return 0;
}
