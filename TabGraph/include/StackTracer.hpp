#pragma once

#ifndef _WIN32
#include <csignal>
#endif

namespace StackTracer {
/**
*	@brief Returns the signal handler file descriptor
*	@return The file descriptor currently used for stack dumping
*/
int handler_fd();

/**
*	@brief Sets the signal handler file descriptor
*	@param fd The new file descriptor to be used for stack dumping
*/
void set_handler_fd(int fd);

/**
*	@brief Prints the call stack of the current thread
*	@param signum The signal received, used internally
*/
void stack_trace(int signum = 0);

/**
*	@brief Returns the thread id (NOT PTHREAD ID)
*	This makes gettid available no matter what system you're using
*	@return The thread id of the current thread
*/
long gettid(void);

/**
*	@brief Returns the process id
*	This makes getpid available no matter what system you're using
*	@return The process id of the current process
*/
long getpid(void);

/**
*	@brief Signal handler to use with signal(SIGNUM, void (*sighandler)(int))
*	@param signum The signal received
*	@see http://manpagesfr.free.fr/man/man2/signal.2.html
*/
void sig_handler(int signum);

#ifndef _WIN32
/**
*	@brief Signal handler to use with
*	@param signum The signal received
*	@see http://manpagesfr.free.fr/man/man2/sigaction.2.html
*/
void sigaction_handler(int signum, siginfo_t* siginfo, void* /*UNUSED*/);
#endif

/**
*	@brief Sets the signal handler for this signum to internal signal handling function
*	@param signum The signal that will be intercepted
*/
void set_signal_handler(int signum);
}
