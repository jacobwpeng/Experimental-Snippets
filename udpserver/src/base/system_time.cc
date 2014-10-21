/*
 * =====================================================================================
 *
 *       Filename:  system_time.cc
 *        Created:  08/22/14 20:43:05
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "system_time.h"
#include <sys/time.h>
#include <cassert>

namespace fx
{
    namespace base
    {
        namespace time
        {
            TimeStamp Now()
            {
                timeval tv;
                int ret = ::gettimeofday(&tv, 0);
                assert (ret == 0);
                (void) ret;

                return static_cast<TimeStamp>(tv.tv_sec) * 1000000 + static_cast<TimeStamp>(tv.tv_usec);
            }

            time_t StringToTime(const char * time)
            {
                const char * fmt = "%Y-%m-%d %H:%M:%S";
                struct tm tm;
                char * end = ::strptime(time, fmt, &tm);
                if (end == NULL || *end != '\0') { return -1; }
                return ::mktime(&tm);
            }
#if 0

            Date::Builder& Date::Builder::year(int y)
            {
                y_ = y;
            }

            Date::Builder& Date::Builder::month(int m)
            {
                m_ = m;
            }

            Date::Builder& Date::Builder::day(int d)
            {
                d_ = d;
            }

            Date Date::Builder::Finish()
            {
                //if (y_ >= 1900 and m_ >= 1 and m_ <= 12)
                //{
                //    if (m_ == 2)
                //    {
                //        int y = y_ + 1900;
                //        bool leap_year = (y % 4 == 0 and (y % 100 != 0 or y % 400 == 0));
                //        if (leap_year) 
                //        {

                //        }
                //    }
                //}
            }
            Year::Year(int y)
                :y_(y)
            {
            }

            bool Year::valid() const
            {
                return y_ == -1;
            }

            Month::Month(int m)
                :m_(m)
            {
            }

            bool Month::valid() const
            {
                return m_ == -1;
            }

            Day::Day(int d)
                :d_(d)
            {
            }

            bool Day::valid() const
            {
                return d_ == -1;
            }

            Year year(int y)
            {
                if (y >= 0) return Year(y);
                else return Year();
            }

            Month month(int m)
            {
                if (y <
            }
#endif

        }
    }
}
