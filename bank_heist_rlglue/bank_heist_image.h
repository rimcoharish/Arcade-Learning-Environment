#ifndef ALE_BANK_HEIST_IMAGE_H
#define ALE_BANK_HEIST_IMAGE_H

#include <vector>
#include <math.h>
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

class bank_heist_image {

public:
    vector<vector<int> > screen;
    loc heist_loc;
    vector<loc> banks_loc;
    // Constructors
    bank_heist_image();

    loc detect_loc(int low, int high);
    void observe_colors();
    vector<loc> detect_banks();
    bool can_move_horizontally(int dir);
    bool can_move_vertically(int dir);
    vector<direction> get_valid_moves();
    void process_screen(vector<vector<int> > &screen);
    void print_image();
};

#endif
