#include "target_based_agent.h"
#include "constants.h"
#include "math.h"
#include <queue>
#include <set>
#include <map>
#include <limits>
#include <ctime>

double euclidean_distance(const pair<int, int> &loc1, const pair<int, int> &loc2) {
    return sqrt(pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2));
}

target_based_agent::target_based_agent() {
    amidar_mode = ROAM;
    target_loc = make_pair(-1, -1);
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

loc nearest_unpainted_loc(const vector<vector<int> > &screen, loc amidar_loc) {
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
            screen[current_loc.first - 1][current_loc.second] != 0 &&
            screen[current_loc.first + 1][current_loc.second] != 0) return current_loc;
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

double manhattan_distance(const pair<int, int> &loc1, const pair<int, int> &loc2) {
    return fabs(loc1.first - loc2.first) + fabs(loc1.second - loc2.second);
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

vector<loc> target_based_agent::junction_neighbors(loc junction) {
    vector<loc> neighbors;
//    loc up_junction = make_pair(junction.first, junction.second - 30);
//    loc down_junction = make_pair(junction.first, junction.second + 30);
//    if (junctions.find(up_junction) != junctions.end()) neighbors.push_back(up_junction);
//    if (junctions.find(down_junction) != junctions.end()) neighbors.push_back(down_junction);
    cout << junction.first << ", " << junction.second << endl;
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
            cout << "right " << right_junction_column << " " << (*it).second << " " << junction.second << endl;
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

bool target_reached = false;
Action repeat_action = PLAYER_A_NOOP;
int paint_steps = 0;

Action target_based_agent::get_action(amidar_image image, vector<vector<int> > &screen) {
    if (target_reached) {
        if (paint_steps < 2){
            paint_steps++;
            return repeat_action;
        }
        else {
            target_reached = false;
            repeat_action = PLAYER_A_NOOP;
            paint_steps = 0;
        }
    }
    vector<loc> ghost_locations = image.detect_ghosts(screen);
    double ghost_cost = 0;
    for (int i = 0; i < ghost_locations.size(); ++i) {

    }
    loc amidar_loc = image.detect_amidar(screen);
    if (amidar_loc.first > 0) {
        if (amidar_mode == ROAM) {
            if (target_loc == make_pair(-1, -1)) {
                target_loc = nearest_unpainted_loc(screen, image.detect_amidar(screen));
                cout << "Unpainted " << target_loc.first << " " << target_loc.second << endl;
                loc least_dist_loc = make_pair(-1, -1);
                double least_dist = 100000;
                loc next_least_dist_loc = make_pair(-1, -1);
                double next_least_dist = 100000;
                for (set<loc>::iterator it = junctions.begin(); it != junctions.end(); it++) {
                    double dist = euclidean_distance(*it, target_loc);
                    if (dist < least_dist) {
                        next_least_dist = least_dist;
                        next_least_dist_loc = least_dist_loc;
                        least_dist = dist;
                        least_dist_loc = *it;
                    }
                    else if (dist < next_least_dist) {
                        next_least_dist = dist;
                        next_least_dist_loc = *it;
                    }
                }
                loc painting_loc = target_loc;
                target_loc = least_dist_loc;
                double angle2 = atan2(target_loc.first - painting_loc.first, target_loc.second - painting_loc.second);
                angle2 = angle2 * 180 / 3.14159265;
                if (angle2 < 0) angle2 = 360 + angle2;
                paint_loc = next_least_dist_loc;
                vector<loc> neighbors = junction_neighbors(target_loc);
                cout << endl;
                cout << "Target and paint: ";
                cout << target_loc.first << " " << target_loc.second << " ";
                cout << paint_loc.first << " " << paint_loc.second << endl;
                cout << "Neighbors: " << endl;
                double angle = 0;
                for (int i = 0; i < neighbors.size(); ++i) {
                    cout << neighbors[i].first << " " << neighbors[i].second << " ";
                    double angle1 = atan2(neighbors[i].first - painting_loc.first, neighbors[i].second - painting_loc.second);
                    angle1 = angle1 * 180 / 3.14159265;
                    if (angle1 < 0) angle1 = 360 + angle1;
                    double a = abs(angle1 - angle2);
                    if (a > 180) a = 360 - a;
                    cout << a << endl;
                    if (a > angle) {
                        paint_loc = neighbors[i];
                        angle = a;
                    }
                }
                cout << "Paint target changed to: " << paint_loc.first << " " << paint_loc.second << endl;
                cout << endl;
            }
            vector<direction> moves = image.get_valid_moves(amidar_loc, screen);
            direction least_cost_dir = NULL_DIR;
            double least_cost = 100000;
            cout << amidar_loc.first << " " << amidar_loc.second << endl;
//            random_shuffle(moves.begin(), moves.end());
            for (int i = 0; i < moves.size(); ++i) {
                cout << "Move " << moves[i] << " ";
                loc next_loc;
                if (moves[i] == UP_DIR) next_loc = make_pair(amidar_loc.first - 1, amidar_loc.second);
                else if (moves[i] == DOWN_DIR) next_loc = make_pair(amidar_loc.first + 1, amidar_loc.second);
                else if (moves[i] == LEFT_DIR) next_loc = make_pair(amidar_loc.first, amidar_loc.second - 1);
                else if (moves[i] == RIGHT_DIR) next_loc = make_pair(amidar_loc.first, amidar_loc.second + 1);
                //double cost = get_distance(screen, next_loc, target_loc);
                double cost = euclidean_distance(next_loc, target_loc);
                cout << "Cost " << cost << endl;
                if (cost < least_cost) {
                    least_cost_dir = moves[i];
                    least_cost = cost;
                }
            }
            Action next_action = PLAYER_A_NOOP;
            switch (least_cost_dir) {
                case UP_DIR:
                    next_action = PLAYER_A_UP;
                    break;
                case DOWN_DIR:
                    next_action = PLAYER_A_DOWN;
                    break;
                case LEFT_DIR:
                    next_action = PLAYER_A_LEFT;
                    break;
                case RIGHT_DIR:
                    next_action = PLAYER_A_RIGHT;
            }
            double dist = 1;
            if (target_loc.first > 160) dist = target_loc.first - 160;
            if (least_cost <= dist) {
                cout << "Target Reached" << endl;
                amidar_mode = PAINT;
                target_loc = make_pair(-1, -1);
                target_reached = true;
                repeat_action = next_action;
            }
            return next_action;
        }
        else if (amidar_mode == PAINT) {
            target_loc = paint_loc;
            cout << "Painting " << target_loc.first << " " << target_loc.second << endl;
            paint_loc = make_pair(-1, -1);
            amidar_mode = ROAM;
            return PLAYER_A_NOOP;
        }
    }
    else return PLAYER_A_NOOP;
}