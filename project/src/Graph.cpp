#include <memory>
#include <stdexcept>

#include "Graph.h"
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

std::vector<std::set<int>> copyAdjVertices(std::vector<std::set<int>> source) {
    auto size = source.size();
    auto newAdjVertices = std::vector<std::set<int>>(size);
    for (unsigned i = 0; i < size; i++) {
        newAdjVertices[i] = std::set<int>(source[i]);
    }
    return newAdjVertices;
}

Graph::Graph(int nVertex, std::vector<std::pair<int, int>> edges) {
    this->nVertex = nVertex;
    this->hasEdges = false;
    this->adjVertices = std::vector<std::set<int>>(nVertex + 1);

    auto nEdges = edges.size();
    if (nEdges > 0) {
        this->hasEdges = true;
    }

    for (unsigned i = 0; i < nEdges; i++) {
        int vertex_1 = edges[i].first;
        int vertex_2 = edges[i].second;

        // duplicate edges are automatically ignored
        this->adjVertices[vertex_1].insert(vertex_2);
        this->adjVertices[vertex_2].insert(vertex_1);
    }
}

std::vector<int> Graph::GetMinimalVertexCover_CNF_SAT() {
    if (!this->hasEdges) {
        return std::vector<int>();
    }

    auto result = std::vector<int>();

    // Check vertex cover with size in [1, nVertex-1] with binary search
    int minSizeLeft = 1;
    int minSizeRight = nVertex-1;
    while (minSizeLeft <= minSizeRight) {
        int minSizeMiddle = (minSizeLeft + minSizeRight) / 2;
        bool hasCover = this->getVertexCoverOfSize_CNF_SAT(minSizeMiddle, result);
        if (hasCover) {
            minSizeRight = result.size() - 1;
        } else {
            minSizeLeft = minSizeMiddle + 1;
        }
    }

    // No need to solve if vertex cover size is same as nVertex, assign the result directly with all vertices
    // This can happen for graphs containing only self loops
    if (result.size() == 0) {
        for (int i = 0; i < nVertex; i++) {
            result.push_back(i+1);
        }
    }

    return result;
}

void Graph::appendSummationClauses_CNF_SAT(const std::unique_ptr<Minisat::Solver> &solver, const std::vector<Minisat::Lit> &linkingVariables, const std::vector<Minisat::Lit> &inputVariables, int left, int right) {
    // linkingVariables[0] is forced to be 1, linkingVariables[linkingVariables.size()-1] is forced to be 0, and in between is the actual value of linkingVariables
    solver->addClause(linkingVariables[0]);
    solver->addClause(~linkingVariables[linkingVariables.size()-1]);

    if (left == right) {
        return;
    }

    // Split to a: [left, middle] and b: [middle+1, right], clauses will be equivilant to (linkingVariables = a + b)
    // values are in in unary representation (e.g. 5-bit 3 is encoded as 11100), and each bit is a variable in the solver
    int middle = (left + right) / 2;

    // left part
    auto leftVariables = std::vector<Minisat::Lit>();
    leftVariables.push_back(Minisat::mkLit(solver->newVar()));
    if (left == middle) {
        // reaches leaf node, use the correcponindg input variable
        leftVariables.push_back(inputVariables[left]);
    }
    else {
        for (int i = 0; i < middle-left+1; i++) {
            leftVariables.push_back(Minisat::mkLit(solver->newVar()));
        }
    }
    leftVariables.push_back(Minisat::mkLit(solver->newVar()));

    // right part
    auto rightVariables = std::vector<Minisat::Lit>();
    rightVariables.push_back(Minisat::mkLit(solver->newVar()));
    if (right == middle + 1) {
        // reaches leaf node, use the correcponindg input variable
        rightVariables.push_back(inputVariables[right]);
    } else {
        for (int i = 0; i < right-middle; i++) {
            rightVariables.push_back(Minisat::mkLit(solver->newVar()));
        }
    }
    rightVariables.push_back(Minisat::mkLit(solver->newVar()));

    // linkingVariables = leftVariables + rightVariables
    for (unsigned sum = 0; sum <= linkingVariables.size()-2; sum++) {
        for (unsigned a = 0; a <= leftVariables.size()-2; a++) {
            unsigned b = sum - a;
            if (b >= 0 && b <= rightVariables.size()-2) {
                solver->addClause(~leftVariables[a], ~rightVariables[b], linkingVariables[sum]);
                solver->addClause(leftVariables[a+1], rightVariables[b+1], ~linkingVariables[sum+1]);
            }
        }
    }

    // recursion
    this->appendSummationClauses_CNF_SAT(solver, leftVariables, inputVariables, left, middle);
    this->appendSummationClauses_CNF_SAT(solver, rightVariables, inputVariables, middle+1, right);
}

/*
    Encoding with https://www.cs.toronto.edu/~fbacchus/csc2512/Assignments/Bailleux-Boufkhad2003_Chapter_EfficientCNFEncodingOfBooleanC.pdf
*/
bool Graph::getVertexCoverOfSize_CNF_SAT(int size, std::vector<int> &result) {
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());

    auto variables = std::vector<Minisat::Lit>();
    variables.push_back(Minisat::Lit());
    for (int i = 1; i <= this->nVertex; i++) {
        variables.push_back(Minisat::mkLit(solver->newVar()));
    }

    auto linkingVariables = std::vector<Minisat::Lit>();
    for (int i = 0; i < this->nVertex + 2; i++) {
        linkingVariables.push_back(Minisat::mkLit(solver->newVar()));
    }

    this->appendSummationClauses_CNF_SAT(solver, linkingVariables, variables, 1, nVertex);

    // value of linkingVariables is equal to size
    for (int i = 1;i <= size; i++) {
        solver->addClause(linkingVariables[i]);
    }
    for (unsigned i = size+1;i < linkingVariables.size()-1; i++) {
        solver->addClause(~linkingVariables[i]);
    }

    // edges
    for (int i = 1; i <= this->nVertex; i++) {
        auto vertices = this->adjVertices[i];
        for (auto iterator = vertices.begin(); iterator != vertices.end(); iterator++) {
            if (*iterator >= i) {
                auto clause = std::unique_ptr<Minisat::vec<Minisat::Lit>>(new Minisat::vec<Minisat::Lit>());
                solver->addClause(variables[i], variables[*iterator]);
            }
        }
    }

    bool isSat = solver->solve();
    if (!isSat) {
        return false;
    }

    auto newResult = std::vector<int>();

    // iterate from lowest index to highest so it is in ascending order
    for (int i = 1; i <= this->nVertex; i++) {
        if (Minisat::toInt(solver->modelValue(variables[i])) == Minisat::toInt(Minisat::l_True)) {
            newResult.push_back(i);
        }
    }

    result = newResult;
    return true;
}

// 1. Pick a vertex of highest degree (most incident edges). Add it to your vertex cover and
// throw away all edges incident on that vertex. Repeat till no edges remain. We will call
// this algorithm APPROX-VC-1.

void Graph::getVertexCoverWithAPPROX_VC_1(std::vector<int> &result){
    if (!this->hasEdges) {
        result = std::vector<int>();
        return;
    }

    auto adjVertices = copyAdjVertices(this->adjVertices);

    std::vector<int> degree(nVertex+1,0);
    std::vector<int> vertexCover;

    for(int i=1;i<=nVertex;i++){
        degree[i] = adjVertices[i].size();
    }

    while(true){
        int maxDegree = 0;
        int maxDegreeVertex = 0;
        for(int i=1;i<=nVertex;i++){
            if(degree[i]>maxDegree){
                maxDegree = degree[i];
                maxDegreeVertex = i;
            }
        }
        vertexCover.push_back(maxDegreeVertex);
        for(auto it = adjVertices[maxDegreeVertex].begin();it!=adjVertices[maxDegreeVertex].end();it++){
            degree[*it]--;
            adjVertices[*it].erase(maxDegreeVertex);
        }
        degree[maxDegreeVertex] = 0;

        int sum = 0;
        for (unsigned i = 1; i < degree.size(); ++i) {
            sum += degree[i];
        }

        if (sum == 0) {
            break;
        }

    }

    std::sort(vertexCover.begin(),vertexCover.end());
    result = vertexCover;
}


// 2. Pick an edge ⟨u,v⟩, and add both u and v to your vertex cover. Throw away all edges
// attached to u and v. Repeat till no edges remain. We will call this algorithm APPROXVC-2.

std::tuple<int, int> getRandomPair(std::set<std::tuple<int, int>>& tupleSet) {
    // Seed for the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Get an iterator to a random element in the set
    auto randomIterator = std::next(std::begin(tupleSet), std::uniform_int_distribution<>(0, tupleSet.size() - 1)(gen));

    // Dereference the iterator to get the randomly selected element
    auto randomElement = *randomIterator;

    return randomElement;
}

void Graph::getVertexCoverWithAPPROX_VC_2(std::vector<int> &result){
    if (!this->hasEdges) {
        result = std::vector<int>();
        return;
    }

    auto adjVertices = copyAdjVertices(this->adjVertices);

    std::vector<int> vertexCover;

    std::set<std::tuple<int, int>> tupleSet;
    for (int i = 1; i <= nVertex; ++i) {
        for (auto it = adjVertices[i].begin(); it != adjVertices[i].end(); ++it) {
            tupleSet.insert(std::make_tuple(i, *it));
        }
    }

    while(true){
        std::tuple<int, int> randomEdge = getRandomPair(tupleSet);
        tupleSet.erase(randomEdge);
        tupleSet.erase(std::make_tuple(std::get<1>(randomEdge), std::get<0>(randomEdge)));
        vertexCover.push_back(std::get<0>(randomEdge));
        vertexCover.push_back(std::get<1>(randomEdge));
        for(auto it = adjVertices[std::get<0>(randomEdge)].begin();it!=adjVertices[std::get<0>(randomEdge)].end();it++){
            adjVertices[*it].erase(std::get<0>(randomEdge));
            tupleSet.erase(std::make_tuple(*it, std::get<0>(randomEdge)));
            tupleSet.erase(std::make_tuple(std::get<0>(randomEdge), *it));
        }
        for(auto it = adjVertices[std::get<1>(randomEdge)].begin();it!=adjVertices[std::get<1>(randomEdge)].end();it++){
            adjVertices[*it].erase(std::get<1>(randomEdge));
            tupleSet.erase(std::make_tuple(*it, std::get<1>(randomEdge)));
            tupleSet.erase(std::make_tuple(std::get<1>(randomEdge), *it));
        }

        if (tupleSet.size() == 0) {
            break;
        }

    }

    std::sort(vertexCover.begin(),vertexCover.end());
    result = vertexCover;
}
