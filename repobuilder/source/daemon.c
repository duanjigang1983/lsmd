#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int daemon (int nochdir, int noclose)
{
  pid_t pid;
  pid = fork ();
  if (pid < 0)
    {
      perror ("fork");
      return -1;
    }
  if (pid != 0)
    exit (0);
  pid = setsid();
  if (pid < -1)
    {
      perror ("setsid");
      return -1;
    }
  if (! nochdir)
    chdir ("/");
  if (! noclose)
    {
      int fd;
      fd = open ("/dev/null", O_RDWR, 0);
      if (fd != -1)
    {
      dup2 (fd, STDIN_FILENO);
      dup2 (fd, STDOUT_FILENO);
      dup2 (fd, STDERR_FILENO);
      close(fd);
    }
    }
  umask (0027);
  return 0;

}

