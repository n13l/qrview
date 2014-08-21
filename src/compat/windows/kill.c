#include <compat/windows.h>
#include <sys/compiler.h>
#include <sys/missing.h>
#include <stdbool.h>
#include <tlhelp32.h>

#define EXIT_TIMEOUT 8000

struct wmq {
	int pid;
	int fds;
};

static bool
postWMQuitToProcessSnap(DWORD processId, HANDLE snapshot);

static bool
postWMQuitToProcess(DWORD processId)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, processId);
	if (snapshot == INVALID_HANDLE_VALUE)
		return false;

	bool r = postWMQuitToProcessSnap(processId, snapshot);

	CloseHandle(snapshot);
	return r;
}

static bool
postWMQuitToProcessSnap(DWORD processId, HANDLE snapshot)
{
	THREADENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);

	if (!Thread32First(snapshot, &entry))
		return false;
	if (entry.th32OwnerProcessID == processId && PostThreadMessage(entry.th32ThreadID, WM_QUIT, 0, 0))
		return true;

	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);

	while (Thread32Next(snapshot, &entry)) {
		if (entry.th32OwnerProcessID != processId)
			continue;
		if (PostThreadMessage(entry.th32ThreadID, WM_QUIT, 0, 0))
			return true;
	}

	return false;
}

static BOOL CALLBACK
TerminateAppEnum(HWND hwnd, LPARAM lparam )
{
	struct wmq *wmq = (struct wmq *)lparam;

	DWORD id;
	GetWindowThreadProcessId(hwnd, &id) ;

	if (id == wmq->pid) {
		wmq->fds++;
		sys_dbg("post close message to pid: %d hwnd: %p", (int)id, hwnd);
		//PostMessage(hwnd, WM_CLOSE, 0, 0);
		//PostMessage(hwnd, WM_QUIT, 0, 0);
		PostMessage(hwnd, WM_QUERYENDSESSION, 0, 0);
	}

	return TRUE;
}

static DWORD WINAPI
TerminateApp(DWORD pid, DWORD timeout)
{
	DWORD rv;

	HANDLE proc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pid);
	if(proc == NULL)
		return 1;

	postWMQuitToProcess(pid);

	struct wmq wmq = {.pid = pid, .fds = 0};

	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM)&wmq);

	sys_dbg("process pid: %d waiting for gracefull exit for maximum %d milliseconds", 
	        (int)pid, (int)timeout);
	if (wmq.fds == 0 || WaitForSingleObject(proc, timeout) != WAIT_OBJECT_0) {
		sys_dbg("process pid=%d killed", (int)pid);
		rv = (TerminateProcess(proc, 0) ? 0: 1);
	} else {
		sys_dbg("process pid=%d gracefull exit", (int)pid);
		rv = 0;
	}

	CloseHandle(proc);
	return rv;
}

static int
proc_exists(int pid)
{
	HANDLE h;
	if ((h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)) == NULL)
		return -1;

	CloseHandle(h);
	return 0;
}

static int
proc_kill(int pid)
{
	HANDLE h;
	if (!(h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)))
		return -1;

	TerminateProcess(h, 1);
	CloseHandle(h);
	return 0;
}

static int
proc_term(int pid)
{
	TerminateApp(pid, EXIT_TIMEOUT);
	return 0;
}

int
kill(int pid, int sig)
{
	switch(sig) {
	case SIGNONE: return proc_exists(pid);
	case SIGTERM: return proc_term(pid);
	case SIGKILL: return proc_kill(pid);
	default:
		return -1;
		break;
	}
}
