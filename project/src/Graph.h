#ifndef GRAPH_H
#define GRAPH_H

#include <set>
#include <vector>
#include <tuple>
#include <memory>
#include <random>
#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

/*
Graph represented with adjancent list.
List are represented with set to automatically ignore duplicate edges.
*/
class Graph {
public:
    /*
    Constructor with vertex count and edges
    This assumes below:
        nVertex is greater than 1
        Vertex index in edges are in range [1, nVertex]
    */
    Graph(int nVertex, std::vector<std::pair<int, int>> edges);

    /*
    Return minimal vertex cover with CNF SAT method
    */
    std::vector<int> GetMinimalVertexCover_CNF_SAT();

    /*
    Return minimal vertex cover with Approx 1 method
    */
    void getVertexCoverWithAPPROX_VC_1(std::vector<int> &result);

    /*
    Return minimal vertex cover with Approx 2 method
    */
    void getVertexCoverWithAPPROX_VC_2(std::vector<int> &result);

private:
    // Count of vertices
    int nVertex;

    // Indicates whether there are edges in the graph
    bool hasEdges;

    // Adjancent list(set) with size equal to nVertex + 1. To avoid vertex index shiftting by ignoring the first element
    std::vector<std::set<int>> adjVertices;

    /*
    Get vertex cover of specific size with result in ascending order of vertex index
    If the cover exists, set it in result and return true, otherwise return false
    */
    bool getVertexCoverOfSize_CNF_SAT(int size, std::vector<int> &result);

    /*
    Append summation clauses for inputVariables ranges in [left, right], with its corresponding linkingVariables
    */
    void appendSummationClauses_CNF_SAT(const std::unique_ptr<Minisat::Solver> &solver, const std::vector<Minisat::Lit> &linkingVariables, const std::vector<Minisat::Lit> &inputVariables, int left, int right);
};

#endif
