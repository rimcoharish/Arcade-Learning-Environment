#include "astar.h"
#include "constants.h"
#include "math.h"
#include <queue>
#include <set>
#include <map>
#include <limits>

#include <iostream>

double euclidean_distance(const pair<int, int> &loc1, const pair<int, int> &loc2) {
    return sqrt(pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2));
}

double manhattan_distance(const pair<int, int> &loc1, const pair<int, int> &loc2) {
    return fabs(loc1.first - loc2.first) + fabs(loc1.second - loc2.second);
}

class heuristic_comparison {
    map<pair<int, int>, int> *f_score;
public:
    heuristic_comparison(map<pair<int, int>, int> &f_score) {
        this->f_score = &f_score;
    }

    bool operator() (const pair<int, int> &loc1, const pair<int, int> &loc2) {
        return (*f_score)[loc1] > (*f_score)[loc2];
    }
};

typedef priority_queue<pair<int, int>, vector<pair<int, int> >, heuristic_comparison> location_queue;

vector<pair<int, int> > get_adjacent_locations(pair<int, int> location, int screen_height) {
    vector<pair<int, int> > adjacent_locations;
    if (location.first != 0) adjacent_locations.push_back(make_pair(location.first - 1, location.second));
    if (location.first != screen_height - 1)
        adjacent_locations.push_back(make_pair(location.first + 1, location.second));
    if (location.second != 0) adjacent_locations.push_back(make_pair(location.first, location.second - 1));
    if (location.second != SCREEN_WIDTH - 1)
        adjacent_locations.push_back(make_pair(location.first, location.second + 1));
    return adjacent_locations;
}

double get_distance(const vector<vector<int> > &screen, pair<int, int> start_loc, pair<int, int> goal) {
    set<pair<int, int> > visited_closed;
    set<pair<int, int> > visited_opened;

    visited_opened.insert(start_loc);

    map<pair<int, int>, pair<int, int> > predecessor;

    map<pair<int, int>, int> g_score;
    g_score[start_loc] = 0;

    map<pair<int, int>, int> f_score;
    f_score[start_loc] = manhattan_distance(start_loc, goal);

    heuristic_comparison cmp(f_score);
    location_queue locations(cmp);
    locations.push(start_loc);
    int nodes_visited = 0;

    bool goal_reached = false;

    while (!locations.empty()) {
        nodes_visited += 1;
        pair<int, int> current_loc = locations.top();
        if (current_loc == goal) {
            goal_reached = true;
            break;
        }
        visited_opened.erase(visited_opened.find(current_loc));
        locations.pop();
        visited_closed.insert(current_loc);
        vector<pair<int, int> > neighbors = get_adjacent_locations(current_loc, screen.size());
        for (int i = 0; i < neighbors.size(); ++i) {
            int color = screen[neighbors[i].first][neighbors[i].second];
            if (color == 0) continue;
            if (visited_closed.find(neighbors[i]) != visited_closed.end()) continue;
            double estimated_g_score = g_score[current_loc] + 1;
            bool neighbor_pushed = false;
            if (visited_opened.find(neighbors[i]) == visited_opened.end()) {
                visited_opened.insert(neighbors[i]);
                neighbor_pushed = true;
            }
            else if (estimated_g_score > g_score[neighbors[i]]) continue;
            predecessor[neighbors[i]] = current_loc;
            g_score[neighbors[i]] = estimated_g_score;
            f_score[neighbors[i]] = g_score[neighbors[i]] + manhattan_distance(neighbors[i], goal);
            if (neighbor_pushed) locations.push(neighbors[i]);
        }
    }

    pair<int, int> current_loc = goal;
    if (!goal_reached)
        return numeric_limits<double>::max();
    double distance = 0;
    while (current_loc != start_loc) {
        current_loc = predecessor[current_loc];
        distance += 1;
    }
    return distance;
}

/*int main() {
    map<pair<int, int>, int> f_score;
    f_score[make_pair(1, 18)] = 37;
    f_score[make_pair(0, 37)] = 39;
    heuristic_comparison cmp(f_score);
    location_queue locations(cmp);
    locations.push(make_pair(1, 18));
    locations.push(make_pair(0, 37));
    cout << locations.top().first << " " << locations.top().second << endl;
    cout << "Compiled" << endl;
}*/
