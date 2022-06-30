#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <tuple>


#include "pugixml.hpp"
#include "makemql.hpp"
#include "parse_latin_nt.hpp"

using namespace std;



long word::last_monad = 0;

vector<word> words;
vector<sentence> sentences;
vector<book> books;
vector<chapter> chapters;
vector<verse> verses;

void adjust_suffix()
{
    for (word& w : words)
        w.suffix(w.presentation_after());
        
    for (const sentence& s : sentences) {
        word& w = words[s.end_monad()-1];
        w.suffix(". ");
    }
}

void adjust_psp()
{
    map<string,string> trans {
      { "A-" , "adjective" },
      { "Df" , "adverb" },
//    { "S-" , "article" },
      { "Ma" , "cardinal_numeral" },
      { "Nb" , "noun" },
      { "C-" , "conjunction" },
      { "Pd" , "demonstrative_pronoun" },
      { "F-" , "foreign_word" },
      { "Px" , "indefinite_pronoun" },
//    { "N-" , "infinitive_marker" },
      { "I-" , "interjection" },
      { "Du" , "interrogative_adverb" },
      { "Pi" , "interrogative_pronoun" },
      { "Mo" , "ordinal_numeral" },
      { "Pp" , "personal_pronoun" },
      { "Pk" , "personal_reflexive_pronoun" },
      { "Ps" , "possessive_pronoun" },
      { "Pt" , "possessive_reflexive_pronoun" },
      { "R-" , "preposition" },
      { "Ne" , "proper_noun" },
//    { "Py" , "quantifier" },
      { "Pc" , "reciprocal_pronoun" },
      { "Dq" , "relative_adverb" },
      { "Pr" , "relative_pronoun" },
      { "G-" , "subjunction" },
      { "V-" , "verb" },
//    { "X-" , "NA" }
    };


    for (word& w : words)
        w.part_of_speech(trans.at(w.part_of_speech()));
}

void adjust_morphology()
{
    map<char,string> person_trans{
        { '1', "first_person" },
        { '2', "second_person" },
        { '3', "third_person" },
//      { 'x', "uncertain_person" }
    };

    map<char,string> number_trans{
        { 's', "singular" },
//      { 'd', "dual" },
        { 'p', "plural" },
//      { 'x', "uncertain_number" }
    };


    array features{"person"s, "number"s};
    array transtable{&person_trans, &number_trans};
    
    for (word& w : words) {
        string morphology = w.morphology();

        for (int n=0; n<2; ++n) {
            if (morphology[n]!='-')
                w.morph(features[n], transtable[n]->at(morphology[n]));
        }
    }
}


tuple<string,int,int> split_ref(const string& ref)
{
    auto space = ref.find(' ');
    auto point = ref.find('.');

    return tuple{ref.substr(0,space), stoi(ref.substr(space+1, point-space-1)), stoi(ref.substr(point+1))};
}


void adjust_bcv()
{
    string last_book;
    int last_chap=0;
    int last_verse=0;
    
    for (word& w : words) {
        auto [ b, c, v ] = split_ref(w.citation_part());

        if (b!=last_book) {
            if (!books.empty()) // Adjust end of last book
                books.back().end_monad(w.monad()-1);
            
            books.emplace_back(b, w.monad());
            last_book = b;
            last_chap = -1;
            last_verse = -1;
        }

        if (c!=last_chap) {
            if (!chapters.empty()) // Adjust end of last chapter
                chapters.back().end_monad(w.monad()-1);
            
            chapters.emplace_back(b, c, w.monad());
            last_chap = c;
            last_verse = 0;
        }

        if (v!=last_verse) {
            if (!verses.empty()) // Adjust end of last verse
                verses.back().end_monad(w.monad()-1);
            
            verses.emplace_back(b, c, v, w.monad());
            last_verse = v;
        }
    }

    // Adjust end of text
    books.back().end_monad(word::last_monad);
    chapters.back().end_monad(word::last_monad);
    verses.back().end_monad(word::last_monad);
}

int main()
{
    pugi::xml_document doc;

    if (!doc.load_file("proiel-treebank/latin-nt.xml")) {
        cerr << "Cannot open proiel-treebank/latin-nt.xml\n";
        return false;
    }

    pugi::xml_node proiel_xml = doc.child("proiel").child("source");

    for (const auto & div : proiel_xml.children("div")) {
        for (const auto & titlesent : div) {
            if (titlesent.name()=="title"s)
                continue;
            
            assert(titlesent.name()=="sentence"s);

            long sentence_start_monad = word::last_monad+1;
            
            for (const auto & token : titlesent) {
                if (token.attribute("empty-token-sort"))
                    continue;
                    
                assert(token.name()=="token"s);
                assert(token.attribute("form"));

                words.emplace_back(token.attribute("citation-part").value(),
                                   token.attribute("form").value(),
                                   token.attribute("presentation-after").value(),
                                   token.attribute("lemma").value(),
                                   token.attribute("part-of-speech").value(),
                                   token.attribute("morphology").value(),
                                   token.attribute("relation").value());
            }

            long sentence_end_monad = word::last_monad;
            sentences.emplace_back(sentence_start_monad, sentence_end_monad, titlesent.attribute("presentation-after").value());
        }
    }

    adjust_suffix();
    adjust_bcv();
    adjust_psp();
    adjust_morphology();
    
    mql mql_file{"jvulgate.mql"};

    mql_file.head();
    mql_file.enums();
    mql_file.types();

    mql_file.start_word();
    for (const word& w : words)
        mql_file.make_word(w);
    mql_file.end_word();

    mql_file.start_sentence();
    for (const sentence& s : sentences)
        mql_file.make_sentence(s);
    mql_file.end_sentence();

    mql_file.start_book();
    for (const book& b : books)
        mql_file.make_book(b);
    mql_file.end_book();

    mql_file.start_chapter();
    for (const chapter& c : chapters)
        mql_file.make_chapter(c);
    mql_file.end_chapter();

    mql_file.start_verse();
    for (const verse& v : verses)
        mql_file.make_verse(v);
    mql_file.end_verse();

    mql_file.tail();
        
}