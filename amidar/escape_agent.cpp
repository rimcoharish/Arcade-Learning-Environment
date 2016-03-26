#include "escape_agent.h"
#include "astar.h"

escape_agent::escape_agent() {}

double euclidean_distance(loc location1, loc location2) {
    return sqrt(pow(abs(location1.first - location2.first), 2) + pow(abs(location1.second - location2.second), 2));
}

double escape_agent::ghost_cost(vector<vector<int> > &screen, loc amidar_loc, loc ghost_loc) {
    double cost = GHOST_COST / get_distance(screen, amidar_loc, ghost_loc);
    return cost;
}

double escape_agent::corner_cost(loc amidar_loc, int screen_height) {
    if (amidar_loc.first > screen_height / 2) amidar_loc.first = screen_height - amidar_loc.first;
    if (amidar_loc.second > SCREEN_WIDTH / 2) amidar_loc.second = SCREEN_WIDTH - amidar_loc.second;
    if (amidar_loc.first < 1 && amidar_loc.second < 1) return CORNER_COST;
    else return CORNER_COST / euclidean_distance(amidar_loc, make_pair(0, 0));
}

Action escape_agent::get_action(amidar_image image, vector<vector<int> > &screen) {
    loc amidar_loc = image.detect_amidar(screen);
    vector<loc> ghost_locations = image.detect_ghosts(screen);
    if (!isnan(amidar_loc.first) && !isnan(amidar_loc.second)){
        vector<direction> valid_moves = image.get_valid_moves(amidar_loc, screen);
        direction least_cost_dir = NULL_DIR;
        double cost = GHOST_COST * 5;
        for (size_t i = 0; i < valid_moves.size(); ++i) {
            direction dir = valid_moves[i];
            loc next_amidar_loc;
            if (dir == UP_DIR) {
                next_amidar_loc = make_pair(amidar_loc.first - 1, amidar_loc.second);
            }
            else if (dir == DOWN_DIR) {
                next_amidar_loc = make_pair(amidar_loc.first + 1, amidar_loc.second);
            }
            else if (dir == LEFT_DIR) {
                next_amidar_loc = make_pair(amidar_loc.first, amidar_loc.second - 1);
            }
            else if (dir == RIGHT_DIR) {
                next_amidar_loc = make_pair(amidar_loc.first, amidar_loc.second + 1);
            }
            double dir_cost = 0;
            dir_cost += corner_cost(next_amidar_loc, screen.size());
            for (int ghost = 0; ghost < ghost_locations.size(); ++ghost) {
                double c = ghost_cost(screen, next_amidar_loc, ghost_locations[ghost]);
                dir_cost += (isnan(c) ? 0: c);
            }
            if (dir_cost < cost) {
                cost = dir_cost;
                least_cost_dir = dir;
            }
        }
        if (least_cost_dir == NULL_DIR) return PLAYER_A_NOOP;
        else if (least_cost_dir == UP_DIR) return PLAYER_A_UP;
        else if (least_cost_dir == DOWN_DIR) return PLAYER_A_DOWN;
        else if (least_cost_dir == LEFT_DIR) return PLAYER_A_LEFT;
        else if (least_cost_dir == RIGHT_DIR) return PLAYER_A_RIGHT;
        else return PLAYER_A_NOOP;
    }
    else return PLAYER_A_NOOP;
}