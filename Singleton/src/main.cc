#include <cstdio>
#include "Base.h"

using namespace std;

int main(int argc, char* argv[])
{
    Base* p = Singleton<Base>::Instance();
}
