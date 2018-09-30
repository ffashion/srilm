/*
 * MultiwordLM.h --
 *	Multiword wrapper language model
 *
 * A language model over a multiword vocabulary is simulated by consulting
 * a language model without multiwords.
 *
 *	p(w4_w5 | w1_w2_w3)  = p(w4 | w1 w2 w3) p(w5 | w1 w2 w3 w4)
 *
 * etc.
 *
 * Copyright (c) 2001,2002 SRI International.  All Rights Reserved.
 *
 * @(#)$Header: /home/srilm/devel/lm/src/RCS/MultiwordLM.h,v 1.3 2002/08/25 17:27:45 stolcke Exp $
 *
 */

#ifndef _MultiwordLM_h_
#define _MultiwordLM_h_

#include "LM.h"
#include "MultiwordVocab.h"

class MultiwordLM: public LM
{
public:
    MultiwordLM(MultiwordVocab &vocab, LM &lm)
      : LM(vocab), vocab(vocab), lm(lm) {};

    /*
     * LM interface
     */
    virtual LogP wordProb(VocabIndex word, const VocabIndex *context);
    virtual void *contextID(VocabIndex word, const VocabIndex *context,
							unsigned &length);
    virtual Boolean isNonWord(VocabIndex word);
    virtual void setState(const char *state);

    /*
     * Propagate changes to running state to wrapped models
     */
    virtual Boolean running() { return _running; }
    virtual Boolean running(Boolean newstate)
      { Boolean old = _running; _running = newstate; 
	lm.running(newstate); return old; };

    /*
     * Propagate changes to Debug state to wrapped models
     */
    void debugme(unsigned level)
	{ lm.debugme(level); Debug::debugme(level); };
    ostream &dout() { return Debug::dout(); };
    ostream &dout(ostream &stream)
	{ lm.dout(stream); return Debug::dout(stream); };

    MultiwordVocab &vocab;		/* multiword vocabulary */
protected:
    LM &lm;				/* wrapped model */
};


#endif /* _MultiwordLM_h_ */
