#include <iostream>
#include <vector>
#include "pacman_image.h"
#include "constants.h"
using namespace std;

pacman_image::pacman_image() {}

pacman_image::pacman_image(const vector<vector<int> > &maze) {
    this->maze = maze;
}

void pacman_image::detect_maze_and_set(const vector<vector<int> > &screen) {
    this->maze = detect_maze(screen);
}

vector<vector<int> >& pacman_image::get_maze() {
    return maze;
}

pair<double, double> pacman_image::detect_loc(const vector<vector<int> > &screen, int low, int high) {
    double row_sum = 0, column_sum = 0;
    vector<double> column_values;
    int total_matches = 0;
    double tmp;
    for (int row = 0; row < screen.size(); ++row) {
        for (int column = 0; column < screen[row].size(); ++column) {
            if (low <= screen[row][column] && screen[row][column] <= high) {
                tmp = min(tmp, double(column));
                total_matches++;
                row_sum += row;
                column_sum += column;
                column_values.push_back(column);
            }
        }
    }
    double column_mean = column_sum / column_values.size();
    double variance = 0, sd = 0;
    for (int i = 0; i < column_values.size(); ++i) {
        variance += pow(column_values[i] - column_mean, 2);
    }
    variance /= column_values.size();
    sd = sqrt(variance);
    //    When the S.D is high => teleportation happening
    //    We return a random location
    if(sd > 10) {
        return make_pair(row_sum/total_matches, tmp);
    }
    pair<double, double> location = make_pair(row_sum / total_matches, column_sum / total_matches);
    return location;
}

pair<double, double> pacman_image::detect_pacman_loc(const vector<vector<int> > &screen) {
    return detect_loc(screen, PACMAN_LOW, PACMAN_HIGH);
}

pair<double, double> pacman_image::detect_ghost_loc(const vector<vector<int> > &screen, int ghost) {
    return detect_loc(screen, ghost_low[ghost], ghost_high[ghost]);
}

vector<pair<double, double> > pacman_image::process_screen(const vector<vector<int> > &screen) {
    vector<pair<double, double> > object_locations;
    object_locations.push_back(detect_pacman_loc(screen));
    for (int ghost = 0; ghost < 4; ++ghost) {
        object_locations.push_back(detect_ghost_loc(screen, ghost));
    }
    return object_locations;
}

pair<bool, bool> pacman_image::can_move_horizontally(const loc pacman_location)  {
    int row = round(pacman_location.first);
    int column = round(pacman_location.second);
    int left = 0, right = 0;
    while (left + right < 14) {
        if (maze[row][column - left] == 1 && maze[row][column + right] == 1) break;
        else if (maze[row][column - left] == 1) right++;
        else if (maze[row][column + right] == 1) left++;
        else {
            if ((left + right) % 2 == 0) left++;
            else right++;
        }
    }
    return make_pair(maze[row][column - left] != 1, maze[row][column + right] != 1);
}

pair<bool, bool> pacman_image::can_move_vertically(const loc pacman_location) {
    int row = round(pacman_location.first);
    int column = round(pacman_location.second);
    int up = 0, down = 0;
    while (up + down < 14) {
        if (maze[row - up][column] == 1 && maze[row + down][column] == 1) break;
        else if (maze[row - up][column] == 1) down++;
        else if (maze[row + down][column] == 1) up++;
        else {
            if ((up + down) % 2 == 0) up++;
            else down++;
        }
    }
    return make_pair(maze[row - up][column] != 1, maze[row + down][column] != 1);
}

vector<direction> pacman_image::get_valid_moves(const loc pacman_location) {
    vector<direction> valid_moves;
    pair<bool, bool> horizontal = can_move_horizontally(pacman_location);
    pair<bool, bool> vertical = can_move_vertically(pacman_location);
    if (horizontal.first) valid_moves.push_back(LEFT_DIR);
    if (horizontal.second) valid_moves.push_back(RIGHT_DIR);
    if (vertical.first) valid_moves.push_back(UP_DIR);
    if (vertical.second) valid_moves.push_back(DOWN_DIR);
    return valid_moves;

}

vector<vector<int> > detect_maze(const vector<vector<int> > &screen) {
    // Maze - 1, Ghost pellet - 2, Normal pellet - 3 and remaining positions 0
    vector<vector<int> > maze;
    for (int row = 0; row < screen.size(); ++row) {
        vector<int> pixel_row;
        for (int column = 0; column < screen[row].size(); ++column) {
            if (screen[row][column] == MAZE_COLOR) {
                if (row == 0 || column == 0) { // Corner is always a maze position if it is maze colored
                    pixel_row.push_back(1);
                }
                else {
                    if (maze[row - 1][column] == 1 || pixel_row[column - 1] == 1) { // Continuation of maze
                        pixel_row.push_back(1);
                    }
                    else if (maze[row - 1][column] == 2 || pixel_row[column - 1] == 2) { // Continuation of ghost pellet
                        pixel_row.push_back(2);
                    }
                    else if (maze[row - 1][column] == 2 || pixel_row[column - 1] == 3) { // Continuation of normal pellet
                        pixel_row.push_back(3);
                    }
                    else {
                        if (screen[row][column + 4] == MAZE_COLOR) { // Not any of the pellet, so must be part of maze
                            pixel_row.push_back(1);
                        }
                        else if (screen[row + 2][column] == MAZE_COLOR) { // Not part of normal pellet
                            if (screen[row + 7][column] == MAZE_COLOR) { // Not part of ghost pellet
                                pixel_row.push_back(1); // Must be part of maze
                            }
                            else {
                                pixel_row.push_back(2);
                            }
                        }
                        else {
                            pixel_row.push_back(3);
                        }
                    }
                }
            }
            else {
                pixel_row.push_back(0);
            }
        }
        maze.push_back(pixel_row);
    }
    return maze;
}

