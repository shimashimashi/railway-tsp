#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace railway {

const double POLE_RADIUS = 6356752.314245;
const double EQUATOR_RADIUS = 6378137.0;
const double E2 =
    (POLE_RADIUS * POLE_RADIUS - EQUATOR_RADIUS * EQUATOR_RADIUS) /
    (POLE_RADIUS * POLE_RADIUS);

struct Station {
    int station_code;
    int station_group_code;
    std::string station_name;
    int line_code;
    int prefecture_code;
    std::string post;
    std::string address;
    double lon;
    double lat;
};

struct Join {
    int line_code;
    int station_code1;
    int station_code2;
};

struct Line {
    int line_code;
    std::string line_name;
};

struct Node {
    int node_id;
    int station_code;
};

struct Group {
    int station_code;
    int leader;
};

using Edge = std::pair<int, int>;

struct Path {
    int from;
    int to;
    int next;
};

struct Coordinate {
    double lat;
    double lon;
};

class StationRepository {
  public:
    explicit StationRepository(const std::vector<Station> &stations) {
        for (const Station &station : stations) {
            station_code_to_station[station.station_code] = station;
            station_group_code_to_stations[station.station_group_code]
                .push_back(station);
        }
    }

    std::optional<Station> getStationByCode(int code) const {
        if (station_code_to_station.count(code) == 0) {
            return std::nullopt;
        }
        return station_code_to_station.at(code);
    }

    std::vector<Station> getStationsByStationGroupCode(int code) const {
        if (station_group_code_to_stations.count(code) == 0) {
            return {};
        }
        return station_group_code_to_stations.at(code);
    }

  private:
    std::map<int, Station> station_code_to_station;
    std::map<int, std::vector<Station>> station_group_code_to_stations;
};

class NodeRepository {
  public:
    void addNode(Node node) {
        station_code_to_node[node.station_code] = node;
        node_id_to_node[node.node_id] = node;
    }

    int size() const { return node_id_to_node.size(); }

    std::optional<Node> getNodeByStationCode(int code) const {
        if (station_code_to_node.count(code) == 0) {
            return std::nullopt;
        }
        return station_code_to_node.at(code);
    }

    std::optional<Node> getNodeById(int id) const {
        if (node_id_to_node.count(id) == 0) {
            return std::nullopt;
        }
        return node_id_to_node.at(id);
    }

  private:
    std::map<int, Node> node_id_to_node;
    std::map<int, Node> station_code_to_node;
};

class Graph {
  public:
    explicit Graph(NodeRepository *nodeRepository)
        : nodeRepository(nodeRepository) {}

    void addEdge(Node node1, Node node2) {
        neighbors[node1.node_id].insert(node2.node_id);
        neighbors[node2.node_id].insert(node1.node_id);

        Edge edge{node1.node_id, node2.node_id};
        edges.insert(edge);
    }

    int getNodeSize() const { return nodeRepository->size(); }

    std::optional<Node> getNodeById(int id) const {
        return nodeRepository->getNodeById(id);
    }

    std::set<int> getNeighbors(int id) const { return neighbors.at(id); }

    std::set<Edge> getEdges() const { return edges; }

  private:
    NodeRepository *nodeRepository;
    std::set<Edge> edges;
    std::map<int, std::set<int>> neighbors;
};

class GroupRepository {
  public:
    explicit GroupRepository(const std::vector<Group> &groups) {
        for (const Group &group : groups) {
            mp[group.station_code] = group.leader;
        }
    }

    bool isSame(int code1, int code2) const {
        if (mp.count(code1) == 0 || mp.count(code2) == 0) {
            return false;
        }
        return mp.at(code1) == mp.at(code2);
    }

  private:
    std::map<int, int> mp;
};

class Prefectures {
  public:
    std::string getPrefectureNameById(int id) const {
        return prefectures.at(id);
    }

  private:
    std::map<int, std::string> prefectures = {
        {1, "北海道"},  {2, "青森県"},    {3, "岩手県"},  {4, "宮城県"},
        {5, "秋田県"},  {6, "山形県"},    {7, "福島県"},  {8, "茨城県"},
        {9, "栃木県"},  {10, "群馬県"},   {11, "埼玉県"}, {12, "千葉県"},
        {13, "東京都"}, {14, "神奈川県"}, {15, "新潟県"}, {16, "富山県"},
        {17, "石川県"}, {18, "福井県"},   {19, "山梨県"}, {20, "長野県"},
        {21, "岐阜県"}, {22, "静岡県"},   {23, "愛知県"}, {24, "三重県"},
        {25, "滋賀県"}, {26, "京都府"},   {27, "大阪府"}, {28, "兵庫県"},
        {29, "奈良県"}, {30, "和歌山県"}, {31, "鳥取県"}, {32, "島根県"},
        {33, "岡山県"}, {34, "広島県"},   {35, "山口県"}, {36, "徳島県"},
        {37, "香川県"}, {38, "愛媛県"},   {39, "高知県"}, {40, "福岡県"},
        {41, "佐賀県"}, {42, "長崎県"},   {43, "熊本県"}, {44, "大分県"},
        {45, "宮崎県"}, {46, "鹿児島県"}, {47, "沖縄県"}};
};

double calcDistance(Coordinate a, Coordinate b) {
    a.lat *= M_PI / 180.0;
    a.lon *= M_PI / 180.0;
    b.lat *= M_PI / 180.0;
    b.lon *= M_PI / 180.0;
    double Dx = a.lat - b.lat;
    double Dy = a.lon - b.lon;
    double P = (a.lat + b.lat) / 2.0;
    double W = sqrt(1 - E2 * pow(sin(P), 2));
    double M = POLE_RADIUS * (1 - E2) / pow(W, 3);
    double N = POLE_RADIUS / W;
    double distance_meter = sqrt(pow(Dx * M, 2) + pow(Dy * N * cos(P), 2));
    return distance_meter / 1000.0;
}

class PathRepository {
  public:
    explicit PathRepository(const std::vector<Path> &paths) {
        for (const Path &path : paths) {
            mp[path.from][path.to] = path.next;
        }
    }

    int getNext(int from, int to) const { return mp.at(from).at(to); }

  private:
    std::map<int, std::map<int, int>> mp;
};

class JoinRepository {
  public:
    explicit JoinRepository(const std::vector<Join> &joins) {
        for (const Join &join : joins) {
            mp[join.station_code1][join.station_code2] = join.line_code;
            mp[join.station_code2][join.station_code1] = join.line_code;
        }
    }

    std::optional<int> getLineCodeByStationCodes(int code1, int code2) const {
        if (mp.count(code1) == 0) {
            return std::nullopt;
        }
        if (mp.at(code1).count(code2) == 0) {
            return std::nullopt;
        }
        return mp.at(code1).at(code2);
    }

  private:
    std::map<int, std::map<int, int>> mp;
};

class LineRepository {
  public:
    explicit LineRepository(const std::vector<Line> &lines) {
        for (const Line &line : lines) {
            mp[line.line_code] = line;
        }
    }

    Line getLineByCode(int code) const { return mp.at(code); }

  private:
    std::map<int, Line> mp;
};

std::vector<Station> readStations(std::string file_path) {
    std::vector<Station> stations;
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return stations;
    }

    std::string line;
    getline(fs, line);
    while (getline(fs, line)) {
        std::stringstream ss{line};
        std::string sep;

        getline(ss, sep, ',');
        int code = stoi(sep);

        getline(ss, sep, ',');
        int group_code = stoi(sep);

        getline(ss, sep, ',');
        std::string name = sep;

        // station_name_k
        getline(ss, sep, ',');

        // station_name_r
        getline(ss, sep, ',');

        getline(ss, sep, ',');
        int line_code = stoi(sep);

        getline(ss, sep, ',');
        int prefecture_code = stoi(sep);

        getline(ss, sep, ',');
        std::string post = sep;

        getline(ss, sep, ',');
        std::string address = sep;

        getline(ss, sep, ',');
        double lon = stod(sep);

        getline(ss, sep, ',');
        double lat = stod(sep);

        Station station{code, group_code, name, line_code, prefecture_code,
                        post, address,    lon,  lat};
        stations.push_back(station);
    }

    return stations;
}

std::vector<Join> readJoins(std::string file_path) {
    std::vector<Join> joins;
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return joins;
    }

    std::string line;
    getline(fs, line);
    while (getline(fs, line)) {
        std::stringstream ss{line};
        std::string sep;

        getline(ss, sep, ',');
        int code = stoi(sep);

        getline(ss, sep, ',');
        int station_code1 = stoi(sep);

        getline(ss, sep, ',');
        int station_code2 = stoi(sep);

        Join join{code, station_code1, station_code2};
        joins.push_back(join);
    }

    return joins;
}

std::vector<Line> readLines(std::string file_path) {
    std::vector<Line> lines;
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return lines;
    }

    std::string line;
    getline(fs, line);
    while (getline(fs, line)) {
        std::stringstream ss{line};
        std::string sep;

        getline(ss, sep, ',');
        int code = stoi(sep);

        getline(ss, sep, ',');
        getline(ss, sep, ',');
        std::string name = sep;

        Line line{code, name};
        lines.push_back(line);
    }

    return lines;
}

std::vector<Group> readGroup(std::string file_path) {
    std::vector<Group> groups;
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return groups;
    }

    std::string line;
    getline(fs, line);
    while (getline(fs, line)) {
        std::stringstream ss{line};
        std::string sep;

        getline(ss, sep, ',');
        int code = stoi(sep);

        getline(ss, sep, ',');
        int leader = stoi(sep);

        Group group{code, leader};
        groups.push_back(group);
    }

    return groups;
}

std::vector<int> readTour(std::string file_path) {
    std::vector<int> tour;
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return tour;
    }

    std::string line;
    for (int i = 0; i < 6; ++i) {
        getline(fs, line);
    }
    while (getline(fs, line)) {
        if (line == "-1") {
            break;
        }
        std::stringstream ss{line};
        std::string sep;

        getline(ss, sep);
        int node_id = stoi(sep) - 1;
        tour.push_back(node_id);
    }

    return tour;
}

std::vector<Node> readNode(std::string file_path) {
    std::vector<Node> nodes;
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return nodes;
    }

    std::string line;
    getline(fs, line);
    while (getline(fs, line)) {
        std::stringstream ss{line};
        std::string sep;

        getline(ss, sep, ',');
        int node_id = stoi(sep);

        getline(ss, sep, ',');
        int station_group_code = stoi(sep);

        Node node{node_id, station_group_code};
        nodes.push_back(node);
    }

    return nodes;
}

std::vector<Path> readShortestPath(std::string file_path) {
    std::ifstream fs(file_path);
    if (fs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        return {};
    }

    std::vector<std::string> lines;
    std::vector<Path> paths;
    while (fs) {
        std::string line;
        fs >> line;
        lines.push_back(line);
    }

    int N = lines.size() - 1;
    for (int i = 0; i < N; ++i) {
        std::stringstream ss{lines[i]};
        for (int j = 0; j < N; ++j) {
            std::string field;

            getline(ss, field, ',');
            int next = stoi(field);

            paths.push_back({i, j, next});
        }
    }

    return paths;
}

}; // namespace railway
