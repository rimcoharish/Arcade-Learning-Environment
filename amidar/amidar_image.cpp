#include "amidar_image.h"
#include "constants.h"
#include <iostream>

pair<bool, bool> amidar_image::can_move_horizontally(const loc amidar_location, vector<vector<int> > &maze)  {
    bool can_move_left = maze[amidar_location.first + 4][amidar_location.second - 3] == MAZE_COLOR;
    bool can_move_right = maze[amidar_location.first + 4][amidar_location.second + 3] == MAZE_COLOR;
    if (amidar_location.first == 160) {
        can_move_left = maze[amidar_location.first + 5][amidar_location.second - 3] == MAZE_COLOR;
        can_move_right = maze[amidar_location.first + 5][amidar_location.second + 3] == MAZE_COLOR;
    }
    return make_pair(can_move_left, can_move_right);
}

pair<bool, bool> amidar_image::can_move_vertically(const loc amidar_location, vector<vector<int> > &maze) {
    bool can_move_up = maze[amidar_location.first - 1][amidar_location.second] == MAZE_COLOR ||
            maze[amidar_location.first - 2][amidar_location.second] == MAZE_COLOR;
    bool can_move_down = maze[amidar_location.first + 6][amidar_location.second - 1] == MAZE_COLOR ||
            maze[amidar_location.first + 7][amidar_location.second - 1] == MAZE_COLOR ||
            maze[amidar_location.first + 7][amidar_location.second + 1] == MAZE_COLOR ||
            maze[amidar_location.first + 8][amidar_location.second + 1] == MAZE_COLOR;
    if (amidar_location.first == 160) can_move_down = false;
    return make_pair(can_move_up, can_move_down);
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

bool amidar_image::check_amidar_intact(loc amidar_location, const vector<vector<int> > &screen) {
    for (int i = 1; i < 3; ++i) {
        if (screen[amidar_location.first][amidar_location.second + i] != AMIDAR_COLOR) return false;
    }
    for (int i = -1; i < 4; ++i) {
        if (screen[amidar_location.first + 1][amidar_location.second + i] != AMIDAR_COLOR) return false;
    }
    return true;
}

loc amidar_image::detect_amidar(const vector<vector<int> > &screen) {
    for (int row = 0; row < MAZE_HIGH; ++row) {
        for (int column = 0; column < screen[row].size(); ++column) {
            if (screen[row][column] == AMIDAR_COLOR) {
                loc possible_amidar_loc = make_pair(row, column);
                if (check_amidar_intact(possible_amidar_loc, screen)) return possible_amidar_loc;
                else return make_pair(-1, -1);
            }
        }
    }
    return make_pair(-1, -1);
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

set<loc> amidar_image::detect_junctions(const vector<vector<int> > &screen) {
    set<loc> junctions;
    for (int row = 15; row < 136; row += 30) {
        for (int column = 0; column < screen[row].size(); ++column) {
            if (screen[row + 7][column] == MAZE_COLOR) {
                junctions.insert(make_pair(row, column));
                junctions.insert(make_pair(row + 30, column));
                column += 4;
            }
        }
    }
    return junctions;
}

vector<vector<loc> > amidar_image::construct_squares(set<loc> &junctions) {
    vector<vector<loc> > squares;
    for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        set<loc>::iterator current_it = it;
        it++;
        set<loc>::iterator it_next = it;
        it--;
        while (it_next != junctions.end() && (*it).first == (*it_next).first && (*it).first != 165) {
            if (junctions.find(make_pair((*it_next).first + 30, (*it_next).second)) != junctions.end() &&
                    junctions.find(make_pair((*it).first + 30, (*it).second)) != junctions.end()) {
                vector<loc> square;
                square.push_back(*it);
                square.push_back(*it_next);
                square.push_back(make_pair((*it_next).first + 30, (*it_next).second));
                square.push_back(make_pair((*it).first + 30, (*it).second));
                squares.push_back(square);
                break;
            }
            else {
                it_next++;
            }
        }
        it_next--;
        it = it_next;
    }
    return squares;
}