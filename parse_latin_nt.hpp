#ifndef _PARSE_LATIN_NT_HPP_
#define _PARSE_LATIN_NT_HPP_

#include <string>
#include <vector>
#include <utility>
#include <tuple>

class word {
  public:
    word(const std::string& citation_part,
         const std::string& form,
         const std::string& presentation_after,
         const std::string& lemma,
         const std::string& part_of_speech,
         const std::string& morphology,
         const std::string& relation)
        : m_citation_part{citation_part},
          m_form{form},
          m_presentation_after{presentation_after},
          m_lemma{lemma},
          m_lemma_variant{0},
          m_part_of_speech{part_of_speech},
          m_morphology{morphology},
          m_relation{relation},
          m_monad{++last_monad}
        {}

    std::string citation_part()      const { return m_citation_part; };
    std::string form()               const { return m_form; };
    std::string presentation_after() const { return m_presentation_after; };

    std::string lemma()              const { return m_lemma; };
    void lemma(const std::string& s) { m_lemma = s; };

    int lemma_variant()               const { return m_lemma_variant; };
    void lemma_variant(int i)         { m_lemma_variant = i; };

    std::string part_of_speech()     const { return m_part_of_speech; };
    void part_of_speech(const std::string& s) { m_part_of_speech = s; };

    std::string morphology()         const { return m_morphology; };
    std::string relation()           const { return m_relation; };

    std::string suffix()        const { return m_suffix; }
    void suffix(const std::string& s) { m_suffix = s; }

    long monad()                const { return m_monad; }

    int occurrences() const { return m_occurrences; }
    void occurrences(int o) { m_occurrences = o; }
    
    int frequency_rank() const { return m_frequency_rank; }
    void frequency_rank(int r) { m_frequency_rank = r; }
    
    std::vector<std::pair<std::string,std::string>> morph;
    
    static long last_monad;

  private:
    std::string m_citation_part;
    std::string m_form;
    std::string m_presentation_after;
    std::string m_lemma;
    int m_lemma_variant;
    std::string m_part_of_speech;
    std::string m_morphology;
    std::string m_relation;

    std::string m_suffix;

    int m_occurrences;
    int m_frequency_rank;

    long m_monad;
};

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


class sentence {
  public:
    sentence(long start_monad, long end_monad, const std::string& presentation_after)
        : m_start_monad{start_monad}, m_end_monad{end_monad}, m_presentation_after{presentation_after}
        {}

    long start_monad()               const { return m_start_monad; }
    long end_monad()                 const { return m_end_monad; }
    std::string presentation_after() const { return m_presentation_after; }

  private:
    long m_start_monad;
    long m_end_monad;
    std::string m_presentation_after;
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
