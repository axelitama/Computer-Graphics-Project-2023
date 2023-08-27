#include "CSVReader.hpp"

#include <fstream>
#include <sstream>

CSVReader::CSVReader(std::string filename, char delimiter) {
    this->filename = filename;
    this->delimiter = delimiter;
    readHeader();
    readData();
}

int CSVReader::getNumVariables() const {
    return numVariables;
}

std::vector<std::string> CSVReader::getVariableNames() const {
    return variableNames;
}

std::vector<std::string> CSVReader::getLine(int lineNumber) const {
    return data[lineNumber];
}

int CSVReader::getNumLines() const {
    return data.size();
}

void CSVReader::readHeader() {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        variableNames.push_back(token);
    }
    numVariables = variableNames.size();
}

void CSVReader::readData() {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> lineData;
        while (std::getline(ss, token, delimiter)) {
            lineData.push_back(token);
        }
        data.push_back(lineData);
    }
}
