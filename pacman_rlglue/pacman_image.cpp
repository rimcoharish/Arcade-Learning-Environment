#include <iostream>
#include <vector>
#include "pacman_image.h"
#include "constants.h"
using namespace std;

pacman_image::pacman_image() {
    orig_pellet_pos.reserve(1000);
    pellet_pos.reserve(1000);
}

pacman_image::pacman_image(const vector<vector<int> > &maze) {
    this->maze = maze;
}

void pacman_image::detect_maze_and_set(const vector<vector<int> > &screen) {
    this->maze = detect_maze(screen);
    this->pellet_pos = detect_pellets(this->maze);
    this->orig_pellet_pos = this->pellet_pos;
}

vector<vector<int> >& pacman_image::get_maze() {
    return maze;
}

vector<loc>& pacman_image::get_pellet_pos() {
    return pellet_pos;
}

pair<double, double> pacman_image::detect_loc(const vector<vector<int> > &screen, int low, int high) {
    double row_sum = 0, column_sum = 0;
    vector<double> column_values;
    int total_matches = 0;
    double tmp;
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
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
    for (size_t i = 0; i < column_values.size(); ++i) {
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

loc pacman_image::detect_ghost1_loc(const vector<vector<int> > &screen) {
    loc ghost1_loc = detect_ghost_loc(screen, 0);
    if (isnan(ghost1_loc.first) || isnan(ghost1_loc.second)) {
        ghost1_loc = detect_loc(screen, GHOST1_BLINK_LOW, GHOST1_BLINK_HIGH);
    }
    return ghost1_loc;
}

loc pacman_image::detect_ghost2_loc(const vector<vector<int> > &screen) {
    loc ghost2_loc = detect_ghost_loc(screen, 1);
    if (isnan(ghost2_loc.first) || isnan(ghost2_loc.second)) {
        ghost2_loc = detect_loc(screen, GHOST2_BLINK_LOW, GHOST2_BLINK_HIGH);
    }
    return ghost2_loc;
}

pair<double, double> pacman_image::detect_ghost_loc(const vector<vector<int> > &screen, int ghost) {
    return detect_loc(screen, ghost_low[ghost], ghost_high[ghost]);
}

vector<pair<double, double> > pacman_image::process_screen(const vector<vector<int> > &screen) {
    vector<pair<double, double> > object_locations;
    object_locations.push_back(detect_pacman_loc(screen));
    object_locations.push_back(detect_ghost1_loc(screen));
    object_locations.push_back(detect_ghost2_loc(screen));
    for (int ghost = 2; ghost < 4; ++ghost) {
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

vector<loc> expand_edible_ghost(const vector<vector<int> > &screen, vector<vector<bool> > &expanded, int row, int column) {
    vector<loc> locations;
    if (row < 0 || row >= MAZE_HEIGHT || column < 0 || column >= SCREEN_WIDTH) return locations; // out of bounds
    else if (expanded[row][column]) return locations;
    else {
        expanded[row][column] = true;
        if (EDIBLE_GHOST_LOW <= screen[row][column] && screen[row][column] <= EDIBLE_GHOST_HIGH) {
            vector<loc> left_loc = expand_edible_ghost(screen, expanded, row - 1, column);
            vector<loc> right_loc = expand_edible_ghost(screen, expanded, row + 1, column);
            vector<loc> up_loc = expand_edible_ghost(screen, expanded, row, column - 1);
            vector<loc> down_loc = expand_edible_ghost(screen, expanded, row, column + 1);
            vector<loc> up_left_loc = expand_edible_ghost(screen, expanded, row - 1, column - 1);
            vector<loc> up_right_loc = expand_edible_ghost(screen, expanded, row - 1, column + 1);
            vector<loc> down_left_loc = expand_edible_ghost(screen, expanded, row + 1, column - 1);
            vector<loc> down_right_loc = expand_edible_ghost(screen, expanded, row + 1, column + 1);
            locations.push_back(make_pair(row, column));
            for (size_t i = 0; i < left_loc.size(); ++i) {
                locations.push_back(left_loc[i]);
            }
            for (size_t i = 0; i < right_loc.size(); ++i) {
                locations.push_back(right_loc[i]);
            }
            for (size_t i = 0; i < up_loc.size(); ++i) {
                locations.push_back(up_loc[i]);
            }
            for (size_t i = 0; i < down_loc.size(); ++i) {
                locations.push_back(down_loc[i]);
            }
            for (size_t i = 0; i < up_left_loc.size(); ++i) {
                locations.push_back(up_left_loc[i]);
            }
            for (size_t i = 0; i < up_right_loc.size(); ++i) {
                locations.push_back(up_right_loc[i]);
            }
            for (size_t i = 0; i < down_left_loc.size(); ++i) {
                locations.push_back(down_left_loc[i]);
            }
            for (size_t i = 0; i < down_right_loc.size(); ++i) {
                locations.push_back(down_right_loc[i]);
            }
            return locations;
        }
        else {
            return locations;
        }
    }
}

vector<loc> pacman_image::detect_edible_ghosts(const vector<vector<int> > &screen) {
    vector<vector<bool> > expanded;
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        vector<bool> row_vec(SCREEN_WIDTH, false);
        expanded.push_back(row_vec);
    }
    vector<loc> edible_ghost_locations;
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            if (!expanded[row][column]) {
                if (EDIBLE_GHOST_LOW <= screen[row][column] && screen[row][column] <= EDIBLE_GHOST_HIGH) {
                    vector<loc> locations = expand_edible_ghost(screen, expanded, row, column);
                    loc average_loc = make_pair(0, 0);
                    for (size_t i = 0; i < locations.size(); ++i) {
                        average_loc.first += locations[i].first;
                        average_loc.second += locations[i].second;
                    }
                    average_loc.first = average_loc.first / locations.size();
                    average_loc.second = average_loc.second / locations.size();
                    if (locations.size() > 10) edible_ghost_locations.push_back(average_loc);
                }
            }
        }
    }
    return edible_ghost_locations;
}

/* 
pellet_pos gets updated with the location of pellets active currently
Known problem: When a ghost and pellet overlap, pellet gets undetected
               for this step.
*/
void pacman_image::update_pellet_pos(const vector<vector<int> > &screen) {
    std::vector<loc> pellet_pos;
    for (vector<loc>::iterator it = orig_pellet_pos.begin(); it != orig_pellet_pos.end(); it++)
    {
        loc tmp = *it;
        if(screen[tmp.first][tmp.second] == MAZE_COLOR)
            pellet_pos.push_back(tmp);
    }
    this->pellet_pos.clear();
    this->pellet_pos = pellet_pos;
}

vector<vector<int> > detect_maze(const vector<vector<int> > &screen) {
    // Maze - 1, Ghost pellet - 2, Normal pellet - 3 and remaining positions 0
    vector<vector<int> > maze;
    for (size_t row = 0; row < screen.size(); ++row) {
        vector<int> pixel_row;
        for (size_t column = 0; column < screen[row].size(); ++column) {
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

vector<loc> detect_pellets(vector<vector<int> > maze) {
    vector<loc> pellet_pos;
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        for (size_t column = 0; column < maze[row].size(); ++column) {
            if(maze[row][column] == 3) {
                pellet_pos.push_back(make_pair(row, column));
                for (int i = 0; i < 2; ++i)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        maze[row+i][column+j] = 0;
                    }
                }
            }
        }
    }
    return pellet_pos;
}

