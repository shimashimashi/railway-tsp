#include "railway.h"
#include <atcoder/all>
#include <bits/stdc++.h>

using namespace std;
using namespace railway;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./group <station_file> <join_file>" << endl;
        return -1;
    }

    string station_file{argv[1]};
    string join_file{argv[2]};

    vector<Station> stations = readStations(station_file);
    StationRepository stationRepository(stations);

    vector<Join> joins = readJoins(join_file);

    NodeRepository nodeRepository;
    Graph graph(&nodeRepository);
    for (const Join &join : joins) {
        if (!stationRepository.getStationByCode(join.station_code1)) {
            continue;
        }
        if (!stationRepository.getStationByCode(join.station_code2)) {
            continue;
        }

        Station station1 =
            stationRepository.getStationByCode(join.station_code1).value();
        Station station2 =
            stationRepository.getStationByCode(join.station_code2).value();

        if (!nodeRepository.getNodeByStationCode(station1.station_code)) {
            int id = nodeRepository.size();
            Node node{id, station1.station_code};
            nodeRepository.addNode(node);
        }
        if (!nodeRepository.getNodeByStationCode(station2.station_code)) {
            int id = nodeRepository.size();
            Node node{id, station2.station_code};
            nodeRepository.addNode(node);
        }

        Node node1 =
            nodeRepository.getNodeByStationCode(station1.station_code).value();
        Node node2 =
            nodeRepository.getNodeByStationCode(station2.station_code).value();
        graph.addEdge(node1, node2);
    }

    set<int> station_group_codes;
    for (const Station &station : stations) {
        if (!nodeRepository.getNodeByStationCode(station.station_code)) {
            continue;
        }
        station_group_codes.insert(station.station_group_code);
    }
    for (int code : station_group_codes) {
        vector<Station> stations =
            stationRepository.getStationsByStationGroupCode(code);
        if (stations.size() <= 1) {
            continue;
        }
        for (int i = 0; i < stations.size(); ++i) {
            Station station1 = stations[i];
            if (!nodeRepository.getNodeByStationCode(station1.station_code)) {
                continue;
            }
            Node node1 =
                nodeRepository.getNodeByStationCode(station1.station_code)
                    .value();
            for (int j = 0; j < stations.size(); ++j) {
                if (i == j) {
                    continue;
                }
                Station station2 = stations[j];
                if (!nodeRepository.getNodeByStationCode(
                        station2.station_code)) {
                    continue;
                }
                Node node2 =
                    nodeRepository.getNodeByStationCode(station2.station_code)
                        .value();
                graph.addEdge(node1, node2);
            }
        }
    }

    set<Edge> edges = graph.getEdges();
    atcoder::dsu d(graph.getNodeSize());
    for (const Edge &edge : edges) {
        auto [from, to] = edge;
        if (d.same(from, to)) {
            continue;
        }
        d.merge(from, to);
    }

    const int N = graph.getNodeSize();
    cout << "station_cd,leader" << endl;
    for (int i = 0; i < N; ++i) {
        int leader = d.leader(i);
        cout << graph.getNodeById(i).value().station_code << ","
             << graph.getNodeById(leader).value().station_code << endl;
    }

    return 0;
}
