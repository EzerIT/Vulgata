# Copyright © 2022 Claus Tøndering.
# Released under an MIT License.

OBJFILES=parse_latin_vc.o makemql.o read_inflection.o
DEPFILES=$(OBJFILES:.o=.d)

CXX=c++
CXXFLAGS=-std=c++20 -MMD -g -I pugixml/src -I ..


VC: VC.mql
	rm -f $@
	mql $<

VC.mql:	parse_latin_vc Villa\ Claudiae.csv
	./parse_latin_vc

parse_latin_vc:	$(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $+

pugixml.o:	pugixml/src/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJFILES) VC VC.mql parse_latin_vc


-include $(DEPFILES)
