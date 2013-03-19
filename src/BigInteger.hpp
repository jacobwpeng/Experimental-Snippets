#ifndef BIG_NUMBER_HPP
#define BIG_NUMBER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <boost/foreach.hpp>

using std::vector;
using std::string;
using std::ostringstream;


class BigInteger
{
    public:
        BigInteger(const string& numString);
        BigInteger(int maxPow, const vector<int>& numbers);

        BigInteger& operator+=(const BigInteger& rhs)
        {
            int minPow = this->maxPow < rhs.maxPow ? this->maxPow : rhs.maxPow;
            bool carry_over = false;
            for(int pow = 0; pow <= maxPow; ++pow)
            {
                int sum = numbers[pow] + rhs.numbers[pow];
                sum += carry_over ? 1 : 0;

                carry_over = (sum >= 10);
                if( carry_over )
                {
                    sum -= 10;
                }
                
                numbers[pow] = sum;
            }

            int newPow = this->maxPow > rhs.maxPow ? this->maxPow : rhs.maxPow;
            if( carry_over )
            {
                newPow += 1;
                this->numbers.push_back(1);
            }
            return *this;
        }

        string DebugString()
        {
            ostringstream oss;
            BOOST_FOREACH(int i, this->numbers)
            {
                oss << i;
            }
            oss << ", pow : " << this->maxPow;
            return oss.str();
        }

        static int ParseFromString(const string& numString, int& maxPow, vector<int>& numbers);
    private:
        int maxPow;
        vector<int> numbers;

        static int CheckString(const string& numString);
};

BigInteger::BigInteger(const string& numString)
{
    int ret = BigInteger::ParseFromString(numString, this->maxPow, this->numbers);
    if( ret ){
        this->maxPow = 0;
        this->numbers.clear();
    }else{

    }
}

BigInteger::BigInteger(int maxPow, const vector<int>& numbers)
    :maxPow(maxPow), numbers(numbers)
{
}

int BigInteger::ParseFromString(const string& numString, int& maxPow, vector<int>& numbers)
{
    if( CheckString(numString) )
    {
        return -1;
    }

    maxPow = numString.size() - 1;
    numbers.clear();
    for( int i = 0; i <= maxPow; ++i )
    {
        numbers.push_back( numString[maxPow-i] - '0' );
    }
    return 0;
}

int BigInteger::CheckString(const string& numString)
{
    string::const_iterator iter = numString.begin();
    while( iter != numString.end() )
    {
        char c = *iter;
        if( c < '0' || c > '9' )
        {
            return -1;
        }
        ++iter;
    }
    return 0;
}


#endif
