/*
 * MultiwordVocab.cc --
 *	Vocabulary containing multiwords.
 *
 */

#ifndef lint
static char Copyright[] = "Copyright (c) 2001 SRI International.  All Rights Reserved.";
static char RcsId[] = "@(#)$Header: /home/srilm/devel/lm/src/RCS/MultiwordVocab.cc,v 1.1 2001/12/30 23:24:12 stolcke Exp $";
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "MultiwordVocab.h"

#include "LHash.cc"

#ifdef INSTANTIATE_TEMPLATES
INSTANTIATE_LHASH(VocabIndex, VocabIndex *);
#endif

MultiwordVocab::~MultiwordVocab()
{
    LHashIter< VocabIndex, VocabIndex * > iter(multiwordMap);
    VocabIndex **expansion;
    VocabIndex word;

    /*
     * free expansion strings
     */
    while (expansion = iter.next(word)) {
	delete [] *expansion;
    }
}

/*
 * Add word to vocabulary.  Also, construct a mapping from multiword vocab
 * indices to strings of indices corresponding to their components.
 */
VocabIndex
MultiwordVocab::addWord(VocabString name)
{
    /*
     * First, add string to vocabulary.
     */
    VocabIndex wid = Vocab::addWord(name);
    if (wid == Vocab_None) {
	return Vocab_None;
    }

    /*
     * Only record mappings for words that are actual multiwords.
     * Nothing to be done if the word is already recorded in multiwordMap.
     */
    if (strchr(name, *multiChar) != NULL && !multiwordMap.find(wid)) {
	/*
	 * split multiword
	 */
	char wordString[strlen(name) + 1];
	VocabIndex widString[maxWordsPerLine + 1];

	strcpy(wordString, name);

	char *cp = strtok(wordString, multiChar);
	assert(cp != 0);

	unsigned numWords = 0;
	do {
	    assert(numWords <= maxWordsPerLine);

	    widString[numWords] = Vocab::addWord(cp);

	    /*
	     * If adding the component word to the Vocab fails for some 
	     * reason we fail the whole operation and undo adding the 
	     * multiword.
	     * XXX: The component words that have been added as a side-effect
	     * will remain.
	     */
	    if (widString[numWords] == Vocab_None) {
		Vocab::remove(wid);
		return Vocab_None;
	    }

	    numWords ++;
	} while (cp = strtok((char *)0, multiChar));

	widString[numWords] = Vocab_None;

	VocabIndex *wids = new VocabIndex[numWords + 1];
	assert(wids != 0);

	Vocab::copy(wids, widString);

	*(multiwordMap.insert(wid)) = wids;
    }

    return wid;
}

/*
 * Remove word from vocabulary.  The multiword mapping for the word, if any,
 * is also removed.
 */
void
MultiwordVocab::remove(VocabString name)
{
    VocabIndex wid = getIndex(name);

    if (wid != Vocab_None) {
	remove(wid);
    }
}

void
MultiwordVocab::remove(VocabIndex index)
{
    VocabIndex **expansion = multiwordMap.remove(index);

    if (expansion) {
	delete [] *expansion;
    }

    Vocab::remove(index);
}

/*
 * Expand a string of multiwords into components
 *	return length of expanded string
 */
unsigned
MultiwordVocab::expandMultiwords(const VocabIndex *words,
				VocabIndex *expanded, unsigned maxExpanded,
				Boolean reverse)
{
    unsigned j = 0;

    for (unsigned i = 0; words[i] != Vocab_None; i ++) {

	if (j == maxExpanded) {
	    break;
	}

	VocabIndex **comps = multiwordMap.find(words[i]);

	/*
	 * if no expansion is defined use the identity
	 */
	if (comps == 0) {
	    expanded[j++] = words[i];
	} else {
	    unsigned compsLength = Vocab::length(*comps);

	    for (unsigned k = 0; k < compsLength; k ++) {

		if (j == maxExpanded) {
		    break;
		}

		if (reverse) {
		    expanded[j] = (*comps)[compsLength - 1 - k];
		} else {
		    expanded[j] = (*comps)[k];
		}
		j ++;
	    }
	}
    }

    expanded[j] = Vocab_None;
    return j;
}

