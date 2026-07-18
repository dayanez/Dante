/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_JSONLEXER_H
#define TNT_JSONLEXER_H

#include "Lexer.h"
#include "JsonishLexeme.h"

namespace matp {

// Breaks down a stream of chars into JsonLexemes.
class JsonishLexer final: public Lexer<JsonLexeme> {
public:
    virtual bool readLexeme() noexcept;
private:
    JsonType readIdentifier() noexcept;
    void readNumber() noexcept;
    void readString() noexcept;
    JsonType readPunctuation() noexcept;
};

} // namespace matp

#endif //TNT_JSONLEXER_H
