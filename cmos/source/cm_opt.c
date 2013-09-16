#include "cmos.h"


static void usage(int argc, char * argv[]);
static void version (void)
{
	printf ("cmos:2011-09-28 creating\n");
	printf ("cmos:2011-09-29 changing 'comment' to 'desc'\n");
	printf ("cmos:2011-10-11 adding completion for cmos\n");
	printf ("cmos:2011-10-13 disabling completion for cmos because of dependency of readline and ncurses\n");
	printf ("cmos:2011-10-13 adding timeout to cmos\n");
	printf ("cmos:2011-10-19 adding parameter support for cmos\n");
	printf ("cmos:2011-10-20 adding logging to stderr support for cmos\n");
	printf ("cmos:2011-11-01 adding user and cmdpath for cmos to support a linked and limited plugin\n");
	printf ("cmos:2011-11-02 duplicating stderr to stdout of each plugin but not cmos self\n");
	printf ("cmos:2011-11-04 adding plugin cmos for cmos as the basic plugin\n");
	printf ("cmos:2012-01-09 adding color switch for cmos and maintanier for each plugin\n");
	printf ("cmos:2012-03-21 updating version for cmos when it is deliverted to cm3\n");
	printf ("cmos:2012-04-19 adding option 'run_time_out' for 'set' command\n");
	printf ("cmos:2012-05-03 rename timeout to cmos-timeout\n");
	exit (0);
}
//int8_t		cm_parseparam (int argc, char* argv[], char *szbuf)
int8_t		cm_parseparam (int argc, char* argv[], cmos_config_t * cfg)
{
	
        int c;
	char * tail = 0;
	char * szbuf = cfg->plugin_dir;
	cfg->keep_alive = 0;
	cfg->input_time_out = INPUT_TIMEOUT; 
	cfg->run_time_out = RUN_TIMEOUT; 
	cfg->op_mask = 0;
	cfg->color_on = 0; //added by jigang.djg@2012-01-09
	while ((c = getopt (argc, argv, "f:hvk")) != -1)
        {
                switch (c)
                {
                        //listen port
                        case 'f':
			
                                if (strlen(optarg) > PATH_LEN)
				{
					printf ("name of data path '%s' is too long(<%u)\n", optarg, PATH_LEN);
					return -1;
				}
				memset (szbuf, 0, PATH_LEN);
				strncpy (szbuf, optarg, strlen(optarg)+1);
				tail = szbuf + strlen(szbuf) - 1;
				if (*tail == '/') *tail = '\0';
				return 1;
                                break;
			case 'v':
				{
					version();
					return 1;
				}
			//added by jigang.djg@2011-10-12 --start
			case 'k':
				{
					cfg->keep_alive = 1;
					break;
				}	
			//added by jigang.djg@2011-10-12 --end
			default:
				usage (argc, argv);
				exit (0);
		}
	}
	usage (argc, argv);
	exit (0);
	return 1;
}
void usage(int argc, char* argv[])
{
	printf ("usage:%s\n\t-f\tPlugin_path\n\t-k\tKeep alive when a command has been run  over\n\t-v\tShow version\n\t-h\tShow this message\n", argv[0]);
}
