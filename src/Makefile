
CXXFLAGS = \
	-D _GLIBCXX_USE_NANOSLEEP \
	-D _GLIBCXX_USE_SCHED_YIELD \
	-D DEBUG \
	-pthread \
	-std=c++11

all: rconteam

rconteam: rconteam.cpp
	g++ $< -o $@ $(CXXFLAGS)

clean:
	rm -fr *.o rconteam
	
	