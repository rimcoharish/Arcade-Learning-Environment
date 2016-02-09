#ifndef ALE_PACMAN_IMAGE_H
#define ALE_PACMAN_IMAGE_H

#include <vector>

using namespace std;

class pacman_image {
private:
    vector<vector<int> > maze;

public:
    pacman_image();
    pacman_image(const vector<vector<int> > &maze);
    void set_maze(const vector<vector<int> > maze);
    vector<vector<int> >& get_maze(void);
    pair<double, double> detect_loc(const vector<vector<int> > &screen, int low, int high);
    pair<double, double> detect_pacman_loc(const vector<vector<int> > &screen);
    vector<pair<double, double> > process_screen(const vector<vector<int> > &screen);
};

vector<vector<int> > detect_maze(const vector<vector<int> > &screen);

#endif
