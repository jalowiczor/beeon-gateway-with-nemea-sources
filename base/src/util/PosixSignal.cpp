#include <unistd.h>
#include <cerrno>
#include <csignal>
#include <cstring>

#include <map>

#include <Poco/Exception.h>
#include <Poco/Mutex.h>
#include <Poco/Thread.h>

#include "util/Backtrace.h"
#include "util/PosixSignal.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

PosixSignal::PosixSignal()
{
}

void PosixSignal::send(long pid, unsigned int num)
{
	pid_t epid = (pid_t) pid;

	if (epid <= 0)
		throw InvalidArgumentException("invalid pid given");

	if (kill(epid, num) < 0)
		throw IllegalStateException(string("failed to send signal ") + strerror(errno));
}

void PosixSignal::send(const Thread &thread, unsigned int num)
{
	pthread_kill(thread.tid(), num);
}

void PosixSignal::ignore(const unsigned int num)
{
	if (signal(num, SIG_IGN) == SIG_ERR)
		throw InvalidArgumentException(strerror(errno));
}

unsigned int PosixSignal::byName(const string &name)
{
	if (name == "SIGTERM")
		return SIGTERM;
	else if (name == "SIGINT")
		return SIGINT;
	else if (name == "SIGKILL")
		return SIGKILL;
	else if (name == "SIGUSR1")
		return SIGUSR1;
	else if (name == "SIGUSR2")
		return SIGUSR2;
	else if (name == "SIGCONT")
		return SIGCONT;
	else if (name == "SIGHUP")
		return SIGHUP;
	else if (name == "SIGXFSZ")
		return SIGXFSZ;
	else
		throw InvalidArgumentException("unrecognized signal name");
}

void PosixSignal::send(long pid, const string name)
{
	send(pid, byName(name));
}

void PosixSignal::send(const Thread &thread, const string &name)
{
	send(thread, byName(name));
}

void PosixSignal::ignore(const string &name)
{
	ignore(byName(name));
}

void PosixSignal::handle(const std::string &name, Handler handler)
{
	handle(byName(name), handler);
}

/**
 * Map of registered signal handlers.
 */
static map<int, PosixSignal::Handler> g_handlers;

/**
 * Lock for accessing the g_handlers map.
 */
static Mutex g_lock;

static void handleSignal(const int sig)
{
	ScopedLockWithUnlock<Mutex> guard(g_lock);

	const auto it = g_handlers.find(sig);
	if (it == g_handlers.end())
		return;

	const PosixSignal::Handler handler = it->second;
	guard.unlock();

	handler(sig);
}

void PosixSignal::handle(const unsigned int num, Handler handler)
{
	Mutex::ScopedLock guard(g_lock);

	g_handlers.emplace(num, handler);
	signal(num, &handleSignal);
}

template <typename Number>
static void safePrint(int fd, Number n)
{
	if (n == 0) {
		write(fd, "0", 1);
		return;
	}

	char buf[(sizeof(Number) * 8) / 3];
	int i = sizeof(buf);

	while (i > 0 && n) {
		buf[--i] = '0' + (n % 10);
		n /= 10;
	}

	write(fd, buf + i, sizeof(buf) - i);
}

template <typename Number>
static void safePrintHex(int fd, Number n)
{
	char buf[sizeof(Number) * 2];
	memset(buf, '0', sizeof(buf));
	int i = sizeof(buf);

	while (i > 0 && n) {
		const char c = n % 16;
		buf[--i] = c < 10 ? ('0' + c) : ('a' + (c - 10));
		n /= 16;
	}

	write(fd, buf, sizeof(buf));
}

static void handleFault(const int sig, siginfo_t *info, void *)
{
	switch (sig) {
	case SIGILL:
#define SIGILL_MSG "sigill ("
		write(STDOUT_FILENO, SIGILL_MSG, sizeof(SIGILL_MSG));
		safePrint(STDOUT_FILENO, info->si_code);
		write(STDOUT_FILENO, ") at ", 5);
		break;

	case SIGFPE:
#define SIGFPE_MSG "sigfpe ("
		write(STDOUT_FILENO, SIGFPE_MSG, sizeof(SIGFPE_MSG));
		break;

	case SIGSEGV:
#define SIGSEGV_MSG "sigsegv ("
		write(STDOUT_FILENO, SIGSEGV_MSG, sizeof(SIGSEGV_MSG));
		break;

	case SIGBUS:
#define SIGBUS_MSG "sigbus ("
		write(STDOUT_FILENO, SIGBUS_MSG, sizeof(SIGBUS_MSG));
		break;

	default:
		safePrint(STDOUT_FILENO, sig);
		write(STDOUT_FILENO, " (", 2);
		break;
	}

	safePrint(STDOUT_FILENO, info->si_code);
	write(STDOUT_FILENO, ") at ", 5);
	safePrintHex(STDOUT_FILENO, (size_t) info->si_addr);

	write(STDOUT_FILENO, "\n", 1);
	fsync(STDOUT_FILENO);

	Backtrace trace;
	trace.dump(STDOUT_FILENO);

	_exit(-2);
}

static void handleAbort(const int sig, siginfo_t *info, void *)
{
	switch (sig) {
	case SIGABRT:
#define SIGABRT_MSG "sigabrt from "
		write(STDOUT_FILENO, SIGABRT_MSG, sizeof(SIGABRT_MSG));
		break;

	default:
		safePrint(STDOUT_FILENO, sig);
		write(STDOUT_FILENO, " from ", 4);
		break;
	}

	safePrint(STDOUT_FILENO, (size_t) info->si_pid);
	write(STDOUT_FILENO, ", ", 2);
	safePrint(STDOUT_FILENO, (size_t) info->si_uid);

	write(STDOUT_FILENO, "\n", 1);
	fsync(STDOUT_FILENO);

	Backtrace trace;
	trace.dump(STDOUT_FILENO);

	_exit(-2);
}

void PosixSignal::trapFatal()
{
	struct sigaction onFault;
	memset(&onFault, 0, sizeof(onFault));
	onFault.sa_flags = SA_SIGINFO;
	onFault.sa_sigaction = &handleFault;

	struct sigaction onAbort;
	memset(&onAbort, 0, sizeof(onAbort));
	onAbort.sa_flags = SA_SIGINFO;
	onAbort.sa_sigaction = &handleAbort;

	sigaction(SIGSEGV, &onFault, NULL);
	sigaction(SIGILL, &onFault, NULL);
	sigaction(SIGBUS, &onFault, NULL);
	sigaction(SIGFPE, &onFault, NULL);
	sigaction(SIGABRT, &onAbort, NULL);
}
