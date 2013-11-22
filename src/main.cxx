#include <fstream>
#include <ostream>

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

#include "net_icon.cxx"
#include "config.h"
#include "net_config.h"
#include "netshared.h"
#include "netversion.h"
#include "netsupport.h"

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

/*
static void checkdirectories(void)
{
	struct {
		string& dir;
		const char* suffix;
		void (*new_dir_func)(void);
	} dirs[] = {
		{ homedir, 0, 0 }
	};

	int r;
	for (size_t i = 0; i < sizeof(dirs)/sizeof(*dirs); i++) {
		if (dirs[i].suffix)
			dirs[i].dir.assign(homedir).append(dirs[i].suffix).append("/");

		if ((r = mkdir(dirs[i].dir.c_str(), 0777)) == -1 && errno != EEXIST) {
			cerr << "Could not make directory" << ' ' << dirs[i].dir
				 << ": " << strerror(errno) << '\n';
			exit(EXIT_FAILURE);
		}
		else if (r == 0 && dirs[i].new_dir_func)
			dirs[i].new_dir_func();
	}
}
*/

void exit_main(Fl_Widget *w)
{
	if (Fl::event_key() == FL_Escape)
		return;
	cleanExit();
}

void net_terminate() {
	std::cerr << "terminating" << std::endl;
	fl_message("Unrecoverable error\nTerminating net");
	cleanExit();
}

int main(int argc, char **argv) 
{
	int arg_idx;

	Fl::args(argc, argv, arg_idx, parse_args);

	Fl::add_handler (handle);
	main_window = newNetControl();
	Fl::visual (FL_DOUBLE|FL_INDEX);

	sprintf (title, "Net Version %s", flnet_VERSION);
	main_window->label(PACKAGE_NAME);
	main_window->callback(exit_main);

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

	strcpy (progdir, argv[0]);
	char *p = strrchr(progdir,'/');
	if (!p) p= strrchr(progdir, '\\');
	if (p) *(p+1) = 0;

#if !defined(__APPLE__)
	if (argc == 2)
		openDB (argv[1]);
	else { 
		char *szChoosen = fl_file_chooser ("Select .csv file", "*.csv", "", 0);
		if (szChoosen == NULL)
			exit(0);
		openDB (szChoosen);
	}
#else
	char *szChoosen = fl_file_chooser ("Select .csv file", "*.csv", "", 0);
	if (szChoosen == NULL)
		exit(0);
	openDB (szChoosen);
#endif

	return Fl::run();
}

int parse_args(int argc, char **argv, int& idx)
{
	if (strcasecmp("--help", argv[idx]) == 0) {
		printf("Usage: \n\
  --help this help text\n\
  --config-dir <DIR>\n\
  --version\n");
		exit(0);
	} 
	if (strcasecmp("--version", argv[idx]) == 0) {
		printf("Version: "VERSION"\n");
		exit (0);
	}
//	if (strcasecmp("--config-dir", argv[idx]) == 0) {
//		homedir = argv[idx + 1];
//		if (homedir[homedir.length()-1] != '/')
//			homedir += '/';
//		idx += 2;
//		return 1;
//	}
	return 0;
}
