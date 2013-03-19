#include "BigInteger.hpp"

#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::string;

int main()
{
    BigInteger i("1048576");
    BigInteger j("123456897");

    j+= i;
    cout << j.DebugString() << endl;
    cout << 1048576 + 123456897 << endl;
}
