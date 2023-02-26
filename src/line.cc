#include "railway.h"
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>

using namespace railway;
using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./line <line_file>" << endl;
        return -1;
    }

    string line_file{argv[1]};
    vector<Line> lines = readLines(line_file);

    json j;
    j["lines"] = json::array();
    for (const Line &line : lines) {
        j["lines"].push_back(
            {{"line_code", line.line_code}, {"line_name", line.line_name}});
    }
    cout << j << endl;

    return 0;
}
