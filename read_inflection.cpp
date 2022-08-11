#include <iostream>
#include <stdexcept>
#include "read_inflection.hpp"
#include "rapidcsv/src/rapidcsv.h"

using namespace std;


static string get_fix_spelling(const rapidcsv::Document& doc, size_t col, size_t row)
{
    string s = doc.GetCell<string>(col,row);
    
    if (s=="3. ") return "3.";
    if (s=="1. (-er)") return "1.";
    if (s=="indekl" || s=="indekl.") return "indeklinabel";
    if (s=="urelmæssig" || s=="uregelmæssig ") return "uregelmæssig";

    return s;
}

static string get_translate_conjugation(const rapidcsv::Document& doc, size_t col, size_t row)
{
    static map<string,string> trans{ { "1.", "first_c" },
                                     { "2.", "second_c" },
                                     { "3.", "third_c" },
                                     { "3. (-io)", "third_c_io" },
                                     { "4.", "fourth_c" },
                                     { "uregelmæssig", "irregular" } };

    try {
        return trans.at( get_fix_spelling(doc, col, row) );
    }
    catch (const out_of_range& e) {
        cerr << e.what() << "\n";
        cerr << "Problem with verb conjugation: '" << get_fix_spelling(doc, col, row) << "'\n";
        exit(1);
    }
}


static string get_translate_declension(const rapidcsv::Document& doc, size_t col, size_t row)
{
    static map<string,string> trans{ { "1.", "first_d" },
                                     { "1./2.", "first_or_second_d" },
                                     { "2.", "second_d" },
                                     { "2. (-er)", "second_d_er" },
                                     { "3.", "third_d" },
                                     { "4.", "fourth_d" },
                                     { "5.", "fifth_d" },
                                     { "indeklinabel", "indeclinable" },
                                     { "uregelmæssig", "irregular" } };

    try {
        return trans.at( get_fix_spelling(doc, col, row) );
    }
    catch (const out_of_range& e) {
        cerr << e.what() << "\n";
        cerr << "Problem with declension: '" << get_fix_spelling(doc, col, row) << "'\n";
        exit(1);
    }
}

static string get_translate_stem(const rapidcsv::Document& doc, size_t col, size_t row)
{
    static map<string,string> trans{ { "a-stamme", "a" },
                                     { "b-stamme", "b" },
                                     { "c-stamme", "c" },
                                     { "d-stamme", "d" },
                                     { "e-stamme", "e" },
                                     { "g-stamme", "g" },
                                     { "i-stamme", "i" },
                                     { "l-stamme", "l" },
                                     { "n-stamme", "n" },
                                     { "o-stamme", "o" },
                                     { "o-/a-stamme", "o_or_a" },
                                     { "p-stamme", "p" },
                                     { "r-stamme", "r" }, 
                                     { "t-stamme", "t" }, 
                                     { "u-stamme", "u" }, 
                                     { "v-stamme", "v" }, 
                                     { "indeklinabel", "indeclinable" },
                                     { "uregelmæssig", "irregular" } };

    try {
        return trans.at( get_fix_spelling(doc, col, row) );
    }
    catch (const out_of_range& e) {
        cerr << e.what() << "\n";
        cerr << "Problem with stem: '" << get_fix_spelling(doc, col, row) << "'\n";
        exit(1);
    }
}

map<string,string> conj_map; // lemma => conjugation
map<string,string> noun_decl_map; // lemma => declination
map<string,string> adj_decl_map; // lemma => declination
map<string,string> noun_stem_map; // lemma => stem
map<string,string> adj_stem_map; // lemma => stem

void read_inflection_spreadsheets()
{
    {
        rapidcsv::Document verbs;

        try {
            verbs.Load("makewordlist/verber (konjugation).csv");
        }
        catch (const ios_base::failure& e) {
            cerr << e.what() << "\n";
            cerr << "Cannot open verbs file\n";
            exit(1);
        }

        for (size_t rix=0; rix<verbs.GetRowCount(); ++rix) {
            auto [it, success] = conj_map.insert(pair{verbs.GetCell<string>(0,rix), get_translate_conjugation(verbs, 1, rix)});

            if (!success) {
                cerr << "Duplicate verb '" << verbs.GetCell<string>(0,rix) << "\n";
                exit(1);
            }
        }
    }
    
    {
        rapidcsv::Document adjectives;

        try {
            adjectives.Load("makewordlist/Adjektiver (deklination + stamme).csv");
        }
        catch (const ios_base::failure& e) {
            cerr << e.what() << "\n";
            cerr << "Cannot open adjectives file\n";
            exit(1);
        }

        for (size_t rix=0; rix<adjectives.GetRowCount(); ++rix) {
            auto [it, success] = adj_decl_map.insert(pair{adjectives.GetCell<string>(0,rix), get_translate_declension(adjectives, 1, rix)});

            if (!success) {
                cerr << "Duplicate adjective '" << adjectives.GetCell<string>(0,rix) << "\n";
                exit(1);
            }

            tie(it, success) = adj_stem_map.insert(pair{adjectives.GetCell<string>(0,rix), get_translate_stem(adjectives, 2, rix)});

            if (!success) {
                cerr << "PANIC: Duplicate adjective '" << adjectives.GetCell<string>(0,rix) << "\n";
                exit(1);
            }
        }
    }
    
    {
        rapidcsv::Document nouns;

        try {
            nouns.Load("makewordlist/substantiver (stamme).csv");
        }
        catch (const ios_base::failure& e) {
            cerr << e.what() << "\n";
            cerr << "Cannot open noun files" << "\n";
            exit(1);
        }

        for (size_t rix=0; rix<nouns.GetRowCount(); ++rix) {
            auto [it, success] = noun_decl_map.insert(pair{nouns.GetCell<string>(0,rix), get_translate_declension(nouns, 1, rix)});

            if (!success) {
                cerr << "Duplicate adjective '" << nouns.GetCell<string>(0,rix) << "\n";
                exit(1);
            }

            tie(it, success) = noun_stem_map.insert(pair{nouns.GetCell<string>(0,rix), get_translate_stem(nouns, 2, rix)});

            if (!success) {
                cerr << "PANIC: Duplicate adjective '" << nouns.GetCell<string>(0,rix) << "\n";
                exit(1);
            }
        }
    }
}
