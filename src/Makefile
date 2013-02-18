
DEBUGFLAGS = \
	-D _GLIBCXX_DEBUG \
	-D _GLIBCXX_DEBUG_PEDANTIC \
	-D DEBUG \
	-O0 \
	-g3

RELEASEFLAGS = \
	-O3 \
	-g0

GENERALFLAGS = \
	-D _GLIBCXX_USE_NANOSLEEP \
	-D _GLIBCXX_USE_SCHED_YIELD \
	-pthread \
	-std=c++11 \
	-Wall \
	-Werror

CXXFLAGS = \
	$(GENERALFLAGS) \
	$(DEBUGFLAGS)
	
CXX = g++
DEPS := $(wildcard *.h)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o, $(SOURCES))

all: rconteam

rconteam: $(OBJECTS) 
	$(CXX) -o rconteam $(OBJECTS)

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -fr $(OBJECTS) rconteam 
	
	
