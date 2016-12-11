# datetime

I have been looking for a simple library of functions in C to manage date and time in a POSIX framework,
but could not find any that would meet my needs.

That's the reason why I wrote this minimal set of functions, comparable to what DateTime offers for .Net
(DateTimeOffset is useless as it does not handle daylight saving time.)

This library is a tool box that facilitates the management of dates and times. It is a superset of lower level POSIX functions.
The functions of this toolbox manipulate instants (points) in time expressed as a date and time of day, in Gregorian calendar.

Instants are internally stored in the structure \p struct \p tm defined by POSIX, with a resolution of one second.
This allows compatible access to low level POSIX functions, such as strftime() or strptime() (see man page of mktime()).

However, objects \p struct \p tm should be considered as abstract data types, and should not be initializeed by hand.
Instants should be initialized with tm_makelocal(), tm_makeutc(), tm_makenow() and tm_maketoday() instead.
The use of these functions is compulsory, as well as easier than handling with \p struct \p tm.

Once initialized, tm_set(), tm_setdatefromstring(), tm_settimefromstring() can be used to modify the instant.

Instants in time can be at will represented either in UTC or local time.
Functions tm_toutc() and tm_tolocal() allow to switch from one representation to the other.
Functions tm_isutc(), tm_islocal() and tm_getrepresentation() permit to know the current representation of an instant in time.
These functions do not affect the instant in time but only the way it is yield. One could think of it as the unit with which the instant is expressed. 
 
Daylight saving time is taken into account in local time representation but is not applicable to UTC:
- When local representation is used, calculations take daylight saving time rules into account.
Days with DST change contain 23 or 25 hours when added or compared. 
Local time is appropriate for acquisition or display in user interfaces of desktop applications.
- On the contrary, in UTC, daylight saving time does not apply, and all days last 24 hours.

Functions tm_isdaylightsavingtime(), tm_isdaylightsavingextrawintertime(), tm_isdaylightsavingextrasummertime()
indicate whether or not Daylight saving time is in effect.
Function tm_hasdaylightsavingtimerules() indicates whether or not daylight saving time rules apply in local timezone. 

Functions for calculation are tm_add... and tm_diff....
Functions for comparison are tm_compare() and tm_equals().
Functions for persistance are tm_tobinary() and tm_frombinary().

Interface is described in dates.h and implementation in dates.c.
Use Makefile as an example for compilation.

File dates_tu_check.c implements unit tests using check as a framework.


Please read dates.h for more details.
