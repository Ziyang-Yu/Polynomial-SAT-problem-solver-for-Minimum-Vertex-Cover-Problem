#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

class Parser {
public:
    /*
    Get vertex count from the command line
    Throws if:
        Not in format "V[ ][i]"
        i is not valid int
        i is not greater than 1
    */
    int GetVertexCount(std::string line);

    /*
    Get edges from the command line
    Throws if:
        Not in format "E[ ]{<[i],[i]>(,<[i],[i]>)*}"
        i is not valid int
        i is not in range [1, nVertex]
    */
    std::vector<std::pair<int, int>> GetEdges(std::string line, int nVertex);

private:
    int getIntFromString(std::string intStr, std::string errorMessage);
    void checkVertexIndex(int vertexIndex, int nVertex);
};

#endif
