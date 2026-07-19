
#include "IncludeCallback.h"

#include <utils/CString.h>

#include <unordered_map>
#include <string>

// Functor used for test cases.
class MockIncluder {

public:

    MockIncluder& sourceForInclude(const std::string& include, const std::string& source) {
        mIncludeMap[include].source = source;
        return *this;
    }

    // Assert that the expected includer is responsible for including a given include file.
    MockIncluder& expectIncludeIncludedBy(const std::string& include, const std::string& includer) {
        mIncludeMap[include].expectedIncluder = includer;
        return *this;
    }

    bool operator()(const utils::CString& includedBy, matp::IncludeResult& result) {
        auto key = result.includeName.c_str();
        auto found = mIncludeMap.find(key);

        if (found == mIncludeMap.end()) {
            return false;
        }

        auto include = found->second;

        if (!include.expectedIncluder.empty()) {
            EXPECT_STREQ(includedBy.c_str_safe(), include.expectedIncluder.c_str());
        }

        if (!include.source.empty()) {
            result.text = utils::CString(include.source.c_str());
            result.name = result.includeName;
            return true;
        }

        return false;
    }

private:

    struct Include {
        std::string source;
        std::string expectedIncluder;
    };

    std::unordered_map<std::string, Include> mIncludeMap;

};
