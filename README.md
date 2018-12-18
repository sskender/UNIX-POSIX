# UNIX-POSIX
C programs that rely on POSIX standards

- - - - 

## Signals and interrupts

* Signals are a limited form of inter-process communication (IPC)

* Used in Unix, Unix-like, and other POSIX-compliant operating systems

* Signal is an asynchronous notification sent to a process or to a specific thread within the same process in order to notify it of an event that occurred

* When a signal is sent, the operating system interrupts the target process' normal flow of execution to deliver the signal
    * If the process has previously registered a signal handler, that routine is executed
    * Otherwise, the default signal handler is executed

```c
#include <signal.h>

int sighold(int sig);
int sigignore(int sig);
int sigpause(int sig);
int sigrelse(int sig);
void (*sigset(int sig, void (*disp)(int)))(int);
```

### Examples

1. [Prime numbers](interrupts/primes.c)
    * SIGALRM, SIGINT
    * Pause program and periodically print status

2. [Signal handler](interrupts/handler.c)
    * SIGUSR1, SIGUSR2
    * Handle and keep track of multiple signals with different priority

3. [Signal generator](interrupts/generator.c)
    * Generate signals for handler

- - - - 

## Multiprocessing and multithreading

 * A thread of execution is the smallest sequence of programmed instructions that can be managed independently by a scheduler, part of the operating system
 
 * A thread is a component of a process
 
 * Multiple threads can exist within one process, executing concurrently and sharing resources such as memory, while different processes do not share these resources
 
 * The threads of a process share its executable code and the values of its dynamically allocated variables and non-thread-local global variables at any given time

Take a loook at:
https://computing.llnl.gov/tutorials/pthreads/

### Examples

[Threads](multithreading/thread.c)

[Processes](multiprocessing/proc.c)

- - - -

## Synchronization

1. Lamport and Dekker algorithms
2. Atomic operations (TAS)
3. Semaphores
4. Mutex
5. Monitor
