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

all: rconteam configreader dbmanagement

rconteam: rconteam.cpp
	$(CC) $< -o $@ $(CXXFLAGS) 

dbmanagement: dbManagement.cpp
	$(CC) $< -o $@ $(CXXFLAGS) 

configreader: configreader.cpp
	$(CC) $< -o $@ $(CXXFLAGS) 

clean:
	rm -fr *.o rconteam 
	
	
