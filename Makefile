# Copyright © 2022 Claus Tøndering.
# Released under an MIT License.

OBJFILES=parse_latin_nt.o makemql.o read_inflection.o pugixml.o
DEPFILES=$(OBJFILES:.o=.d)

CXX=c++
CXXFLAGS=-std=c++20 -MMD -O3 -I pugixml/src


jvulgate: jvulgate.mql
	rm -f $@
	mql $<

jvulgate.mql:	parse_latin_nt
	./parse_latin_nt

parse_latin_nt:	$(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $+

pugixml.o:	pugixml/src/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJFILES) jvulgate jvulgate.mql parse_latin_nt


-include $(DEPFILES)
