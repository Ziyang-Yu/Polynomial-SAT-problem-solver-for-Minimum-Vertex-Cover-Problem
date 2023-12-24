#include <vector>
#include <set>

#include "doctest.h"
#include "../src/Graph.h"

void validateVertexCoverResult_CNF_SAT(const std::vector<int> result, int nExpectedSize, const std::vector<std::pair<int, int>> edgesToCover) {
    REQUIRE_EQ(result.size(), nExpectedSize);
    auto coverSet = std::set<int>();

    // no need to validate if result vertices falls into nVertex, since nExpectedSize is already the minumum cover, if there is extra vertices, the cover will not cover all edges

    for (unsigned i = 0; i < result.size(); i++) {
        if (i != 0) {
            // ascending order
            REQUIRE_LT(result[i-1], result[i]);
        }
        coverSet.insert(result[i]);
    }

    // vertices can cover all edges
    for (unsigned i = 0; i < edgesToCover.size(); i++) {
        REQUIRE((coverSet.find(edgesToCover[i].first) != coverSet.end() || coverSet.find(edgesToCover[i].second) != coverSet.end()));
    }
}

void validateVertexCoverResult_APPROX_VC_2(const std::vector<int> result, std::vector<std::pair<int, int>> edgesToCover) {
    auto coverSet = std::set<int>();

    for (unsigned i = 0; i < result.size(); i++) {
        if (i != 0) {
            // ascending order
            REQUIRE_LT(result[i-1], result[i]);
        }
        coverSet.insert(result[i]);
    }

    // vertices can cover all edges
    for (unsigned i = 0; i < edgesToCover.size(); i++) {
        REQUIRE((coverSet.find(edgesToCover[i].first) != coverSet.end() || coverSet.find(edgesToCover[i].second) != coverSet.end()));
    }

    // size should be even
    REQUIRE((result.size() % 2 == 0));

    // none or both nodes of the edges should be added
    // only a partial validation: for all edges, if both vertices are in the result, remove them, and all vertices should be removed at last
    auto coverSetToOperate = std::set<int>(coverSet);
    for (unsigned i = 0; i < edgesToCover.size(); i++) {
        if (coverSet.find(edgesToCover[i].first) != coverSet.end() && coverSet.find(edgesToCover[i].second) != coverSet.end()) {
            if (coverSetToOperate.find(edgesToCover[i].first) != coverSetToOperate.end()) {
                coverSetToOperate.erase(edgesToCover[i].first);
            }
            if (coverSetToOperate.find(edgesToCover[i].second) != coverSetToOperate.end()) {
                coverSetToOperate.erase(edgesToCover[i].second);
            }
        }
    }

    REQUIRE_EQ(coverSetToOperate.size(), 0);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_Sample_1") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 5),
        std::pair<int, int>(5, 2),
        std::pair<int, int>(1, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(2, 4)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 2, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_ResultSizeIsZero") {
    std::vector<std::pair<int, int>> edges = {};

    Graph graph = Graph(2, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 0, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_ResultSizeIsOne") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 2),
    };

    Graph graph = Graph(2, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 1, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_ResultSizeIsTwo") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(4, 5),
        std::pair<int, int>(1, 2)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 2, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_RequireAllVertices") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3)
    };

    Graph graph = Graph(3, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 3, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_CheckUndirected") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(2, 1),
    };

    Graph graph = Graph(2, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 1, edges);

    std::vector<std::pair<int, int>> edges2 = {
        std::pair<int, int>(1, 2),
    };

    Graph graph2 = Graph(2, edges2);
    std::vector<int> result2 = graph2.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 1, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_ConsiderSelfLoopEdge") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 1),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(3, 3),
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 3, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_IgnoreDuplicatedEdge") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 2),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 1),
    };

    Graph graph = Graph(2, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 1, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_MaximumSize") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
    };

    Graph graph = Graph(3, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 3, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_MultipleResult") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 2),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(1, 4),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(4, 3),
    };

    Graph graph = Graph(4, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 3, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_LargeVWithOneEdge") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(50, 80),
    };

    Graph graph = Graph(100, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 1, edges);
}

TEST_CASE("GetMinimalVertexCover_CNF_SAT_ComplexExample") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(21, 4),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(3, 20),
        std::pair<int, int>(23, 14),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(13, 15),
        std::pair<int, int>(15, 22),
        std::pair<int, int>(25, 16),
        std::pair<int, int>(16, 10),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(2, 6),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(13, 19),
    };

    Graph graph = Graph(32, edges);
    std::vector<int> result = graph.GetMinimalVertexCover_CNF_SAT();
    validateVertexCoverResult_CNF_SAT(result, 8, edges);
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_1_Sample") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(2, 5),
        std::pair<int, int>(5, 4)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_1(result);
    REQUIRE_EQ(result.size(), 2);
    REQUIRE_EQ(result[0], 3);
    REQUIRE_EQ(result[1], 5);
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_1_TestAccending") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(5, 2),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(3, 4)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_1(result);
    REQUIRE_EQ(result.size(), 2);
    REQUIRE_EQ(result[0], 3);
    REQUIRE_EQ(result[1], 5);
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_1_MaxDegreeFirst") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(3, 4)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_1(result);
    REQUIRE_EQ(result.size(), 1);
    REQUIRE_EQ(result[0], 3);
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_1_Complex") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(21, 4),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(3, 20),
        std::pair<int, int>(23, 14),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(13, 15),
        std::pair<int, int>(15, 22),
        std::pair<int, int>(25, 16),
        std::pair<int, int>(16, 10),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(2, 6),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(13, 19)
    };

    Graph graph = Graph(32, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_1(result);
    REQUIRE_EQ(result.size(), 8);
    REQUIRE_EQ(result[0], 1);
    REQUIRE_EQ(result[1], 2);
    REQUIRE(((result[2] == 3 && result[3] == 4 && result[4] == 13 && result[5] == 14 && result[6] == 15 && result[7] == 16)
        || (result[2] == 3 && result[3] == 4 && result[4] == 13 && result[5] == 14 && result[6] == 16 && result[7] == 22)
        || (result[2] == 3 && result[3] == 13 && result[4] == 14 && result[5] == 15 && result[6] == 16 && result[7] == 21)
        || (result[2] == 3 && result[3] == 13 && result[4] == 14 && result[5] == 16 && result[6] == 21 && result[7] == 22)
        || (result[2] == 4 && result[3] == 13 && result[4] == 14 && result[5] == 15 && result[6] == 16 && result[7] == 20)
        || (result[2] == 4 && result[3] == 13 && result[4] == 14 && result[5] == 16 && result[6] == 20 && result[7] == 22)
        || (result[2] == 13 && result[3] == 14 && result[4] == 15 && result[5] == 16 && result[6] == 20 && result[7] == 21)
        || (result[2] == 13 && result[3] == 14 && result[4] == 16 && result[5] == 20 && result[6] == 21 && result[7] == 22)));
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_2_Sample") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(2, 5),
        std::pair<int, int>(5, 4)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_2(result);
    REQUIRE_EQ(result.size(), 4);
    REQUIRE(((result[0] == 1 && result[1] == 2 && result[2] == 3 && result[3] == 5)
        || (result[0] == 1 && result[1] == 3 && result[2] == 4 && result[3] == 5)
        || (result[0] == 2 && result[1] == 3 && result[2] == 4 && result[3] == 5)));
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_2_TestAccending") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(5, 2),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(3, 4)
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_2(result);
    REQUIRE_EQ(result.size(), 4);
    REQUIRE(((result[0] == 1 && result[1] == 2 && result[2] == 3 && result[3] == 5)
        || (result[0] == 1 && result[1] == 3 && result[2] == 4 && result[3] == 5)
        || (result[0] == 2 && result[1] == 3 && result[2] == 4 && result[3] == 5)));
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_2_BothVerticesFromPair") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(1, 2),
    };

    Graph graph = Graph(5, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_2(result);
    REQUIRE_EQ(result.size(), 2);
    REQUIRE_EQ(result[0], 1);
    REQUIRE_EQ(result[1], 2);
}

TEST_CASE("GetMinimalVertexCover_APPROX_VC_2_Complex") {
    std::vector<std::pair<int, int>> edges = {
        std::pair<int, int>(21, 4),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(3, 20),
        std::pair<int, int>(23, 14),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(13, 15),
        std::pair<int, int>(15, 22),
        std::pair<int, int>(25, 16),
        std::pair<int, int>(16, 10),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(2, 6),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(13, 19),
    };

    Graph graph = Graph(32, edges);
    std::vector<int> result = std::vector<int>();
    graph.getVertexCoverWithAPPROX_VC_2(result);
    validateVertexCoverResult_APPROX_VC_2(result, edges);
}
