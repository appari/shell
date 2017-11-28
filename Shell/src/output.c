#include "../include/output.h"

void prompt(char *home){
	char hname[HNAME_SIZE], cdir[PATH_SIZE];
	struct utsname unameData;
	uname(&unameData);
	gethostname(hname, HNAME_SIZE);
	struct passwd *tf;
	register uid_t uid = geteuid ();
	tf=getpwuid(uid);
	getcwd(cdir, PATH_SIZE);
	char *rel = relative(cdir,home);
	printf("<%s@%s:%s> ", tf->pw_name, unameData.sysname, rel);
}

char *relative(char *abs,char *home)
{
	//char *home = getpwuid(getuid()) -> pw_dir;
	while (*home && (*home++ == *abs++));
	abs[-1] = '~';
	return abs - 1;
}
