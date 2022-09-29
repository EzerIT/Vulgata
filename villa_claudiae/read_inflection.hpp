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
void add_conjugation(const std::string& w, const std::string& s);
void add_adj_declension(const std::string& w, const std::string& s, const std::string& stem);
void add_noun_declension(const std::string& w, const std::string& s, const std::string& stem);

#endif // _READ_INFLECTION_HPP_
