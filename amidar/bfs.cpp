#include "bfs.h"
#include "constants.h"
#include <queue>

#include <iostream>

vector<pair<int, int> > get_adjacent_locations(pair<int, int> location) {
    vector<pair<int, int> > adjacent_locations;
    if (location.first != 0) adjacent_locations.push_back(make_pair(location.first - 1, location.second));
    if (location.first != SCREEN_HEIGHT - 1)
        adjacent_locations.push_back(make_pair(location.first + 1, location.second));
    if (location.second != 0) adjacent_locations.push_back(make_pair(location.first, location.second - 1));
    if (location.second != SCREEN_WIDTH - 1)
        adjacent_locations.push_back(make_pair(location.first, location.second + 1));
    return adjacent_locations;
}

double get_distance(const vector<vector<int> > &screen, pair<int, int> start_loc, pair<int, int> goal) {
    vector<vector<bool> > visited;
    vector<vector<pair<int, int> > > predecessor;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<bool> visited_row;
        vector<pair<int, int> > predecessor_row;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            visited_row.push_back(false);
            predecessor_row.push_back(make_pair(-1, -1));
        }
        visited.push_back(visited_row);
        predecessor.push_back(predecessor_row);
    }
    visited[start_loc.first][start_loc.second] = true;
    queue<pair<int, int> > locations;
    locations.push(start_loc);
    while (!locations.empty()) {
        pair<int, int> current_loc = locations.front();
        locations.pop();
        vector<pair<int, int> > adjacent_locations = get_adjacent_locations(current_loc);
        for (int i = 0; i < adjacent_locations.size(); ++i) {
            if (screen[adjacent_locations[i].first][adjacent_locations[i].second] != MAZE_COLOR &&
                    !visited[adjacent_locations[i].first][adjacent_locations[i].second]) {
                visited[adjacent_locations[i].first][adjacent_locations[i].second] = true;
                predecessor[adjacent_locations[i].first][adjacent_locations[i].second] = current_loc;
                locations.push(adjacent_locations[i]);
            }
        }
    }
    pair<int, int> back_track_pos = goal;
    int distance = 0;
    while (back_track_pos != start_loc) {
        back_track_pos = predecessor[back_track_pos.first][back_track_pos.second];
        distance += 1;
    }
    return distance;
}