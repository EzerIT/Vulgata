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


void mql::enums()
{
    os << "CREATE ENUMERATION psp_t = {\n"
       << "  adjective                    =  1,\n"
       << "  adverb                       =  2,\n"
       << "  cardinal_numeral             =  3,\n"
       << "  conjunction                  =  4,\n"
       << "  demonstrative_pronoun        =  5,\n"
       << "  foreign_word                 =  6,\n"
       << "  indefinite_pronoun           =  7,\n"
       << "  interjection                 =  8,\n"
       << "  interrogative_adverb         =  9,\n"
       << "  interrogative_pronoun        = 10,\n"
       << "  noun                         = 11,\n"
       << "  ordinal_numeral              = 12,\n"
       << "  personal_pronoun             = 13,\n"
       << "  personal_reflexive_pronoun   = 14,\n"
       << "  possessive_pronoun           = 15,\n"
       << "  possessive_reflexive_pronoun = 16,\n"
       << "  preposition                  = 17,\n"
       << "  proper_noun                  = 18,\n"
       << "  reciprocal_pronoun           = 19,\n"
       << "  relative_adverb              = 20,\n"
       << "  relative_pronoun             = 21,\n"
       << "  subjunction                  = 22,\n"
       << "  verb                         = 23\n"
       << "}\n"
       << "GO\n"
       << "\n"
       << "CREATE ENUMERATION person_t = {\n"
       << "  DEFAULT NA    = 1,\n"
       << "  first_person  = 2,\n"
       << "  second_person = 3,\n"
       << "  third_person  = 4\n"
       << "}\n"
       << "GO\n"
       << "\n"
       << "CREATE ENUMERATION number_t = {\n"
       << "  DEFAULT NA = 1,\n"
       << "  singular   = 2,\n"
       << "  plural     = 3\n"
       << "}\n"
       << "GO\n"
       << "\n";
}

void mql::types()
{
    os << "// Object type sentence\n"
       << "CREATE OBJECT TYPE\n"
       << "[sentence\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type book\n"
       << "CREATE OBJECT TYPE\n"
       << "[book\n"
       << "  title_sletmig : string;\n"
//       << "  book : book_name_t DEFAULT Genesis;\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type chapter\n"
       << "CREATE OBJECT TYPE\n"
       << "[chapter\n"
       << "  title_sletmig : string;\n"
       << "  chapter : integer DEFAULT 0;\n"
//       << "  book : book_name_t DEFAULT Genesis;\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type verse\n"
       << "CREATE OBJECT TYPE\n"
       << "[verse\n"
       << "  title_sletmig : string;\n"
       << "  chapter : integer DEFAULT 0;\n"
       << "  verse : integer DEFAULT 0;\n"
//       << "  book : book_name_t DEFAULT Genesis;\n"
       << "]\n"
       << "GO\n"
       << "\n";

    os << "// Object type word\n"
       << "CREATE OBJECT TYPE\n"
       << "[word\n"
       << "  frequency_rank : integer DEFAULT 99999;\n"
       << "  lexeme_occurrences : integer DEFAULT 0;\n"
       << "  surface : string FROM SET  DEFAULT \"\";\n"
       << "  lemma : string FROM SET  WITH INDEX  DEFAULT \"\";\n"
       << "  psp : psp_t;\n"
       << "  person : person_t DEFAULT NA;\n"
       << "  number : number_t DEFAULT NA;\n"
//       << "  case : case_t DEFAULT NA;\n"
//       << "  gender : gender_t DEFAULT NA;\n"
//       << "  tense : tense_t DEFAULT NA;\n"
//       << "  voice : voice_t DEFAULT NA;\n"
//       << "  mood : mood_t DEFAULT NA;\n"
       << "  suffix : string FROM SET  DEFAULT \"\";\n"
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
       << "frequency_rank:=0;\n"
       << "lexeme_occurrences:=0;\n"
       << "surface:=\"" << w.form() << "\";\n"
       << "lemma:=\"" << w.lemma() << "\";\n"
       << "suffix:=\"" << w.suffix() << "\";\n"
       << "psp:=" << w.part_of_speech() << ";\n";


    for (const string& key : {"person", "number"}) {
        string val = w.morph(key);

        if (val!="")
            os << key << ":=" << val << ";\n";
    }

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
       << "title_sletmig:=\"" << b.book_name() << "\";\n"
       << "]\n";
}

void mql::make_chapter(const chapter& c)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << c.start_monad() << "-" << c.end_monad() << " }\n"
       << "[\n"
       << "title_sletmig:=\"" << c.book_name() << "\";\n"
       << "chapter:=" << c.chap_no() << ";\n"
       << "]\n";
}

void mql::make_verse(const verse& v)
{
    os << "\n"
       << "CREATE OBJECT\n"
       << "FROM MONADS= { " << v.start_monad() << "-" << v.end_monad() << " }\n"
       << "[\n"
       << "title_sletmig:=\"" << v.book_name() << "\";\n"
       << "chapter:=" << v.chap_no() << ";\n"
       << "verse:=" << v.verse_no() << ";\n"
       << "]\n";
}


void mql::tail()
{
    os << "VACUUM DATABASE ANALYZE GO\n\n";
}
