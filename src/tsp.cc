#include "railway.h"
#include <bits/stdc++.h>
#include <omp.h>

using namespace railway;
using namespace std;

const int TOKYO = 1130101;

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: ./tsp <station_file> <join_file> <group_file> "
                "<output_dir>"
             << endl;
        return -1;
    }

    string station_file{argv[1]};
    string join_file{argv[2]};
    string group_file{argv[3]};
    string output_dir{argv[4]};

    vector<Station> stations = readStations(station_file);
    StationRepository stationRepository(stations);

    vector<Join> joins = readJoins(join_file);

    vector<Group> groups = readGroup(group_file);
    GroupRepository groupRepository(groups);

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
            if (!groupRepository.isSame(station1.station_code, TOKYO)) {
                continue;
            }
            int id = nodeRepository.size();
            Node node{id, station1.station_code};
            nodeRepository.addNode(node);
        }
        if (!nodeRepository.getNodeByStationCode(station2.station_code)) {
            if (!groupRepository.isSame(station2.station_code, TOKYO)) {
                continue;
            }
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

    const int N = graph.getNodeSize();

    set<int> station_group_codes;
    for (const Station &station : stations) {
        if (!nodeRepository.getNodeByStationCode(station.station_code)) {
            continue;
        }
        if (!groupRepository.isSame(station.station_code, TOKYO)) {
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

    vector<vector<double>> cost(N, vector<double>(N, DBL_MAX));
    set<Edge> edges = graph.getEdges();
    for (const Edge &edge : edges) {
        auto [from, to] = edge;

        Node node1 = nodeRepository.getNodeById(from).value();
        Station station1 =
            stationRepository.getStationByCode(node1.station_code).value();
        Node node2 = nodeRepository.getNodeById(to).value();
        Station station2 =
            stationRepository.getStationByCode(node2.station_code).value();

        double distance = calcDistance({station1.lat, station1.lon},
                                       {station2.lat, station2.lon});
        cost[from][to] = distance;
        cost[to][from] = distance;
    }

    vector<vector<double>> distance(N, vector<double>(N, DBL_MAX));
    vector<vector<int>> next(N, vector<int>(N, -1));
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        distance[i][i] = 0;
        priority_queue<pair<double, int>, vector<pair<double, int>>,
                       greater<pair<double, int>>>
            pq;
        pq.push({0, i});
        while (!pq.empty()) {
            auto [d, current] = pq.top();
            pq.pop();
            if (d > distance[i][current]) {
                continue;
            }
            for (int neighbor : graph.getNeighbors(current)) {
                if (d + cost[current][neighbor] < distance[i][neighbor]) {
                    distance[i][neighbor] = d + cost[current][neighbor];
                    pq.push({distance[i][neighbor], neighbor});
                    next[neighbor][i] = current;
                }
            }
        }
    }

    ofstream distance_file;
    distance_file.open(output_dir + "/shortest_path.csv", ios::out);

    vector<string> lines(N);
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        // 高速化のため複数行まとめる
        string s = "";
        for (int j = 0; j < N; ++j) {
            int n = next[i][j] == -1
                        ? -1
                        : graph.getNodeById(next[i][j]).value().node_id;
            s += to_string(n) + ",";
        }
        lines[i] = s;
    }
    for (const string &line : lines) {
        distance_file << line << endl;
    }

    ofstream node_file;
    node_file.open(output_dir + "/node.csv", ios::out);
    node_file << "node_id,station_cd" << endl;
    for (int i = 0; i < N; ++i) {
        Node node = graph.getNodeById(i).value();
        node_file << to_string(node.node_id) << ","
                  << to_string(node.station_code) << endl;
    }

    ofstream tsp_file;
    tsp_file.open(output_dir + "/railway.tsp", ios::out);
    tsp_file << "NAME : railway" << endl;
    tsp_file << "COMMENT : Japanese railway problem" << endl;
    tsp_file << "TYPE : tsp" << endl;
    tsp_file << "DIMENSION : " << N << endl;
    tsp_file << "EDGE_WEIGHT_TYPE : EXPLICIT" << endl;
    tsp_file << "EDGE_WEIGHT_FORMAT : FULL_MATRIX" << endl;
    tsp_file << "EDGE_WEIGHT_SECTION" << endl;

    lines = vector<string>(N);
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        // 高速化のため複数行まとめる
        string s = "";
        for (int j = 0; j < N; ++j) {
            if (distance[i][j] == DBL_MAX) {
                s += "-1 ";
            } else {
                s += to_string(lround(distance[i][j])) + " ";
            }
        }
        lines[i] = s;
    }
    for (const string &line : lines) {
        tsp_file << line;
    }
    tsp_file << endl << "EOF" << endl;

    return 0;
}
