CXX=c++-11.1.0
CXXFLAGS=-std=c++20 -I pugixml/src

#all:	parse_latin_nt make_vulg fra_annot.txt fra_source.txt

#traverse_walker:	traverse_walker.o pugixml.o
# 	$(CXX) $(CXXFLAGS) -o $@ $+
# 
#traverse_walker.o:	traverse_walker.cpp
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

jvulgate: jvulgate.mql
	rm $@
	mql $<

jvulgate.mql:	parse_latin_nt
	./parse_latin_nt

parse_latin_nt:	parse_latin_nt.o makemql.o pugixml.o 
	$(CXX) $(CXXFLAGS) -o $@ $+

parse_latin_nt.o:	parse_latin_nt.cpp makemql.hpp parse_latin_nt.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

makemql.o:	makemql.cpp makemql.hpp parse_latin_nt.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

make_vulg:	make_vulg.o vul2vector.o parse_annotated.o pugixml.o
	$(CXX) $(CXXFLAGS) -o $@ $+

make_vulg.o:	make_vulg.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

parse_annotated.o:	parse_annotated.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

vul2vector.o:	vul2vector.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

pugixml.o:	pugixml/src/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


fra_annot.txt: 	parse_latin_nt
	./parse_latin_nt > fra_annot.txt 

fra_source.txt:	make_vulg
	./make_vulg > fra_source.txt
