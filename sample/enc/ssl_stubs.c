#include <string.h>
#include <openenclave/enclave.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <openssl/rand.h>

#include <sys/select.h>
#include <sys/poll.h>
#include <string.h>
#include <stdio.h>

int __fprintf_chk(FILE * stream, int flag, const char * format)
{
  return fprintf(stream, flag, format);
}

struct passwd *getpwnam(const char *name);

struct passwd *getpwuid(uid_t uid);

int getpwnam_r(const char *name, struct passwd *pwd,
            char *buf, size_t buflen, struct passwd **result);

int getpwuid_r(uid_t uid, struct passwd *pwd,
            char *buf, size_t buflen, struct passwd **result)
{
 return 0;
}

int __sprintf_chk()
{
 return 0;
}

void* __rawmemchr(const void* s, int c)
{
  return memchr(s, c, 10000000);
}

typedef long int __fd_mask;
#define __NFDBITS   (8 * (int) sizeof (__fd_mask))


typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler)
{
  return NULL;
}

int __poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
return poll(fds, nfds, timeout);
}

int
__poll_chk (struct pollfd *fds, nfds_t nfds, int timeout, __SIZE_TYPE__ fdslen)
{
return __poll (fds, nfds, timeout);
}


int getifaddrs(struct ifaddrs **ifap)
{
  return 0;
}

void freeifaddrs(struct ifaddrs *ifa)
{
  return;
}

long int __fdelt_chk (long int d)
{

  return d / __NFDBITS;
}


int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
  return 0;
}


char *__strtok_r(char *str, const char *delim, char **saveptr)
{
	return strtok_r(str, delim, saveptr);
}

char *secure_getenv(const char *name)
{
    return getenv(name);
}

struct hostent *gethostbyname(const char *name)
{
    return NULL;
}


char *oe_gai_strerror(int err)
{
  char *str = strerror(err);

  return str;
}

int getcontext(void *ucp)
{
  return -1;
}

int setcontext(const void *ucp)
{
  return -1;
}

void makecontext(void *ucp, void (*func)(), int argc, ...)
{
  return;
}


int mprotect(void *addr, size_t len, int prot) {
  errno = EACCES;
  return -1;
}

int madvise(void *addr, size_t len, int advice) {
  return 0;
}

int mlock(const void *__addr, size_t __len) {
  return 0;
}


int pthread_atfork(void (*prepare)(void), void (*parent)(void),
       void (*child)(void))
{
  return EPERM;
}

int tcgetattr(int fd, struct termios *termios_p)
{
  return 0;
}

int tcsetattr(int fd, int optional_actions,
              const struct termios *termios_p)
{
  return 0;
}

gid_t getgid(void)
{
  gid_t gid = {0};
  return gid;
}

gid_t getegid(void)
{
  gid_t gid = {0};
  return gid;
}

