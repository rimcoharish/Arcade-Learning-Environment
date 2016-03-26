#include "amidar_image.h"
#include "constants.h"
#include <iostream>

pair<bool, bool> amidar_image::can_move_horizontally(const loc amidar_location, vector<vector<int> > &maze)  {
    int row = round(amidar_location.first);
    int column = round(amidar_location.second);
    int left = 0, right = 0;
    while (left + right < 10) {
        if (column - left < 0) right++;
        else if (column + right >= SCREEN_WIDTH - 1) left++;
        else if (maze[row][column - left] == 0 && maze[row][column + right] == 0) break;
        else if (maze[row][column - left] == 0) right++;
        else if (maze[row][column + right] == 0) left++;
        else {
            if ((left + right) % 2 == 0) left++;
            else right++;
        }
    }
    return make_pair(maze[row][column - left] != 0, maze[row][column + right] != 0);
}

pair<bool, bool> amidar_image::can_move_vertically(const loc amidar_location, vector<vector<int> > &maze) {
    int row = round(amidar_location.first);
    int column = round(amidar_location.second);
    int up = 0, down = 0;
    while (up + down < 10) {
        if (row - up < 0) down++;
        else if (row + down >= maze.size() - 1) up++;
        else if (maze[row - up][column] == 0 && maze[row + down][column] == 0) break;
        else if (maze[row - up][column] == 0) down++;
        else if (maze[row + down][column] == 0) up++;
        else {
            if ((up + down) % 2 == 0) up++;
            else down++;
        }
    }
    return make_pair(maze[row - up][column] != 0, maze[row + down][column] != 0);
}

vector<direction> amidar_image::get_valid_moves(const loc amidar_location, vector<vector<int> > &maze) {
    vector<direction> valid_moves;
    pair<bool, bool> horizontal = can_move_horizontally(amidar_location, maze);
    pair<bool, bool> vertical = can_move_vertically(amidar_location, maze);
    if (horizontal.first) valid_moves.push_back(LEFT_DIR);
    if (horizontal.second) valid_moves.push_back(RIGHT_DIR);
    if (vertical.first) valid_moves.push_back(UP_DIR);
    if (vertical.second) valid_moves.push_back(DOWN_DIR);
    return valid_moves;

}

loc amidar_image::detect_amidar(const vector<vector<int> > &screen) {
    double row_sum = 0, column_sum = 0, count = 0;
    for (int row = 0; row < screen.size(); ++row) {
        for (int column = 0; column < screen[row].size(); ++column) {
            if (screen[row][column] == AMIDAR_COLOR) {
                row_sum += row;
                column_sum += column;
                count += 1;
            }
        }
    }
    return make_pair(row_sum / count, column_sum / count);
}

vector<loc> expand_ghost(const vector<vector<int> > &screen, vector<vector<bool> > &expanded, int row, int column) {
    vector<loc> locations;
    if (row < 0 || row >= screen.size() || column < 0 || column >= SCREEN_WIDTH) return locations; // out of bounds
    else if (expanded[row][column]) return locations;
    else {
        expanded[row][column] = true;
        if (GHOST_LOW <= screen[row][column] && screen[row][column] <= GHOST_HIGH) {
            vector<loc> left_loc = expand_ghost(screen, expanded, row - 1, column);
            vector<loc> right_loc = expand_ghost(screen, expanded, row + 1, column);
            vector<loc> up_loc = expand_ghost(screen, expanded, row, column - 1);
            vector<loc> down_loc = expand_ghost(screen, expanded, row, column + 1);
            vector<loc> up_left_loc = expand_ghost(screen, expanded, row - 1, column - 1);
            vector<loc> up_right_loc = expand_ghost(screen, expanded, row - 1, column + 1);
            vector<loc> down_left_loc = expand_ghost(screen, expanded, row + 1, column - 1);
            vector<loc> down_right_loc = expand_ghost(screen, expanded, row + 1, column + 1);
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

vector<loc> amidar_image::detect_ghosts(const vector<vector<int> > &screen) {
    vector<vector<bool> > expanded;
    for (int row = 0; row < screen.size(); ++row) {
        vector<bool> row_vec(SCREEN_WIDTH, false);
        expanded.push_back(row_vec);
    }
    vector<loc> ghost_locations;
    for (int row = 0; row < screen.size(); ++row) {
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            if (!expanded[row][column]) {
                if (GHOST_LOW <= screen[row][column] && screen[row][column] <= GHOST_HIGH) {
                    vector<loc> locations = expand_ghost(screen, expanded, row, column);
                    loc average_loc = make_pair(0, 0);
                    for (size_t i = 0; i < locations.size(); ++i) {
                        average_loc.first += locations[i].first;
                        average_loc.second += locations[i].second;
                    }
                    average_loc.first = average_loc.first / locations.size();
                    average_loc.second = average_loc.second / locations.size();
                    if (locations.size() > 10) ghost_locations.push_back(average_loc);
                }
            }
        }
    }
    return ghost_locations;
}