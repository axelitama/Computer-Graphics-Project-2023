#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSVReader {
    private:
        std::string filename;
        char delimiter;
        int numVariables;
        std::vector<std::string> variableNames;
        std::vector<std::vector<std::string>> data;

        void readHeader();
        void readData();

    public:
        CSVReader(std::string filename, char delimiter=',');
        int getNumVariables() const;
        std::vector<std::string> getVariableNames() const;
        std::vector<std::string> getLine(int lineNumber) const;
        int getNumLines() const;
};

#endif // CSVREADER_HPP