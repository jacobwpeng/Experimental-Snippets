CC ?= clang
CXX ?= clang++
INCS = -I/opt/include/ -Iinclude/
LDLIBS = -lpthread
LDFLAGS = 
RELEASE_FLAGS = $(LDFLAGS) -O2 -std=c++11
DEBUG_FLAGS = $(LDFLAGS) -DTEST -DDEBUG -D_DEBUG
BIN = run
BUILD_DIR = build
OBJS = main.o Base.o Base2.o
RM = rm -rf

ifdef DEBUG
	CXXFLAGS = $(DEBUG_FLAGS)
else
	CXXFLAGS = $(RELEASE_FLAGS)
endif

vpath %.cc src
vpath %.h include

all : $(BUILD_DIR) $(BIN)

$(BIN) : $(addprefix $(BUILD_DIR)/, $(OBJS) )
	$(CXX) $^ $(LDLIBS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o : %.cc
	$(CXX) -c $(INCS) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR):
	mkdir -p $@

clean : 
	$(RM) $(BUILD_DIR)

.PHONY : clean
