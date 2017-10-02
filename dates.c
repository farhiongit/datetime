
/** @file dates.c
 */
#define _GNU_SOURCE
#define _POSIX_SOURCE

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "dates.h"

/// Returns the name of UTC timezone.
/// @returns The name of UTC timezone
static const char *
tm_utctimezone (void)
{
  static const char *UTC_TZ = 0;

  if (!UTC_TZ)
  {
    time_t now = 0;

    errno = 0;
    if (time (&now) != (time_t) - 1 || !errno)
    {
      struct tm result;

      // gmtime_r converts to "GMT" timezone
      UTC_TZ = strdup (gmtime_r (&now, &result)->tm_zone);
    }
  }
  return UTC_TZ;
}

/// Initializes instant in time from local date and time data.
/// @param [in,out] tm Pointer to broken-down time structure
/// @returns Absolute calendar time
/// @remark Calls mktime. The tm_normalizetolocal() function is equivalent to the POSIX standard function mktime()
static time_t
tm_normalizetolocal (struct tm *tm)
{
  /* (mktime man page)
     The mktime() function converts a broken-down time structure, expressed as local time, to calendar time representation.
     The function ignores the values supplied by the caller in the tm_wday, tm_yday and tm_gmtoff fields.
     If structure members are outside their valid interval,  they will be normalized  (so that,  for example, 40 October is
     changed into 9 November).

     The value  specified in the tm_isdst field  informs mktime() whether or not daylight saving time (DST) is in effect for the time supplied in
     the tm structure:
     - a positive value means DST is in effect;
     - zero means that DST is not in effect;
     - and a negative value means that mktime() should (use timezone information and system databases to) attempt  to determine whether DST is in
     effect at the specified time.

     The mktime() function modifies the fields of the tm structure as follows: tm_wday and tm_yday are set to values determined from the contents
     of the other fields; tm_gmtoff is set according to the current timezone of the system (environment variable TZ).
     If structure members are outside their valid interval, they will be normalized (so that, for example, 40 October is changed into 9 November).
     tm_isdst  is set (regardless of its initial value) to a positive value or to 0, respectively, to indicate whether DST is or is not in effect
     at the specified time.

     Calling mktime() also sets the external variable tzname with information about the current timezone.
   */
  return mktime (tm);           /* May apply daylight saving if tm_isdst is not negative before function call */
}

/// Initializes instant in time from UTC date and time data.
/// @param [in,out] tm Pointer to broken-down time structure
/// @returns Absolute calendar time
/// @remark Portable version of timegm(): set the TZ environment variable to UTC, call mktime and restore the value of TZ.
/// @see man mktime and timegm
static time_t
tm_normalizetoutc (struct tm *tm)
{
  char *tz = getenv ("TZ");

  // getenv() returns a pointer to a string within the environment list which should not be modified.
  // The string pointed to by the return value of getenv() may be statically allocated.

  if (tz)
    tz = strdup (tz);

  /* If the TZ variable does appear in the environment, and its value is empty, then Coordinated Universal Time (UTC) is used.
   * But setenv to empty timezone ("") can not be used here because, in this case, mktime () would convert to "UTC" timezone.
   * Behavior is therefore not consistent with gmtime() which converts to "GMT" timezone */
  if (setenv ("TZ", tm_utctimezone (), 1) != 0)
    return (time_t) - 1;

  tzset ();

  time_t ret = mktime (tm);

  if (tz)
    setenv ("TZ", tz, 1);
  else
    unsetenv ("TZ");

  tzset ();

  return ret;
}

/*****************************************************
*   CONSTRUCTORS                                     *
*****************************************************/
static time_t tm_normalize (struct tm *date);

tm_status
tm_makenow (struct tm *tm)
{
  time_t now;

  errno = 0;
  if (time (&now) == (time_t) - 1 && errno)
    return TM_ERROR;

  tzset ();
  localtime_r (&now, tm);
  return TM_OK;
}

tm_status
tm_maketoday (struct tm * tm)
{
  if (tm_makenow (tm) == TM_ERROR)
    return TM_ERROR;
  else
    return tm_trimtime (tm);
}

tm_status
tm_makelocal (struct tm * tm, int year, tm_month month, int day, int hour, int min, int sec)
{
  tm->tm_year = year - 1900;
  tm->tm_mon = month - 1;
  tm->tm_mday = day;
  tm->tm_hour = hour;
  tm->tm_min = min;
  tm->tm_sec = sec;
  tm->tm_isdst = -1;            // Let timezone information and system databases define DST flag.

  errno = 0;
  time_t ret = tm_normalizetolocal (tm);        // Normalize

  if (tm->tm_year == year - 1900 && tm->tm_mon == month - 1 && tm->tm_mday == day && tm->tm_hour == hour
      && tm->tm_min == min && tm->tm_sec == sec)
    return ret != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
  else
    return TM_ERROR;
}

/// Initialize instant in time with absolute calendar time.
/// @param [in] timep Absolute calendar time
/// @param [out] tm Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Default representation is set to local.
static tm_status
tm_makelocalfromcalendartime (time_t timep, struct tm *tm)
{
  // data type time_t represents calendar time. which is the number of seconds elapsed since 1970-01-01 00:00:00 UTC.
  tzset ();
  return localtime_r (&timep, tm) ? TM_OK : TM_ERROR;
}

/// Initializes instant in time with absolute calendar time.
/// @param [in] timep Absolute calendar time
/// @param [out] tm Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Default representation is set to UTC.
static tm_status
tm_makeutcfromcalendartime (time_t timep, struct tm *tm)
{
  // data type time_t represents calendar time. which is the number of seconds elapsed since 1970-01-01 00:00:00 UTC.
  return gmtime_r (&timep, tm) ? TM_OK : TM_ERROR;
}

tm_status
tm_makeutc (struct tm * tm, int year, tm_month month, int day, int hour, int min, int sec)
{
  struct tm date;

  if (!tm)
    tm = &date;

  tm->tm_year = year - 1900;
  tm->tm_mon = month - 1;
  tm->tm_mday = day;
  tm->tm_hour = hour;
  tm->tm_min = min;
  tm->tm_sec = sec;

  errno = 0;
  time_t ret = tm_normalizetoutc (tm);  /* Normalize, ignoring date timezone and saving daylight */

  if (tm->tm_year == year - 1900 && tm->tm_mon == month - 1 && tm->tm_mday == day && tm->tm_hour == hour
      && tm->tm_min == min && tm->tm_sec == sec)
    return ret != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
  else
    return TM_ERROR;
}

tm_status
tm_setdatefromstring (struct tm * tm, const char *buf)
{
  char *ret;

  ret = strptime (buf, "%x", tm);
  if (!ret || *ret)
    ret = strptime (buf, "%Ex", tm);
  if (!ret || *ret)
    ret = strptime (buf, "%Y-%m-%d", tm);
  if (!ret || *ret)
    return TM_ERROR;

  int year = tm->tm_year + 1900;        /* tm_year is the number of years since 1900. */

  if (year >= 0 && year < 100)
  {
    time_t now;

    time (&now);
    int current_year = localtime (&now)->tm_year + 1900;        /* tm_year is the number of years since 1900. */

    tm->tm_year += lround ((current_year - year) / 100.) * 100;
  }

  tm->tm_isdst = -1;

  struct tm copy = *tm;

  errno = 0;
  time_t retval = tm_normalize (tm);

  if (tm->tm_year == copy.tm_year && tm->tm_mon == copy.tm_mon && tm->tm_mday == copy.tm_mday
      && tm->tm_hour == copy.tm_hour && tm->tm_min == copy.tm_min && tm->tm_sec == copy.tm_sec)
    return retval != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
  else
    return TM_ERROR;
}

tm_status
tm_settimefromstring (struct tm * tm, const char *buf)
{
  char *ret;

  tm->tm_hour = tm->tm_min = tm->tm_sec = 0;
  ret = strptime (buf, "%X", tm);
  if (!ret || *ret)
    ret = strptime (buf, "%EX", tm);
  if (!ret || *ret)
    ret = strptime (buf, "%T", tm);
  if (!ret || *ret)
    ret = strptime (buf, "%R", tm);
  if (!ret || *ret)
    return TM_ERROR;

  tm->tm_isdst = -1;

  struct tm copy = *tm;

  errno = 0;
  time_t retval = tm_normalize (tm);

  if (tm->tm_year == copy.tm_year && tm->tm_mon == copy.tm_mon && tm->tm_mday == copy.tm_mday
      && tm->tm_hour == copy.tm_hour && tm->tm_min == copy.tm_min && tm->tm_sec == copy.tm_sec)
    return retval != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
  else
    return TM_ERROR;
}

tm_status
tm_gettimeintostring (struct tm dt, char *str, size_t max)
{
  return strftime (str, max, "%X", &dt) ? TM_OK : TM_ERROR;
}

tm_status
tm_getdateintostring (struct tm dt, char *str, size_t max)
{
  return strftime (str, max, "%x", &dt) ? TM_OK : TM_ERROR;
}

/*****************************************************
*   CONVERTERS                                       *
*****************************************************/
tm_status
tm_toutcrepresentation (struct tm * date)
{
  if (tm_islocalrepresentation (*date))
  {
    errno = 0;
    time_t local = tm_normalizetolocal (date);

    if (local != (time_t) - 1 || !errno)
      return tm_makeutcfromcalendartime (local, date);
    else
      return TM_ERROR;
  }
  else
    return TM_OK;
}

tm_status
tm_tolocalrepresentation (struct tm * date)
{
  if (tm_isutcrepresentation (*date))
  {
    errno = 0;
    time_t utc = tm_normalizetoutc (date);

    if (utc != (time_t) - 1 || !errno)
      return tm_makelocalfromcalendartime (utc, date);
    else
      return TM_ERROR;
  }
  else
    return TM_OK;
}

/// Normalizes instant in time.
/// @param [in,out] date Pointer to broken-down time structure
/// @returns Absolute calendar time
static time_t
tm_normalize (struct tm *date)
{
  if (tm_isutcrepresentation (*date))
    return tm_normalizetoutc (date);
  else
    return tm_normalizetolocal (date);
}

/*****************************************************
*   CALENDAR PROPERTIES                              *
*****************************************************/

int
tm_isleapyear (int year)
{
  // Nonzero if year is a leap year (every 4 years, except every 100th isn't, and every 400th is).
  return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int
tm_getweeksinisoyear (int isoyear)
{
  struct tm date;

  if (tm_makelocal (&date, isoyear + 1, TM_MONTH_JANUARY, 4, 0, 0, 0) == TM_ERROR || tm_adddays (&date, -7) == TM_ERROR)
    return -1;

  return tm_getisoweek (date);
}

int
tm_getdaysinmonth (int year, tm_month month)
{
  int ret = 1;

  struct tm date;

  if (tm_makelocal (&date, year, month, 1, 0, 0, 0) == TM_ERROR)
    return -1;

  ret -= tm_getdayofyear (date);
  tm_addmonths (&date, 1);
  tm_adddays (&date, -1);
  ret += tm_getdayofyear (date);

  return ret;
}

int
tm_getsecondsinlocalday (int year, tm_month month, int day)
{
  long int ret = 0;

  struct tm date;

  if (tm_makelocal (&date, year, month, day, 0, 0, 0) == TM_ERROR)
    return -1;

  ret = -tm_normalize (&date);
  tm_adddays (&date, 1);
  ret += tm_normalize (&date);

  return (int) ret;
}

int
tm_getfirstweekdayinmonth (int year, tm_month month, tm_dayofweek dow)
{
  struct tm date;

  if (tm_makelocal (&date, year, month, 1, 0, 0, 0) == TM_ERROR)
    return -1;

  return (dow - tm_getdayofweek (date) + 7) % 7 + 1;
}

int
tm_getlastweekdayinmonth (int year, tm_month month, tm_dayofweek dow)
{
  struct tm date;

  int last = tm_getdaysinmonth (year, month);

  if (tm_makelocal (&date, year, month, last, 0, 0, 0) == TM_ERROR)
    return -1;

  int diff = dow - tm_getdayofweek (date);

  return last + diff + (diff > 0 ? -7 : 0);
}

int
tm_getfirstweekdayinisoyear (int isoyear, tm_dayofweek dow)
{
  struct tm date;

  int ret = tm_getfirstweekdayinmonth (isoyear, TM_MONTH_JANUARY, dow) + 7;

  if (tm_makelocal (&date, isoyear, TM_MONTH_JANUARY, ret, 0, 0, 0) == TM_ERROR)
    return -1;

  return ret + 7 - 7 * tm_getisoweek (date);
}

/*****************************************************
*   DATE PROPERTIES                                  *
*****************************************************/

tm_dayofweek
tm_getdayofweek (struct tm date)
{
  return (date.tm_wday + 6) % 7 + 1;    /* Monday = 1, Sunday = 7 */
}

tm_month
tm_getmonth (struct tm date)
{
  return date.tm_mon + 1;       /* January = 1, December = 12 */
}

int
tm_getyear (struct tm date)
{
  return date.tm_year + 1900;
}

int
tm_getday (struct tm date)
{
  return date.tm_mday;
}

int
tm_gethour (struct tm date)
{
  return date.tm_hour;
}

int
tm_getminute (struct tm date)
{
  return date.tm_min;
}

int
tm_getsecond (struct tm date)
{
  return date.tm_sec;
}

int
tm_getdayofyear (struct tm date)
{
  return date.tm_yday + 1;      /* 1/1 = 1, 31/12 = 365 or 366 */
}

int
tm_getisoweek (struct tm date)
{

  /** ISO 8601 week date: The first week of a year (starting on Monday) is :
     - the first week that contains at least 4 days of calendar year.
     - the week that contains the first Thursday of a year.
     - the week with January 4 in it
   */

  int week = (date.tm_yday - (date.tm_wday + 6) % 7 + 10) / 7;

  if (week == 0)
    return (date.tm_yday + 365 + tm_isleapyear (date.tm_year + 1900 - 1) - (date.tm_wday + 6) % 7 + 10) / 7;
  else if (week > 52
           && ((date.tm_yday - 365 - tm_isleapyear (date.tm_year + 1900) - (date.tm_wday + 6) % 7 + 10) / 7) > 0)
    return (date.tm_yday - 365 - tm_isleapyear (date.tm_year + 1900) - (date.tm_wday + 6) % 7 + 10) / 7;
  else
    return week;
}

int
tm_getisoyear (struct tm date)
{
  /* Year of which ISO week of date belongs to. */
  int week = (date.tm_yday - (date.tm_wday + 6) % 7 + 10) / 7;

  if (week == 0)
    return date.tm_year + 1900 - 1;
  else if (week > 52
           && ((date.tm_yday - 365 - tm_isleapyear (date.tm_year + 1900) - (date.tm_wday + 6) % 7 + 10) / 7) > 0)
    return date.tm_year + 1900 + 1;
  else
    return date.tm_year + 1900;
}

int
tm_isutcrepresentation (struct tm date)
{
  return (tm_islocalrepresentation (date) ? 0 : 1);
}

int
tm_islocalrepresentation (struct tm date)
{
  return (date.tm_zone && strcmp (tm_utctimezone (), date.tm_zone) ? 1 : 0);
}

tm_representation
tm_getrepresentation (struct tm date)
{
  return tm_islocalrepresentation (date) ? TM_REP_LOCAL : TM_REP_UTC;
}

int
tm_hasdaylightsavingtimerules (void)
{
  tzset ();
  return daylight;
}

int
tm_isdaylightsavingtime (struct tm date)
{
  return date.tm_isdst;
}

int
tm_isdaylightsavingextrasummertime (struct tm date)
{
  if (!date.tm_isdst)
    return 0;

  date.tm_isdst = 0;
  tm_normalize (&date);

  // Returns 1 during the hour before DST loses effect (from summer to winter time switch), 0 otherwise.
  return (date.tm_isdst ? 0 : 1);
}

int
tm_isdaylightsavingextrawintertime (struct tm date)
{
  if (date.tm_isdst)
    return 0;

  date.tm_isdst = 1;
  tm_normalize (&date);

  // Returns 1 during the hour after DST loses effect (from summer to winter time switch), 0 otherwise.
  return (date.tm_isdst ? 1 : 0);
}

tm_status
tm_todaylightsavingextrawintertime (struct tm *date)
{
  if (!tm_isdaylightsavingextrasummertime (*date))
    return TM_ERROR;

  date->tm_isdst = 0;
  tm_normalize (date);

  // Returns tm_OK during the hour before DST loses effect (from summer to winter time switch), TM_ERROR otherwise.
  return (date->tm_isdst ? TM_ERROR : TM_OK);
}

tm_status
tm_todaylightsavingextrasummertime (struct tm *date)
{
  if (!tm_isdaylightsavingextrawintertime (*date))
    return TM_ERROR;

  date->tm_isdst = 1;
  tm_normalize (date);

  // Returns tm_OK during the hour after DST loses effect (from summer to winter time switch), TM_ERROR otherwise.
  return (date->tm_isdst ? TM_OK : TM_ERROR);
}

int
tm_getutcoffset (struct tm date)
{
  return date.tm_gmtoff;
}

const char *
tm_gettimezone (struct tm date)
{
  // Statically allocated.
  return date.tm_zone;
}

int
tm_getsecondsofday (struct tm date)
{
  struct tm tmp = date;

  tm_set (&tmp, tm_getyear (date), tm_getmonth (date), tm_getday (date), 0, 0, 0);

  return tm_diffseconds (tmp, date);
}

tm_status
tm_set (struct tm * tm, int year, tm_month month, int day, int hour, int min, int sec)
{
  if (tm_islocalrepresentation (*tm))
    return tm_makelocal (tm, year, month, day, hour, min, sec);
  else
    return tm_makeutc (tm, year, month, day, hour, min, sec);
}

/*****************************************************
*   OPERATORS                                        *
*****************************************************/

tm_status
tm_addseconds (struct tm * date, long int nbSecs)
{
  date->tm_sec += nbSecs;
  // Don't modify date->tm_isdst so that absolute number of seconds is added.

  errno = 0;
  return tm_normalize (date) != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
}

tm_status
tm_adddays (struct tm * date, int nbDays)
{
  date->tm_mday += nbDays;
  date->tm_isdst = -1;          // Let timezone information and system databases define DST flag.

  errno = 0;
  return tm_normalize (date) != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
}

tm_status
tm_addmonths (struct tm * date, int nbMonths)
{
  int mday = date->tm_mday;

  date->tm_mon += nbMonths;
  date->tm_isdst = -1;          // Let timezone information and system databases define DST flag.

  errno = 0;
  tm_status ret = tm_normalize (date) != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;

  if (ret == TM_OK && date->tm_mday != mday)
  {
    date->tm_mday = 0;
    ret = tm_normalize (date) != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
  }

  return ret;
}

tm_status
tm_addyears (struct tm * date, int nbYears)
{
  return tm_addmonths (date, 12 * nbYears);
}

tm_status
tm_trimtime (struct tm * tm)
{
  tm->tm_sec = tm->tm_min = tm->tm_hour = 0;
  tm->tm_isdst = -1;            // Let timezone information and system databases define DST flag.

  errno = 0;
  return tm_normalize (tm) != (time_t) - 1 || !errno ? TM_OK : TM_ERROR;
}

/*****************************************************
*   COMPARATORS                                      *
*****************************************************/
int
tm_equals (struct tm a, struct tm b)
{
  return (a.tm_sec == b.tm_sec && a.tm_min == b.tm_min && a.tm_hour == b.tm_hour &&
          a.tm_mday == b.tm_mday && a.tm_mon == b.tm_mon && a.tm_year == b.tm_year && a.tm_gmtoff == b.tm_gmtoff
          && !strcmp (a.tm_zone, b.tm_zone));
}

long int
tm_diffseconds (struct tm debut, struct tm fin)
{
  return tm_normalize (&fin) - tm_normalize (&debut);
}

int
tm_compare (const void *pdebut, const void *pfin)
{
  long int diff = -tm_diffseconds (*(struct tm *) pdebut, *(struct tm *) pfin);

  return diff < 0 ? -1 : (diff > 0 ? 1 : 0);
}

int
tm_diffcalendardays (struct tm debut, struct tm fin)
{
  if (tm_getrepresentation (debut) != tm_getrepresentation (fin))
  {
    errno = EINVAL;
    return 0;
  }

  int coeff = 1;
  int ret = 0;

  if (fin.tm_year < debut.tm_year)
  {
    struct tm tmp = debut;

    debut = fin;
    fin = tmp;
    coeff = -1;
  }
  ret = fin.tm_yday - debut.tm_yday;

  debut.tm_mon = 11;
  debut.tm_mday = 31;
  for (; debut.tm_year < fin.tm_year; debut.tm_year++)
  {
    tm_normalize (&debut);
    ret += debut.tm_yday + 1;
  }

  return coeff * ret;
}

int
tm_diffdays (struct tm debut, struct tm fin, int *seconds)
{
  if (tm_getrepresentation (debut) != tm_getrepresentation (fin))
  {
    errno = EINVAL;
    return 0;
  }

  int coeff = 1;

  if (tm_diffseconds (debut, fin) < 0)
  {
    struct tm tmp = debut;

    debut = fin;
    fin = tmp;
    coeff = -1;
  }

  int ret = tm_diffcalendardays (debut, fin);

  if (fin.tm_hour < debut.tm_hour || (fin.tm_hour == debut.tm_hour && fin.tm_min < debut.tm_min) ||
      (fin.tm_hour == debut.tm_hour && fin.tm_min == debut.tm_min && fin.tm_sec < debut.tm_sec))
    if (ret > 0)
      ret--;

  if (seconds)
  {
    tm_adddays (&debut, ret);
    *seconds = coeff * tm_diffseconds (debut, fin);
  }

  return coeff * ret;
}

int
tm_diffweeks (struct tm debut, struct tm fin, int *days, int *seconds)
{
  int d = tm_diffdays (debut, fin, seconds);
  int s = seconds ? *seconds : 0;

  if (days)
    *days = d >= 0 ? d % 7 : -((-d) % 7);
  else if (seconds)
    *seconds = s;

  return d / 7;
}

int
tm_diffcalendarmonths (struct tm debut, struct tm fin)
{
  if (tm_getrepresentation (debut) != tm_getrepresentation (fin))
  {
    errno = EINVAL;
    return 0;
  }

  return 12 * (fin.tm_year - debut.tm_year) + fin.tm_mon - debut.tm_mon;
}

int
tm_diffmonths (struct tm debut, struct tm fin, int *days, int *seconds)
{
  if (tm_getrepresentation (debut) != tm_getrepresentation (fin))
  {
    errno = EINVAL;
    return 0;
  }

  int coeff = 1;

  if (tm_diffseconds (debut, fin) < 0)
  {
    struct tm tmp = debut;

    debut = fin;
    fin = tmp;
    coeff = -1;
  }

  int ret = tm_diffcalendarmonths (debut, fin);

  if (fin.tm_mday < debut.tm_mday ||
      (fin.tm_mday == debut.tm_mday && fin.tm_hour < debut.tm_hour) ||
      (fin.tm_mday == debut.tm_mday && fin.tm_hour == debut.tm_hour && fin.tm_min < debut.tm_min) ||
      (fin.tm_mday == debut.tm_mday && fin.tm_hour == debut.tm_hour && fin.tm_min == debut.tm_min
       && fin.tm_sec < debut.tm_sec))
    if (ret > 0)
      ret--;

  if (days)
  {
    tm_addmonths (&debut, ret);
    *days = coeff * tm_diffdays (debut, fin, seconds);
    if (seconds)
      *seconds *= coeff;
  }

  return coeff * ret;
}

int
tm_diffcalendaryears (struct tm debut, struct tm fin)
{
  if (tm_getrepresentation (debut) != tm_getrepresentation (fin))
  {
    errno = EINVAL;
    return 0;
  }

  return tm_getyear (fin) - tm_getyear (debut);
}

int
tm_diffyears (struct tm debut, struct tm fin, int *months, int *days, int *seconds)
{
  int m = tm_diffmonths (debut, fin, days, seconds);
  int d = days ? *days : 0;
  int s = seconds ? *seconds : 0;

  if (months)
    *months = m >= 0 ? m % 12 : -((-m) % 12);
  else
  {
    if (days)
      *days = d;
    if (seconds)
      *seconds = s;
  }
  return m / 12;
}

int
tm_diffisoyears (struct tm debut, struct tm fin)
{
  if (tm_getrepresentation (debut) != tm_getrepresentation (fin))
  {
    errno = EINVAL;
    return 0;
  }

  return tm_getisoyear (fin) - tm_getisoyear (debut);
}

void
tm_getintimezone (struct tm date, const char *tz, int *year, tm_month * month, int *day, int *hour, int *minute,
                  int *second, int *isdst)
{
  const char *oldtz = getenv ("TZ");

  // Switch to UTC
  tm_toutcrepresentation (&date);

  // Switch to local time in timezone tz
  if (tz)
    setenv ("TZ", tz, 1);
  else
    unsetenv ("TZ");
  tm_tolocalrepresentation (&date);

  if (year)
    *year = tm_getyear (date);
  if (month)
    *month = tm_getmonth (date);
  if (day)
    *day = tm_getday (date);
  if (hour)
    *hour = tm_gethour (date);
  if (minute)
    *minute = tm_getminute (date);
  if (second)
    *second = tm_getsecond (date);
  if (isdst)
    *isdst = tm_isdaylightsavingtime (date);

  // Back to local timezone
  if (oldtz)
    setenv ("TZ", oldtz, 1);
  else
    unsetenv ("TZ");
}

time_t
tm_tobinary (struct tm date)
{
  return tm_normalize (&date);
}

tm_status
tm_frombinary (struct tm * date, time_t binary)
{
  return tm_makelocalfromcalendartime (binary, date);
}
