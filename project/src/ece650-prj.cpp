#include <iostream>
#include <memory>

#include "VertexCoverSolver.h"

int main(int argc, char** argv) {
    auto solver = std::unique_ptr<VertexCoverSolver>(new VertexCoverSolver());

    while (!std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);

        try {
            auto result = solver->AcceptLine(line);
            if (!result.empty()) {
                std::cout << result;
            }
        }
        catch (std::invalid_argument &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
