/* Alpha syslog */

#ifndef __ASYSLOG_HEADER__
#define __ASYSLOG_HEADER__

/* Option flags for Alpha Networks Log Message*/
#define ALOG_SYSACT	(24<<3)	/* system activity (192) */
#define ALOG_DEBUG	(25<<3)	/* debug information (200) */
#define ALOG_ATTACK	(26<<3)	/* attacks (208) */
#define ALOG_DROP	(27<<3)	/* dropped packet (216) */
#define ALOG_NOTICE	(28<<3)	/* notice (224) */

/* redefine LOG_NFACILITIES */
#ifdef LOG_NFACILITIES
#undef LOG_NFACILITIES
#endif
#define	LOG_NFACILITIES	29	/* current number of facilities */


#ifdef ASYSLOG_NAMES
CODE afacilitynames[] =
{
	{"sysact", ALOG_SYSACT},
	{"debug", ALOG_DEBUG},
	{"attack", ALOG_ATTACK},
	{"drop", ALOG_DROP},
	{"notice", ALOG_NOTICE},
	{ NULL, -1 }
};
#endif

#endif
