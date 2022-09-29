#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <tuple>
#include <functional>
#include <cctype>
#include <stdexcept>

#include "rapidcsv/src/rapidcsv.h"


//#include "pugixml.hpp"
#include "makemql.hpp"
#include "parse_latin_vc.hpp"
#include "read_inflection.hpp"

using namespace std;



long last_monad = 0;

vector<vector<string>> words;
vector<pair<long,long>> sentences;

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

void trim_all()
{
    for (vector<string>& w : words) {
        for (string& x : w) {
            while (x[0]==' ') x = x.substr(1);
            while (x.back()==' ') x.pop_back();
        }
    }
}

void adjust_suffix()
{
    for (vector<string>& w : words) {
        char lastch = w[SURFACE].back();
        switch (lastch) {
          case '-':
                w.push_back("");
                break;
                
          case ',':
          case ';':
                w.push_back(string{lastch} + " ");
                w[SURFACE].pop_back();
                break;
                
          default:
                w.push_back(" ");
                break;
        }
    }

    for (const pair<long,long>& s : sentences)
        words[s.second-1][SUFFIX] = ". ";
}


// Look for lemmas of the form "abcd#2", where 2 is the variant
void adjust_lemma()
{
    map<tuple<string,int,string>,int> lemmacount;  // <lemma,variant,part_of_speech> => count
    map<tuple<string,int,string>,int> lemmarank;   // <lemma,variant,part_of_speech> => rank

    for (const vector<string>& w : words) {
        ++lemmacount[tuple{w[LEMMA], 0, w[PSP]}];
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

    for (vector<string>& w : words) {
        auto key = tuple{w[LEMMA], 0, w[PSP]};
        w.push_back(to_string(lemmacount[key]));
        w.push_back(to_string(lemmarank[key]));
    }

//    for (auto x : lemmacount)
//        cout << x.second << ":" << get<0>(x.first) << " " << get<1>(x.first) << " " << get<2>(x.first) << "\n";
}


void adjust_bcv()
{
    string last_book;
    int last_chap=0;
    int last_verse=0;
    
    for (vector<string>& w : words) {
        string b = "Villa_Claudiae";
        int c = 1;
        int v = stoi(w[REF]);

//		lexitem lexi{w.lemma(), w.lemma_variant(), w.part_of_speech(), w.occurrences(), b, c, v};
		lexitem lexi{w[LEMMA], 0, w[PSP], stoi(w[OCCURRENCES]), b, c, v};
		auto key = lexi.make_key();
		if (!lexicon.contains(key))
			lexicon[key] = lexi;
            
        if (b!=last_book) {
            if (!books.empty()) // Adjust end of last book
                books.back().end_monad(stoi(w[MONAD])-1);
            
            books.emplace_back(b, stoi(w[MONAD]));
            last_book = b;
            last_chap = -1;
            last_verse = -1;
        }

        if (c!=last_chap) {

            if (!chapters.empty()) // Adjust end of last chapter
                chapters.back().end_monad(stoi(w[MONAD])-1);
            
            chapters.emplace_back(b, c, stoi(w[MONAD]));
            last_chap = c;
            last_verse = 0;
        }

        if (v!=last_verse) {
            if (!verses.empty()) // Adjust end of last verse
                verses.back().end_monad(stoi(w[MONAD])-1);

            verses.emplace_back(b, c, v, w[REF], stoi(w[MONAD]));
            last_verse = v;
        }
    }

    // Adjust end of text
    books.back().end_monad(last_monad);
    chapters.back().end_monad(last_monad);
    verses.back().end_monad(last_monad);
}

void adjust_morph()
{
    for (vector<string>& w : words) {
        for (int i=PERSON; i<INFLECTION; ++i)
            if (w[i].empty())
                w[i] = "NA";

        if (w[INFLECTION]=="ja")
            w[INFLECTION] = "inflecting";
        else if (w[INFLECTION]=="nej" || w[INFLECTION]=="Nej" || w[INFLECTION].empty())
            w[INFLECTION] = "non_inflecting";
        else {
            cerr << "Word: " << w[SURFACE] << " INFLECTION=" << w[INFLECTION] << "\n";
        }

        if (!w[CONJUGATION].empty()) {
            assert(w[PSP]=="verb");
            add_conjugation(w[LEMMA], w[CONJUGATION]);
        }
        if (!w[DECLENSION].empty()) {
            assert(w[PSP]=="noun" || w[PSP]=="adjective");
            assert(!w[STEM].empty());
            if (w[PSP]=="noun")
                add_noun_declension(w[LEMMA], w[DECLENSION], w[STEM]);
            else
                add_adj_declension(w[LEMMA], w[DECLENSION], w[STEM]);
        }
        if (!w[STEM].empty()) {
            if (w[PSP]!="noun" && w[PSP]!="adjective") {
                // Ignore stem
                w[STEM]="NA";
            }
        }

    }
}



// Note: This function relies on lemma variants having been set by adjust_lemma(). Therefore this
// function must be called after adjust_lemma().
void add_inflection()
{
    for (vector<string>& w : words) {
        string lemma_with_v = w[LEMMA];

        try {
            if (w[PSP]=="noun") {
                w[DECLENSION] = noun_decl_map.at(lemma_with_v);
                w[STEM] = noun_stem_map.at(lemma_with_v);
            }
            else if (w[PSP]=="adjective") {
                w[DECLENSION] = adj_decl_map.at(lemma_with_v);
                w[STEM] = adj_stem_map.at(lemma_with_v);
            }
            else if (w[PSP]=="verb") {
                w[CONJUGATION] = conj_map.at(lemma_with_v);
            }
        }
        catch (const out_of_range& e) {
            cerr << "Missing inflection information for '" << lemma_with_v << "'\n";
        }
    }
}

void print_lexicon()
{
    ofstream lexfile{"lexifile.sql"};

    lexfile << "INSERT INTO `bol_lexicon_latin2` (`id`,`lemma`,`part_of_speech`,`sortorder`,`tally`,`firstbook`,`firstchapter`,`firstverse`) VALUES\n";

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
    read_inflection_spreadsheets();

    rapidcsv::Document VC;

    try {
        VC.Load("Villa Claudiae.csv",
                rapidcsv::LabelParams{},
                rapidcsv::SeparatorParams{',', true /* trim */, rapidcsv::sPlatformHasCR, true /* allow multiple lines */});
    }
    catch (const ios_base::failure& e) {
        cerr << e.what() << "\n";
        cerr << "Cannot open 'Villa Claudiae.csv'\n";
        exit(1);
    }

    long sentence_start_monad = 1;


    for (size_t rix=0; rix<VC.GetRowCount(); ++rix) {
        vector<string> row = VC.GetRow<string>(rix);

        if (row[0]=="Bibelreference") {
            // Start of new sentence
            if (last_monad!=0)
                sentences.emplace_back(pair<long,long>{sentence_start_monad, last_monad});
            sentence_start_monad = last_monad+1;
        }
        else {
            row.push_back(to_string(++last_monad));
            words.emplace_back(row);
        }
    }

    sentences.emplace_back(pair<long,long>{sentence_start_monad, last_monad});

    
    trim_all();
    
    adjust_suffix();

    adjust_lemma();
    adjust_bcv();

    adjust_morph();
    
    add_inflection(); // Note: Must be called after adjust_lemma()

    
    mql mql_file{"VC.mql"};

    mql_file.head();
    mql_file.enums();
    mql_file.types();

    mql_file.start_word();
    for (const vector<string>& w : words)
        mql_file.make_word(w);
    mql_file.end_word();

    mql_file.start_sentence();
    for (const pair<long,long>& s : sentences)
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
