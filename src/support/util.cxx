// ----------------------------------------------------------------------------
// util.cxx
//
// Copyright (C) 2007-2009
//		Stelios Bounanos, M0GLD
// Copyright (C) 2009-2019
//		Dave Freese, W1HKJ
// Copyright (C) 2013
//		Remi Chateauneu, F4ECW
// Copyright (C) 2015
//		Robert Stiles, KK5VD
//
// This file is part of flnet.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <cstdlib>
#include <string>

#include <FL/fl_utf8.h>

#include "config.h"
#include "util.h"
#include "threads.h"

#ifdef __MINGW32__
#  include "compat.h"
#endif

/** ********************************************************************
 * Return the smallest power of 2 not less than n
 ***********************************************************************/
uint32_t ceil2(uint32_t n)
{
        --n;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;

        return n + 1;
}

/** ********************************************************************
 * Return the largest power of 2 not greater than n
 ***********************************************************************/
/// Return the largest power of 2 not greater than n
uint32_t floor2(uint32_t n)
{
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;

        return n - (n >> 1);
}

#include <stdlib.h>

/** ********************************************************************
 * Transforms the version, as a string, into an integer, so comparisons
 * are possible.
 ***********************************************************************/
unsigned long ver2int(const char* version)
{
	unsigned long v = 0L;
	const char* p = version;
	while (*p) {
		if (isdigit(*p)) v = v*10 + *p - '0';
		p++;
	}

	return v;
}

/** ********************************************************************
 * from git 1.6.3.1 compat/setenv.c
 ***********************************************************************/
#if !HAVE_SETENV
int setenv(const char *name, const char *value, int replace)
{
	int out;
	size_t namelen, valuelen;
	char *envstr;

	if (!name || !value) return -1;
	if (!replace) {
		char *oldval = NULL;
		oldval = getenv(name);
		if (oldval) return 0;
	}

	namelen = strlen(name);
	valuelen = strlen(value);
	envstr = (char*)malloc((namelen + valuelen + 2));
	if (!envstr) return -1;

	memcpy(envstr, name, namelen);
	envstr[namelen] = '=';
	memcpy(envstr + namelen + 1, value, valuelen);
	envstr[namelen + valuelen + 1] = 0;

	out = putenv(envstr);
	/* putenv(3) makes the argument string part of the environment,
	 * and changing that string modifies the environment --- which
	 * means we do not own that storage anymore.  Do not free
	 * envstr.
	 */

	return out;
}
#endif

/***********************************************************************
 * from git 1.6.3.1 compat/setenv.c
 ***********************************************************************/
#if !HAVE_UNSETENV
int unsetenv(const char *name)
{
	extern char **environ;
	int src, dst;
	size_t nmln;

	nmln = strlen(name);

	for (src = dst = 0; environ[src]; ++src) {
		size_t enln;
		enln = strlen(environ[src]);
		if (enln > nmln) {
			/* might match, and can test for '=' safely */
			if (0 == strncmp (environ[src], name, nmln)
			    && '=' == environ[src][nmln])
				/* matches, so skip */
				continue;
		}
		environ[dst] = environ[src];
		++dst;
	}
	environ[dst] = NULL;

	return 0;
}
#endif


/** ********************************************************************
 *
 ***********************************************************************/
#ifdef __MINGW32__
int set_cloexec(int fd, unsigned char v) { return 0; }
#else
#  include <unistd.h>
#  include <fcntl.h>
int set_cloexec(int fd, unsigned char v)
{
	int f = fcntl(fd, F_GETFD);
	return f == -1 ? f : fcntl(fd, F_SETFD, (v ? f | FD_CLOEXEC : f & ~FD_CLOEXEC));
}
#endif // __MINGW32__

/** ********************************************************************
 *
 ***********************************************************************/
int set_nonblock(int fd, unsigned char v)
{
#ifndef __MINGW32__
	int f = fcntl(fd, F_GETFL);
	return f == -1 ? f : fcntl(fd, F_SETFL, (v ? f | O_NONBLOCK : f & ~O_NONBLOCK));
#else // __MINGW32__
	u_long v_ = (u_long)v;
	errno = 0;
	if (ioctlsocket(fd, FIONBIO, &v_) == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return -1;
	}
	else
		return 0;
#endif // __MINGW32__
}

#ifndef __MINGW32__
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#endif

/** ********************************************************************
 *
 ***********************************************************************/
int set_nodelay(int fd, unsigned char v)
{
	int val = v;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val));
}

#ifdef __MINGW32__
#  include <ws2tcpip.h>
#endif

/** ********************************************************************
 *
 ***********************************************************************/
int get_bufsize(int fd, int dir, int* len)
{
	socklen_t optlen = sizeof(*len);
	return getsockopt(fd, SOL_SOCKET, (dir == 0 ? SO_RCVBUF : SO_SNDBUF),
			  (char*)len, &optlen);
}

/** ********************************************************************
 *
 ***********************************************************************/
int set_bufsize(int fd, int dir, int len)
{
	return setsockopt(fd, SOL_SOCKET, (dir == 0 ? SO_RCVBUF : SO_SNDBUF),
			  (const char*)&len, sizeof(len));
}

#ifndef __MINGW32__
#include <pthread.h>
#include <signal.h>
#ifndef NSIG
#  define NSIG 64
#endif
static size_t nsig = 0;
static struct sigaction* sigact = 0;
static pthread_mutex_t sigmutex = PTHREAD_MUTEX_INITIALIZER;
#endif

/** ********************************************************************
 *
 ***********************************************************************/
void save_signals(void)
{
#ifndef __MINGW32__
	pthread_mutex_lock(&sigmutex);
	if (!sigact)
		sigact = new struct sigaction[NSIG];
	for (nsig = 1; nsig <= NSIG; nsig++)
		if (sigaction(nsig, NULL, &sigact[nsig-1]) == -1)
			break;
	pthread_mutex_unlock(&sigmutex);
#endif
}

/** ********************************************************************
 *
 ***********************************************************************/
void restore_signals(void)
{
#ifndef __MINGW32__
	pthread_mutex_lock(&sigmutex);
	for (size_t i = 1; i <= nsig; i++)
		sigaction(i, &sigact[i-1], NULL);
	delete [] sigact;
	sigact = 0;
	nsig = 0;
	pthread_mutex_unlock(&sigmutex);
#endif
}

/** ********************************************************************
 *
 ***********************************************************************/
uint32_t simple_hash_data(const unsigned char* buf, size_t len, uint32_t code)
{
	for (size_t i = 0; i < len; i++)
		code = ((code << 4) | (code >> (32 - 4))) ^ (uint32_t)buf[i];

	return code;
}

/** ********************************************************************
 *
 ***********************************************************************/
uint32_t simple_hash_str(const unsigned char* str, uint32_t code)
{
	while (*str)
		code = ((code << 4) | (code >> (32 - 4))) ^ (uint32_t)*str++;
	return code;
}

#include <vector>
#include <climits>

static const char hexsym[] = "0123456789ABCDEF";

static std::vector<char>* hexbuf;
/** ********************************************************************
 *
 ***********************************************************************/
const char* str2hex(const unsigned char* str, size_t len)
{
	if (unlikely(len == 0))
		return "";
	if (unlikely(!hexbuf)) {
		hexbuf = new std::vector<char>;
		hexbuf->reserve(192);
	}
	if (unlikely(hexbuf->size() < len * 3))
		hexbuf->resize(len * 3);

	char* p = &(*hexbuf)[0];
	size_t i;
	for (i = 0; i < len; i++) {
		*p++ = hexsym[str[i] >> 4];
		*p++ = hexsym[str[i] & 0xF];
		*p++ = ' ';
	}
	*(p - 1) = '\0';

	return &(*hexbuf)[0];
}

/** ********************************************************************
 *
 ***********************************************************************/
const char* str2hex(const char* str, size_t len)
{
	return str2hex((const unsigned char*)str, len ? len : strlen(str));
}

static std::vector<char>* binbuf;
/** ********************************************************************
 *
 ***********************************************************************/
const char* uint2bin(unsigned u, size_t len)
{
	if (unlikely(len == 0))
		len = sizeof(u) * CHAR_BIT;

	if (unlikely(!binbuf)) {
		binbuf = new std::vector<char>;
		binbuf->reserve(sizeof(u) * CHAR_BIT);
	}
	if (unlikely(binbuf->size() < len + 1))
		binbuf->resize(len + 1);

	for (size_t i = 0; i < len; i++) {
		(*binbuf)[len - i - 1] = '0' + (u & 1);
		u >>= 1;
	}
	(*binbuf)[len] = '\0';

	return &(*binbuf)[0];
}

/** ********************************************************************
 *
 ***********************************************************************/
void MilliSleep(long msecs)
{
#ifndef __MINGW32__
	struct timespec tv[2] = { {msecs / 1000L, msecs % 1000L * 1000000L} };
	nanosleep(&tv[0], &tv[1]);
#else
	Sleep(msecs);
#endif
}

/** ********************************************************************
 * Returns 0 if a process is running, 0 if not there and -1 if the
 * test cannot be made.
 ***********************************************************************/
int test_process(int pid)
{
#ifdef __MINGW32__
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
#elif defined(__linux__)
	/// This is dependent on procfs.
	char buf[32];
	snprintf(buf, sizeof(buf), "/proc/%d/cmdline",pid);
	FILE * tmpF = fl_fopen( buf, "r" );
	if( tmpF != NULL ) {
		fclose(tmpF);
		return 1 ;
	}
	return 0 ;
#else
	// This would work on Linux also.
	int ret = kill(pid,0);
	if(ret == 0) return 1;
	if(errno == ESRCH) return 0;
	fprintf(stderr,"kill pid=%d failed r=%d e=%d %s\n", pid, ret, errno, strerror(errno) );
	return -1 ;
#endif
}

#ifdef __MINGW32__
/// This includes Windows.h
#include <winbase.h>

/** ********************************************************************
 * Retrieve the system error message for the last-error code
 ***********************************************************************/
static const char * WindowsError(DWORD dw)
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    /// BEWARE, this is NOT reentrant !
    static char buffer[2048];
    strcpy( buffer, (const char *)lpMsgBuf );
    LocalFree(lpMsgBuf);
    return buffer ;
}
#endif

/** ********************************************************************
 * Starts a process and returns its pid, and -1 if error. Returns 0 if
 * this cannot be made.
 ***********************************************************************/
int fork_process( const char * cmd )
{
#ifdef __MINGW32__
	char* cmd_local = strdup(cmd);

	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	if (!CreateProcess(NULL, cmd_local, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		fprintf(stderr,"CreateProcess failed: %s", WindowsError(GetLastError()) );
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	free(cmd_local);
	return pi.dwProcessId ;
#else
	pid_t newpid = fork();
	switch(newpid) {
	case -1:
		return -1 ;
	case 0:
		execl("/bin/sh", "sh", "-c", cmd, NULL );
		fprintf(stderr,"execl failed with %s", strerror(errno) );
		/// Ideally we should warn the main process.
		exit(EXIT_FAILURE);
	}
	return newpid ;
#endif
}

/** ********************************************************************
 *  Returns true if OK. Beware, the error case is not reentrant.
 ***********************************************************************/
const char * create_directory( const char * dir )
{
	if ( mkdir(dir, 0777) == -1 )
		if( errno != EEXIST ) return strerror(errno);
	return NULL ;
}

//======================================================================
// create back up files
//======================================================================
static pthread_mutex_t backup_mutex = PTHREAD_MUTEX_INITIALIZER;

//----------------------------------------------------------------------
// create copy of file 'from' --> 'to'
//----------------------------------------------------------------------

void fcopy(std::string from, std::string to)
{
	char buffer[65536];
	FILE *fp_from, *fp_to;
	size_t n;
	if ((fp_from = fopen(from.c_str(), "rb")) != NULL) {
		if ((fp_to = fopen(to.c_str(), "wb")) != NULL) {
			while(1) {
				if (feof(fp_from))
					break;
				memset(buffer, 0, sizeof(buffer));
				if ((n = fread(buffer, 1, sizeof(buffer), fp_from)) > 0)
					n = fwrite(buffer, 1, n, fp_to);
				else
					break;
			}
			fflush(fp_to);
			fclose(fp_to);
		}
		fclose(fp_from);
	}
}

//----------------------------------------------------------------------
// backup a file with up to two aging copies
// fname.ext --> fname.ext.1 --> fname.ext.2
// use mutex to prevent reentry or use by more than 1 thread at a time
//----------------------------------------------------------------------

void backup(std::string filename)
{
	guard_lock rlock(&backup_mutex);

	std::string oldfn, newfn;
	const char *ext[] = {".1", ".2"};

	for (int i = 1; i > 0; i--) {
		newfn.assign(filename).append(ext[i]);
		oldfn.assign(filename).append(ext[i - 1]);
		fcopy(oldfn, newfn);
	}
	newfn.assign(filename).append(ext[0]);
	fcopy(filename, newfn);
}

