#include <string>
#include <iostream>
#include "makemql.hpp"


using namespace std;

void mql::head()
{
    os << "// Create database\n"
       << "CREATE DATABASE 'jvulgate'\n"
       << "GO\n"
       << "\n"
       << "// Switch to using database\n"
       << "USE DATABASE 'jvulgate' GO\n"
       << "\n"
       << "\n"
       << "\n";
}

void mql::list_enum(const string& name, const initializer_list<string>& il, bool first_is_default)
{
    os << "CREATE ENUMERATION " << name << " = {\n";

    bool first = first_is_default;
    int num = 0;
    
    for (const string& s : il) {
        os << "  " << (first ? "DEFAULT " : "") << s << " = " << ++num << (s != *(il.end()-1) ? ",\n" : "\n");
        first = false;
    }
        
    os << "}\n"
       << "GO\n"
       << "\n";
}
 
void mql::enums()
{
    list_enum("book_name_t",
              { "Matthew", "Mark", "Luke", "John", "Acts", "Romans",
                "I_Corinthians", "II_Corinthians", "Galatians", "Ephesians", "Philippians",
                "Colossians", "I_Thessalonians", "II_Thessalonians", "I_Timothy", "II_Timothy",
                "Titus", "Philemon", "Hebrews", "James", "I_Peter", "II_Peter",
                "I_John", "II_John", "III_John", "Jude", "Revelation" },
              false);
    
    list_enum("psp_t",
              { "adjective", "adverb", "cardinal_numeral", "conjunction", "demonstrative_pronoun", "foreign_word",
                "indefinite_pronoun", "interjection", "interrogative_adverb", "interrogative_pronoun", "noun",
                "ordinal_numeral", "personal_pronoun", "personal_reflexive_pronoun", "possessive_pronoun",
                "possessive_reflexive_pronoun", "preposition", "proper_noun", "reciprocal_pronoun",
                "relative_adverb", "relative_pronoun", "subjunction", "verb" },
              false);

    list_enum("person_t",{ "NA", "first_person", "second_person", "third_person" });
    list_enum("number_t",{ "NA", "singular", "plural" });
    list_enum("tense_t", { "NA", "future", "future_perfect", "imperfect", "perfect", "pluperfect", "present" });
    list_enum("mood_t",  { "NA", "indicative", "subjunctive", "imperative", "infinitive", "participle", "gerund", "gerundive", "supine"});
    list_enum("voice_t", { "NA", "active", "passive" });
    list_enum("gender_t",{ "NA", "masculine", "feminine", "neuter", "masculine_or_feminine", "masculine_or_neuter", "feminine_or_neuter", "masculine_or_feminine_or_neuter" });
    list_enum("case_t",  { "NA", "nominative", "accusative", "genitive", "dative", "ablative", "vocative" });
    list_enum("degree_t",{ "NA", "positive", "comparative", "superlative" });
    list_enum("inflection_t", { "non_inflecting", "inflecting" }, false);
    list_enum("conjugation_t", { "NA", "first_c", "second_c", "third_c", "third_c_io", "fourth_c", "irregular" });
    list_enum("declension_t", { "NA", "first_d", "first_or_second_d", "second_d", "second_d_er", "third_d", "fourth_d", "fifth_d", "indeclinable", "irregular" });
    list_enum("stem_t", { "NA", "a", "b", "c", "d", "e", "g", "i", "l", "n", "o", "o_or_a", "p", "r", "t", "u", "v", "indeclinable", "irregular" });
}

void mql::types()
{
    os << "// Object type word\n"
       << "CREATE OBJECT TYPE\n"
       << "[word\n"
       << "  frequency_rank : integer DEFAULT 99999;\n"
       << "  lexeme_occurrences : integer DEFAULT 0;\n"
       << "  ref : string DEFAULT \"\";\n"
       << "  surface : string FROM SET DEFAULT \"\";\n"
       << "  suffix : string FROM SET  DEFAULT \"\";\n"
       << "  normalized : string FROM SET DEFAULT \"\";\n"
       << "  lemma : string FROM SET WITH INDEX DEFAULT \"\";\n"
       << "  lemma_variant : integer DEFAULT 0;\n"
       << "  lemma_with_variant : string FROM SET WITH INDEX DEFAULT \"\";\n"
       << "  psp : psp_t;\n"
       << "  person : person_t DEFAULT NA;\n"
       << "  number : number_t DEFAULT NA;\n"
       << "  case : case_t DEFAULT NA;\n"
       << "  gender : gender_t DEFAULT NA;\n"
       << "  tense : tense_t DEFAULT NA;\n"
       << "  voice : voice_t DEFAULT NA;\n"
       << "  mood : mood_t DEFAULT NA;\n"
       << "  degree : degree_t DEFAULT NA;\n"
       << "  inflection : inflection_t;\n"
       << "  conjugation : conjugation_t DEFAULT NA;\n"
       << "  declension : declension_t DEFAULT NA;\n"
       << "  stem : stem_t DEFAULT NA;\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type book\n"
       << "CREATE OBJECT TYPE\n"
       << "[book\n"
       << "  book : book_name_t;\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type chapter\n"
       << "CREATE OBJECT TYPE\n"
       << "[chapter\n"
       << "  book : book_name_t;\n"
       << "  chapter : integer DEFAULT 0;\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type verse\n"
       << "CREATE OBJECT TYPE\n"
       << "[verse\n"
       << "  book : book_name_t;\n"
       << "  chapter : integer DEFAULT 0;\n"
       << "  verse : integer DEFAULT 0;\n"
       << "  ref : string DEFAULT \"\";\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type sentence\n"
       << "CREATE OBJECT TYPE\n"
       << "[sentence\n"
       << "]\n"
       << "GO\n"
       << "\n";
}


void mql::start_type(const string& t)
{
    os << "DROP INDEXES ON OBJECT TYPE[" << t << "] GO\n\n";

    os << "CREATE OBJECTS\n"
       << "WITH OBJECT TYPE[" << t << "]\n";
}

void mql::end_type(const string& t)
{
    os << "GO\n";
    os << "CREATE INDEXES ON OBJECT TYPE[" << t << "] GO\n\n";
}



void mql::make_word(const word& w)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << w.monad() << " }\n"
       << "[\n"
       << "frequency_rank:=" << w.frequency_rank() << ";\n"
       << "lexeme_occurrences:=" << w.occurrences() << ";\n"
       << "ref:=\"" << w.citation_part() << "\";\n"
       << "surface:=\"" << w.form() << "\";\n"
       << "normalized:=\""  << w.form() << "\";\n"  // Same as surface, but exercises will not add suffix
       << "lemma:=\"" << w.lemma() << "\";\n"
       << "lemma_variant:=" << w.lemma_variant() << ";\n"
       << "lemma_with_variant:=\"" << w.lemma() << (w.lemma_variant()==0 ? "" :
                                                    w.lemma_variant()==1 ? " I" :
                                                    w.lemma_variant()==2 ? " II" :
                                                    w.lemma_variant()==3 ? " III" : " ?") << "\";\n"
       << "suffix:=\"" << w.suffix() << "\";\n"
       << "psp:=" << w.part_of_speech() << ";\n"
       << "conjugation:=" << w.conjugation() << ";\n"
       << "declension:=" << w.declension() << ";\n"
       << "stem:=" << w.stem() << ";\n";

    for (const pair<string,string>& m : w.morph)
        os << m.first << ":=" << m.second << ";\n";

    os << "]\n";
}

void mql::make_sentence(const sentence& s)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << s.start_monad() << "-" << s.end_monad() << " }\n"
       << "[\n"
       << "]\n";
}

void mql::make_book(const book& b)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << b.start_monad() << "-" << b.end_monad() << " }\n"
       << "[\n"
       << "book:=" << b.book_name() << ";\n"
       << "]\n";
}

void mql::make_chapter(const chapter& c)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << c.start_monad() << "-" << c.end_monad() << " }\n"
       << "[\n"
       << "book:=" << c.book_name() << ";\n"
       << "chapter:=" << c.chap_no() << ";\n"
       << "]\n";
}

void mql::make_verse(const verse& v)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << v.start_monad() << "-" << v.end_monad() << " }\n"
       << "[\n"
       << "book:=" << v.book_name() << ";\n"
       << "chapter:=" << v.chap_no() << ";\n"
       << "verse:=" << v.verse_no() << ";\n"
       << "ref:=\"" << v.ref() << "\";\n"
       << "]\n";
}


void mql::tail()
{
    os << "VACUUM DATABASE ANALYZE GO\n\n";
}
