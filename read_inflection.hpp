#ifndef _READ_INFLECTION_HPP_
#define _READ_INFLECTION_HPP_

#include <map>
#include <string>


extern std::map<std::string,std::string> conj_map; // lemma => conjugation
extern std::map<std::string,std::string> noun_decl_map; // lemma => declination
extern std::map<std::string,std::string> adj_decl_map; // lemma => declination
extern std::map<std::string,std::string> noun_stem_map; // lemma => stem
extern std::map<std::string,std::string> adj_stem_map; // lemma => stem

void read_inflection_spreadsheets();

#endif // _READ_INFLECTION_HPP_
