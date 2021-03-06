
/** @file dates.h
 */
#ifndef TM_DATES_H
#define TM_DATES_H
#pragma once

///@page Introduction
/// This library is a tool box that facilitates the management of dates and times. It is a superset of lower level POSIX functions.
/// The functions of this toolbox manipulate instants (points) in time expressed as a date and time of day, in Gregorian calendar.
///
/// Instants are internally stored in the structure \p struct \p tm defined by POSIX, with a resolution of one second.
/// This allows compatible access to low level POSIX functions, such as strftime() or strptime() (see man page of mktime()).
///
/// However, objects \p struct \p tm should be considered as abstract data types, and should not be initializeed by hand.
/// Instants should be initialized with tm_makelocal(), tm_makeutc(), tm_makenow() and tm_maketoday() instead.
/// The use of these functions is compulsory, as well as easier than handling with \p struct \p tm.
///
/// Once initialized, tm_set(), tm_setdatefromstring(), tm_settimefromstring() can be used to modify the instant.
///
/// Instants in time can be at will represented either in UTC or local time.
/// Functions tm_toutcrepresentation() and tm_tolocalrepresentation() allow to switch from one representation to the other.
/// Functions tm_isutcrepresentation(), tm_islocalrepresentation() and tm_getrepresentation() permit to know the current representation of an instant in time.
/// These functions do not affect the instant in time but only the way it is yield. One could think of it as the unit with which the instant is expressed. 
///
/// Daylight saving time is taken into account in local time representation but is not applicable to UTC:
/// - When local representation is used, calculations take daylight saving time rules into account.
/// Days with DST change contain 23 or 25 hours when added or compared.
/// Local time is appropriate for acquisition or display in user interfaces of desktop applications.
/// - On the contrary, in UTC, daylight saving time does not apply, and all days last 24 hours.
///
/// Functions tm_isdaylightsavingtime(), tm_isdaylightsavingextrawintertime(), tm_isdaylightsavingextrasummertime()
/// indicate whether or not Daylight saving time is in effect.
/// Function tm_hasdaylightsavingtimerules() indicates whether or not daylight saving time rules apply in local timezone.
///
/// Functions for calculation are tm_add... and tm_diff....
/// Functions for comparison are tm_compare() and tm_equals().
/// Functions for persistance are tm_tobinary() and tm_frombinary().

///@page Usage
/// Usage requires including
///@verbatim
/// #define _BSD_SOURCE
/// #include <time.h>
/// #include "dates.h"
///@endverbatim

///@name Data types
///@{

///@typedef tm_status
/// Values of status
typedef enum
{
  TM_OK = EXIT_SUCCESS,         ///< Sucess
  TM_ERROR = EXIT_FAILURE,      ///< Error
} tm_status;

///@typedef tm_representation
/// Kinds of representation for instant in time.
typedef enum
{
  TM_REP_LOCAL,                 ///< Local time
  TM_REP_UTC,                   ///< UTC
} tm_representation;

///@typedef tm_dayofweek
/// Days of week
typedef enum
{
  TM_WEEKDAY_MONDAY = 1,        ///< Monday (1)
  TM_WEEKDAY_TUESDAY,           ///< Tuesday (2)
  TM_WEEKDAY_WEDNESDAY,         ///< Wednesday (3)
  TM_WEEKDAY_THURSDAY,          ///< Thursday (4)
  TM_WEEKDAY_FRIDAY,            ///< Friday (5)
  TM_WEEKDAY_SATURDAY,          ///< Saturday (6)
  TM_WEEKDAY_SUNDAY,            ///< Sunday (7)
} tm_dayofweek;

///@typedef tm_month
/// Months
typedef enum
{
  TM_MONTH_JANUARY = 1,         ///< January (1)
  TM_MONTH_FEBRUARY,            ///< February (2)
  TM_MONTH_MARCH,               ///< March (3)
  TM_MONTH_APRIL,               ///< April (4)
  TM_MONTH_MAY,                 ///< May (5)
  TM_MONTH_JUNE,                ///< June (6)
  TM_MONTH_JULY,                ///< July (7)
  TM_MONTH_AUGUST,              ///< August (8)
  TM_MONTH_SEPTEMBER,           ///< September (9)
  TM_MONTH_OCTOBER,             ///< October (10)
  TM_MONTH_NOVEMBER,            ///< November (11)
  TM_MONTH_DECEMBER,            ///< December (12)
} tm_month;

///@}

/*****************************************************
*   CONSTRUCTORS                                     *
*****************************************************/
///@name Constructors
/// Parameter \p dt should have been previously allocated, otherwise behavior is unpredictable.
///@{

/// Initializes (or reinitializes) an instant in time with current date and time.
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark The instant (point in time) is initialized in local time representation by default.
tm_status tm_makenow (struct tm *dt);

/// Initializes (or reinitializes) instant in time with current date, beginning of day, local time.
/// Initializes (or reinitializes) an instant in time set to today's date, with the time component set to 00:00:00, local time.
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark The instant (point in time) is initialized in local time representation by default.
tm_status tm_maketoday (struct tm *dt);

/// Initializes (or reinitializes) instant intime with local date and time attributes.
/// @param [in] year The year, specified as a 4-digit number (for example, 1996), interpreted as a year in the Gregorian calendar (local time)
/// @param [in] month The month (local time)
/// @param [in] day The day (1 through the number of days in month) of month (local time)
/// @param [in] hour The hours (0 through 23) (local time)
/// @param [in] min The minutes (0 through 59) (local time)
/// @param [in] sec The seconds (0 through 59) (local time)
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow or invalid arguments)
/// @remark The instant (point in time) is initialized in local time representation by default.
tm_status tm_makelocal (struct tm *dt, int year, tm_month month, int day, int hour, int min, int sec);

/// Initializes (or reinitializes) instant in time with UTC date and time attributes.
/// @param [in] year Year, UTC
/// @param [in] month Month, UTC
/// @param [in] day Day of month, UTC
/// @param [in] hour Hour of day, UTC
/// @param [in] min Minutes, UTC
/// @param [in] sec Seconds, UTC
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow or invalid arguments)
/// @remark The instant (point in time) is initialized in UTC representation by default.
tm_status tm_makeutc (struct tm *dt, int year, tm_month month, int day, int hour, int min, int sec);

///@}

/*****************************************************
*   DATE SETTERS                                     *
*****************************************************/
///@name Setters
/// Parameter \p dt should have been previously allocated, otherwise behavior is unpredictable.
///@{

/// Sets instant in time with date and time attributes with regards to time representation.
/// @param [in] year Year
/// @param [in] month Month
/// @param [in] day Day of month
/// @param [in] hour Hour of day
/// @param [in] min Minutes
/// @param [in] sec Seconds
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Behavior depends on time representation. Time representation is kept unchanged.
tm_status tm_set (struct tm *dt, int year, tm_month month, int day, int hour, int min, int sec);

/// Sets time from string.
/// Recognized formats are : the locale's time format, the locale's alternative time representation, HH:MM:SS, HH:MM, (where HH is between 0 and 23)
/// @param [in] str string representation of time (without date)
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Behavior depends on time representation. Time representation is kept unchanged.
tm_status tm_settimefromstring (struct tm *dt, const char *str);

/// Sets date from string.
/// Recognized formats are : the locale's date format, the locale's alternative date representation, the ISO 8601 date format (YYYY-mm-dd).
/// A year specified on 2 digits is converted to the closest year on 4 digits.
/// @param [in] str string representation of date (without time)
/// @param [out] dt Pointer to broken-down time structure
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Behavior depends on time representation. Time representation is kept unchanged. Makes use of strptime().
tm_status tm_setdatefromstring (struct tm *dt, const char *str);

///@}

/*****************************************************
*   DATE FORMATTERS                                  *
*****************************************************/
///@name Formatters
///@{

/// Formats date into string.
/// Formats the date and time according to the preferred date (without the time) display format for the current locale
/// and places the result in the character array \p str of size \p max. \p str should have been previously allocated elsewhere.
/// @param [in] dt Broken-down time structure
/// @param [in] max Size of the previously allocated string \p str
/// @param [out] str null terminated string
/// @returns \p TM_OK if the result string, including the terminating null byte, does not exceed \p max bytes,
///          \p TM_ERROR otherwise (and the contents of the string \p str are then undefined.)
/// @remark Behavior depends on time representation. Makes call to strftime().
tm_status tm_getdateintostring (struct tm dt, char *str, size_t max);

/// Formats time into string.
/// Formats the date and time according to the preferred time (without the date) display format for the current locale
/// and places the result in the character array \p str of size \p max. \p str should have been previously allocated elsewhere.
/// @param [in] dt Broken-down time structure
/// @param [in] max Size of the previously allocated string \p str
/// @param [out] str null terminated string. 
/// @returns \p TM_OK if the result string, including the terminating null byte, does not exceed \p max bytes,
///          \p TM_ERROR otherwise (and the contents of the string \p str are then undefined.)
/// @remark Behavior depends on time representation. Makes call to strftime().
tm_status tm_gettimeintostring (struct tm dt, char *str, size_t max);

///@}

/*****************************************************
*   OPERATORS                                        *
*****************************************************/
///@name Operators
/// Those operators take into account local day length (24, 23 or 25 hours) when representation is local time.
///@{

/// Adds seconds to the instant of time.
/// @param [in,out] date Pointer to broken-down time structure
/// @param [in] nbSecs Number of seconds to add to \p date
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Representation is kept unchanged (either local time or UTC).
tm_status tm_addseconds (struct tm *date, long int nbSecs);

/// Adds full days to the instant of time, changing year, month and day of month without altering hours, minutes and seconds (if possible).
/// It takes into account leap years and the number of days in a month.
///
/// Behavior depends on the current representation of the instant of time : in local time representation, adding one day might correspond to adding 23, 24 or 25 hours, depending whether or not there is a daylight saving time change.
/// In case representation for both instants of time is local, days including a switch between standard time and daylight saving time count for 23 or 25 hours rather than 24.
/// I.e., adding 14 days to march the 14th, 2016, 9 am, local Paris time, yields march the 28th, 9 am (rather than 10 am if a multiple of 24 hours were added), that is only 335 hours.
///
/// In order to add an exact multiple of 24 hours, use tm_addseconds() instead.
///
/// In local time, if adding days results in an hour that is not valid in the resulting day (in case of daylight saving time change from winter to summer rule), an extra hour is added.
/// For example, the transition from standard time to daylight saving time occurs in the U.S. Pacific Time zone on March 14, 2010, at 2:00 A.M., when the time advances by one hour, to 3:00 A.M.
/// This hour interval is an invalid time, that is, a time interval that does not exist in this time zone.
/// Thus, adding one day to March 13, 2010, 2:30 A.M. will result in March 14, 2010, 3:30 A.M, rather than 2:30 A.M.
///
/// @param [in,out] date Pointer to broken-down time structure
/// @param [in] nbDays Number of days to add to \p date
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Behavior depends on time representation. Time representation is kept unchanged.
tm_status tm_adddays (struct tm *date, int nbDays);

/// Adds full months to the instant of time, changing year, month without altering day of month, hours, minutes and seconds (if possible).
/// It takes into account leap years and the number of days in a month, then adjusts the day part of the resulting instant in time.
/// If adding months results in a day that is not a valid day in the resulting month, the last day of the resulting month is used.
/// I.e., adding three months to January, the 31st, yields April, the 30th.
///
/// Behavior depends on the current representation of the instant of time : in local time representation, adding one day might correspond to adding 23, 24 or 25 hours, depending whether or not there is a daylight saving time change.
///
/// In local time, if adding days results in an hour that is not valid in the resulting day (in case of daylight saving time change from winter to summer rule), an extra hour is added.
/// For example, the transition from standard time to daylight saving time occurs in the U.S. Pacific Time zone on March 14, 2010, at 2:00 A.M., when the time advances by one hour, to 3:00 A.M.
/// This hour interval is an invalid time, that is, a time interval that does not exist in this time zone.
/// Thus, adding one month to February 14, 2010, 2:30 A.M. will result in March 14, 2010, 3:30 A.M, rather than 2:30 A.M.
///
/// @param [in,out] date Pointer to broken-down time structure
/// @param [in] nbMonths Number of months to add to \p date
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Behavior depends on time representation. Time representation is kept unchanged.
tm_status tm_addmonths (struct tm *date, int nbMonths);

/// Adds full years to the instant of time, changing year without altering month, day of month, hours, minutes and seconds (if possible).
/// Behaves as if tm_addmonths() were called with argument \p nbMonths equal to 12 x \p nbYears.
/// @param [in,out] date Pointer to broken-down time structure
/// @param [in] nbYears Number of months to add to \p date
/// @returns \p TM_OK or \p TM_ERROR (in case of overflow)
/// @remark Behavior depends on time representation. Time representation is kept unchanged.
tm_status tm_addyears (struct tm *date, int nbYears);

/// Sets the time value to 0am (beginning of the day) and keeps the date component unchanged.
/// @param [in,out] date Pointer to broken-down time structure
/// @remark Behavior depends on time representation. Time representation is kept unchanged.
tm_status tm_trimtime (struct tm *date);

/// Changes the zone offset to the earlier of the two valid offsets at a local time-line overlap.
/// This function only has any effect when the local time-line overlaps, such as at an autumn daylight savings cutover.
/// In this scenario, there are two valid offsets for the local date-time.
/// Calling this function will modify a local zoned date-time with the earlier of the two selected.
/// This is only useful in very rare case to disambiguate an instant initialized from local date and time
/// occuring during the overlapping period at an autumn daylight savings cutover.
/// @param [in,out] date Broken-down time structure
/// @returns TM_OK if instant was shifted, TM_ERROR otherwise.
/// @remark Behavior depends on time representation. This function has no effect in UTC representation.
tm_status tm_todaylightsavingextrasummertime (struct tm *date);

/// Changes the zone offset to the later of the two valid offsets at a local time-line overlap.
/// This function only has any effect when the local time-line overlaps, such as at an autumn daylight savings cutover.
/// In this scenario, there are two valid offsets for the local date-time.
/// Calling this function will modify a local zoned date-time with the later of the two selected.
/// This is only useful in very rare case to disambiguate an instant initialized from local date and time
/// occuring during the overlapping period at an autumn daylight savings cutover.
/// @param [in,out] date Broken-down time structure
/// @returns TM_OK if instant was shifted, TM_ERROR otherwise.
/// @remark Behavior depends on time representation. This function has no effect in UTC representation.
tm_status tm_todaylightsavingextrawintertime (struct tm *date);

///@}

/*****************************************************
*   COMPARATORS                                      *
*****************************************************/
///@name Comparators
/// Those comparators take into account local day length (24, 23 or 25 hours) when representation is local time.
///@{

/// Returns a value indicating whether two broken-down time have the same value (including representation).
/// @param [in] a Broken-down time structure
/// @param [in] b Broken-down time structure
/// @remark Behavior depends on time representation. Use tm_diffseconds() or tm_compare() for an absolute date and time comparison.
/// @returns 1 if the two broken-down time have the same value, 0 otherwise.
int tm_equals (struct tm a, struct tm b);

/// Gets number of seconds between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @returns Number of seconds between \p debut and \p fin. Negative of \p debut is after \p fin, positive if \p debut is before \p fin.
/// @remark The representation of instants of time are not considered.
/// @remark A difference of 0 seconds means \p debut and \p fin correspond to the same instant, independently of representation.
long int tm_diffseconds (struct tm debut, struct tm fin);

/// Compares two dates.
/// @param [in] debut Pointer to broken-down time structure
/// @param [in] fin Pointer to broken-down time structure
/// @return -1 if \p debut is before \p fin, 1 if \p debut is after \p fin, 0 if \p debut and \p fin are at same instant, independently of representation.
/// @remark The representation of instants of time are not considered.
/// @remark Compatible for use with qsort().
int tm_compare (const void *debut, const void *fin);

/// Gets number of partial days between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @returns Number of partial or complete days between \p debut and \p fin.
/// @remark Partial days are counted as 1.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffcalendardays (struct tm debut, struct tm fin);

/// Gets number of complete days between two dates.
/// In case representation for both indtants of time is local, days including between standard time and daylight saving time count for 23 or 25 hours rather than 24.
/// I.e., difference between march the 14th, 9 am and march the 28th, 9 am, 2016, local Paris time, is 14 days, even though it includes only 335 hours.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @param [out] seconds Remainder in seconds (optional)
/// @returns Number of complete days between \p debut and \p fin.
/// @remark Partial days are counted as 0.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffdays (struct tm debut, struct tm fin, int *seconds);

/// Gets number of complete weeks between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @param [out] days Remainder in days (optional)
/// @param [out] seconds Remainder in seconds (optional)
/// @returns Number of complete weeks between \p debut and \p fin.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffweeks (struct tm debut, struct tm fin, int *days, int *seconds);

/// Gets number of partial months between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @returns Number of partial or complete months between \p debut and \p fin.
/// @remark Partial months are counted as 1.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffcalendarmonths (struct tm debut, struct tm fin);

/// Gets number of complete months between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @param [out] days Remainder in days (optional)
/// @param [out] seconds Remainder in seconds (optional)
/// @returns Number of complete months between \p debut and \p fin.
/// @remark Partial months are counted as 0.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffmonths (struct tm debut, struct tm fin, int *days, int *seconds);

/// Gets number of partial years between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @returns Number of partial or complete years between \p debut and \p fin.
/// @remark Partial years are counted as 1.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffcalendaryears (struct tm debut, struct tm fin);

/// Gets number of complete years between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @param [out] months Remainder in months (optional)
/// @param [out] days Remainder in days (optional)
/// @param [out] seconds Remainder in seconds (optional)
/// @returns Number of complete years between \p debut and \p fin.
/// @remark Partial years are counted as 0.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffyears (struct tm debut, struct tm fin, int *months, int *days, int *seconds);

/// Gets number of partial ISO years between two dates.
/// @param [in] debut Broken-down time structure
/// @param [in] fin Broken-down time structure
/// @returns Number of partial or complete ISO years between \p debut and \p fin.
/// @remark Partial ISO years are counted as 1.
/// @remark Behavior depends on time representation.
/// @remark Both \p debut and \p fin should have identical representation, otherwise result is unspecified.
int tm_diffisoyears (struct tm debut, struct tm fin);

///@}

/*****************************************************
*   REPRESENTATION CONVERTERS                        *
*****************************************************/

///@name Representation of instant in time
///@{

/// Switches representation of instant in time to UTC
/// @param [in,out] date Pointer to broken-down time structure
/// @remark Has no effect if time representation is UTC already.
/// @returns TM_OK on success, TM_ERROR otherwise.
tm_status tm_toutcrepresentation (struct tm *date);

/// Switches representation of instant in time to local time
/// @param [in,out] date Pointer to broken-down time structure
/// @remark Has no effect if time representation is local time already.
tm_status tm_tolocalrepresentation (struct tm *date);

/// Indicates that the representation of instant in time is UTC.
/// @param [in] date Broken-down time structure
/// @returns 1 if \p date is in UTC representation, 0 otherwise.
int tm_isutcrepresentation (struct tm date);

/// Indicates that the representation of instant in time is local time.
/// @param [in] date Broken-down time structure
/// @returns 1 if \p date is in local time representation, 0 otherwise.
int tm_islocalrepresentation (struct tm date);

/// Gets the current representation of instant in time.
/// @param [in] date Broken-down time structure
/// @returns Date and time representation (\p TM_LOCAL or \p TM_UTC)
tm_representation tm_getrepresentation (struct tm date);

///@}

/*****************************************************
*   DATE PROPERTIES                                  *
*****************************************************/
///@name Properties
///@{

/// Indicates if the system local timezone does have any daylight saving time rules.
/// @returns 0 if this timezone does not have any daylight saving time rules, or nonzero if there is a time, past, present or future when daylight saving time applies.
/// @remark Behavior depends on time representation.
int tm_hasdaylightsavingtimerules (void);

/// Indicates that daylight saving time is in effect.
/// @param [in] date Broken-down time structure
/// @returns 1 if DST is set, 0 otherwise.
/// @remark Behavior depends on time representation. In UTC represntation, 0 is returned.
int tm_isdaylightsavingtime (struct tm date);

/// Indicates that (local) date and time occurs during the overlapping period at DST cutoff and will be repeated after DST looses effect.
/// @param [in] date Broken-down time structure
/// @returns 1 if time is duplicated (before DST change), 0 otherwise.
/// @remark Behavior depends on time representation.
int tm_isdaylightsavingextrasummertime (struct tm date);

/// Indicates that (local) date and time occurs during the overlapping period at DST cutoff and has already occured before DST lost effect.
/// @param [in] date Broken-down time structure
/// @returns 1 if time is duplicated (after DST change), 0 otherwise.
/// @remark Behavior depends on time representation.
int tm_isdaylightsavingextrawintertime (struct tm date);

///@}

/*****************************************************
*   PROPERTIES GETTERS                               *
*****************************************************/
///@name Getters
///@{

/// Gets the year, in the Gregorian calendar.
/// @param [in] date Broken-down time structure
/// @returns Year
/// @remark Behavior depends on time representation.
int tm_getyear (struct tm date);

/// Gets the month in year, in the Gregorian calendar.
/// @param [in] date Broken-down time structure
/// @returns Month (1 = January, ..., 12=December)
/// @remark Behavior depends on time representation.
tm_month tm_getmonth (struct tm date);

/// Gets the day of the month, in the Gregorian calendar.
/// @param [in] date Broken-down time structure
/// @returns Day of month
/// @remark Behavior depends on time representation.
int tm_getday (struct tm date);

/// Gets hours.
/// @param [in] date Broken-down time structure
/// @returns Hours (between 0 and 23)
/// @remark Behavior depends on time representation.
int tm_gethour (struct tm date);

/// Gets minutes.
/// @param [in] date Broken-down time structure
/// @returns Minutes (between 0 and 59)
/// @remark Behavior depends on time representation.
int tm_getminute (struct tm date);

/// Gets seconds.
/// @param [in] date Broken-down time structure
/// @returns Seconds (between 0 and 59)
/// @remark Behavior depends on time representation.
int tm_getsecond (struct tm date);

/// Gets day of year.
/// @param [in] date Broken-down time structure
/// @returns Day of year (1 = January, the 1st)
/// @remark Behavior depends on time representation.
int tm_getdayofyear (struct tm date);

/// Gets day of week.
/// @param [in] date Broken-down time structure
/// @returns Day of week (1 = Monday, 7 = Sunday)
/// @remark Behavior depends on time representation.
tm_dayofweek tm_getdayofweek (struct tm date);

/// Gets ISO week.
/// @param [in] date Broken-down time structure
/// @returns ISO 8601 week
/// @remark Behavior depends on time representation.
int tm_getisoweek (struct tm date);

/// Gets ISO year.
/// @param [in] date Broken-down time structure
/// @returns ISO 8601 year
/// @remark Behavior depends on time representation.
int tm_getisoyear (struct tm date);

/// Gets offset between UTC and local time.
/// @param [in] date Broken-down time structure
/// @returns Offset, in seconds, between UTC and time representation (local or UTC)
/// @remark Behavior depends on time representation.
int tm_getutcoffset (struct tm date);

/// Gets the name of the time zone (either UTC or local time depending on current representation).
/// @param [in] date Broken-down time structure
/// @returns Timezone abbreviation
/// @remark Behavior depends on time representation.
const char *tm_gettimezone (struct tm date);

/// Gets seconds of day.
/// @param [in] date Broken-down time structure
/// @returns Elapsed seconds since beginning of day.
/// @remark Behavior depends on time representation.
int tm_getsecondsofday (struct tm date);

///@}

/*****************************************************
*   HELPERS                                  *
*****************************************************/
///@name Helpers
///@{

/// Gets time in another target timezone.
/// Daylight saving times are considered.
/// @param [in] date Broken-down time structure, either in local timezone or UTC representation
/// @param [in] tz Target timezone (see "man tzset" for details on possible values for \p tz)
/// @param [out] year Year at the time described in target timezone
/// @param [out] month Month at the time described in target timezone
/// @param [out] day Day at the time described in target timezone
/// @param [out] hour Hour at the time described in target timezone
/// @param [out] minute Minute at the time described in target timezone
/// @param [out] second Second at the time described in target timezone
/// @param [out] is_dst_on Indicates whether (1) or not (0) daylight saving time is in effect at the time described in target timezone
void tm_getintimezone (struct tm date, const char *tz, int *year, tm_month * month, int *day, int *hour, int *minute,
                       int *second, int *is_dst_on);

///@}

/*****************************************************
*   CALENDAR PROPERTIES                              *
*****************************************************/
///@name Calendar properties
///@{

/// Indicates leap years.
/// @param [in] year year
/// @returns 1 if \p year is a leap year, 0 otherwise
int tm_isleapyear (int year);

/// Returns the number of weeks in ISO year.
/// @param [in] isoyear year
/// @returns The number of weeks in ISO year
int tm_getweeksinisoyear (int isoyear);

/// Returns the number of days in the specified month and year.
/// It interprets month and year as the month and year of the Gregorian calendar, taking leap years into account.
/// @param [in] year The year specified as a 4-digit number (for example, 1996), interpreted as a year in the Gregorian calendar.
/// @param [in] month Month
/// @returns Number of days in month \p month of year \p year
int tm_getdaysinmonth (int year, tm_month month);

/// Returns the number of seconds in the specified day, month and year.
/// It interprets day, month and year as the day, month and year of the Gregorian calendar, taking leap years and daylight saving time tules into account.
/// @param [in] year Year
/// @param [in] month Month
/// @param [in] day Day of month
/// @returns Number of seconds in day \p day of month \p month of year \p year, in local time
int tm_getsecondsinlocalday (int year, tm_month month, int day);

/// Returns the day of the first weekday in the specified month.
/// @param [in] year Year
/// @param [in] month Month
/// @param [in] dow Day of week
/// @returns The day of the first weekday in the specified month.
int tm_getfirstweekdayinmonth (int year, tm_month month, tm_dayofweek dow);

/// Returns the day of the last weekday in the specified month.
/// @param [in] year Year
/// @param [in] month Month
/// @param [in] dow Day of week
/// @returns The day of the last weekday in the specified month.
int tm_getlastweekdayinmonth (int year, tm_month month, tm_dayofweek dow);

/// Returns the day of the first weekday in the specified ISO-year.
/// @param [in] isoyear year
/// @param [in] dow Day of week
/// @returns The day of the first weekday in the specified month.
int tm_getfirstweekdayinisoyear (int isoyear, tm_dayofweek dow);

///@}

/*****************************************************
*   SERIALIEZRS                                      *
*****************************************************/
///@name Serializers
///@{

/// Serializes the instant of time to a binary value that subsequently can be used to recreate the instant of time.
/// This binary value is suitable for database recording.
/// It identifies an instant of time unambiguously, whatever the representation (local time or UTC).
/// @param [in] date Broken-down time structure, either in local timezone or UTC representation
/// @returns Binary representation of instant (point in time).
time_t tm_tobinary (struct tm date);

/// Deserializes a binary value and recreates an original serialized date and time.
/// @param [out] date Pointer to broken-down time structure, in local timezone representation
/// @param [in] binary representation of instant (point in time).
/// @returns TM_OK on sucess, TM_ERROR otherwise (overflow).
/// @remark The instant (point in time) is presented in local time representation by default.
tm_status tm_frombinary (struct tm *date, time_t binary);

///@}

#endif
