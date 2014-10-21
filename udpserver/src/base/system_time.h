/*
 * =====================================================================================
 *
 *       Filename:  system_time.h
 *        Created:  08/22/14 20:41:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __SYSTEM_TIME_H__
#define  __SYSTEM_TIME_H__

#include <stdint.h>
#include <ctime>

namespace fx
{
    namespace base
    {
        namespace time
        {
            typedef uint64_t TimeStamp;

            //Unix timestamp in us
            TimeStamp Now();

            /*-----------------------------------------------------------------------------
             *  format -> 2014-08-22 01:23:45
             *-----------------------------------------------------------------------------*/
            time_t StringToTime(const char * time);

#if 0
            class Year
            {
                public:
                    bool valid() const;

                private:
                    Year(int y = -1);
                    int y_;
                    friend Year year(int y);
            };

            class Month
            {
                public:
                    bool valid() const;

                private:
                    Month(int m = -1);
                    int m_;
                    friend Month month(int m);
            };

            class Day
            {
                public:
                    bool valid() const;

                private:
                    Day(int d = -1);
                    int d_;
                    friend Day day(int d);
            };

            Year year(int y);
            Month month(int m);
            Day day(int d);

            class DateTime
            {
                public:
                    class Builder
                    {
                        public:
                            Builder & set_year(int y); 
                            Builder & set_month(int m);
                            Builder & set_day(int d);

                            Date Finish();
                        private:
                            int y_, m_, d_;
                    };

                private:
                    DateTime();
                    DateTime(int y, int m, int d);

                private:
                    time_t time_;
                    friend class Builder;
            };
#endif
        }
    }
}

#endif   /* ----- #ifndef __SYSTEM_TIME_H__  ----- */
