CXX=g++
CXXFLAGS=-Werror -Wall -g

mkdata: mkdata.o raw_data.o
	$(CXX) $(CXXFLAGS) -o mkdata $^

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

.PHONY:
clean:
	-rm -f mkdata *.o
