#include "target_based_agent.h"
#include "constants.h"
#include "math.h"
#include <queue>
#include <set>
#include <map>
#include <limits>
#include <ctime>
#include <unistd.h>
#include <algorithm>

double euclidean_distance(const pair<int, int> &loc1, const pair<int, int> &loc2) {
    return sqrt(pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2));
}

double manhattan_distance(const pair<int, int> &loc1, const pair<int, int> &loc2) {
    return fabs(loc1.first - loc2.first) + fabs(loc1.second - loc2.second);
}

target_based_agent::target_based_agent() {
    amidar_location = make_pair(-1, -1);
    amidar_mode = ROAM;
    action_taken = PLAYER_A_NOOP;
}

vector<loc> get_adjacent_locations(loc location) {
    vector<loc> adjacent_locations;
    if (location.first != 0) adjacent_locations.push_back(make_pair(location.first - 1, location.second));
    if (location.first != SCREEN_HEIGHT - 1)
        adjacent_locations.push_back(make_pair(location.first + 1, location.second));
    if (location.second != 0) adjacent_locations.push_back(make_pair(location.first, location.second - 1));
    if (location.second != SCREEN_WIDTH - 1)
        adjacent_locations.push_back(make_pair(location.first, location.second + 1));
    return adjacent_locations;
}

double corner_cost(loc amidar_location) {
    double cost = 0;
    cost += CORNER_COST / pow(euclidean_distance(amidar_location, make_pair(15, 13)), 2);
    cost += CORNER_COST / pow(euclidean_distance(amidar_location, make_pair(15, 143)), 2);
    cost += CORNER_COST / pow(euclidean_distance(amidar_location, make_pair(167, 13)), 2);
    cost += CORNER_COST / pow(euclidean_distance(amidar_location, make_pair(167, 143)), 2);
    return cost;
}

void target_based_agent::clear_targets() {
    while (!targets.empty()) targets.pop();
}

double ghost_cost(loc ghost_location, loc amidar_loc) {
    if (ghost_location.first < 20 || ghost_location.first > 160) {
//        cout << "here1" << endl;
        return GHOST_COST / pow(manhattan_distance(ghost_location, amidar_loc), 2);
    }
    else if (ghost_location.second < 20 || ghost_location.second > 138) {
//        cout << "here2" << endl;
//        cout << ghost_location.first << ", " << ghost_location.second << endl;
//        cout << amidar_loc.first << ", " << amidar_loc.second << endl;
        return GHOST_COST / pow(manhattan_distance(ghost_location, amidar_loc), 2);
    }
    else return GHOST_COST / pow(euclidean_distance(ghost_location, amidar_loc), 2);
}

void target_based_agent::update_amidar_location(loc amidar_loc) {
    if (amidar_location.first < 0) amidar_location = amidar_loc;
    else if (amidar_loc.first > 0) amidar_location = amidar_loc;
    else {
        switch (action_taken) {
            case PLAYER_A_UP:
                amidar_location = make_pair(amidar_location.first - 1, amidar_location.second);
                break;
            case PLAYER_A_DOWN:
                amidar_location = make_pair(amidar_location.first + 1, amidar_location.second);
                break;
            case PLAYER_A_LEFT:
                amidar_location = make_pair(amidar_location.first, amidar_location.second - 1);
                break;
            case PLAYER_A_RIGHT:
                amidar_location = make_pair(amidar_location.first, amidar_location.second + 1);
                break;
            default:
                break;
        }
    }
}

direction get_ghost_direction(loc prev_loc, loc current_loc) {
    if (prev_loc.first == current_loc.first) {
        if (prev_loc.second < current_loc.second) return RIGHT_DIR;
        else if (prev_loc.second > current_loc.second) return LEFT_DIR;
    }
    else if (prev_loc.second == current_loc.second) {
        if (prev_loc.first < current_loc.first) return DOWN_DIR;
        else if (prev_loc.first > current_loc.first) return UP_DIR;
    }
    else return NULL_DIR;
}

void target_based_agent::update_ghost_locations(vector<loc> current_ghost_locations) {
    if (current_ghost_locations.size() < ghost_locations.size()) {
        set<loc> updated_ghost_locations;
        for (int i = 0; i < current_ghost_locations.size(); ++i) {
            set<loc>::iterator nearest_prev_loc = ghost_locations.begin();
            double least_distance = euclidean_distance(*nearest_prev_loc, current_ghost_locations[i]);
            for (set<loc>::iterator it = ghost_locations.begin(); it != ghost_locations.end(); ++it) {
                double dist = euclidean_distance(*it, current_ghost_locations[i]);
                if (dist < least_distance) {
                    least_distance = dist;
                    nearest_prev_loc = it;
                }
            }
            loc ghost_loc = make_pair((*nearest_prev_loc).first, (*nearest_prev_loc).second);
            updated_ghost_locations.insert(current_ghost_locations[i]);
            if (ghost_locations.find(ghost_loc) == ghost_locations.end()) {
                cout << "not found" << endl;
                exit(0);
            }
            ghost_locations.erase(ghost_loc);
        }
        ghost_locations.insert(updated_ghost_locations.begin(), updated_ghost_locations.end());
    }
    else {
        ghost_locations.clear();
        ghost_locations.insert(current_ghost_locations.begin(), current_ghost_locations.end());
    }
}

void target_based_agent::update_ghost_locations_new(vector<loc> current_ghost_locations) {
    map<loc, direction> updated_ghost_dir;
    set<loc> updated_ghost_locations;
    for (int i = 0; i < current_ghost_locations.size(); ++i) {
        if (ghost_locations.empty()) {
            updated_ghost_locations.insert(current_ghost_locations[i]);
            updated_ghost_dir[current_ghost_locations[i]] = NULL_DIR;
        }
        else {
            set<loc>::iterator nearest_prev_loc = ghost_locations.begin();
            double least_distance = euclidean_distance(*nearest_prev_loc, current_ghost_locations[i]);
            for (set<loc>::iterator it = ghost_locations.begin(); it != ghost_locations.end(); ++it) {
                double dist = euclidean_distance(*it, current_ghost_locations[i]);
                if (dist < least_distance) {
                    least_distance = dist;
                    nearest_prev_loc = it;
                }
            }
            loc ghost_loc = make_pair((*nearest_prev_loc).first, (*nearest_prev_loc).second);
            updated_ghost_locations.insert(current_ghost_locations[i]);
            if (ghost_locations.find(ghost_loc) == ghost_locations.end()) {
                cout << "not found" << endl;
                exit(0);
            }
            ghost_locations.erase(ghost_loc);
        }
    }
    ghost_locations.insert(updated_ghost_locations.begin(), updated_ghost_locations.end());
}

bool target_based_agent::corner_section(loc location) {
    for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        loc junction_loc = *it;
        if ((junction_loc.first - 4 <= location.first && location.first <= junction_loc.first + 3)
                && (junction_loc.second - 3 <= location.second && location.second <= junction_loc.second + 3)) {
            return true;
        }
    }
    return false;
}

bool target_based_agent::amidar_at_junction(loc location) {
    for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        loc junction_loc = *it;
        if ((junction_loc.first - 2 <= location.first && location.first <= junction_loc.first + 1)
            && (junction_loc.second - 1 <= location.second && location.second <= junction_loc.second + 1)) {
            return true;
        }
    }
    return false;
}

pair<loc, loc> target_based_agent::get_edge(loc location) {
    int horizontal_edge = -1;
    for (int row = 15; row < 166; row += 30) {
        if (row <= location.first && location.first <= row + 3) {
            horizontal_edge = row;
            break;
        }
    }
    if (horizontal_edge == -1) {
        pair<loc, loc> edge = make_pair(make_pair(-1, -1), make_pair(-1, -1));
        for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
            if ((*it).first <= location.first && (*it).second <= location.second &&
                    location.second <= (*it).second + 3) {
                edge.first = *it;
                edge.second = make_pair((*it).first + 30, (*it).second);
            }
        }
        return edge;
    }
    else {
        for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
            if ((*it).first == horizontal_edge && location.second < (*it).second) {
                if (it == junctions.begin()) {
                    cout << "Not possible" << endl;
                    return make_pair(make_pair(-1, -1), make_pair(-1, -1));
                }
                else {
                    loc l2 = *it;
                    it--;
                    return make_pair(*it, l2);
                }
            }
        }
        return make_pair(make_pair(-1, -1), make_pair(-1, -1));
    }
}

loc target_based_agent::nearest_unpainted_loc(const vector<vector<int> > &screen, loc amidar_loc) {
    vector<vector<bool> > visited;
    vector<vector<loc> > predecessor;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<bool> visited_row;
        vector<loc> predecessor_row;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            visited_row.push_back(false);
            predecessor_row.push_back(make_pair(-1, -1));
        }
        visited.push_back(visited_row);
        predecessor.push_back(predecessor_row);
    }
    visited[amidar_loc.first][amidar_loc.second] = true;
    queue<loc> locations;
    locations.push(amidar_loc);
    while (!locations.empty()) {
        loc current_loc = locations.front();
        locations.pop();
        if (screen[current_loc.first][current_loc.second] == 0 &&
            screen[current_loc.first - 1][current_loc.second] == MAZE_COLOR &&
            screen[current_loc.first + 1][current_loc.second] == MAZE_COLOR &&
            !corner_section(current_loc)) return current_loc;
        vector<loc> adjacent_locations = get_adjacent_locations(current_loc);
        for (int i = 0; i < adjacent_locations.size(); ++i) {
            if (screen[adjacent_locations[i].first][adjacent_locations[i].second] == 0 &&
                    (screen[adjacent_locations[i].first - 1][adjacent_locations[i].second] == 0 ||
                            screen[adjacent_locations[i].first + 1][adjacent_locations[i].second] == 0))
                continue;
            if (!visited[adjacent_locations[i].first][adjacent_locations[i].second]) {
                visited[adjacent_locations[i].first][adjacent_locations[i].second] = true;
                predecessor[adjacent_locations[i].first][adjacent_locations[i].second] = current_loc;
                locations.push(adjacent_locations[i]);
            }
        }
    }
    return make_pair(-1, -1);
}

class heuristic_comparison {
    map<pair<int, int>, int> *f_score;
public:
    heuristic_comparison(map<pair<int, int>, int> &f_score) {
        this->f_score = &f_score;
    }

    bool operator() (const pair<int, int> &loc1, const pair<int, int> &loc2) {
        return (*f_score)[loc1] > (*f_score)[loc2];
    }
};

typedef priority_queue<pair<int, int>, vector<pair<int, int> >, heuristic_comparison> location_queue;

double get_distance(const vector<vector<int> > &screen, pair<int, int> start_loc, pair<int, int> goal) {
    set<pair<int, int> > visited_closed;
    set<pair<int, int> > visited_opened;

    visited_opened.insert(start_loc);

    map<pair<int, int>, pair<int, int> > predecessor;

    map<pair<int, int>, int> g_score;
    g_score[start_loc] = 0;

    map<pair<int, int>, int> f_score;
    f_score[start_loc] = manhattan_distance(start_loc, goal);

    heuristic_comparison cmp(f_score);
    location_queue locations(cmp);
    locations.push(start_loc);
    int nodes_visited = 0;

    bool goal_reached = false;

    while (!locations.empty()) {
        nodes_visited += 1;
        pair<int, int> current_loc = locations.top();
        if (current_loc == goal) {
            goal_reached = true;
            break;
        }
        visited_opened.erase(visited_opened.find(current_loc));
        locations.pop();
        visited_closed.insert(current_loc);
        vector<pair<int, int> > neighbors = get_adjacent_locations(current_loc);
        for (int i = 0; i < neighbors.size(); ++i) {
            if (screen[neighbors[i].first][neighbors[i].second] == 0 &&
                (screen[neighbors[i].first - 1][neighbors[i].second] == 0 ||
                 screen[neighbors[i].first + 1][neighbors[i].second] == 0))
                continue;
            if (visited_closed.find(neighbors[i]) != visited_closed.end()) continue;
            double estimated_g_score = g_score[current_loc] + 1;
            bool neighbor_pushed = false;
            if (visited_opened.find(neighbors[i]) == visited_opened.end()) {
                visited_opened.insert(neighbors[i]);
                neighbor_pushed = true;
            }
            else if (estimated_g_score > g_score[neighbors[i]]) continue;
            predecessor[neighbors[i]] = current_loc;
            g_score[neighbors[i]] = estimated_g_score;
            f_score[neighbors[i]] = g_score[neighbors[i]] + manhattan_distance(neighbors[i], goal);
            if (neighbor_pushed) locations.push(neighbors[i]);
        }
    }

    pair<int, int> current_loc = goal;
    if (!goal_reached)
        return numeric_limits<double>::max();
    double distance = 0;
    while (current_loc != start_loc) {
        current_loc = predecessor[current_loc];
        distance += 1;
    }
    return distance;
}

void target_based_agent::set_junctions(set<loc> junctions) {
    this->junctions = junctions;
}

void target_based_agent::set_squares(vector<vector<loc> > squares) {
    this->squares = squares;
}

vector<loc> target_based_agent::get_square(pair<loc, loc> edge) {
    for (int i = 0; i < squares.size(); ++i) {
        if (find(squares[i].begin(), squares[i].end(), edge.first) != squares[i].end() &&
                find(squares[i].begin(), squares[i].end(), edge.second) != squares[i].end()) {
            return squares[i];
        }
    }
    vector<loc> empty_vec;
    return empty_vec;
}

vector<loc> target_based_agent::junction_neighbors(loc junction) {
    vector<loc> neighbors;
    set<loc>::iterator left_junction = junctions.end();
    set<loc>::iterator right_junction = junctions.end();
    int left_junction_column = 0;
    int right_junction_column = 10000;
    for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        if ((*it).second == junction.second
            && ((*it).first == junction.first - 30 || (*it).first == junction.first + 30)) {
            neighbors.push_back((*it));
        }
        if ((*it).first == junction.first && (*it).second > junction.second
            && (*it).second < right_junction_column) {
            right_junction_column = (*it).second;
            right_junction = it;
        }
        if ((*it).first == junction.first && (*it).second < junction.second
            && (*it).second > left_junction_column) {
            left_junction_column = (*it).second;
            left_junction = it;
        }
    }
    if (right_junction != junctions.end()) neighbors.push_back(*right_junction);
    if (left_junction != junctions.end()) neighbors.push_back(*left_junction);
    return neighbors;
}

bool edge_painted(pair<loc, loc> edge, vector<vector<int> > &screen) {
    if (edge.first.first == edge.second.first) { // Horizontal Edge
        if (edge.first.second < edge.second.second) {
            for (int i = 1; edge.first.second + i != edge.second.second; i++) {
                if (screen[edge.first.first + 4][edge.first.second + i] == 0) return false;
            }
            return true;
        }
        else {
            for (int i = 1; edge.second.second + i != edge.first.second; i++) {
                if (screen[edge.second.first + 4][edge.second.second + i] == 0) return false;
            }
            return true;
        }
    }
    else { // Vertical Edge
        if (edge.first.first < edge.second.first) {
            if (screen[edge.first.first + 6][edge.first.second] == 0) return false;
            else return true;
        }
        else {
            if (screen[edge.second.first + 6][edge.second.second] == 0) return false;
            else return true;
        }
    }
}

void target_based_agent::target_square(vector<vector<int> > &screen, loc amidar_loc) {
    loc target_loc = nearest_unpainted_loc(screen, amidar_loc);
//    cout << "Target found: " << target_loc.first << ", " << target_loc.second << endl;
    pair<loc, loc> edge = get_edge(target_loc);
    vector<loc> square_vec = get_square(edge);
    if (!square_vec.empty()) {
//        cout << "Part of a square" << endl;
        int least_index = -1;
        double least_dist = 10000;
        for (int i = 0; i < square_vec.size(); ++i) {
            double dist = euclidean_distance(square_vec[i], amidar_loc);
            if (dist < least_dist) {
                least_dist = dist;
                least_index = i;
            }
        }
        int left_index = least_index - 1, right_index = least_index + 1;
        if (least_index == 0) left_index = square_vec.size() - 1;
        else if (least_index == square_vec.size() - 1) right_index = 0;
        bool right_edge_painted = edge_painted(make_pair(square_vec[least_index], square_vec[right_index]), screen);
        bool left_edge_painted = edge_painted(make_pair(square_vec[least_index], square_vec[least_index]), screen);
        bool next_right_edge_painted = edge_painted(make_pair(square_vec[right_index],
                                                    square_vec[(right_index + 1) % square_vec.size()]), screen);
        bool next_left_edge_painted = edge_painted(make_pair(square_vec[left_index],
                                                   square_vec[(right_index + 1) % square_vec.size()]), screen);
        if (right_edge_painted) {
            if (left_edge_painted) {
//                cout << "both left and right edges painted" << endl;
                if (next_right_edge_painted) { // 3 edges painted
                    targets.push(square_vec[left_index]);
                    targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                    sleep(10);
                    return;
                }
                else if (next_left_edge_painted) { // 3 edges painted
                    targets.push(square_vec[right_index]);
                    targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                    return;
                }
                else {
                    double right_edge_length = euclidean_distance(square_vec[least_index], square_vec[right_index]);
                    double left_edge_length = euclidean_distance(square_vec[least_index], square_vec[left_index]);
                    if (right_edge_length < left_edge_length) {
                        /*targets.push(square_vec[least_index]);*/
                        targets.push(square_vec[right_index]);
                        targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                        targets.push(square_vec[left_index]);
                        return;
                    }
                    else {
                        /*targets.push(square_vec[least_index]);*/
                        targets.push(square_vec[left_index]);
                        targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                        targets.push(square_vec[right_index]);
                        return;
                    }
                }
            }
            else {
                if (next_right_edge_painted) {
                    if (next_left_edge_painted) {
                        targets.push(square_vec[least_index]);
                        targets.push(square_vec[left_index]);
                        return;
                    }
                    else {
                        targets.push(square_vec[least_index]);
                        targets.push(square_vec[left_index]);
                        targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                        return;
                    }
                }
                else {
                    targets.push(square_vec[least_index]);
                    targets.push(square_vec[left_index]);
                    targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                    targets.push(square_vec[right_index]);
                    return;
                }
            }
        }
        else {
            if (left_edge_painted) {
                if (next_right_edge_painted) {
                    if (next_left_edge_painted) {
                        targets.push(square_vec[least_index]);
                        targets.push(square_vec[right_index]);
                        return;
                    }
                    else {
                        targets.push(square_vec[least_index]);
                        targets.push(square_vec[right_index]);
                        targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                        targets.push(square_vec[left_index]);
                        return;
                    }
                }
                else {
                    if (next_left_edge_painted) {
                        targets.push(square_vec[least_index]);
                        targets.push(square_vec[right_index]);
                        targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                        return;
                    }
                    else {
                        targets.push(square_vec[least_index]);
                        targets.push(square_vec[left_index]);
                        targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                        targets.push(square_vec[right_index]);
                        return;
                    }
                }
            }
            else {
                targets.push(square_vec[least_index]);
                targets.push(square_vec[right_index]);
                targets.push(square_vec[(right_index + 1) % square_vec.size()]);
                targets.push(square_vec[left_index]);
                targets.push(square_vec[least_index]);
                return;
            }
        }
    }
    else {
        if (euclidean_distance(edge.first, amidar_loc) < euclidean_distance(edge.second, amidar_loc)) {
            targets.push(edge.first);
            targets.push(edge.second);
        }
        else {
            targets.push(edge.second);
            targets.push(edge.first);
        }
    }
}

bool amidar_on_border(loc amidar_loc) {
    if (amidar_loc.first == 14 || amidar_loc.first == 160 || amidar_loc.second == 17 || amidar_loc.second == 141) {
        return true;
    }
    else return false;
}

bool target_reached = false;
int paint_steps = 0;

Action target_based_agent::get_action(amidar_image image, vector<vector<int> > &screen) {
//    cout << targets.empty() << endl;
    if (target_reached) {
        if (paint_steps < 2){
            paint_steps++;
            return action_taken;
        }
        else {
            target_reached = false;
            paint_steps = 0;
        }
    }
    loc amidar_loc = image.detect_amidar(screen);
    update_amidar_location(amidar_loc);
    amidar_loc = amidar_location;
    update_ghost_locations(image.detect_ghosts(screen));
    if (ghost_locations.size() > 5) {
        ghost_locations.clear();
        amidar_mode = ROAM;
        while (!targets.empty()) targets.pop();
        target_reached = false;
        paint_steps = 0;
        action_taken = PLAYER_A_NOOP;
        return action_taken;
    }
    vector<loc> current_ghost_locations(ghost_locations.begin(), ghost_locations.end());
    bool near_ghost = false;
    for (int i = 0; i < current_ghost_locations.size(); ++i) {
        double dist = euclidean_distance(amidar_loc, current_ghost_locations[i]);
        if (dist < 25) {
            near_ghost = true;
            amidar_mode = ESCAPE;
            while (!targets.empty()) targets.pop();
//            cout << "Target size " << targets.size() << endl;
            break;
        }
    }
    if (!near_ghost && amidar_mode == ESCAPE) {
        amidar_mode = ROAM;
    }
    if (amidar_loc.first > 0 && image.check_amidar_intact(amidar_loc, screen)) {
        /*cout << "Amidar Location: " << endl;
        cout << amidar_loc.first << ", " << amidar_loc.second << endl;
        cout << "Ghost Locations: " << ghost_locations.size() << endl;
        for (int i = 0; i < current_ghost_locations.size(); ++i) {
            cout << current_ghost_locations[i].first << ", " << current_ghost_locations[i].second << endl;
        }
        cout << "Amidar mode: " << amidar_mode << endl;*/
        vector<direction> moves = image.get_valid_moves(amidar_loc, screen);
        if (amidar_at_junction(amidar_loc)) {
            if (amidar_loc.second != 141 &&
                    find(moves.begin(), moves.end(), RIGHT_DIR) == moves.end()) moves.push_back(RIGHT_DIR);
            if (amidar_loc.second != 17 &&
                    find(moves.begin(), moves.end(), LEFT_DIR) == moves.end()) moves.push_back(LEFT_DIR);
        }
        if (amidar_mode == ESCAPE) {
            direction least_cost_dir = NULL_DIR;
            double least_cost = 100000;
            for (int i = 0; i < moves.size(); ++i) {
                loc next_loc;
                if (moves[i] == UP_DIR) {
                    next_loc = make_pair(amidar_loc.first - 1, amidar_loc.second);
//                    cout << "Up dir" << endl;
                }
                else if (moves[i] == DOWN_DIR) {
                    next_loc = make_pair(amidar_loc.first + 1, amidar_loc.second);
//                    cout << "Down dir" << endl;
                }
                else if (moves[i] == LEFT_DIR) {
                    next_loc = make_pair(amidar_loc.first, amidar_loc.second - 1);
//                    cout << "Left dir" << endl;
                }
                else if (moves[i] == RIGHT_DIR) {
                    next_loc = make_pair(amidar_loc.first, amidar_loc.second + 1);
//                    cout << "Right dir" << endl;
                }
                double cost = corner_cost(next_loc);
                for (int j = 0; j < current_ghost_locations.size(); ++j) {
                    double ghost_cost_val = euclidean_distance(next_loc, current_ghost_locations[j]);
                    if (ghost_cost_val < 50) cost += ghost_cost(current_ghost_locations[j], next_loc);
                }
//                cout << cost << endl;
                if (cost < least_cost) {
                    least_cost_dir = moves[i];
                    least_cost = cost;
                }
            }
            switch (least_cost_dir) {
                case UP_DIR:
                    action_taken = PLAYER_A_UP;
                    break;
                case DOWN_DIR:
                    action_taken = PLAYER_A_DOWN;
                    break;
                case LEFT_DIR:
                    action_taken = PLAYER_A_LEFT;
                    break;
                case RIGHT_DIR:
                    action_taken = PLAYER_A_RIGHT;
            }
//            cout << action_taken << endl;
//            cout << endl;
            return action_taken;
        }
        else if (amidar_mode == ROAM) {
            if (targets.empty()) {
                target_square(screen, amidar_loc);
            }
            if (targets.empty()) {
                cout << "Targets empty" << endl;
                exit(0);
            }
            direction least_cost_dir = NULL_DIR;
            double least_cost = 100000;
//            cout << "Target: " << targets.front().first << ", " << targets.front().second << endl;
            for (int i = 0; i < moves.size(); ++i) {
                loc next_loc;
//                cout << moves[i] << endl;
                if (moves[i] == UP_DIR) next_loc = make_pair(amidar_loc.first - 1, amidar_loc.second);
                else if (moves[i] == DOWN_DIR) next_loc = make_pair(amidar_loc.first + 1, amidar_loc.second);
                else if (moves[i] == LEFT_DIR) next_loc = make_pair(amidar_loc.first, amidar_loc.second - 1);
                else if (moves[i] == RIGHT_DIR) next_loc = make_pair(amidar_loc.first, amidar_loc.second + 1);
                double cost = euclidean_distance(next_loc, targets.front());
                if (cost < least_cost) {
                    least_cost_dir = moves[i];
                    least_cost = cost;
                }
            }
            switch (least_cost_dir) {
                case UP_DIR:
                    action_taken = PLAYER_A_UP;
                    break;
                case DOWN_DIR:
                    action_taken = PLAYER_A_DOWN;
                    break;
                case LEFT_DIR:
                    action_taken = PLAYER_A_LEFT;
                    break;
                case RIGHT_DIR:
                    action_taken = PLAYER_A_RIGHT;
            }
//            cout << action_taken << endl;
//            cout << endl;
            double dist = 1;
            if (targets.front().first > 160) dist = targets.front().first - 160;
            if (targets.front().first == 165 && targets.front().second == 16) dist = dist + 1;
            if (least_cost <= dist) {
                targets.pop();
                target_reached = true;
            }
            return action_taken;
        }
    }
    else return action_taken;
}