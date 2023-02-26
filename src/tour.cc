#include "railway.h"
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>

using namespace railway;
using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: ./tour <station_file> <node_file> <tour_file> "
                "<path_file>"
             << endl;
        return -1;
    }

    string station_file{argv[1]};
    string node_file{argv[2]};
    string tour_file{argv[3]};
    string path_file{argv[4]};

    vector<Station> stations = readStations(station_file);
    StationRepository stationRepository(stations);

    vector<Node> nodes = readNode(node_file);
    NodeRepository nodeRepository;
    for (const Node &node : nodes) {
        nodeRepository.addNode(node);
    }

    vector<int> tour = readTour(tour_file);

    vector<Path> paths = readShortestPath(path_file);
    PathRepository pathRepository(paths);

    json j;
    j["tour"] = json::array();
    for (int i = 0; i < tour.size(); ++i) {
        Node current_node = nodeRepository.getNodeById(tour[i]).value();
        int to_node_id = i < tour.size() - 1 ? tour[i + 1] : tour[0];

        while (current_node.node_id != to_node_id) {
            Station station =
                stationRepository.getStationByCode(current_node.station_code)
                    .value();

            j["tour"].push_back({
                {"station_code", station.station_code},
            });
            int next_node_id =
                pathRepository.getNext(current_node.node_id, to_node_id);
            current_node = nodeRepository.getNodeById(next_node_id).value();
        }
    }
    cout << j << endl;

    return 0;
}
