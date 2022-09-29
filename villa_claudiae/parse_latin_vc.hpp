#ifndef _PARSE_LATIN_NT_HPP_
#define _PARSE_LATIN_NT_HPP_

#include <string>
#include <vector>
#include <utility>
#include <tuple>

// Spread sheet indexes

constexpr int REF = 0;
constexpr int SURFACE = 1;
constexpr int LEMMA = 2;
constexpr int PSP = 3;
constexpr int PERSON = 4;
constexpr int NUMBER = 5;
constexpr int TENSE = 6;
constexpr int MOOD = 7;
constexpr int VOICE = 8;
constexpr int GENDER = 9;
constexpr int CASE = 10;
constexpr int DEGREE = 11;
constexpr int INFLECTION = 12;
constexpr int CONJUGATION = 13;
constexpr int DECLENSION = 14;
constexpr int STEM = 15;
constexpr int TEXT = 16;

// Added by program:

constexpr int MONAD = 17;
constexpr int SUFFIX = 18;
constexpr int OCCURRENCES = 19;
constexpr int FREQUENCY_RANK = 20;


// For the {PRE}lexicon_latin database table
class lexitem {
  public:
    lexitem() = default;
    
    lexitem(const std::string& lemma,
            int lemma_variant,
            const std::string& part_of_speech,
            int tally,
            const std::string& first_book,
            int first_chapter,
            int first_verse) 
        : m_lemma{lemma},
          m_lemma_variant{lemma_variant},
          m_part_of_speech{part_of_speech},
          m_tally{tally},
          m_first_book{first_book},
          m_first_chapter{first_chapter},
          m_first_verse{first_verse}
        {
            make_sortorder(); // m_lemma must be initialized before this is called
        }

    void make_sortorder();
    
    std::string lemma()          const { return m_lemma; };
    int lemma_variant()          const { return m_lemma_variant; };
    std::string part_of_speech() const { return m_part_of_speech; };
    std::string sortorder()      const { return m_sortorder; };
    int tally()                  const { return m_tally; };
    std::string first_book()     const { return m_first_book; };
    int first_chapter()          const { return m_first_chapter; };
    int first_verse()            const { return m_first_verse; };

    std::tuple<std::string, int, std::string> make_key() { return std::tuple{m_lemma, m_lemma_variant, m_part_of_speech}; }

  private:
    std::string m_lemma;
    int m_lemma_variant;
    std::string m_part_of_speech;
    std::string m_sortorder;
    int m_tally;
    std::string m_first_book;
    int m_first_chapter;
    int m_first_verse;
};




class book {
  public:
    book(const std::string& book_name, long start_monad, long end_monad=0)
        : m_book_name{book_name}, m_start_monad{start_monad}, m_end_monad{end_monad}
        {}

    std::string book_name() const { return m_book_name; }
    long start_monad()  const { return m_start_monad; }
    long end_monad()    const { return m_end_monad; }
    void end_monad(long m) { m_end_monad = m; }

  private:
    std::string m_book_name;
    long m_start_monad;
    long m_end_monad;
};



class chapter {
  public:
    chapter(const std::string& book_name, int chap_no, long start_monad, long end_monad=0)
        : m_book_name{book_name}, m_chap_no{chap_no}, m_start_monad{start_monad}, m_end_monad{end_monad}
        {}

    std::string book_name() const { return m_book_name; }
    int chap_no()           const { return m_chap_no; }
    long start_monad()      const { return m_start_monad; }
    long end_monad()        const { return m_end_monad; }
    void end_monad(long m) { m_end_monad = m; }

  private:
    std::string m_book_name;
    int m_chap_no;
    long m_start_monad;
    long m_end_monad;
};

class verse {
  public:
    verse(const std::string& book_name, int chap_no, int verse_no, const std::string& ref, long start_monad, long end_monad=0)
        : m_book_name{book_name}, m_chap_no{chap_no}, m_verse_no{verse_no}, m_ref{ref},
          m_start_monad{start_monad}, m_end_monad{end_monad}
        {}

    std::string book_name() const { return m_book_name; }
    int chap_no()           const { return m_chap_no; }
    int verse_no()          const { return m_verse_no; }
    std::string ref()       const { return m_ref; }
    long start_monad()      const { return m_start_monad; }
    long end_monad()        const { return m_end_monad; }
    void end_monad(long m) { m_end_monad = m; }

  private:
    std::string m_book_name;
    int m_chap_no;
    int m_verse_no;
    std::string m_ref;
    long m_start_monad;
    long m_end_monad;
};



#endif //_PARSE_LATIN_NT_HPP_
