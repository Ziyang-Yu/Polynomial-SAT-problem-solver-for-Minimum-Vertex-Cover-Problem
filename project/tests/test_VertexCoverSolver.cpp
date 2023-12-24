#include "doctest.h"
#include "../src/VertexCoverSolver.h"

TEST_CASE("AcceptLine_Sample") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    auto result = solver.AcceptLine("E {<3,2>,<3,1>,<3,4>,<2,5>,<5,4>}");
    REQUIRE((result == "CNF-SAT-VC: 3,5\nAPPROX-VC-1: 3,5\nAPPROX-VC-2: 2,3,4,5\n"
            || result == "CNF-SAT-VC: 3,5\nAPPROX-VC-1: 3,5\nAPPROX-VC-2: 1,2,3,5\n"
            || result == "CNF-SAT-VC: 3,5\nAPPROX-VC-1: 3,5\nAPPROX-VC-2: 1,3,4,5\n"));
}

TEST_CASE("AcceptLine_SizeIsZero") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    REQUIRE_EQ(solver.AcceptLine("E {}"), "CNF-SAT-VC: \nAPPROX-VC-1: \nAPPROX-VC-2: \n");
}

TEST_CASE("AcceptLine_SizeIsOne") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    auto result = solver.AcceptLine("E {<5,4>}");
    REQUIRE((result == "CNF-SAT-VC: 4\nAPPROX-VC-1: 4\nAPPROX-VC-2: 4,5\n"
        || result == "CNF-SAT-VC: 4\nAPPROX-VC-1: 5\nAPPROX-VC-2: 4,5\n"
        || result == "CNF-SAT-VC: 5\nAPPROX-VC-1: 4\nAPPROX-VC-2: 4,5\n"
        || result == "CNF-SAT-VC: 5\nAPPROX-VC-1: 5\nAPPROX-VC-2: 4,5\n"));
}

TEST_CASE("AcceptLine_Empty") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine(""), "");
    REQUIRE_EQ(solver.AcceptLine(" "), "");
    REQUIRE_EQ(solver.AcceptLine("  "), "");
    REQUIRE_EQ(solver.AcceptLine(" V 15 "), "");
    auto result = solver.AcceptLine(" E {<1,2>} ");
    REQUIRE((result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"));
}

TEST_CASE("AcceptLine_EdgeOutOfRange") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    REQUIRE_THROWS(solver.AcceptLine("E {<2,10>}"));
}

TEST_CASE("AcceptLine_MutipleV") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    REQUIRE_THROWS(solver.AcceptLine("V 15"));
}

TEST_CASE("AcceptLine_VEVE_FirstVInvalid") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_THROWS(solver.AcceptLine("V 1"));
    REQUIRE_THROWS(solver.AcceptLine("E {}"));

    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    auto result = solver.AcceptLine("E {<1,2>}");
    REQUIRE((result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"));
}

TEST_CASE("AcceptLine_VEVE_SecondVInvalid") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    auto result = solver.AcceptLine("E {<1,2>}");
    REQUIRE((result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"));

    REQUIRE_THROWS(solver.AcceptLine("V 1"));
    REQUIRE_THROWS(solver.AcceptLine("E {}"));

    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    result = solver.AcceptLine("E {<1,2>}");
    REQUIRE((result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"));
}

TEST_CASE("AcceptLine_VEVE_FristEInvalid") {
    VertexCoverSolver solver = VertexCoverSolver();
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    REQUIRE_THROWS(solver.AcceptLine("E "));
    REQUIRE_EQ(solver.AcceptLine("V 5"), "");
    auto result = solver.AcceptLine("E {<1,2>}");
    REQUIRE((result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 1\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 1\nAPPROX-VC-2: 1,2\n"
        || result == "CNF-SAT-VC: 2\nAPPROX-VC-1: 2\nAPPROX-VC-2: 1,2\n"));
}
