#include "railway.h"
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>

using namespace railway;
using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./station <station_file>" << endl;
        return -1;
    }

    string station_file{argv[1]};
    vector<Station> stations = readStations(station_file);

    json j;
    j["stations"] = json::array();
    Prefectures prefecture;
    for (const Station &station : stations) {
        j["stations"].push_back(
            {{"station_code", station.station_code},
             {"station_group_code", station.station_group_code},
             {"station_name", station.station_name},
             {"line_code", station.line_code},
             {"prefecture",
              prefecture.getPrefectureNameById(station.prefecture_code)},
             {"post", station.post},
             {"address", station.address},
             {"lon", station.lon},
             {"lat", station.lat}});
    }
    cout << j << endl;

    return 0;
}
