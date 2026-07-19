
#include <gtest/gtest.h>

#include "TestMaterialParser.h"
#include "utils/JobSystem.h"
#include <dante-matp/MaterialParser.h>

static std::string_view jsonMaterialSourceSimple(R"(
material {
    name: test_compute,
    domain: compute,
    groupSize: [8, 8, 1],
    parameters: [
    ]
}
compute {
    void compute() {
    }
}
)");

TEST(TestParseAndComputeMaterial, JsonMaterialCompilerSimple) {
    matp::MaterialParser parser;
    TestMaterialParser testParser(parser);

    filamat::MaterialBuilder::init();
    filamat::MaterialBuilder builder;

    utils::Status result = testParser.parseMaterial(
            jsonMaterialSourceSimple.data(), jsonMaterialSourceSimple.size(), builder);

    EXPECT_EQ(result.getCode(), utils::StatusCode::OK);

    utils::JobSystem js;
    js.adopt();

    auto package = builder.build(js);

    EXPECT_TRUE(package.isValid());

    js.emancipate();
    filamat::MaterialBuilder::shutdown();
}

