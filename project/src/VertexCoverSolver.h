#ifndef VERTEX_COVER_SOLVER_H
#define VERTEX_COVER_SOLVER_H

#include <memory>
#include <sstream>
#include <string>

#include "Parser.h"

// The main solver to calculate the vertex cover path
class VertexCoverSolver {
public:
    VertexCoverSolver();

    /*
    Accept and handle a command line input, return output if there is.
    For V command, store the vertex count, and return empty string.
    For E command, construct the graph and solve the problem, return the result.
    */
    std::string AcceptLine(std::string AcceptLine);

private:
    // Internal state to decide which kind of command line to accept
    int state;

    // Vertex count of the current graph
    int nVertex;

    // Timeout for CNF-SAT-VC
    int timeoutInSeconds;

    // Parser to get the information from input
    std::unique_ptr<Parser> parser;

    // Helper method to append result to output
    void appendResult(std::stringstream &ss, const std::vector<int> &result);
};

#endif
