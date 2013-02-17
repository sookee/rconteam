
CXXFLAGS = \
	-D _GLIBCXX_USE_NANOSLEEP \
	-D _GLIBCXX_USE_SCHED_YIELD \
	-D DEBUG \
	-pthread \
	-std=c++11

CXX = g++
DEPS = TeamPolicy.h
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o, $(SOURCES))

all: rconteam

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

rconteam: $(OBJECTS) 
	$(CXX) -o rconteam $(OBJECTS)

clean:
	rm -fr *.o rconteam
	
	