
# requirements
REQ_CXX11_TEST = $(shell echo "int main(){}" | g++ -o /dev/null -x c++ -std=c++11 - 2>/dev/null; echo $$?)
REQ_CXX11_FAIL = "ERROR: Requires g++ v4.7 or above to comiile."

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
	
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o, $(SOURCES))

all: reqs rconteam

reqs:
	@if (($(REQ_CXX11_TEST))); then echo $(REQ_CXX11_FAIL); exit 1; fi

rconteam: $(OBJECTS)
	$(CXX) -o rconteam $(OBJECTS)

%.o: %.cpp %.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -fr $(OBJECTS) rconteam 
	
.PHONY: reqs

