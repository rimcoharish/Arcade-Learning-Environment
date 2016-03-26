#ifndef ALE_ASTAR_H
#define ALE_ASTAR_H

#include <vector>
#include <utility>

using namespace std;

double get_distance(const vector<vector<int> > &screen, pair<int, int> start_loc, pair<int, int> goal);

#endif
