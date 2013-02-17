CC = g++
CXXFLAGS = \
	-D _GLIBCXX_USE_NANOSLEEP \
	-D _GLIBCXX_USE_SCHED_YIELD \
	-D DEBUG \
	-pthread \
	-std=c++11\
	-Wall\
	-Werror\
	-O3

CXX = g++
DEPS := $(wildcard *.h)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o, $(SOURCES))

all: rconteam

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

rconteam: $(OBJECTS) 
	$(CXX) -o rconteam $(OBJECTS)

clean:
	rm -fr *.o rconteam 
	
	
