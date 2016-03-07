#ifndef ALE_BANK_HEIST_IMAGE_H
#define ALE_BANK_HEIST_IMAGE_H

#include <vector>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;

#define loc pair<double, double>
#define mp make_pair

class bank_heist_image {

public:
    loc heist_loc;
    vector<loc> banks_loc;
    // Constructors
    bank_heist_image();

    loc detect_loc(const vector<vector<int> > &screen, int low, int high);
    void observe_colors(const vector<vector<int> > &screen);
    vector<loc> detect_banks(const vector<vector<int> > &screen);
    void process_screen(const vector<vector<int> > &screen);
    void print_image(const vector<vector<int> > &screen);
};

#endif
