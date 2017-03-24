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

  strftime (buf, sizeof (buf), "* %x %X\n" "* %c\n" "* %G-W%V-%u\n" "* %Y-D%j", &date);

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
  ck_assert (tm_islocalrepresentation (maintenant));
  ck_assert (tm_getrepresentation (maintenant) == TM_LOCAL);
  tm_toutcrepresentation (&maintenant);
  tm_print (maintenant);
  ck_assert (tm_isutcrepresentation (maintenant));
  ck_assert (tm_getutcoffset (maintenant) == 0);
  ck_assert (tm_getrepresentation (maintenant) == TM_UTC);
}

END_TEST
START_TEST (tu_today)
{
  struct tm aujourdhui;

  tm_maketoday (&aujourdhui);
  tm_print (aujourdhui);
  ck_assert (tm_islocalrepresentation (aujourdhui));
  ck_assert (tm_getrepresentation (aujourdhui) == TM_LOCAL);
  ck_assert (tm_gethours (aujourdhui) == 0);
  ck_assert (tm_getminutes (aujourdhui) == 0);
  ck_assert (tm_getseconds (aujourdhui) == 0);
  tm_toutcrepresentation (&aujourdhui);
  tm_print (aujourdhui);
  ck_assert (tm_isutcrepresentation (aujourdhui));
  ck_assert (tm_getutcoffset (aujourdhui) == 0);
  ck_assert (tm_getrepresentation (aujourdhui) == TM_UTC);
}

END_TEST
START_TEST (tu_today_utc)
{
  struct tm aujourdhui;

  tm_makenow (&aujourdhui);
  tm_toutcrepresentation (&aujourdhui);
  tm_trimtime (&aujourdhui);
  tm_print (aujourdhui);
  ck_assert (tm_isutcrepresentation (aujourdhui));
  ck_assert (tm_getutcoffset (aujourdhui) == 0);
  ck_assert (tm_getrepresentation (aujourdhui) == TM_UTC);
  ck_assert (tm_gethours (aujourdhui) == 0);
  ck_assert (tm_getminutes (aujourdhui) == 0);
  ck_assert (tm_getseconds (aujourdhui) == 0);
  tm_tolocalrepresentation (&aujourdhui);
  tm_print (aujourdhui);
  ck_assert (tm_islocalrepresentation (aujourdhui));
  ck_assert (tm_getrepresentation (aujourdhui) == TM_LOCAL);
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
  ck_assert (tm_islocalrepresentation (dt));
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
  ck_assert (tm_isutcrepresentation (dt));
  ck_assert (tm_getutcoffset (dt) == 0);
  ck_assert (tm_getrepresentation (dt) == TM_UTC);
}

END_TEST
START_TEST (tu_set_from_local)
{
  struct tm dt;

  tm_makenow (&dt);
  ck_assert (tm_islocalrepresentation (dt));
  ck_assert (tm_set (&dt, 2016, 3, 27, 2, 12, 21) == TM_ERROR);
  ck_assert (tm_islocalrepresentation (dt));
  ck_assert (tm_set (&dt, 2012, 12, 31, 23, 59, 59) != TM_ERROR);
  ck_assert (tm_islocalrepresentation (dt));

  ck_assert (tm_setdatefromstring (&dt, "23/4/1987") != TM_ERROR);
  ck_assert (tm_setdatefromstring (&dt, "23/09/1987") != TM_ERROR);
  ck_assert (tm_islocalrepresentation (dt));
  ck_assert (tm_setdatefromstring (&dt, "33/4/1987") == TM_ERROR);
  ck_assert (tm_setdatefromstring (&dt, "23/4") == TM_ERROR);
  ck_assert (tm_islocalrepresentation (dt));
  ck_assert (tm_setdatefromstring (&dt, "1999-03-17") != TM_ERROR);     // ISO format
  ck_assert (tm_setdatefromstring (&dt, "1999-02-29") == TM_ERROR);

  ck_assert (tm_settimefromstring (&dt, "23:04") != TM_ERROR);
  ck_assert (tm_settimefromstring (&dt, "25:04") == TM_ERROR);  // 25
  ck_assert (tm_islocalrepresentation (dt));
  ck_assert (tm_settimefromstring (&dt, "33/4/1987") == TM_ERROR);      // date
  ck_assert (tm_settimefromstring (&dt, "23:04:03") != TM_ERROR);
  ck_assert (tm_islocalrepresentation (dt));
}

END_TEST
START_TEST (tu_set_from_utc)
{
  struct tm dt;

  tm_toutcrepresentation (&dt);
  ck_assert (tm_isutcrepresentation (dt));
  ck_assert (tm_set (&dt, 2012, 12, 31, 23, 59, 59) != TM_ERROR);
  ck_assert (tm_set (&dt, 1970, 1, 1, 0, 0, 0) == TM_OK);
  ck_assert (tm_isutcrepresentation (dt));
  ck_assert (tm_getutcoffset (dt) == 0);

  ck_assert (tm_setdatefromstring (&dt, "23/4/1987") != TM_ERROR);
  ck_assert (tm_setdatefromstring (&dt, "23/09/1987") != TM_ERROR);
  ck_assert (tm_isutcrepresentation (dt));
  ck_assert (tm_setdatefromstring (&dt, "33/4/1987") == TM_ERROR);      // 33
  ck_assert (tm_setdatefromstring (&dt, "23/4") == TM_ERROR);
  ck_assert (tm_isutcrepresentation (dt));
  ck_assert (tm_getutcoffset (dt) == 0);
  ck_assert (tm_setdatefromstring (&dt, "1999-03-17") != TM_ERROR);     // ISO format
  ck_assert (tm_setdatefromstring (&dt, "1999-02-29") == TM_ERROR);

  ck_assert (tm_settimefromstring (&dt, "23:04") != TM_ERROR);
  ck_assert (tm_settimefromstring (&dt, "25:04") == TM_ERROR);  // 25
  ck_assert (tm_isutcrepresentation (dt));
  ck_assert (tm_settimefromstring (&dt, "33/4/1987") == TM_ERROR);      // date
  ck_assert (tm_settimefromstring (&dt, "23:04:03") != TM_ERROR);
  ck_assert (tm_isutcrepresentation (dt));
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

  ck_assert (tm_makelocal (&dt, 2016, TM_MARCH, 14, 9, 0, 0) != TM_ERROR);
  ck_assert (tm_adddays (&dt, 14) == TM_OK);
  ck_assert (tm_getday (dt) == 28);
  ck_assert (tm_gethours (dt) == 9);
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
  ck_assert (tm_getsecondsinlocalday (2016, 9, 27) == 24 * 3600);
  ck_assert (tm_getsecondsinlocalday (2016, 10, 30) == 25 * 3600);
  ck_assert (tm_getsecondsinlocalday (2016, 3, 27) == 23 * 3600);
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
  ck_assert (tm_getweeksinisoyear (2003) == 52);

  tm_makelocal (&aDate, 2003, 12, 29, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2004);
  ck_assert (tm_getisoweek (aDate) == 1);

  tm_makelocal (&aDate, 2005, 1, 2, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2004);
  ck_assert (tm_getisoweek (aDate) == 53);
  ck_assert (tm_getweeksinisoyear (2004) == 53);

  tm_makelocal (&aDate, 2005, 1, 3, 10, 0, 0);
  ck_assert (tm_getisoyear (aDate) == 2005);
  ck_assert (tm_getisoweek (aDate) == 1);
  ck_assert (tm_getweeksinisoyear (2005) == 52);
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

  ck_assert (tm_makelocal (&debut, 2016, TM_MARCH, 14, 9, 0, 0) == TM_OK);
  ck_assert (tm_makelocal (&fin, 2016, TM_MARCH, 28, 9, 0, 0) == TM_OK);

  ck_assert (tm_difffulldays (debut, fin) == 14);
  ck_assert (tm_diffseconds (debut, fin) == 335 * 3600);        // 335 hours instaed of 336
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

  ck_assert (tm_makeutc (&debut, 2016, TM_MARCH, 14, 9, 0, 0) == TM_OK);
  ck_assert (tm_makeutc (&fin, 2016, TM_MARCH, 28, 9, 0, 0) == TM_OK);

  ck_assert (tm_difffulldays (debut, fin) == 14);
  ck_assert (tm_diffseconds (debut, fin) == 336 * 3600);
}

END_TEST
START_TEST (tu_equality)
{
  struct tm now, utcnow;

  tm_makenow (&now);
  utcnow = now;
  tm_toutcrepresentation (&utcnow);

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
  tm_isutcrepresentation (utc);

  long int instant = tm_tobinary (utc);

  struct tm local;

  ck_assert (tm_frombinary (&local, instant) == TM_OK);
  tm_islocalrepresentation (local);

  ck_assert (tm_diffseconds (utc, local) == 0);
  ck_assert (tm_compare (&utc, &local) == 0);
  ck_assert (tm_equals (utc, local) == 0);

  tm_tolocalrepresentation (&utc);
  tm_islocalrepresentation (utc);

  ck_assert (tm_diffseconds (utc, local) == 0);
  ck_assert (tm_compare (&utc, &local) == 0);
  ck_assert (tm_equals (utc, local) != 0);
}

END_TEST
START_TEST (tu_day_loop)
{
  struct tm hour;
  struct tm end_of_day;
  const char *result;
  char string[8];

  result =
    "00-01 ;01-02 ;03-04 ;04-05 ;05-06 ;06-07 ;07-08 ;08-09 ;09-10 ;10-11 ;11-12 ;12-13 ;13-14 ;14-15 ;15-16 ;16-17 ;17-18 ;18-19 ;19-20 ;20-21 ;21-22 ;22-23 ;23-24 ;";
  tm_makelocal (&end_of_day, 2016, TM_MARCH, 28, 0, 0, 0);
  for (tm_makelocal (&hour, 2016, TM_MARCH, 27, 0, 0, 0); tm_compare (&hour, &end_of_day) < 0;
       tm_addseconds (&hour, 3600))
  {
    sprintf (string, "%02i-%02i%s;", tm_gethours (hour), tm_gethours (hour) + 1,
             tm_isdaylightsavingextrasummertime (hour) ? "A" : tm_isdaylightsavingextrawintertime (hour) ? "B" : " ");
    ck_assert (strncmp (string, result, 7) == 0);
    result += 7;
  }
  ck_assert (*result == 0);

  result =
    "00-01 ;01-02 ;02-03 ;03-04 ;04-05 ;05-06 ;06-07 ;07-08 ;08-09 ;09-10 ;10-11 ;11-12 ;12-13 ;13-14 ;14-15 ;15-16 ;16-17 ;17-18 ;18-19 ;19-20 ;20-21 ;21-22 ;22-23 ;23-24 ;";
  tm_makelocal (&end_of_day, 2016, TM_AUGUST, 28, 0, 0, 0);
  for (tm_makelocal (&hour, 2016, TM_AUGUST, 27, 0, 0, 0); tm_compare (&hour, &end_of_day) < 0;
       tm_addseconds (&hour, 3600))
  {
    sprintf (string, "%02i-%02i%s;", tm_gethours (hour), tm_gethours (hour) + 1,
             tm_isdaylightsavingextrasummertime (hour) ? "A" : tm_isdaylightsavingextrawintertime (hour) ? "B" : " ");
    ck_assert (strncmp (string, result, 7) == 0);
    result += 7;
  }
  ck_assert (*result == 0);

  result =
    "00-01 ;01-02 ;02-03A;02-03B;03-04 ;04-05 ;05-06 ;06-07 ;07-08 ;08-09 ;09-10 ;10-11 ;11-12 ;12-13 ;13-14 ;14-15 ;15-16 ;16-17 ;17-18 ;18-19 ;19-20 ;20-21 ;21-22 ;22-23 ;23-24 ;";
  tm_makelocal (&end_of_day, 2016, TM_OCTOBER, 31, 0, 0, 0);
  for (tm_makelocal (&hour, 2016, TM_OCTOBER, 30, 0, 0, 0); tm_compare (&hour, &end_of_day) < 0;
       tm_addseconds (&hour, 3600))
  {
    sprintf (string, "%02i-%02i%s;", tm_gethours (hour), tm_gethours (hour) + 1,
             tm_isdaylightsavingextrasummertime (hour) ? "A" : tm_isdaylightsavingextrawintertime (hour) ? "B" : " ");
    ck_assert (strncmp (string, result, 7) == 0);
    result += 7;
  }
  ck_assert (*result == 0);
}

END_TEST
START_TEST (tu_beginingoftheday)
{
  struct tm date;

  tm_makelocal (&date, 2016, TM_DECEMBER, 25, 14, 58, 39);
  ck_assert (tm_trimtime (&date) != TM_ERROR);
  ck_assert (tm_getrepresentation (date) == TM_LOCAL);
  ck_assert (tm_getyear (date) == 2016);
  ck_assert (tm_getmonth (date) == TM_DECEMBER);
  ck_assert (tm_getday (date) == 25);
  ck_assert (tm_gethours (date) == 0);
  ck_assert (tm_getminutes (date) == 0);
  ck_assert (tm_getseconds (date) == 0);
  ck_assert (tm_getsecondsofday (date) == 0);

  tm_makeutc (&date, 2016, TM_DECEMBER, 25, 14, 58, 39);
  ck_assert (tm_trimtime (&date) != TM_ERROR);
  ck_assert (tm_getrepresentation (date) == TM_UTC);
  ck_assert (tm_getyear (date) == 2016);
  ck_assert (tm_getmonth (date) == TM_DECEMBER);
  ck_assert (tm_getday (date) == 25);
  ck_assert (tm_gethours (date) == 0);
  ck_assert (tm_getminutes (date) == 0);
  ck_assert (tm_getseconds (date) == 0);
  ck_assert (tm_getsecondsofday (date) == 0);
}

END_TEST
START_TEST (tu_moon_walk)
{
  struct tm moon_walk;
  int y, d, h, m, s, dst;
  tm_month M;

  const char *tz = getenv ("TZ");

  setenv ("TZ", "America/New_York", 1);

  // Moon walk in NYC: 1969-07-20 22:56:00 -04:00
  ck_assert (tm_makelocal (&moon_walk, 1969, TM_JULY, 20, 22, 56, 0) == TM_OK);
  ck_assert (tm_getutcoffset (moon_walk) == -4 * 3600);
  tm_print (moon_walk);

  tm_getintimezone (moon_walk, "Australia/Sydney", &y, &M, &d, &h, &m, &s, &dst);

  // Moon walk in SYD: 1969-07-21 12:56:00 +10:00
  ck_assert (y == 1969);
  ck_assert (M == TM_JULY);
  ck_assert (d == 21);
  ck_assert (h == 12);
  ck_assert (m == 56);
  ck_assert (s == 0);

  // Moon walk in UTC: 1969-07-21 02:56:00Z
  ck_assert (tm_makeutc (&moon_walk, 1969, TM_JULY, 21, 2, 56, 0) == TM_OK);
  ck_assert (tm_getutcoffset (moon_walk) == 0);
  tm_print (moon_walk);

  tm_getintimezone (moon_walk, "Europe/Chisinau", &y, &M, &d, &h, &m, &s, &dst);

  // Moon walk in Moldavia: 1969-07-21 05:56:00 +03:00
  ck_assert (y == 1969);
  ck_assert (M == TM_JULY);
  ck_assert (d == 21);
  ck_assert (h == 5);
  ck_assert (m == 56);
  ck_assert (s == 0);

  // Back to local timezone
  if (tz)
    setenv ("TZ", tz, 1);
  else
    unsetenv ("TZ");
}

END_TEST
START_TEST (tu_weekday)
{
  ck_assert (tm_getfirstweekdayinmonth (2017, TM_MARCH, TM_TUESDAY) == 7);
  ck_assert (tm_getfirstweekdayinmonth (2017, TM_MARCH, TM_SATURDAY) == 4);

  ck_assert (tm_getlastweekdayinmonth (2017, TM_MARCH, TM_TUESDAY) == 28);
  ck_assert (tm_getlastweekdayinmonth (2017, TM_MARCH, TM_SATURDAY) == 25);

  ck_assert (tm_getfirstweekdayinisoyear (2017, TM_TUESDAY) == 3);
  ck_assert (tm_getfirstweekdayinisoyear (2017, TM_SUNDAY) == 8);
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
  tcase_add_test (tc, tu_today_utc);
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
  tcase_add_test (tc, tu_day_loop);
  tcase_add_test (tc, tu_beginingoftheday);
  tcase_add_test (tc, tu_moon_walk);
  tcase_add_test (tc, tu_weekday);

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
