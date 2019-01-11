/* vi: set sw=4 ts=4: */
/* main.c */

#include <stdio.h>
#include <string.h>

extern int upnpd_main(int, char **);
extern int hnap_main(int, char **);

int main(int argc, char * argv[], char * env[])
{
	char * base;
	int ret = 1;

	base = strrchr(argv[0], '/');
	if (base) base = base + 1;
	else base = argv[0];
 
	if (strcmp(base, "upnpd")==0)	ret = upnpd_main(argc, argv); else
	if (strcmp(base, "hnap")==0)	ret = hnap_main(argc, argv); else
		printf("Not found match basename %s\n", base);

	return ret;
}
