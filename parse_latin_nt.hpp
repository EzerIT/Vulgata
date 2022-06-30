#ifndef _PARSE_LATIN_NT_HPP_
#define _PARSE_LATIN_NT_HPP_

#include <string>
#include <map>

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
          m_part_of_speech{part_of_speech},
          m_morphology{morphology},
          m_relation{relation},
          m_monad{++last_monad}
        {}

    std::string citation_part()      const { return m_citation_part; };
    std::string form()               const { return m_form; };
    std::string presentation_after() const { return m_presentation_after; };
    std::string lemma()              const { return m_lemma; };
    std::string part_of_speech()     const { return m_part_of_speech; };
    void part_of_speech(const std::string& s) { m_part_of_speech = s; };

    std::string morphology()         const { return m_morphology; };
    std::string relation()           const { return m_relation; };

    std::string suffix()        const { return m_suffix; }
    void suffix(const std::string& s) { m_suffix = s; }

    std::string morph(const std::string& feature) const { return m_morphmap.contains(feature) ? m_morphmap.at(feature) : ""; }
    void morph(const std::string& feature, const std::string& value) { m_morphmap[feature] = value; }

    long monad()                const { return m_monad; }

    static long last_monad;

  private:
    std::string m_citation_part;
    std::string m_form;
    std::string m_presentation_after;
    std::string m_lemma;
    std::string m_part_of_speech;
    std::string m_morphology;
    std::string m_relation;

    std::string m_suffix;

    std::map<std::string,std::string> m_morphmap;
    
    long m_monad;
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
    verse(const std::string& book_name, int chap_no, int verse_no, long start_monad, long end_monad=0)
        : m_book_name{book_name}, m_chap_no{chap_no}, m_verse_no{verse_no}, m_start_monad{start_monad}, m_end_monad{end_monad}
        {}

    std::string book_name() const { return m_book_name; }
    int chap_no()           const { return m_chap_no; }
    int verse_no()          const { return m_verse_no; }
    long start_monad()      const { return m_start_monad; }
    long end_monad()        const { return m_end_monad; }
    void end_monad(long m) { m_end_monad = m; }

  private:
    std::string m_book_name;
    int m_chap_no;
    int m_verse_no;
    long m_start_monad;
    long m_end_monad;
};



#endif //_PARSE_LATIN_NT_HPP_
