/*
 * RefList.h --
 *	Lists of words strings identified by string ids
 *
 * Copyright (c) 1998 SRI International.  All Rights Reserved.
 *
 * @(#)$Header: /home/srilm/devel/lm/src/RCS/RefList.h,v 1.3 2002/08/08 19:11:19 stolcke Exp $
 *
 */

#ifndef _RefList_h_
#define _RefList_h_

#include <iostream.h>

#include "Boolean.h"
#include "File.h"
#include "Vocab.h"
#include "LHash.h"

typedef const char *RefString;

RefString idFromFilename(const char *filename);

class RefList 
{
public:
    RefList(Vocab &vocab);
    ~RefList();

    Boolean read(File &file, Boolean addWords = false);
    Boolean write(File &file);

    VocabIndex *findRef(RefString id);

private:
    Vocab &vocab;
    LHash<RefString, VocabIndex *> reflist;
};

#endif /* _RefList_h_ */
