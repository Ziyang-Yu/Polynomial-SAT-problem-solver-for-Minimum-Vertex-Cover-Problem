#include <vector>

#include "doctest.h"
#include "../src/Parser.h"

TEST_CASE("GetVertexCount_Sample_1") {
    Parser lineParser = Parser();
    REQUIRE_EQ(lineParser.GetVertexCount("V 15"), 15);
}

TEST_CASE("GetVertexCount_Sample_2") {
    Parser lineParser = Parser();
    REQUIRE_EQ(lineParser.GetVertexCount("V 5"), 5);
}

TEST_CASE("GetVertexCount_LargeNumber") {
    Parser lineParser = Parser();
    REQUIRE_EQ(lineParser.GetVertexCount("V 123456789"), 123456789);
}

TEST_CASE("GetVertexCount_MinimumValid") {
    Parser lineParser = Parser();
    REQUIRE_EQ(lineParser.GetVertexCount("V 2"), 2);
}

TEST_CASE("GetVertexCount_One") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V 1"));
}

TEST_CASE("GetVertexCount_Zero") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V 0"));
}

TEST_CASE("GetVertexCount_Negative") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V -15"));
}

TEST_CASE("GetVertexCount_Decimal") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V 15.0"));
}

TEST_CASE("GetVertexCount_ExtraZeroAtFront") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V 015"));
}

TEST_CASE("GetVertexCount_SpecialCharacter") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V x"));
}

TEST_CASE("GetVertexCount_EmptyLine") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount(""));
}

TEST_CASE("GetVertexCount_MissingSpace") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V15"));
}

TEST_CASE("GetVertexCount_ExtraSpace") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V  15"));
}

TEST_CASE("GetVertexCount_MissingVertexCount") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("V "));
}

TEST_CASE("GetVertexCount_WrongCommand") {
    Parser lineParser = Parser();
    REQUIRE_THROWS(lineParser.GetVertexCount("x 15"));
}

TEST_CASE("GetEdges_Sample_1") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}", 15);

    int expectedSize = 7;
    std::vector<std::pair<int, int>> expected = std::vector<std::pair<int, int>>();
    expected.push_back(std::pair<int, int>(2, 6));
    expected.push_back(std::pair<int, int>(2, 8));
    expected.push_back(std::pair<int, int>(2, 5));
    expected.push_back(std::pair<int, int>(6, 5));
    expected.push_back(std::pair<int, int>(5, 8));
    expected.push_back(std::pair<int, int>(6, 10));
    expected.push_back(std::pair<int, int>(10, 8));

    REQUIRE_EQ(result.size(), expectedSize);
    for (int i = 0; i < expectedSize; i++) {
        REQUIRE_EQ(result[i].first, expected[i].first);
        REQUIRE_EQ(result[i].second, expected[i].second);
    }
}

TEST_CASE("GetEdges_Sample_2") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {<1,3>,<3,2>,<3,4>,<4,5>,<5,2>}", 15);

    int expectedSize = 5;
    std::vector<std::pair<int, int>> expected = std::vector<std::pair<int, int>>();
    expected.push_back(std::pair<int, int>(1, 3));
    expected.push_back(std::pair<int, int>(3, 2));
    expected.push_back(std::pair<int, int>(3, 4));
    expected.push_back(std::pair<int, int>(4, 5));
    expected.push_back(std::pair<int, int>(5, 2));

    REQUIRE_EQ(result.size(), expectedSize);
    for (int i = 0; i < expectedSize; i++) {
        REQUIRE_EQ(result[i].first, expected[i].first);
        REQUIRE_EQ(result[i].second, expected[i].second);
    }
}

TEST_CASE("GetEdges_NoEdge") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {}", 15);

    REQUIRE_EQ(result.size(), 0);
}

TEST_CASE("GetEdges_SingleEdge") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {<1,2>}", 2);

    REQUIRE_EQ(result.size(), 1);
    REQUIRE_EQ(result[0].first, 1);
    REQUIRE_EQ(result[0].second, 2);
}

TEST_CASE("GetEdges_MultipleEdges") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {<1,2>,<2,3>,<3,1>}", 3);

    REQUIRE_EQ(result.size(), 3);
    REQUIRE_EQ(result[0].first, 1);
    REQUIRE_EQ(result[0].second, 2);
    REQUIRE_EQ(result[1].first, 2);
    REQUIRE_EQ(result[1].second, 3);
    REQUIRE_EQ(result[2].first, 3);
    REQUIRE_EQ(result[2].second, 1);
}

TEST_CASE("GetEdges_AcceptSelfLoop") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {<1,1>}", 5);

    REQUIRE_EQ(result.size(), 1);
}

TEST_CASE("GetEdges_AcceptDuplicate") {
    Parser parser = Parser();
    std::vector<std::pair<int, int>> result = parser.GetEdges("E {<3,2>,<2,3>,<2,3>}", 5);

    REQUIRE_EQ(result.size(), 3);
}

TEST_CASE("GetEdges_VertexInEdgeOutOfRange") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E {<1,3>}", 2));
}

TEST_CASE("GetEdges_Zero") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E {<0,3>}", 15));
}

TEST_CASE("GetEdges_Negative") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E {<1,-3>}", 15));
}

TEST_CASE("GetEdges_Decimal") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E {<1,3.0>}", 15));
}

TEST_CASE("GetEdges_ExtraZeroAtFront") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E {<1,03>}", 15));
}

TEST_CASE("GetEdges_SpecialCharacter") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E {<x,3>}", 15));
}

TEST_CASE("GetEdges_EmptyLine") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("", 15));
}

TEST_CASE("GetEdges_MissingSpace") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E{<1,3>}", 15));
}

TEST_CASE("GetEdges_ExtraSpace") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E <1, 3>,<2,3>", 15));
}

TEST_CASE("GetEdges_MissingEdges") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E ", 15));
}

TEST_CASE("GetEdges_WrongCommand") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("x {<1,3>}", 15));
}

TEST_CASE("GetEdges_WrongFormat") {
    Parser parser = Parser();
    REQUIRE_THROWS(parser.GetEdges("E <1,3>,<2,3>", 15));
}
