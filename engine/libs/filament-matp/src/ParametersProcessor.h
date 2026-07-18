/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_PARAMETERSPROCESSOR_H
#define TNT_PARAMETERSPROCESSOR_H

#include <unordered_map>
#include <string>
#include <variant>

#include "JsonishLexeme.h"
#include "JsonishParser.h"

#include <filamat/MaterialBuilder.h>
#include <utils/Status.h>

namespace matp {

class ParametersProcessor {

public:
    ParametersProcessor();
    ~ParametersProcessor() = default;
    utils::Status process(filamat::MaterialBuilder& builder, const JsonishObject& jsonObject);
    utils::Status process(filamat::MaterialBuilder& builder, const std::string& key, const std::string& value);

private:

    using Callback = utils::Status (*)(filamat::MaterialBuilder& builder, const JsonishValue& value);

    struct ParameterInfo {
        Callback callback;
        JsonishValue::Type rootAssert;
    };

    std::unordered_map<std::string, ParameterInfo> mParameters;
};

} // namespace matp

#endif //TNT_PARAMETERSPROCESSOR_H
