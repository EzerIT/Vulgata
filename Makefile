# Copyright © 2022 Claus Tøndering.
# Released under an MIT License.

OBJFILES=parse_latin_nt.o makemql.o pugixml.o
DEPFILES=$(OBJFILES:.o=.d)

CXX=c++-11.1.0
CXXFLAGS=-std=c++20 -MMD -O3 -I pugixml/src


jvulgate: jvulgate.mql
	rm -f $@
	mql $<

jvulgate.mql:	parse_latin_nt
	./parse_latin_nt

parse_latin_nt:	parse_latin_nt.o makemql.o pugixml.o
	$(CXX) $(CXXFLAGS) -o $@ $+

parse_latin_nt.o:	parse_latin_nt.cpp makemql.hpp parse_latin_nt.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

makemql.o:	makemql.cpp makemql.hpp parse_latin_nt.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

pugixml.o:	pugixml/src/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJFILES) jvulgate jvulgate.mql parse_latin_nt


-include $(DEPFILES)
