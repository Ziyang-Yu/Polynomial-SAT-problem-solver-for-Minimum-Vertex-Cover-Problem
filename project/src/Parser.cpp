#include <stdexcept>

#include "Parser.h"

int Parser::GetVertexCount(std::string line) {
    if (line.size() < 2 || line[0] != 'V' || line[1] != ' ') {
        throw std::invalid_argument("wrong format when parsing vertex count");
    }

    std::string countRaw = line.substr(2);
    int count = getIntFromString(countRaw, "not able to covert vertex count to int when parsing vertex count");
    if (count < 2) {
        throw std::invalid_argument("there should be at least 2 vertices");
    }

    return count;
}

std::vector<std::pair<int, int>> Parser::GetEdges(std::string line, int nVertex) {
    auto lineSize = line.size();
    if (lineSize < 4 || line[0] != 'E' || line[1] != ' ' || line[2] != '{' || line[lineSize - 1] != '}') {
        throw std::invalid_argument("wrong format when parsing edges");
    }

    auto result = std::vector<std::pair<int, int>>();

    std::string edgesRaw = line.substr(3, lineSize - 4);
    auto edgesRawSize = edgesRaw.size();

    if (edgesRawSize == 0) {
        return result;
    }

    int state = 0;
    int numberStartIndex = 0;
    int firstNumber = 0;
    unsigned i = 0;
    while (i < edgesRawSize) {
        if (state == 0) {
            // accepting '<', ending with '<'
            if (edgesRaw[i] != '<') {
                throw std::invalid_argument("expecting '<' when parsing edges");
            }
            numberStartIndex = i + 1;
            state = 1;
        }
        else if (state == 1) {
            // accepting first vertex number, ending with ','
            if (edgesRaw[i] == ',') {
                std::string numberRaw = edgesRaw.substr(numberStartIndex, i - numberStartIndex);
                firstNumber = getIntFromString(numberRaw, "not able to convert vertex index to int when parsing edges");
                checkVertexIndex(firstNumber, nVertex);
                numberStartIndex = i + 1;
                state = 2;
            }
        }
        else if (state == 2) {
            // accepting second vertex number, ending with '>'
            if (edgesRaw[i] == '>') {
                std::string numberRaw = edgesRaw.substr(numberStartIndex, i - numberStartIndex);
                int secondNumber = getIntFromString(numberRaw, "not able to convert vertex index to int when parsing edges");
                checkVertexIndex(secondNumber, nVertex);
                result.push_back(std::pair<int, int>(firstNumber, secondNumber));
                state = 3;
            }
        }
        else if (state == 3) {
            // accepting ',', ending with ','
            if (edgesRaw[i] != ',') {
                throw std::invalid_argument("expecting ',' when parsing edges");
            }
            state = 0;
        }

        i++;
    }

    if (state != 3) {
        throw std::invalid_argument("wrong format when parsing edges");
    }

    return result;
}

int Parser::getIntFromString(std::string intStr, std::string errorMessage) {
    if (intStr.size() == 0) {
        throw std::invalid_argument(errorMessage);
    }

    if (intStr.size() == 1) {
        if (!std::isdigit(intStr[0])) {
            throw std::invalid_argument(errorMessage);
        }
        return intStr[0] - '0';
    }

    if (intStr[0] == '0') {
        throw std::invalid_argument(errorMessage);
    }

    for (unsigned i = 0; i < intStr.size(); i++) {
        if (!std::isdigit(intStr[i])) {
            throw std::invalid_argument(errorMessage);
        }
    }

    try {
        return std::stoi(intStr);
    }
    catch (...) {
        throw std::invalid_argument(errorMessage);
    }
}

void Parser::checkVertexIndex(int vertexIndex, int nVertex) {
    if (vertexIndex <= 0) {
        throw std::invalid_argument("vertex index should be greater than 0");
    }

    if (vertexIndex > nVertex) {
        throw std::invalid_argument("vertex index exceeds the maximum allowed value");
    }
}
