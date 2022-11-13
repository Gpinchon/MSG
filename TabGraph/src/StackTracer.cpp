/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:17
*/

#include <StackTracer.hpp>
#include <inttypes.h>

static void putstr(const char* str);
static void putnbr(const int nbr);
static int& _get_handler_fd();

#define _DEBUGLOG(file, line, message) \
    {                                  \
        putstr("file:///");            \
        putstr(file);                  \
        putstr(":");                   \
        putnbr(line);                  \
        putstr(": ");                  \
        putstr(message);               \
        putstr("\n");                  \
    } //std::cerr << "file:///" << file << ":" << line << ": " << message << std::endl;
#define DEBUGLOG(message) _DEBUGLOG(__FILE__, __LINE__, message)

static inline char* itoa_safe(intmax_t value, char* result)
{
    intmax_t tmp_value;
    char *ptr, *ptr2;
    ptr = result;
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[35 + (tmp_value - value * 10)];
    } while (value);
    if (tmp_value < 0)
        *ptr++ = '-';
    ptr2 = result;
    result = ptr;
    *ptr-- = '\0';
    while (ptr2 < ptr) {
        char tmp_char = *ptr;
        *ptr-- = *ptr2;
        *ptr2++ = tmp_char;
    }
    return result;
}

int StackTracer::handler_fd()
{
    return _get_handler_fd();
}

void StackTracer::set_handler_fd(int fd)
{
    _get_handler_fd() = fd;
}

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <cstring>
#include <io.h>
#include <iostream>
#include <process.h>
#include <signal.h>
#include <stdio.h>


int& _get_handler_fd()
{
    static int fd = _fileno(stderr);
    return fd;
}

inline void putstr(const char* str)
{
    if (str)
        _write(StackTracer::handler_fd(), str, strlen(str));
    else
        _write(StackTracer::handler_fd(), "???", 3);
}

inline void putnbr(const int nbr)
{
    char buffer[1024];
    itoa_safe(nbr, buffer);
    _write(StackTracer::handler_fd(), buffer, strlen(buffer));
}

static inline DWORD GetMainThreadId()
{
    static DWORD result = 0;
    const HANDLE hThreadSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));
    if (hThreadSnapshot == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetMainThreadId failed");
    }
    THREADENTRY32 tEntry;
    tEntry.dwSize = sizeof(THREADENTRY32);

    DWORD currentPID = GetCurrentProcessId();
    for (BOOL success = Thread32First(hThreadSnapshot, &tEntry);
         !result && success && GetLastError() != ERROR_NO_MORE_FILES;
         success = Thread32Next(hThreadSnapshot, &tEntry)) {
        if (tEntry.th32OwnerProcessID == currentPID) {
            result = tEntry.th32ThreadID;
            break;
        }
    }
    CloseHandle(hThreadSnapshot);
    return result;
}

#define IS_MAIN_THREAD (GetCurrentThreadId() == GetMainThreadId())

STACKFRAME64 FillStackFrame(CONTEXT context)
{
    STACKFRAME64 s = {};

#ifdef _M_IX86
    s.AddrPC.Offset = context.Eip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = context.Ebp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrStack.Offset = context.Esp;
    s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    s.AddrPC.Offset = context.Rip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrStack.Offset = context.Rsp;
    s.AddrStack.Mode = AddrModeFlat;
    s.AddrFrame.Offset = context.Rbp;
    s.AddrFrame.Mode = AddrModeFlat;
#elif _M_IA64
    s.AddrPC.Offset = context.StIIP;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = context.IntSp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrBStore.Offset = context.RsBSP;
    s.AddrBStore.Mode = AddrModeFlat;
    s.AddrStack.Offset = context.IntSp;
    s.AddrStack.Mode = AddrModeFlat;
#endif
    return s;
}

DWORD GetMachineType()
{
#ifdef _M_IX86
    return IMAGE_FILE_MACHINE_I386;
#elif _M_X64
    return IMAGE_FILE_MACHINE_AMD64;
#elif _M_IA64
    return IMAGE_FILE_MACHINE_IA64;
#endif
}

void stackWalkPrint(HANDLE process, HANDLE thread)
{
    DWORD machineType = GetMachineType();
    CONTEXT context = {};
    STACKFRAME64 stackframe;

    context.ContextFlags = CONTEXT_FULL;
    if (!GetThreadContext(thread, &context)) {
        DEBUGLOG("Couldn't get thread context");
        return;
    }
    stackframe = FillStackFrame(context);
    SymSetOptions(SYMOPT_LOAD_LINES);
    SymInitialize(process, NULL, TRUE);

    while (StackWalk64(
        machineType, process, thread,
        &stackframe, &context, NULL,
        SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {

        BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME] = {};
        DWORD offset = 0;
        PIMAGEHLP_SYMBOL64 symbol = (PIMAGEHLP_SYMBOL64)symbolBuffer;
        HINSTANCE moduleBase = (HINSTANCE)SymGetModuleBase64(process, stackframe.AddrPC.Offset);

        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME;
        symbol->MaxNameLength = MAX_SYM_NAME;

        putstr("\t");
        if (moduleBase) {
            char moduleBuff[MAX_PATH];
            DWORD64 displacement = 0;
            IMAGEHLP_LINE64 line = {};
            line.SizeOfStruct = sizeof(line);

            if (GetModuleFileName(moduleBase, moduleBuff, MAX_PATH)) {
                putstr("[Module : ");
                putstr(moduleBuff);
                putstr("]");
            } else {
                putstr("[Module : ERROR ");
                putnbr(GetLastError());
                putstr("]");
            }
            if (SymGetLineFromAddr64(process, stackframe.AddrPC.Offset, &offset, &line)) {
                putstr("[Location : file:///");
                putstr(line.FileName);
                putstr(":");
                putnbr(line.LineNumber);
                putstr("]");
            } else {
                putstr("[Location : ERROR(");
                putnbr(GetLastError());
                putstr(")]");
            }
            if (SymGetSymFromAddr64(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
                putstr("[Symbol : ");
                putstr(symbol->Name);
                putstr("]");
            } else {
                putstr("[Symbol : ERROR ");
                putnbr(GetLastError());
                putstr("]");
            }
            putstr("\n");
        } else
            DEBUGLOG("Couldn't get module base");
    }
}

inline void StackTracer::stack_trace(int signum)
{
    if (!IS_MAIN_THREAD) {
        return;
    }
    putstr("Catched signal(");
    putnbr(signum);
    putstr(")\n");
    HANDLE process = GetCurrentProcess();
    HANDLE shapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (shapshot == INVALID_HANDLE_VALUE)
        return;
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(threadEntry);
    if (Thread32First(shapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID != GetCurrentProcessId())
                continue;
            if (threadEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry.th32OwnerProcessID)) {
                if (GetMainThreadId() == threadEntry.th32ThreadID)
                    putstr("Main thread stack :\n");
                else {
                    putstr("Thread ");
                    putnbr(threadEntry.th32ThreadID);
                    putstr(" stack :\n");
                }
                auto hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, false, threadEntry.th32ThreadID);
                if (hThread == NULL) {
                    DEBUGLOG("Couldn't get thread handle");
                } else {
                    stackWalkPrint(process, hThread);
                }
            }
            threadEntry.dwSize = sizeof(threadEntry);
        } while (Thread32Next(shapshot, &threadEntry));
        CloseHandle(shapshot);
    }
    SymCleanup(process);
}

long StackTracer::gettid()
{
    return GetCurrentThreadId();
}

long StackTracer::getpid()
{
    return _getpid();
}

void StackTracer::set_signal_handler(int signum)
{
    signal(signum, StackTracer::stack_trace);
}

#else

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <execinfo.h>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef tkill
#define tkill(tid, sig) syscall(SYS_tgkill, getpid(), tid, sig)
#endif

#define MAX_STACK_FRAMES 64
#define IS_MAIN_THREAD (StackTracer::getpid() == StackTracer::gettid())

static inline char* program_path()
{
    static char buf[4096];
    static size_t len;
    if (len > 0)
        return buf;
    len = readlink("/proc/self/exe", buf, 4095);
    buf[len - 1] = 0;
    return buf;
}

static inline void putstr(const char* str)
{
    if (str)
        write(StackTracer::handler_fd(), str, strlen(str));
    else
        write(StackTracer::handler_fd(), "???", 3);
}

static inline void putnbr(const int nbr)
{
    char buffer[1024];
    itoa_safe(nbr, buffer);
    write(StackTracer::handler_fd(), buffer, strlen(buffer));
}

template <typename T>
class AtomicVariable {
public:
    AtomicVariable(T value)
    {
        pthread_rwlock_init(&_lock, NULL);
        lockWrite();
        _value = value;
        lockWrite(false);
    };
    ~AtomicVariable()
    {
        pthread_rwlock_destroy(&_lock);
    }

    void lockRead(bool lock = true)
    {
        if (lock)
            pthread_rwlock_rdlock(&_lock);
        else
            pthread_rwlock_unlock(&_lock);
    }

    void lockWrite(bool lock = true)
    {
        if (lock)
            pthread_rwlock_wrlock(&_lock);
        else
            pthread_rwlock_unlock(&_lock);
    }

    T get(void)
    {
        lockRead();
        T localValue = _value;
        lockRead(false);
        return localValue;
    }

    void set(T value)
    {
        lockWrite();
        _value = value;
        lockWrite(false);
    }

    T operator++(int)
    {
        T localValue = *this;
        localValue++;
        *this = localValue;
        return *this;
    }

    T operator+(T value)
    {
        *this = *this + value;
        return *this;
    }

    T operator=(T value)
    {
        set(value);
        return *this;
    }

    operator T()
    {
        return get();
    }

private:
    T _value;
    pthread_rwlock_t _lock;
};

static AtomicVariable<size_t> threadNbr(0);
static AtomicVariable<size_t> threadDone(0);
static AtomicVariable<bool> handlingSignal(false);

/** @brief Wait for threads to print their stack for 30 secs at maximum.
*/
static inline void waitForThreads()
{
    int failSafe = 0;
    while (threadNbr > threadDone && failSafe < 30) {
        sleep(1);
        failSafe++;
    }
}

struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
    char d_name[];
};

#define DIR_BUF_SIZE 1024

static inline void killAllThreads(int signum)
{
    int dir_fd;
    char dir_buf[DIR_BUF_SIZE];
    struct linux_dirent* dir_entry;
    int r, pos;

    dir_fd = open("/proc/self/task", O_RDONLY | O_DIRECTORY);
    r = syscall(SYS_getdents, dir_fd, dir_buf, DIR_BUF_SIZE);
    for (pos = 0; pos < r; pos += dir_entry->d_reclen) {
        dir_entry = (struct linux_dirent*)(dir_buf + pos);
        if (dir_entry->d_name[0] == '.')
            continue;
        int threadId = atoi(dir_entry->d_name);
        if (threadId == StackTracer::gettid())
            continue;
        if (syscall(SYS_tgkill, StackTracer::getpid(), threadId, signum) < 0) {
            DEBUGLOG("Could not send signal to thread");
            continue;
        }
        ++threadNbr;
    }
    close(dir_fd);
}

static inline int& _get_handler_fd()
{
    static int fd = STDERR_FILENO;
    return fd;
}

void StackTracer::stack_trace(int signum)
{
    static pthread_rwlock_t writeLock;
    static AtomicVariable<bool> first = true;
    if (first) {
        pthread_rwlock_init(&writeLock, NULL);
        first = false;
    }
    pthread_rwlock_wrlock(&writeLock);
    putstr("--------------------------------------------------------------------------------\n");
    if (IS_MAIN_THREAD)
        putstr("Main Thread");
    else {
        putstr("Thread ");
        putnbr(StackTracer::gettid() - StackTracer::getpid());
    }
    putstr(" received signal(");
    putnbr(signum);
    putstr(")\n");
    void* stackFrames[MAX_STACK_FRAMES];
    size_t size = backtrace(stackFrames, MAX_STACK_FRAMES);
    backtrace_symbols_fd(stackFrames, size, StackTracer::handler_fd());
    putstr("--------------------------------------------------------------------------------\n");
    pthread_rwlock_unlock(&writeLock);
    if (signum == 0)
        return;
    if (!IS_MAIN_THREAD) {
        threadDone++;
        if (!handlingSignal)
            syscall(SYS_tgkill, StackTracer::getpid(), StackTracer::getpid(), signum);
        //kill(0, signum);
        while (handlingSignal) {
        }
        return;
        //pthread_exit(NULL);
    }
    killAllThreads(signum);
    waitForThreads();
    pthread_rwlock_wrlock(&writeLock);
    pthread_rwlock_destroy(&writeLock);
}

void StackTracer::sig_handler(int signum)
{
    if (IS_MAIN_THREAD)
        handlingSignal = true;
    putstr("Received signal (");
    putnbr(signum);
    putstr(")\n");
    StackTracer::stack_trace(signum);
    if (IS_MAIN_THREAD)
        handlingSignal = false;
}

void StackTracer::sigaction_handler(int signum, siginfo_t* siginfo, void*)
{
    if (IS_MAIN_THREAD) {
        handlingSignal = true;
        psiginfo(siginfo, NULL);
    }
    StackTracer::stack_trace(signum);
    if (IS_MAIN_THREAD) {
        handlingSignal = false;
        threadDone = 0;
        threadNbr = 0;
        signal(signum, SIG_DFL);
        raise(signum);
    }
}

void StackTracer::set_signal_handler(int signum)
{
    struct sigaction sa = {};
    sa.sa_handler = StackTracer::sig_handler;
    sa.sa_restorer = NULL;
    sa.sa_sigaction = StackTracer::sigaction_handler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(signum, &sa, NULL);
}

#ifdef gettid
long StackTracer::gettid(void)
{
    return ::gettid();
}
#else
long StackTracer::gettid(void)
{
    return syscall(SYS_gettid);
}
#endif //gettid

long StackTracer::getpid(void)
{
    return ::getpid();
}

#endif //_WIN32
