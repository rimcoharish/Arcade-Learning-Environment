#include <iostream>
#include <vector>
#include "pacman_image.h"
#include "constants.h"
using namespace std;

pacman_image::pacman_image() {}

pacman_image::pacman_image(const vector<vector<int> > &maze) {
    this->maze = maze;
}

void pacman_image::set_maze(const vector<vector<int> > maze) {
    this->maze = maze;
}

vector<vector<int> >& pacman_image::get_maze() {
    return maze;
}

pair<double, double> pacman_image::detect_loc(const vector<vector<int> > &screen, int low, int high) {
    double row_sum = 0, column_sum = 0;
    int total_matches = 0;
    for (int row = 0; row < screen.size(); ++row) {
        for (int column = 0; column < screen[row].size(); ++column) {
            if (low <= screen[row][column] && screen[row][column] <= high) {
                total_matches++;
                row_sum += row;
                column_sum += column;
            }
        }
    }
    pair<double, double> location = make_pair(row_sum / total_matches, column_sum / total_matches);
    return location;
}

pair<double, double> pacman_image::detect_pacman_loc(const vector<vector<int> > &screen) {
    return detect_loc(screen, PACMAN_LOW, PACMAN_HIGH);
}

vector<pair<double, double> > pacman_image::process_screen(const vector<vector<int> > &screen) {
    vector<pair<double, double> > object_locations;
    object_locations.push_back(detect_pacman_loc(screen));
    return object_locations;
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

