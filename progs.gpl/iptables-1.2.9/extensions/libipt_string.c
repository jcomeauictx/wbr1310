/* Shared library add-on to iptables to add string matching support. 
 * 
 * Copyright (C) 2000 Emmanuel Roger  <winfield@freegates.be>
 *
 * ChangeLog
 *     27.01.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 *             Changed --tos to --string in save(). Also
 *             updated to work with slightly modified
 *             ipt_string_info.
 */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

#include <iptables.h>
#include <linux/netfilter_ipv4/ipt_string.h>


/* Function which prints out usage message. */
static void
help(void)
{
	printf(
"STRING match v%s options:\n"
"--string [!] string          Match a string in a packet\n"
"--hex-string [!] string      Match a hex string in a packet\n"
"--url [!] string      Match a http url string in a packet\n",
IPTABLES_VERSION);
}


static struct option opts[] = {
	{ .name = "string",     .has_arg = 1, .flag = 0, .val = '1' },
	{ .name = "hex-string", .has_arg = 1, .flag = 0, .val = '2' },
//+++ joel	
	{ .name = "url", .has_arg = 1, .flag = 0, .val = '3' },
	{ .name = "dns", .has_arg = 1, .flag = 0, .val = '4' },
	{ .name = "http_req", .has_arg = 0, .flag = 0, .val = '5' },
//--- joel	
	{ .name = 0 }
};


/* Initialize the match. */
static void
init(struct ipt_entry_match *m, unsigned int *nfcache)
{
	*nfcache |= NFC_UNKNOWN;
}


static void
parse_string(const unsigned char *s, struct ipt_string_info *info)
{	
	if (strlen(s) <= BM_MAX_NLEN) strcpy(info->string, s);
	else exit_error(PARAMETER_PROBLEM, "STRING too long `%s'", s);
}


static void
parse_hex_string(const unsigned char *s, struct ipt_string_info *info)
{
	int i=0, slen, sindex=0, schar;
	short hex_f = 0, literal_f = 0;
	char hextmp[3];

	slen = strlen(s);

	if (slen == 0) {
		exit_error(PARAMETER_PROBLEM,
			"STRING must contain at least one char");
	}

	while (i < slen) {
		if (s[i] == '\\' && !hex_f) {
			literal_f = 1;
		} else if (s[i] == '\\') {
			exit_error(PARAMETER_PROBLEM,
				"Cannot include literals in hex data");
		} else if (s[i] == '|') {
			if (hex_f)
				hex_f = 0;
			else
				hex_f = 1;
			if (i+1 >= slen)
				break;
			else
				i++;  /* advance to the next character */
		}

		if (literal_f) {
			if (i+1 >= slen) {
				exit_error(PARAMETER_PROBLEM,
					"Bad literal placement at end of string");
			}
			info->string[sindex] = s[i+1];
			i += 2;  /* skip over literal char */
			literal_f = 0;
		} else if (hex_f) {
			if (i+1 >= slen) {
				exit_error(PARAMETER_PROBLEM,
					"Odd number of hex digits");
			}
			if (i+2 >= slen) {
				/* must end with a "|" */
				exit_error(PARAMETER_PROBLEM, "Invalid hex block");
			}
			if (! isxdigit(s[i])) /* check for valid hex char */
				exit_error(PARAMETER_PROBLEM, "Invalid hex char `%c'", s[i]);
			if (! isxdigit(s[i+1])) /* check for valid hex char */
				exit_error(PARAMETER_PROBLEM, "Invalid hex char `%c'", s[i+1]);
			hextmp[0] = s[i];
			hextmp[1] = s[i+1];
			hextmp[2] = '\0';
			if (! sscanf(hextmp, "%x", &schar))
				exit_error(PARAMETER_PROBLEM,
					"Invalid hex char `%c'", s[i]);
			info->string[sindex] = (char) schar;
			if (s[i+2] == ' ')
				i += 3;  /* spaces included in the hex block */
			else
				i += 2;
		} else {  /* the char is not part of hex data, so just copy */
			info->string[sindex] = s[i];
			i++;
		}
		if (sindex > BM_MAX_NLEN)
			exit_error(PARAMETER_PROBLEM, "STRING too long `%s'", s);
		sindex++;
	}
	info->len = sindex;
}
//+++joel
//for dns query ! www.kimo.com.tw==>3www4kimo3com2tw(3=>www,4=>kimo,3=>com,2=>tw)
static void
parse_dns_string(const unsigned char *in, struct ipt_string_info *info)
{
    if (strlen(in) >= BM_MAX_NLEN) 
    {
        exit_error(PARAMETER_PROBLEM, "STRING too long `%s'", in);
        return ;
    }
    else
    {
	    char *pdot=NULL;
	    char *pout=info->string;
	    while(*in==' ') in++;//skip space
	    pdot=pout++;
	    while(*in!='\0')
	    {
		    if(*in=='.')
		    {
			    *pdot=pout-pdot-1;
			    pdot=pout;
		    }
		*pout++=*in++;
	   }
	    *pdot=pout-pdot-1;
	    *pout='\0';
	    
    }
}
//---joel
/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      unsigned int *nfcache,
      struct ipt_entry_match **match)
{
	struct ipt_string_info *stringinfo = (struct ipt_string_info *)(*match)->data;

	switch (c) {
	case '1':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
				   "Can't specify multiple strings");

		check_inverse(optarg, &invert, &optind, 0);
		parse_string(argv[optind-1], stringinfo);
		if (invert)
			stringinfo->invert = 1;
		stringinfo->len=strlen((char *)&stringinfo->string);
		*flags = 1;
		stringinfo->flags=IPT_GEN_STRING;//joel add
		break;

	case '2':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
				   "Can't specify multiple strings");

		check_inverse(optarg, &invert, &optind, 0);
		parse_hex_string(argv[optind-1], stringinfo);  /* sets length */
		if (invert)
			stringinfo->invert = 1;
		*flags = 1;
		stringinfo->flags=IPT_GEN_STRING;//joel add
		break;
//+++++joel		
    case '3':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
				   "Can't specify multiple strings");

		check_inverse(optarg, &invert, &optind, 0);
		parse_string(argv[optind-1], stringinfo);
		if (invert)
			stringinfo->invert = 1;
		stringinfo->len=strlen((char *)&stringinfo->string);
		stringinfo->flags=IPT_URL_STRING;
		*flags = 1;
		break;
	case '4':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
				   "Can't specify multiple strings");

		check_inverse(optarg, &invert, &optind, 0);
		parse_dns_string(argv[optind-1], stringinfo);
		if (invert)
			stringinfo->invert = 1;
		stringinfo->len=strlen((char *)&stringinfo->string);
		stringinfo->flags=IPT_GEN_STRING;
		*flags = 1;
		break;
	case '5':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
				   "Can't specify multiple strings");

		check_inverse(optarg, &invert, &optind, 0);
		if (invert)
			stringinfo->invert = 1;
		stringinfo->len=0;
		stringinfo->flags=IPT_HTTP_REQ;
		*flags = 1;
		break;
		
//---- joel		
	default:
		return 0;
	}
	return 1;
}


/* Final check; must have specified --string. */
static void
final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
			   "STRING match: You must specify `--string'");
}


/* Prints out the matchinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_match *match,
      int numeric)
{
	char *str[]={"string","url"};//joel
	const struct ipt_string_info *info =
	    (const struct ipt_string_info*) match->data;

	printf("STRING match %s %s%s ", str[info->flags],(info->invert) ? "!" : "", info->string);//joel
}


/* Saves the union ipt_matchinfo in parseable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{
	const struct ipt_string_info *info =
	    (const struct ipt_string_info*) match->data;

	printf("--string %s%s ", (info->invert) ? "! ": "", info->string);
}


static struct iptables_match string = {
    .name          = "string",
    .version       = IPTABLES_VERSION,
    .size          = IPT_ALIGN(sizeof(struct ipt_string_info)),
    .userspacesize = IPT_ALIGN(sizeof(struct ipt_string_info)),
    .help          = &help,
    .init          = &init,
    .parse         = &parse,
    .final_check   = &final_check,
    .print         = &print,
    .save          = &save,
    .extra_opts    = opts
};


void _init(void)
{
	register_match(&string);
}
