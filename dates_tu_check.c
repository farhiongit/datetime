#define _GNU_SOURCE
#define _POSIX_C_SOURCE

#include <check.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include "dates.h"

/*************** INITIALISATION *************************/

/// Done once for all tests
static void
unckecked_setup (void)
{
}

/// Done once for all tests
static void
unckecked_teardown (void)
{
  //free (UTCtimezone ());
}

/// Done once for every test
static void
ckecked_setup (void)
{
}

/// Done once for every test
static void
ckecked_teardown (void)
{
}

/****************************************************/

static void
tm_print (struct tm date)
{
  char buf[1000];

  strftime (buf, sizeof (buf), "*%x %X\n" "*%c\n" "*%G-W%V-%u\n" "*%Y-D%j", &date);

  char strd[1000];
  char strt[1000];

  tm_getdateintostring (date, strd, 1000);
  tm_gettimeintostring (date, strt, 1000);

  printf ("----\n"
          "Structure tm (%p):\n"
          " tm_year: %i\n"
          " tm_mon: %i\n"
          " tm_mday: %i\n"
          " tm_hour: %i\n"
          " tm_min: %i\n"
          " tm_sec: %i\n"
          " tm_wday: %i\n"
          " tm_yday: %i\n"
          " tm_isdst: %i\n"
          " tm_isdst_extrasummertime: %i\n"
          " tm_isdst_extrawintertime: %i\n"
          " tm_gmtoff: %+li\n"
          " tm_zone: %s (%p)\n"
          "%04i-W%02i-%i\n"
          "%04i-D%03i%+is\n"
          "%s\n"
          "%s %s\n----\n",
          (void *) &date,
          date.tm_year,
          date.tm_mon,
          date.tm_mday,
          date.tm_hour,
          date.tm_min,
          date.tm_sec,
          date.tm_wday,
          date.tm_yday,
          date.tm_isdst,
          tm_isdaylightsavingextrasummertime (date),
          tm_isdaylightsavingextrawintertime (date),
          date.tm_gmtoff,
          date.tm_zone, (void *) date.tm_zone,
          tm_getisoyear (date), tm_getisoweek (date), tm_getdayofweek (date),
          tm_getyear (date), tm_getdayofyear (date), tm_getsecondsofday (date), buf, strd, strt);
}

/************************* TESTS ***********************/
START_TEST (tu_now)
{
  struct tm maintenant;

  tm_makenow (&maintenant);
  tm_print (maintenant);
  ck_assert (tm_islocal (maintenant));
  ck_assert (tm_getrepresentation (maintenant) == TM_LOCAL);
  tm_toutc (&maintenant);
  tm_print (maintenant);
  ck_assert (tm_isutc (maintenant));
  ck_assert (tm_getutcoffset (maintenant) == 0);
  ck_assert (tm_getrepresentation (maintenant) == TM_UTC);
}

END_TEST
START_TEST (tu_today)
{
  struct tm aujourdhui;

  tm_maketoday (&aujourdhui);
  tm_print (aujourdhui);
  ck_assert (tm_islocal (aujourdhui));
  ck_assert (tm_getrepresentation (aujourdhui) == TM_LOCAL);
  ck_assert (tm_gethours (aujourdhui) == 0);
  ck_assert (tm_getminutes (aujourdhui) == 0);
  ck_assert (tm_getseconds (aujourdhui) == 0);
  tm_toutc (&aujourdhui);
  tm_print (aujourdhui);
  ck_assert (tm_isutc (aujourdhui));
  ck_assert (tm_getutcoffset (aujourdhui) == 0);
  ck_assert (tm_getrepresentation (aujourdhui) == TM_UTC);
}

END_TEST
START_TEST (tu_local)
{
  struct tm dt;

  ck_assert (tm_makelocal (&dt, 2010, 5, 30, 17, 55, 21) != TM_ERROR);
  ck_assert (tm_makelocal (&dt, 2010, 4, 31, 17, 55, 21) == TM_ERROR);  // Does not exist
  ck_assert (tm_makelocal (&dt, 2015, 2, 29, 17, 55, 21) == TM_ERROR);
  ck_assert (tm_makelocal (&dt, 2016, 2, 29, 17, 55, 21) != TM_ERROR);
  ck_assert (tm_makelocal (&dt, 2016, 2, 29, 17, 65, 21) == TM_ERROR);  // Does not exist
  ck_assert (tm_makelocal (&dt, 2016, 3, 27, 2, 12, 21) == TM_ERROR);   // Does not exist (DST change)
  ck_assert (tm_makelocal (&dt, 2016, 10, 30, 2, 22, 21) != TM_ERROR);
  ck_assert (tm_islocal (dt));
  ck_assert (tm_getrepresentation (dt) == TM_LOCAL);
}

END_TEST
START_TEST (tu_utc)
{
  struct tm dt;

  ck_assert (tm_makeutc (&dt, 1969, 12, 31, 23, 59, 59) != TM_ERROR);
  ck_assert (tm_makeutc (&dt, 2010, 5, 30, 17, 55, 21) != TM_ERROR);
  ck_assert (tm_makeutc (&dt, 2010, 4, 31, 17, 55, 21) == TM_ERROR);    // Does not exist
  ck_assert (tm_makeutc (&dt, 2015, 2, 29, 17, 55, 21) == TM_ERROR);    // Does not exist
  ck_assert (tm_makeutc (&dt, 2016, 2, 29, 17, 55, 21) != TM_ERROR);
  ck_assert (tm_makeutc (&dt, 2016, 2, 29, 17, 65, 21) == TM_ERROR);    // Does not exist
  ck_assert (tm_makeutc (&dt, 2016, 3, 27, 2, 12, 21) != TM_ERROR);
  ck_assert (tm_makeutc (&dt, 2016, 10, 30, 2, 22, 21) != TM_ERROR);
  ck_assert (tm_isutc (dt));
  ck_assert (tm_getutcoffset (dt) == 0);
  ck_assert (tm_getrepresentation (dt) == TM_UTC);
}

END_TEST
START_TEST (tu_set_from_local)
{
  struct tm dt;

  tm_makenow (&dt);
  ck_assert (tm_islocal (dt));
  ck_assert (tm_set (&dt, 2016, 3, 27, 2, 12, 21) == TM_ERROR);
  ck_assert (tm_islocal (dt));
  ck_assert (tm_set (&dt, 2012, 12, 31, 23, 59, 59) != TM_ERROR);
  ck_assert (tm_islocal (dt));

  ck_assert (tm_setdatefromstring (&dt, "23/4/1987") != TM_ERROR);
  ck_assert (tm_islocal (dt));
  ck_assert (tm_setdatefromstring (&dt, "33/4/1987") == TM_ERROR);
  ck_assert (tm_setdatefromstring (&dt, "23/4") == TM_ERROR);
  ck_assert (tm_islocal (dt));

  ck_assert (tm_settimefromstring (&dt, "23:04") != TM_ERROR);
  ck_assert (tm_settimefromstring (&dt, "25:04") == TM_ERROR);
  ck_assert (tm_islocal (dt));
  ck_assert (tm_settimefromstring (&dt, "33/4/1987") == TM_ERROR);      // date
  ck_assert (tm_settimefromstring (&dt, "23:04:03") != TM_ERROR);
  ck_assert (tm_islocal (dt));
}

END_TEST
START_TEST (tu_set_from_utc)
{
  struct tm dt;

  tm_toutc (&dt);
  ck_assert (tm_isutc (dt));
  ck_assert (tm_set (&dt, 2012, 12, 31, 23, 59, 59) != TM_ERROR);
  ck_assert (tm_set (&dt, 1970, 1, 1, 0, 0, 0) == TM_OK);
  ck_assert (tm_isutc (dt));
  ck_assert (tm_getutcoffset (dt) == 0);

  ck_assert (tm_setdatefromstring (&dt, "23/4/1987") != TM_ERROR);
  ck_assert (tm_isutc (dt));
  ck_assert (tm_setdatefromstring (&dt, "33/4/1987") == TM_ERROR);      // 33
  ck_assert (tm_setdatefromstring (&dt, "23/4") == TM_ERROR);
  ck_assert (tm_isutc (dt));
  ck_assert (tm_getutcoffset (dt) == 0);

  ck_assert (tm_settimefromstring (&dt, "23:04") != TM_ERROR);
  ck_assert (tm_settimefromstring (&dt, "25:04") == TM_ERROR);  // 25
  ck_assert (tm_isutc (dt));
  ck_assert (tm_settimefromstring (&dt, "33/4/1987") == TM_ERROR);      // date
  ck_assert (tm_settimefromstring (&dt, "23:04:03") != TM_ERROR);
  ck_assert (tm_isutc (dt));
  ck_assert (tm_getutcoffset (dt) == 0);
}

END_TEST
START_TEST (tu_tostring_local)
{
  struct tm dt;
  char tooshort[2];
  char str[100];

  ck_assert (tm_makelocal (&dt, 2012, 12, 31, 23, 59, 59) != TM_ERROR);

  ck_assert (tm_getdateintostring (dt, tooshort, sizeof (tooshort) / sizeof (*tooshort)) == TM_ERROR);

  ck_assert (tm_getdateintostring (dt, str, sizeof (str) / sizeof (*str)) == TM_OK);
  ck_assert (strcmp (str, "31/12/2012") == 0);
  ck_assert (tm_gettimeintostring (dt, str, sizeof (str) / sizeof (*str)) == TM_OK);
  ck_assert (strcmp (str, "23:59:59") == 0);
}

END_TEST
START_TEST (tu_tostring_utc)
{
  struct tm dt;
  char tooshort[2];
  char str[100];

  ck_assert (tm_makeutc (&dt, 2013, 11, 30, 22, 58, 57) != TM_ERROR);

  ck_assert (tm_getdateintostring (dt, tooshort, sizeof (tooshort) / sizeof (*tooshort)) == TM_ERROR);

  ck_assert (tm_getdateintostring (dt, str, sizeof (str) / sizeof (*str)) == TM_OK);
  ck_assert (strcmp (str, "30/11/2013") == 0);
  ck_assert (tm_gettimeintostring (dt, str, sizeof (str) / sizeof (*str)) == TM_OK);
  ck_assert (strcmp (str, "22:58:57") == 0);
}

END_TEST
START_TEST (tu_getters_local)
{
  struct tm dt;

  ck_assert (tm_makelocal (&dt, 2012, 12, 31, 23, 59, 58) != TM_ERROR);
  ck_assert (tm_getyear (dt) == 2012);
  ck_assert (tm_getmonth (dt) == TM_DECEMBER);
  ck_assert (tm_getday (dt) == 31);
  ck_assert (tm_gethours (dt) == 23);
  ck_assert (tm_getminutes (dt) == 59);
  ck_assert (tm_getseconds (dt) == 58);
  ck_assert (tm_getdayofyear (dt) == 366);
  ck_assert (tm_getdayofweek (dt) == TM_MONDAY);
}

END_TEST
START_TEST (tu_getters_utc)
{
  struct tm dt;

  ck_assert (tm_makeutc (&dt, 2013, 11, 30, 22, 58, 57) != TM_ERROR);
  ck_assert (tm_getyear (dt) == 2013);
  ck_assert (tm_getmonth (dt) == TM_NOVEMBER);
  ck_assert (tm_getday (dt) == 30);
  ck_assert (tm_gethours (dt) == 22);
  ck_assert (tm_getminutes (dt) == 58);
  ck_assert (tm_getseconds (dt) == 57);
  ck_assert (tm_getdayofyear (dt) == 334);
  ck_assert (tm_getdayofweek (dt) == TM_SATURDAY);
}

END_TEST
START_TEST (tu_ops_local)
{
  struct tm dt;

  ck_assert (tm_makelocal (&dt, 2016, 5, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addseconds (&dt, 6 * 24 * 3600) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, 5, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 6) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, 5, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 9) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_FEBRUARY);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, TM_MARCH, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addseconds (&dt, 6 * 24 * 3600) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 2);

  ck_assert (tm_makelocal (&dt, 2016, TM_MARCH, 26, 2, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 1) == TM_OK);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 3);

  ck_assert (tm_makelocal (&dt, 2016, TM_MARCH, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 6) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, TM_FEBRUARY, 27, 2, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 1) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_MARCH);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 3);

  ck_assert (tm_makelocal (&dt, 2016, TM_MARCH, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 9) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_DECEMBER);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, 10, 30, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addseconds (&dt, 6 * 24 * 3600) == TM_OK);
  ck_assert (tm_getday (dt) == 5);
  ck_assert (tm_gethours (dt) == 0);

  ck_assert (tm_makelocal (&dt, 2016, 10, 30, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 6) == TM_OK);
  ck_assert (tm_getday (dt) == 5);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, TM_OCTOBER, 30, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 6) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_APRIL);
  ck_assert (tm_getday (dt) == 30);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makelocal (&dt, 2016, TM_OCTOBER, 31, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 1) == TM_OK);
  ck_assert (tm_getday (dt) == 30);
  ck_assert (tm_addmonths (&dt, 3) == TM_OK);
  ck_assert (tm_getday (dt) == 28);
  ck_assert (tm_addmonths (&dt, -4) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_OCTOBER);
  ck_assert (tm_getday (dt) == 28);

  ck_assert (tm_makelocal (&dt, 2016, 3, 26, 2, 12, 21) != TM_ERROR);
  ck_assert (tm_getday (dt) == 26);
  ck_assert (tm_gethours (dt) == 2);
  ck_assert (tm_adddays (&dt, 1) == TM_OK);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 3);
  ck_assert (tm_adddays (&dt, -1) == TM_OK);
  ck_assert (tm_getday (dt) == 26);
  ck_assert (tm_gethours (dt) == 3);
}

END_TEST
START_TEST (tu_ops_utc)
{
  struct tm dt;

  ck_assert (tm_makeutc (&dt, 2016, 5, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addseconds (&dt, 6 * 24 * 3600) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 5, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 6) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 5, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 9) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_FEBRUARY);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 3, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addseconds (&dt, 6 * 24 * 3600) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 3, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 6) == TM_OK);
  ck_assert (tm_getday (dt) == 2);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 3, 27, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 9) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_DECEMBER);
  ck_assert (tm_getday (dt) == 27);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 10, 30, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addseconds (&dt, 6 * 24 * 3600) == TM_OK);
  ck_assert (tm_getday (dt) == 5);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 10, 30, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 6) == TM_OK);
  ck_assert (tm_getday (dt) == 5);
  ck_assert (tm_gethours (dt) == 1);

  ck_assert (tm_makeutc (&dt, 2016, 10, 30, 1, 12, 21) != TM_ERROR);
  ck_assert (tm_addmonths (&dt, 6) == TM_OK);
  ck_assert (tm_getmonth (dt) == TM_APRIL);
  ck_assert (tm_getday (dt) == 30);
  ck_assert (tm_gethours (dt) == 1);
}

END_TEST
START_TEST (tu_dst)
{
  ck_assert (tm_getnumberofsecondsinlocalday (2016, 9, 27) == 24 * 3600);
  ck_assert (tm_getnumberofsecondsinlocalday (2016, 10, 30) == 25 * 3600);
  ck_assert (tm_getnumberofsecondsinlocalday (2016, 3, 27) == 23 * 3600);
  ck_assert (tm_hasdaylightsavingtimerules ());
}

END_TEST
START_TEST (tu_dst_summer)
{
  struct tm aDate;

  tm_makelocal (&aDate, 2016, 3, 27, 1, 30, 0);
  ck_assert (!tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 1);
  int utcoffset = tm_getutcoffset (aDate);

  ck_assert (tm_getsecondsofday (aDate) == 3600 + 1800);

  tm_addseconds (&aDate, 3600);
  ck_assert (tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 3);
  ck_assert (tm_getutcoffset (aDate) == utcoffset + 3600);
  ck_assert (tm_getsecondsofday (aDate) == 2 * 3600 + 1800);
}

END_TEST
START_TEST (tu_dst_winter)
{
  struct tm aDate;

  tm_makelocal (&aDate, 2002, 10, 27, 1, 30, 0);
  ck_assert (tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 1);
  int utcoffset = tm_getutcoffset (aDate);

  ck_assert (tm_getsecondsofday (aDate) == 3600 + 1800);

  tm_addseconds (&aDate, 3600);
  ck_assert (tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 2);
  ck_assert (tm_getutcoffset (aDate) == utcoffset);
  ck_assert (tm_getsecondsofday (aDate) == 2 * 3600 + 1800);

  tm_addseconds (&aDate, 3600);
  ck_assert (!tm_isdaylightsavingtime (aDate));
  ck_assert (tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 2);
  ck_assert (tm_getutcoffset (aDate) == utcoffset - 3600);
  ck_assert (tm_getsecondsofday (aDate) == 3 * 3600 + 1800);

  tm_addseconds (&aDate, 3600);
  ck_assert (!tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 3);
  ck_assert (tm_getutcoffset (aDate) == utcoffset - 3600);
  ck_assert (tm_getsecondsofday (aDate) == 4 * 3600 + 1800);

  tm_addseconds (&aDate, -3600);
  ck_assert (!tm_isdaylightsavingtime (aDate));
  ck_assert (tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 2);
  ck_assert (tm_getutcoffset (aDate) == utcoffset - 3600);
  ck_assert (tm_getsecondsofday (aDate) == 3 * 3600 + 1800);

  tm_addseconds (&aDate, -3600);
  ck_assert (tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 2);
  ck_assert (tm_getutcoffset (aDate) == utcoffset);
  ck_assert (tm_getsecondsofday (aDate) == 2 * 3600 + 1800);

  tm_addseconds (&aDate, -3600);
  ck_assert (tm_isdaylightsavingtime (aDate));
  ck_assert (!tm_isdaylightsavingextrawintertime (aDate));
  ck_assert (!tm_isdaylightsavingextrasummertime (aDate));
  ck_assert (tm_gethours (aDate) == 1);
  ck_assert (tm_getutcoffset (aDate) == utcoffset);
  ck_assert (tm_getsecondsofday (aDate) == 3600 + 1800);
}

END_TEST
START_TEST (tu_iso)
{
  struct tm aDate;

  tm_makelocal (&aDate, 2003, 12, 28, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2003);
  ck_assert (tm_getisoweek (aDate) == 52);

  tm_makelocal (&aDate, 2003, 12, 29, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2004);
  ck_assert (tm_getisoweek (aDate) == 1);

  tm_makelocal (&aDate, 2005, 1, 2, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2004);
  ck_assert (tm_getisoweek (aDate) == 53);

  tm_makelocal (&aDate, 2005, 1, 3, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2005);
  ck_assert (tm_getisoweek (aDate) == 1);
}

END_TEST
START_TEST (tu_calendar)
{
  ck_assert (!tm_isleapyear (1900));
  ck_assert (tm_isleapyear (2000));
  ck_assert (!tm_isleapyear (2001));
  ck_assert (tm_isleapyear (2004));

  ck_assert (tm_getdaysinmonth (2000, 1) == 31);
  ck_assert (tm_getdaysinmonth (2000, 2) == 29);
  ck_assert (tm_getdaysinmonth (2000, 3) == 31);
  ck_assert (tm_getdaysinmonth (2000, 4) == 30);
  ck_assert (tm_getdaysinmonth (2000, 5) == 31);
  ck_assert (tm_getdaysinmonth (2000, 6) == 30);
  ck_assert (tm_getdaysinmonth (2000, 7) == 31);
  ck_assert (tm_getdaysinmonth (2000, 8) == 31);
  ck_assert (tm_getdaysinmonth (2000, 9) == 30);
  ck_assert (tm_getdaysinmonth (2000, 10) == 31);
  ck_assert (tm_getdaysinmonth (2000, 11) == 30);
  ck_assert (tm_getdaysinmonth (2000, 12) == 31);
}

END_TEST
START_TEST (tu_diff_local)
{
  struct tm debut, fin;

  ck_assert (tm_makelocal (&debut, 2015, 11, 28, 11, 20, 0) == TM_OK);
  ck_assert (tm_makelocal (&fin, 2016, 7, 16, 9, 20, 0) == TM_OK);

  ck_assert (tm_diffseconds (debut, fin) ==
             (30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) * 24 * 3600 - 12 * 24 * 3600 - 2 * 3600 - 3600);
  ck_assert (tm_diffdays (debut, fin) == (30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) - 12);
  ck_assert (tm_difffulldays (debut, fin) == (30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) - 12 - 1);
  ck_assert (tm_difffullweeks (debut, fin) == ((30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) - 12 - 1) / 7);
  ck_assert (tm_diffmonths (debut, fin) == 12 - 4);
  ck_assert (tm_difffullmonths (debut, fin) == 12 - 4 - 1);
  ck_assert (tm_diffyears (debut, fin) == 1);
  ck_assert (tm_difffullyears (debut, fin) == 0);
  ck_assert (tm_diffisoyears (debut, fin) == 1);
}

END_TEST
START_TEST (tu_diff_utc)
{
  struct tm debut, fin;

  ck_assert (tm_makeutc (&debut, 2015, 11, 28, 11, 20, 0) == TM_OK);
  ck_assert (tm_makeutc (&fin, 2016, 7, 16, 9, 20, 0) == TM_OK);

  ck_assert (tm_diffseconds (debut, fin) ==
             (30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) * 24 * 3600 - 12 * 24 * 3600 - 2 * 3600);
  ck_assert (tm_diffdays (debut, fin) == (30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) - 12);
  ck_assert (tm_difffulldays (debut, fin) == (30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) - 12 - 1);
  ck_assert (tm_difffullweeks (debut, fin) == ((30 + 31 + 31 + 29 + 31 + 30 + 31 + 30) - 12 - 1) / 7);
  ck_assert (tm_diffmonths (debut, fin) == 12 - 4);
  ck_assert (tm_difffullmonths (debut, fin) == 12 - 4 - 1);
  ck_assert (tm_diffyears (debut, fin) == 1);
  ck_assert (tm_difffullyears (debut, fin) == 0);
  ck_assert (tm_diffisoyears (debut, fin) == 1);
}

END_TEST
START_TEST (tu_equality)
{
  struct tm now, utcnow;

  tm_makenow (&now);
  utcnow = now;
  tm_toutc (&utcnow);

  ck_assert (tm_diffseconds (now, utcnow) == 0);
  ck_assert (tm_compare (&now, &utcnow) == 0);
  ck_assert (tm_equals (now, utcnow) == 0);
}

END_TEST
START_TEST (tu_change_timezone)
{
  // Make use of command zdump -v -c2009,2011 'Europe/Kiev' 'Australia/Adelaide' 'America/Los_Angeles' 'Europe/Paris' for informations on timezonzs

  const char *tz = getenv ("TZ");

  // Setting local time in Kiev
  setenv ("TZ", "Europe/Kiev", 1);
  struct tm ki;

  tm_makelocal (&ki, 2010, TM_MARCH, 21, 18, 0, 0);     // UTC+2h
  ck_assert (tm_isdaylightsavingtime (ki) == 0);        // DST starts on march, the 28th, at 3am

  int y, d, h, m, s, dst;
  tm_month M;

  // Time in Adelaide, 8h30 ahead of Kiev (UTC+10:30h)
  tm_getintimezone (ki, "Australia/Adelaide", &y, &M, &d, &h, &m, &s, &dst);

  ck_assert (dst != 0);         // DST ends on april, the 4th, at 2am
  ck_assert (d == 21 + (18 + 8) / 24);
  ck_assert (h == (18 + 8) % 24);
  ck_assert (m == (0 + 30) % 60);

  // Time in Los Angeles, 9h before Kiev (UTC-7h)
  tm_getintimezone (ki, "America/Los_Angeles", &y, &M, &d, &h, &m, &s, &dst);

  ck_assert (dst != 0);         // DST starts on march, the 14th, at 2am
  ck_assert (d == 21 + (18 - 9) / 24);
  ck_assert (h == (18 - 9) % 24);
  ck_assert (m == (0 + 0) % 60);

  // Time in Paris, 1h before Kiev (UTC+1h)
  tm_getintimezone (ki, "Europe/Paris", &y, &M, &d, &h, &m, &s, &dst);

  ck_assert (dst == 0);         // DST starts on march, the 28th, at 2am
  ck_assert (d == 21 + (18 - 1) / 24);
  ck_assert (h == (18 - 1) % 24);
  ck_assert (m == (0 + 0) % 60);

  // Back to local timezone
  if (tz)
    setenv ("TZ", tz, 1);
  else
    unsetenv ("TZ");
}

END_TEST
START_TEST (tu_serialization)
{
  struct tm utc;

  tm_makeutc (&utc, 2016, TM_JANUARY, 1, 18, 0, 0);
  tm_isutc (utc);

  long int instant = tm_tobinary (utc);

  struct tm local;

  ck_assert (tm_frombinary (&local, instant) == TM_OK);
  tm_islocal (local);

  ck_assert (tm_diffseconds (utc, local) == 0);
  ck_assert (tm_compare (&utc, &local) == 0);
  ck_assert (tm_equals (utc, local) == 0);

  tm_tolocal (&utc);
  tm_islocal (utc);

  ck_assert (tm_diffseconds (utc, local) == 0);
  ck_assert (tm_compare (&utc, &local) == 0);
  ck_assert (tm_equals (utc, local) != 0);
}

END_TEST

/**************** SEQUENCEMENT DES TESTS ***************/
static Suite *
mm_suite (void)
{
  setlocale (LC_ALL, "");

  Suite *s = suite_create ("Dates toolkit");

  //-----

  TCase *tc = tcase_create ("Tests");

  // Unchecked fixture : le code setup n'est exécuté qu'une seule fois avant tous les cas de tests.
  tcase_add_unchecked_fixture (tc, unckecked_setup, unckecked_teardown);
  tcase_add_checked_fixture (tc, ckecked_setup, ckecked_teardown);

  tcase_add_test (tc, tu_now);
  tcase_add_test (tc, tu_today);
  tcase_add_test (tc, tu_local);
  tcase_add_test (tc, tu_utc);
  tcase_add_test (tc, tu_set_from_local);
  tcase_add_test (tc, tu_set_from_utc);
  tcase_add_test (tc, tu_tostring_local);
  tcase_add_test (tc, tu_tostring_utc);
  tcase_add_test (tc, tu_getters_local);
  tcase_add_test (tc, tu_getters_utc);
  tcase_add_test (tc, tu_ops_local);
  tcase_add_test (tc, tu_ops_utc);
  tcase_add_test (tc, tu_diff_local);
  tcase_add_test (tc, tu_diff_utc);
  tcase_add_test (tc, tu_dst);
  tcase_add_test (tc, tu_dst_winter);
  tcase_add_test (tc, tu_dst_summer);
  tcase_add_test (tc, tu_iso);
  tcase_add_test (tc, tu_calendar);
  tcase_add_test (tc, tu_equality);
  tcase_add_test (tc, tu_change_timezone);
  tcase_add_test (tc, tu_serialization);

  suite_add_tcase (s, tc);

  //-----

  return s;
}

/**************** EXECUTION DES TESTS ******************/

int
main (void)
{
  // Required localization for designed unit tests:
  setenv ("TZ", "Europe/Paris", 1);     // Localized in Paris timezone
  setenv ("LC_ALL", "fr_FR.utf8", 1);   // French language, in France

  setlocale (LC_ALL, "");

  Suite *s = mm_suite ();
  SRunner *sr = srunner_create (s);

  srunner_run_all (sr, CK_ENV);
  int number_failed = srunner_ntests_failed (sr);

  srunner_free (sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
