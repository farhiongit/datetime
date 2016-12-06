#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>

#include "dates.h"

/****************************************************/

static void
tm_print (struct tm date)
{
  char buf[1000];

  strftime (buf, sizeof (buf), "*%x %X\n" "*%c\n" "*%G-W%V-%u\n" "*%Y-D%j", &date);

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
          "%s----\n",
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
          tm_getyear (date), tm_getdayofyear (date), tm_getsecondsofday (date), buf, asctime (&date));
}

static void
test_date (void)
{
  char buf[255];

  fscanf (stdin, "%254s", buf);

  struct tm tm;

  tm_makenow (&tm);

  if (tm_setdatefromstring (&tm, buf) == TM_OK)
    tm_print (tm);
  else
    puts ("Incorrect");
}

int
main (int argc, char *argv[])
{
  setlocale (LC_ALL, "");

  time_t tnow;

  time (&tnow);

  tzset ();
  struct tm now;

  localtime_r (&tnow, &now);

  char s_now[200];

  strftime (s_now, sizeof (s_now), "%x %X", &now);
  puts (s_now);

  struct tm aDate, bDate;

  tm_makelocal (&aDate, 2002, 10, 27, 1, 30, 0);
  tm_print (aDate);
  tm_addseconds (&aDate, 3600);
  tm_print (aDate);
  tm_addseconds (&aDate, 3600);
  tm_print (aDate);
  tm_addseconds (&aDate, 3600);
  tm_print (aDate);
  tm_addseconds (&aDate, -3600);
  tm_print (aDate);
  tm_addseconds (&aDate, -3600);
  tm_print (aDate);
  tm_addseconds (&aDate, -3600);
  tm_print (aDate);

  tm_makelocal (&aDate, 2010, 6, 30, 10, 0, 0);
  tm_print (aDate);
  tm_adddays (&aDate, 0);
  tm_print (aDate);
  tm_adddays (&aDate, 180);
  tm_print (aDate);
  tm_adddays (&aDate, 0);
  tm_print (aDate);
  tm_adddays (&aDate, 200);
  tm_print (aDate);
  tm_adddays (&aDate, 0);
  tm_print (aDate);

  tm_makeutc (&aDate, 2010, 8, 21, 10, 0, 0);
  tm_print (aDate);
  tm_adddays (&aDate, 0);
  tm_print (aDate);
  tm_adddays (&aDate, 180);
  tm_print (aDate);
  tm_adddays (&aDate, 0);
  tm_print (aDate);
  tm_adddays (&aDate, 200);
  tm_print (aDate);
  tm_adddays (&aDate, 0);
  tm_print (aDate);

  tm_tolocal (&aDate);
  tm_print (aDate);
  tm_addmonths (&aDate, 0);
  tm_print (aDate);
  tm_addmonths (&aDate, 6);
  tm_print (aDate);
  tm_addmonths (&aDate, 0);
  tm_print (aDate);
  tm_addmonths (&aDate, 6);
  tm_print (aDate);
  tm_addmonths (&aDate, 0);
  tm_print (aDate);

  tm_toutc (&aDate);
  tm_print (aDate);
  tm_addmonths (&aDate, 0);
  tm_print (aDate);
  tm_addmonths (&aDate, 6);
  tm_print (aDate);
  tm_addmonths (&aDate, 0);
  tm_print (aDate);
  tm_addmonths (&aDate, 6);
  tm_print (aDate);
  tm_addmonths (&aDate, 0);
  tm_print (aDate);

  tm_tolocal (&aDate);

  bDate = aDate;
  tm_addmonths (&bDate, 6);
  tm_adddays (&bDate, 27);
  tm_addseconds (&bDate, 3600 * 18);
  tm_print (aDate);
  tm_print (bDate);
  printf ("%+i jours\n", tm_diffdays (aDate, bDate));
  printf ("%+i jours\n", tm_diffdays (bDate, aDate));
  printf ("%+i jours\n", tm_difffulldays (aDate, bDate));
  printf ("%+i jours\n", tm_difffulldays (bDate, aDate));
  printf ("%+i semaines\n", tm_difffullweeks (aDate, bDate));
  printf ("%+i semaines\n", tm_difffullweeks (bDate, aDate));
  printf ("%+i mois\n", tm_diffmonths (aDate, bDate));
  printf ("%+i mois\n", tm_diffmonths (bDate, aDate));
  printf ("%+i mois\n", tm_difffullmonths (aDate, bDate));
  printf ("%+i mois\n", tm_difffullmonths (bDate, aDate));
  printf ("%+li secondes\n", tm_diffseconds (aDate, bDate));
  printf ("%+li secondes\n", tm_diffseconds (bDate, aDate));

  tm_addmonths (&aDate, 6);
  tm_addmonths (&bDate, 6);
  tm_adddays (&bDate, 32);
  tm_addseconds (&bDate, 3600 * 18);
  tm_print (aDate);
  tm_print (bDate);
  printf ("%+i jours\n", tm_diffdays (aDate, bDate));
  printf ("%+i jours\n", tm_diffdays (bDate, aDate));
  printf ("%+i jours\n", tm_difffulldays (aDate, bDate));
  printf ("%+i jours\n", tm_difffulldays (bDate, aDate));
  printf ("%+i semaines\n", tm_difffullweeks (aDate, bDate));
  printf ("%+i semaines\n", tm_difffullweeks (bDate, aDate));
  printf ("%+i mois\n", tm_diffmonths (aDate, bDate));
  printf ("%+i mois\n", tm_diffmonths (bDate, aDate));
  printf ("%+i mois\n", tm_difffullmonths (aDate, bDate));
  printf ("%+i mois\n", tm_difffullmonths (bDate, aDate));
  printf ("%+li secondes\n", tm_diffseconds (aDate, bDate));
  printf ("%+li secondes\n", tm_diffseconds (bDate, aDate));

  tm_makelocal (&aDate, 2003, 12, 28, 10, 0, 0);
  tm_print (aDate);
  tm_makelocal (&aDate, 2003, 12, 29, 10, 0, 0);
  tm_print (aDate);
  tm_makelocal (&aDate, 2005, 1, 2, 10, 0, 0);
  tm_print (aDate);
  tm_makelocal (&aDate, 2005, 1, 3, 10, 0, 0);
  tm_print (aDate);

  tm_makelocal (&aDate, 2003, 12, 28, 12, 0, 0);
  tm_toutc (&aDate);
  tm_print (aDate);
  tm_toutc (&aDate);
  tm_print (aDate);
  tm_tolocal (&aDate);
  tm_print (aDate);
  tm_tolocal (&aDate);
  tm_print (aDate);
  tm_makelocal (&aDate, 2003, 6, 28, 12, 0, 0);
  tm_toutc (&aDate);
  tm_print (aDate);
  tm_toutc (&aDate);
  tm_print (aDate);
  tm_tolocal (&aDate);
  tm_print (aDate);
  tm_tolocal (&aDate);
  tm_print (aDate);

  if (tm_setdatefromstring (&aDate, "2002-02-01") == TM_OK)
    tm_print (aDate);
  if (tm_setdatefromstring (&aDate, "3/4/3") == TM_OK)
    tm_print (aDate);
  if (tm_setdatefromstring (&aDate, "4/5/04") == TM_OK)
    tm_print (aDate);
  if (tm_settimefromstring (&aDate, "13:02:45") == TM_OK)
    tm_print (aDate);

  tm_toutc (&aDate);

  if (tm_setdatefromstring (&aDate, "05/06/05") == TM_OK)
    tm_print (aDate);
  if (tm_settimefromstring (&aDate, "11:32:05") == TM_OK)
    tm_print (aDate);
  if (tm_settimefromstring (&aDate, "05/06/2006") == TM_OK)     // Invalid input
    tm_print (aDate);
  if (tm_settimefromstring (&aDate, "11:35") == TM_OK)
    tm_print (aDate);

  tm_maketoday (&aDate);
  tm_print (aDate);
  tm_makenow (&aDate);
  tm_print (aDate);
  tm_tolocal (&aDate);
  tm_print (aDate);
  tm_toutc (&aDate);
  tm_print (aDate);
  tm_tolocal (&aDate);

  while (1)
    test_date ();

  return 0;
}
