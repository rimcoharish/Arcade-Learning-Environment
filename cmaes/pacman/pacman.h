#ifndef ALE_PACMAN_H
#define ALE_PACMAN_H

#include <string>

using namespace std;

double evaluate(string rom_file, int total_episodes, const double *params, int N);

#endif //ALE_PACMAN_H
