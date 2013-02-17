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

<<<<<<< HEAD
all: rconteam configreader dbmanagement

rconteam: rconteam.cpp
	$(CC) $< -o $@ $(CXXFLAGS) 

dbmanagement: dbManagement.cpp
	$(CC) $< -o $@ $(CXXFLAGS) 

configreader: configreader.cpp
	$(CC) $< -o $@ $(CXXFLAGS) 
=======
CXX = g++
DEPS := $(wildcard *.h)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o, $(SOURCES))

all: rconteam

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

rconteam: $(OBJECTS) 
	$(CXX) -o rconteam $(OBJECTS)
>>>>>>> 8ac6d500ab27d8d35d86911d48ee955c7b3e7360

clean:
	rm -fr *.o rconteam 
	
	
