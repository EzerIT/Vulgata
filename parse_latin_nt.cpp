#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <tuple>
#include <functional>
#include <cctype>

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

map<tuple<string,int,string>,lexitem> lexicon; // <lexeme,variant,part_of_speech> => lexeme item

// Normalizes the lemma by makint it all lowercase and removing text in parentheses
void lexitem::make_sortorder()
{
    string so = m_lemma;

    for (char& c : so)
        c = tolower(c);

    auto pos1 = so.find('(');
    if (pos1==string::npos) // String does not contain a (
        m_sortorder = so;
    else {
        auto pos2 = so.find(')');
        assert(pos2>pos1);

        // We assume that there is at most one (...) pair

        m_sortorder = so.substr(0,pos1) + so.substr(pos2+1);
    }
}

void adjust_suffix()
{
    for (word& w : words)
        w.suffix(w.presentation_after());
        
    for (const sentence& s : sentences) {
        word& w = words[s.end_monad()-1];
        w.suffix(". ");
    }
}

// Remove variant from these words:
set remove_variant{"Apollo"s, "colligo"s, "comparo"s, "consterno"s, "edo"s, "eo"s, "incido"s};

// Look for lemmas of the form "abcd#2", where 2 is the variant
void adjust_lemma()
{
    map<tuple<string,int,string>,int> lemmacount;  // <lemma,variant,part_of_speech> => count
    map<tuple<string,int,string>,int> lemmarank;   // <lemma,variant,part_of_speech> => rank

    for (word& w : words) {
        string lemma = w.lemma();

        auto pos = lemma.find('#');
        if (pos!=string::npos) {
            string lemma2 = lemma.substr(0,pos);

            if (!remove_variant.contains(lemma2))
                w.lemma_variant(stoi(lemma.substr(pos+1)));

            w.lemma(lemma2);
        }

        ++lemmacount[tuple{w.lemma(), w.lemma_variant(), w.part_of_speech()}];
    }

    // Generate occurrences and rank
    multimap<int,tuple<string,int,string>,greater<int>> freq2lemma;      // Maps frequency to <lemma,variant,part_of_speech>, sorted by descending frequency
        
    for (const auto& me : lemmacount)
        freq2lemma.insert(make_pair(me.second, me.first));

    // Use frequencies to generate frequency ranks

    int rank = 1;		// Current rank
    int last_freq = 0;	// Last frequency inserted
    int last_rank = 0;	// Rank of last frequency inserted

    for (const auto& p : freq2lemma) {
        // p.first is frequency, p.second is <lemma,variant,part_of_speech>
        if (p.first==last_freq)
            lemmarank[p.second] = last_rank;
        else {
            lemmarank[p.second] = rank;
            last_freq = p.first;
            last_rank = rank;
        }
        ++rank;
    }

    for (word& w : words) {
        auto key = tuple{w.lemma(), w.lemma_variant(), w.part_of_speech()};
        w.occurrences(lemmacount[key]);
        w.frequency_rank(lemmarank[key]);
    }

//    for (auto x : lemmacount)
//        cout << x.second << ":" << get<0>(x.first) << " " << get<1>(x.first) << " " << get<2>(x.first) << "\n";
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

    map<char,string> tense_trans{
        { 'p', "present" },
        { 'i', "imperfect" },
        { 'r', "perfect" },
//      { 's', "resultative" },
//      { 'a', "aorist" },
//      { 'u', "past" },
        { 'l', "pluperfect" },
        { 'f', "future" },
        { 't', "future_perfect" },
//      { 'x', "uncertain tense" }
    };


    map<char,string> mood_trans{
        { 'i', "indicative" },
        { 's', "subjunctive" },
        { 'm', "imperative" },
//      { 'o', "optative" },
        { 'n', "infinitive" },
        { 'p', "participle" },
        { 'd', "gerund" },
        { 'g', "gerundive" },
        { 'u', "supine" },
//      { 'x', "uncertain mood" },
//      { 'y', "finiteness unspecified" },
//      { 'e', "indicative or subjunctive" },
//      { 'f', "indicative or imperative" },
//      { 'h', "subjunctive or imperative" },
//      { 't', "finite" },
    };

    map<char,string> voice_trans{
        { 'a', "active" },
//      { 'm', "middle" },
        { 'p', "passive" },
//      { 'e', "middle or passive" },
//      { 'x', "unspecified" },
    };

    map<char,string> gender_trans{
        { 'm', "masculine" },
        { 'f', "feminine" },
        { 'n', "neuter" },
        { 'p', "masculine_or_feminine" },
        { 'o', "masculine_or_neuter" },
        { 'r', "feminine_or_neuter" },
        { 'q', "masculine_or_feminine_or_neuter" },
//      { 'x', "uncertain gender" },
    };

    map<char,string> case_trans{
        { 'n', "nominative" },
        { 'a', "accusative" },
//      { 'o', "oblique" },
        { 'g', "genitive" },
//      { 'c', "genitive or dative" },
//      { 'e', "accusative or dative" },
        { 'd', "dative" },
        { 'b', "ablative" },
//      { 'i', "instrumental" },
//      { 'l', "locative" },
        { 'v', "vocative" },
//      { 'x', "uncertain case" },
//      { 'z', "no case" },
    };

    map<char,string> degree_trans{
        { 'p', "positive" },
        { 'c', "comparative" },
        { 's', "superlative" },
//      { 'x', "uncertain degree" },
//      { 'z', "no degree" },
    };

    map<char,string> strength_trans{ // Not used, but takes up a position in the "morphology=..." attribute
//      { 'w', "weak" },
//      { 's', "strong" },
//      { 't', "weak or strong" },
    };

    map<char,string> inflection_trans{
        { 'n', "non_inflecting" },
        { 'i', "inflecting" },
    };

    array features{"person"s, "number"s, "tense"s, "mood"s, "voice"s, "gender"s, "case"s, "degree"s, "strength"s, "inflection"s };
    array transtable{&person_trans, &number_trans, &tense_trans, &mood_trans, &voice_trans, &gender_trans,
                     &case_trans, &degree_trans, &strength_trans, &inflection_trans};
    vector<map<char,int>> used;

    // Build 'used' vector
    for (auto x : transtable) {
        used.push_back(map<char,int>{});
        
        for (auto p : *x)
            used.back()[p.first] = 0;
    }
    
    for (word& w : words) {
        string morphology = w.morphology();

        for (int n=0; n<features.size(); ++n) {
            if (morphology[n]!='-') {
                w.morph.emplace_back(features[n], transtable[n]->at(morphology[n]));
                ++used[n][morphology[n]];
            }
        }
    }

    for (const auto& u : used) {
        for (auto p : u)
            if (p.second==0)
                cerr << "Unused: " << features[&u - used.data()] << ":"  << p.first << "\n";
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
    map<string,string> book_trans{
        { "MATT",   "Matthew" },
        { "MARK",   "Mark" },
        { "LUKE",   "Luke" },
        { "JOHN",   "John" },
        { "ACTS",   "Acts" },
        { "ROM",    "Romans" },
        { "1COR",   "I_Corinthians" },
        { "2COR",   "II_Corinthians" },
        { "GAL",    "Galatians" },
        { "EPH",    "Ephesians" },
        { "PHIL",   "Philippians" },
        { "COL",    "Colossians" },
        { "1THESS", "I_Thessalonians" },
        { "2THESS", "II_Thessalonians" },
        { "1TIM",   "I_Timothy" },
        { "2TIM",   "II_Timothy" },
        { "TIT",    "Titus" },
        { "PHILEM", "Philemon" },
        { "HEB",    "Hebrews" },
        { "JAS",    "James" },
        { "1PET",   "I_Peter" },
        { "2PET",   "II_Peter" },
        { "1JOHN",  "I_John" },
        { "2JOHN",  "II_John" },
        { "3JOHN",  "III_John" },
        { "JUDE",   "Jude" },
        { "REV",     "Revelation" },
    };

    
    string last_book;
    int last_chap=0;
    int last_verse=0;
    
    for (word& w : words) {
        auto [ b, c, v ] = split_ref(w.citation_part());

        b = book_trans.at(b);

        lexitem lexi{w.lemma(), w.lemma_variant(), w.part_of_speech(), w.occurrences(), b, c, v};
        auto key = lexi.make_key();
        if (!lexicon.contains(key))
            lexicon[key] = lexi;
            
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
            
            verses.emplace_back(b, c, v, w.citation_part(), w.monad());
            last_verse = v;
        }
    }

    // Adjust end of text
    books.back().end_monad(word::last_monad);
    chapters.back().end_monad(word::last_monad);
    verses.back().end_monad(word::last_monad);
}

void print_lexicon()
{
    ofstream lexfile{"lexifile.sql"};

    lexfile << "INSERT INTO `bol_lexicon_latin` (`id`,`lemma`,`part_of_speech`,`sortorder`,`tally`,`firstbook`,`firstchapter`,`firstverse`) VALUES\n";

    bool first = true;
    int id = 0;
    
    for (const auto& lexi : lexicon) {
        if (!first)
            lexfile << ",\n";
        else
            first = false;
        
        lexfile << "("
                << ++id << ","
                << "\"" << lexi.second.lemma() << (lexi.second.lemma_variant()==0 ? "" :
                                                   lexi.second.lemma_variant()==1 ? " I" :
                                                   lexi.second.lemma_variant()==2 ? " II" :
                                                   lexi.second.lemma_variant()==3 ? " III" : " ?") << "\","
                << "\"" << lexi.second.part_of_speech() << "\","
                << "\"" << lexi.second.sortorder() << "\","
                << lexi.second.tally() << ","
                << "\"" << lexi.second.first_book() << "\","
                << lexi.second.first_chapter() << ","
                << lexi.second.first_verse() << ")";
    }
    lexfile << ";\n";
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
    adjust_psp();
    adjust_lemma();
    adjust_morphology();
    adjust_bcv();
    
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

    print_lexicon();
}
