//by duanjigang1983@gmail.com
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[])
{

    if(argc < 3)
    {
        printf ("usage:%s timevalue command parm1 param2..\n", argv[0]);
        return 1;
    }

    int     timeout = atoi (argv[1]);

    if(timeout <= 0)
    {
        printf ("error timeout '%s'\n", argv[1]);
        return 1;
    }
    pid_t   pid = fork ();

    if(pid < 0)
    {
        printf ("fork failed\n");
        return 1;
    }

    if(0 == pid)
    {
        char  **argvs = NULL;
        int     i = 0;

        dup2 (1, 2);
        argvs = (char **) malloc (sizeof (char *) * (argc - 1));
        if(!argvs)
        {
            printf ("malloc failed\n");
            exit (1);
        }
        memset (argvs, 0, sizeof (char *) * (argc - 1));

        for(i = 2; i < argc; i++)
        {
            argvs[i - 2] = argv[i];
        }
        argvs[i - 2] = NULL;
        if(execvp (argv[2], argvs) < 0)
        {
            perror ("Error on execv:");
        }
        exit (0);
    }
    else
    {
        pid_t   ret_pid = 0;
        time_t  last = time (0) + timeout;

        do
        {
            ret_pid = waitpid (pid, NULL, WNOHANG);
            if(ret_pid <= 0)
            {
                usleep (500);
            }
            else
            {
                break;
            }
        }
        while(time (0) < last);

        if(ret_pid <= 0)
        {
            kill (pid, SIGTERM);
            usleep (50);
            if(waitpid (pid, NULL, WNOHANG) <= 0)
            {
                kill (pid, SIGKILL);
            }
        }

    }
    return 0;
}
