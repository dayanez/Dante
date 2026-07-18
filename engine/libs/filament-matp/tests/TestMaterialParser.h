/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_TESTMATERIALPARSER_H
#define TNT_TESTMATERIALPARSER_H

#include "filamat/MaterialBuilder.h"
 #include <filament-matp/MaterialParser.h>

class TestMaterialParser {
public:
    explicit TestMaterialParser(const matp::MaterialParser& materialParser) :
            mMaterialParser(materialParser) {}

        utils::Status parseMaterial(const char* buffer, size_t size, filamat::MaterialBuilder& builder)
        noexcept{
            return mMaterialParser.parseMaterial(buffer, size, builder);
        }

        utils::Status parseMaterialAsJSON(const char* buffer, size_t size, filamat::MaterialBuilder& builder)
        noexcept{
            return mMaterialParser.parseMaterialAsJSON(buffer, size, builder);
        }

private:
    const matp::MaterialParser mMaterialParser;
};

#endif // TNT_TESTMATERIALPARSER_H
