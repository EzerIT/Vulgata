for psp in "adjective" "adverb" "cardinal_numeral" "conjunction" "demonstrative_pronoun" "foreign_word" "indefinite_pronoun" "interjection" "interrogative_adverb" "interrogative_pronoun" "noun" "ordinal_numeral" "personal_pronoun" "personal_reflexive_pronoun" "possessive_pronoun" "possessive_reflexive_pronoun" "preposition" "proper_noun" "reciprocal_pronoun" "relative_adverb" "relative_pronoun" "subjunction" "verb"; do

    echo $psp
    echo "select all objects where [word psp=$psp get lemma_with_variant]" | mql -d ../jvulgate | grep word | sed -e 's/.*variant="\(.*\)").*/\1/' | sort | uniq > $psp.txt
done
