#ifndef TIME_OFFSET_H
#define TIME_OFFSET_H
/*
    PURPOSE: ( TIME_OFFSET struct )
*/
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

    int day;        /* --  Day of the year, 0 .. 364 */
    int hour;       /* --  Hour of the day, 0 .. 23 */
    int min;        /* --  Minute of the hour, 0 .. 59 */
    double sec;     /* --  Seconds of the hour 0.0 .. 59.99999999 */

} TIME_OFFSET ;

// For backwards compatibility
typedef TIME_OFFSET GMTTIME ;

#ifdef __cplusplus
}
#endif
#endif

