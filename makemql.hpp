#ifndef _MAKEMQL_H_
#define _MAKEMQL_H_

#include <string>
#include <fstream>

#include "parse_latin_nt.hpp"


class mql {
  public:
    mql(const std::string& filename)
        : os{filename}
        {}

    void head();
    void enums();
    void types();

    void start_word() { start_type("word"); }
    void make_word(const word& w);
    void end_word() { end_type("word"); }

    void start_sentence() { start_type("sentence"); }
    void make_sentence(const sentence& s);
    void end_sentence() { end_type("sentence"); }


    void start_book() { start_type("book"); }
    void make_book(const book& b);
    void end_book() { end_type("book"); }

    void start_chapter() { start_type("chapter"); }
    void make_chapter(const chapter& c);
    void end_chapter() { end_type("chapter"); }

    void start_verse() { start_type("verse"); }
    void make_verse(const verse& v);
    void end_verse() { end_type("verse"); }


    void tail();
    
  private:
    void start_type(const std::string& t);
    void end_type(const std::string& t);
    
    std::ofstream os;
};

#endif // _MAKEMQL_H_
