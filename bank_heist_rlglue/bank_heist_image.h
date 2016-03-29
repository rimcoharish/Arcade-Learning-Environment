#ifndef ALE_BANK_HEIST_IMAGE_H
#define ALE_BANK_HEIST_IMAGE_H

#include <bits/stdc++.h>

using namespace std;

#define loc pair<double, double>
#define mp make_pair

enum direction {
    UP_DIR,
    DOWN_DIR,
    RIGHT_DIR,
    LEFT_DIR,
    NULL_DIR
};

struct bank_heist_image {
    int step;
    vector<vector<int> > screen;
    loc heist_loc;
    vector<loc> police_loc;
    vector<loc> banks_loc;
    vector<int> banks_visited;
    vector<int> time_of_visit;
    int prev_maze_color;
    vector<vector<double> > dist1, dist2, dist3, dist4;

    // Constructors
    bank_heist_image();

    loc detect_loc(int low, int high);
    void observe_colors();
    vector<loc> detect_banks();
    vector<loc> detect_polices();
    bool is_heist(int row, int column);
    bool can_move_horizontally(loc heist, int dir);
    bool can_move_vertically(loc heist, int dir);
    vector<direction> get_valid_moves();
    bool heist_loc_possible(loc heist);
    void process_screen(vector<vector<int> > &screen);
    void analyze_maze();
    void print_image();
    double distance(loc source, loc destination);
    vector<vector<double> > complete_dfs(loc source);
    vector<loc> next_loc(loc location);
    double police_cost(loc location);
    double euclidean_distance(loc location1, loc location2);
};

#endif
