#include <iostream>
#include <vector>
#include <list>
#include "pacman_image.h"
#include "constants.h"
using namespace std;

pacman_image::pacman_image() {
    orig_pellet_pos.reserve(1000);
    pellet_pos.reserve(1000);
}

void pacman_image::detect_maze_and_set(const ALEScreen &screen) {
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

vector<loc> pacman_image::detect_object_loc(const ALEScreen &screen) {
    vector<double> row_sum(7, 0), column_sum(7, 0);
    vector<vector<double> > column_values(7);
    vector<int> total_matches(7, 0);
    vector<double> rand_loc(7);

    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        for (size_t column = 0; column < SCREEN_WIDTH; ++column) {
            int val = screen.get(row, column);
            if (ghost_low[0] <= val && val <= ghost_high[0]) {
                total_matches[0]++;
                row_sum[0]+=row;
                column_sum[0]+=column;
                rand_loc[0] = min(rand_loc[0], double(column));
                column_values[0].push_back(column);
            }
            if (ghost_low[1] <= val && val <= ghost_high[1]) {
                total_matches[1]++;
                row_sum[1]+=row;
                column_sum[1]+=column;
                rand_loc[1] = min(rand_loc[1], double(column));
                column_values[1].push_back(column);
            }
            if (ghost_low[2] <= val && val <= ghost_high[2]) {
                total_matches[2]++;
                row_sum[2]+=row;
                column_sum[2]+=column;
                rand_loc[2] = min(rand_loc[2], double(column));
                column_values[2].push_back(column);
            }
            if (ghost_low[3] <= val && val <= ghost_high[3]) {
                total_matches[3]++;
                row_sum[3]+=row;
                column_sum[3]+=column;
                rand_loc[3] = min(rand_loc[3], double(column));
                column_values[3].push_back(column);
            }
            // detect pacman
            if (PACMAN_LOW <= val && val <= PACMAN_HIGH) {
                total_matches[4]++;
                row_sum[4]+=row;
                column_sum[4]+=column;
                rand_loc[4] = min(rand_loc[4], double(column));
                column_values[4].push_back(column);
            }
            if (GHOST1_BLINK_LOW <= val && val <= GHOST1_BLINK_HIGH) {
                total_matches[5]++;
                row_sum[5]+=row;
                column_sum[5]+=column;
                rand_loc[5] = min(rand_loc[5], double(column));
                column_values[5].push_back(column);
            }
            if (GHOST2_BLINK_LOW <= val && val <= GHOST2_BLINK_HIGH) {
                total_matches[6]++;
                row_sum[6]+=row;
                column_sum[6]+=column;
                rand_loc[6] = min(rand_loc[6], double(column));
                column_values[6].push_back(column);
            }
        }
    }
    vector<loc> ret, ret1;
    for(int i=0; i<7; i++) {
        ret1.push_back(make_pair(row_sum[i] / total_matches[i], column_sum[i] / total_matches[i]));
    }
    for(int i=0; i<7; i++) {
        double column_mean = column_sum[i] / total_matches[i];
        double variance = 0;
        for(size_t j = 0; j < column_values[i].size(); ++j) {
            variance += (column_values[i][j] - column_mean) * (column_values[i][j] - column_mean);
        }
        variance /= total_matches[i];
        if(variance > 100)
            ret1[i] = make_pair(row_sum[i] / total_matches[i], rand_loc[i]);
    }
    ret.push_back(ret1[4]);
    for(int i=0; i<4; i++) {
        ret.push_back(ret1[i]);
    }
    if(isnan(ret[1].first + ret[1].second))
        ret[1] = ret1[5];
    if(isnan(ret[2].first + ret[2].second))
        ret[2] = ret1[6];
    return ret;
}

vector<loc> pacman_image::process_screen(const ALEScreen &screen) {
    return detect_object_loc(screen);
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

list<loc> expand_edible_ghost(const ALEScreen &screen, vector<vector<int> > &expanded, int row, int column) {
    list<loc> locations;
    if (row < 0 || row >= MAZE_HEIGHT || column < 0 || column >= SCREEN_WIDTH) return locations; // out of bounds
    else if (expanded[row][column]) return locations;
    else {
        expanded[row][column] = 1;
        int val = screen.get(row, column);
        if (EDIBLE_GHOST_LOW <= val && val <= EDIBLE_GHOST_HIGH) {
            list<loc> tmp = expand_edible_ghost(screen, expanded, row - 1, column);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row + 1, column);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row, column - 1);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row, column + 1);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row - 1, column - 1);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row - 1, column + 1);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row + 1, column - 1);
            locations.splice(locations.end(), tmp);
            tmp = expand_edible_ghost(screen, expanded, row + 1, column + 1);
            locations.splice(locations.end(), tmp);
            locations.push_back(make_pair(row, column));
            return locations;
        }
        else {
            return locations;
        }
    }
}

vector<loc> pacman_image::detect_edible_ghosts(const ALEScreen &screen) {
    vector<vector<int> > expanded;
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        vector<int> row_vec(SCREEN_WIDTH, 0);
        expanded.push_back(row_vec);
    }
    vector<loc> edible_ghost_locations;
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            if (!expanded[row][column]) {
                int val = screen.get(row, column);
                if (EDIBLE_GHOST_LOW <= val && val <= EDIBLE_GHOST_HIGH) {
                    list<loc> locations = expand_edible_ghost(screen, expanded, row, column);
                    loc average_loc = make_pair(0, 0);
                    for (list<loc>::iterator it=locations.begin(); it!=locations.end(); it++) {
                        average_loc.first += (*it).first;
                        average_loc.second += (*it).second;
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
void pacman_image::update_pellet_pos(const ALEScreen &screen) {
    std::vector<loc> pellet_pos;
    for (vector<loc>::iterator it = orig_pellet_pos.begin(); it != orig_pellet_pos.end(); it++)
    {
        loc tmp = *it;
        if(screen.get(tmp.first, tmp.second) == MAZE_COLOR)
            pellet_pos.push_back(tmp);
    }
    this->pellet_pos.clear();
    this->pellet_pos = pellet_pos;
}

vector<vector<int> > detect_maze(const ALEScreen &screen) {
    // Maze - 1, Ghost pellet - 2, Normal pellet - 3 and remaining positions 0
    vector<vector<int> > maze;
    for (size_t row = 0; row < MAZE_HEIGHT; ++row) {
        vector<int> pixel_row;
        for (size_t column = 0; column < SCREEN_WIDTH; ++column) {
            if (screen.get(row, column) == MAZE_COLOR) {
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
                        if (screen.get(row, column + 4) == MAZE_COLOR) { // Not any of the pellet, so must be part of maze
                            pixel_row.push_back(1);
                        }
                        else if (screen.get(row + 2, column) == MAZE_COLOR) { // Not part of normal pellet
                            if (screen.get(row + 7, column) == MAZE_COLOR) { // Not part of ghost pellet
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
