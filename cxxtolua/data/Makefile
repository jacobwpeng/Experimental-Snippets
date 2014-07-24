INCS = -I /opt/include
LIBS = -llua
CXXFLAGS = -O3 -DNDEBUG

all: code_generate array.so

code_generate:
	echo '#include "userdata_wrapper.h"' > output.cc
	echo '#include "dynamic_array.h"' >> output.cc
	echo '#include <lua.hpp>' >> output.cc
	echo 'extern "C" {' >> output.cc
	../bin/run.py dynamic_array.h >> output.cc
	astyle output.cc
	rm -rf output.cc.orig
	echo '}' >> output.cc

array.so: dynamic_array.o output.o
	clang++ -shared -o $@ $^ ${LIBS}

%.o: %.cc
	clang++ ${INCS} ${CXXFLAGS} -c -o $@ $^

clean:
	rm -rf output.cc *.o
