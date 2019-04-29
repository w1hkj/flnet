// ----------------------------------------------------------------------------
//      debug.cxx
//
// Copyright (C) 2008, 2012
//              Stelios Bounanos, M0GLD, Dave Freese, W1HKJ
// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flnet
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------


#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <string>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Button.H>

#include <FL/Fl_Browser.H>

#include "debug.h"
#include "icons.h"
#include "gettext.h"
#include "netshared.h"
#include "threads.h"

using namespace std;

#define MAX_LINES 65536

static FILE* wfile;
static FILE* rfile;
static int rfd;

static Fl_Double_Window*	window;
static Fl_Browser*			btext;
static pthread_mutex_t debug_mutex  = PTHREAD_MUTEX_INITIALIZER;

debug* debug::inst = 0;
debug::level_e debug::level = debug::INFO_LEVEL;
uint32_t debug::mask = ~0u;

const char* prefix[] = { "Quiet", "Error", "Warning", "Info", "Debug" };

static void slider_cb(Fl_Widget* w, void*);
static void clear_cb(Fl_Widget *w, void*);

static char fmt[1024];
static char sztemp[1024];
static string slines = "";

void debug::start(const char* filename)
{
	if (debug::inst) return;
	inst = new debug(filename);

	window = new Fl_Double_Window(600, 256, "Event log");

	Fl_Slider* slider = new Fl_Slider(2, 2, 128, 20, prefix[level]);
	slider->tooltip("Change log level");
	slider->align(FL_ALIGN_RIGHT);
	slider->type(FL_HOR_NICE_SLIDER);
	slider->range(0.0, LOG_NLEVELS - 1);
	slider->step(1.0);
	slider->value(level);
	slider->callback(slider_cb);

	Fl_Button* clearbtn = new Fl_Button(window->w() - 65, 2, 60, 20, "clear");
	clearbtn->callback(clear_cb);

	btext = new Fl_Browser(
			2,  slider->h()+2,
			window->w() - 4, window->h() - slider->h() - 4, 0);
	btext->textfont(FL_COURIER);
	window->resizable(btext);

	window->end();
}

void debug::stop(void)
{
	delete inst;
	inst = 0;
	if(window) {
		delete window;
		window = 0;
	}
}

void debug::log(level_e level, const char* func, const char* srcf, int line, const char* format, ...)
{
	if (!inst) return;
	if (level > debug::level) return;

	snprintf(fmt, sizeof(fmt), "%c: %s: %s\n", *prefix[level], func, format);

{
	guard_lock text_lock(&debug_mutex);
	va_list args;
	va_start(args, format);
	sztemp[0] = 0;
	vsnprintf(sztemp, sizeof(sztemp), fmt, args);
	va_end(args);

	fprintf(wfile, "%s", sztemp);
	fflush(wfile);
	slines.append(sztemp);
}
	Fl::awake(sync_text);
}

void debug::slog(level_e level, const char* func, const char* srcf, int line, const char* format, ...)
{
	if (!inst) return;
	if (level > debug::level) return;

	snprintf(fmt, sizeof(fmt), "%c:%s\n", *prefix[level], format);
{
	guard_lock text_lock(&debug_mutex);
	va_list args;
	va_start(args, format);
	vsnprintf(sztemp, sizeof(sztemp), fmt, args);
	va_end(args);
	fflush(wfile);
	slines.append(sztemp);
}
	Fl::awake(sync_text);
}

void debug::elog(const char* func, const char* srcf, int line, const char* text)
{
	log(ERROR_LEVEL, func, srcf, line, "%s: %s", text, strerror(errno));
}

void debug::show(void)
{
	window->show();
}

void debug::sync_text(void* arg)
{
	guard_lock text_lock(&debug_mutex);
	size_t p = string::npos;
	while (!slines.empty()) {
		p = slines.find('\n');
		btext->add(slines.substr(0, p).c_str());
		slines.erase(0, p + 1);
	}
	if (btext->size()) btext->show(btext->size());
	btext->redraw();
	slines.clear();
	return;
}

debug::debug(const char* filename)
{
	if(!filename) return;

	if ((wfile = fopen(filename, "w")) == NULL)
		throw strerror(errno);
	setvbuf(wfile, (char*)NULL, _IOLBF, 0);

	if ((rfile = fopen(filename, "r")) == NULL)
		throw strerror(errno);
	rfd = fileno(rfile);
#ifndef __WIN32__
	int f;
	if ((f = fcntl(rfd, F_GETFL)) == -1)
		throw strerror(errno);
	if (fcntl(rfd, F_SETFL, f | O_NONBLOCK) == -1)
		throw strerror(errno);
#endif
}

debug::~debug()
{
	fclose(wfile);
	fclose(rfile);
}

static void slider_cb(Fl_Widget* w, void*)
{
	debug::level = (debug::level_e)((Fl_Slider*)w)->value();
	w->label(prefix[debug::level]);
	w->parent()->redraw();
}

static void clear_cb(Fl_Widget* w, void*)
{
	btext->clear();
}
