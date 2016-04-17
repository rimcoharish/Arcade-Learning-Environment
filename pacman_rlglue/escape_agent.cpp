#include "escape_agent.h"

escape_agent::escape_agent() {}

double escape_agent::euclidean_distance(loc location1, loc location2) {
    return sqrt(pow(abs(location1.first - location2.first), 2) + pow(abs(location1.second - location2.second), 2));
}

double escape_agent::ghost_cost(loc pacman_loc, loc ghost_loc) {
    double cost = GHOST_COST / euclidean_distance(pacman_loc, ghost_loc);
    // cout << "Ghost: " << cost << ", ";
    return cost;
}

double escape_agent::corner_cost(loc pacman_loc) {
    if (pacman_loc.first > SCREEN_HEIGHT / 2) pacman_loc.first = MAZE_HEIGHT - pacman_loc.first;
    if (pacman_loc.second > SCREEN_WIDTH / 2) pacman_loc.second = SCREEN_WIDTH - pacman_loc.second;
    if (pacman_loc.first < 1 && pacman_loc.second < 1) return CORNER_COST;
    else return CORNER_COST / euclidean_distance(pacman_loc, make_pair(0, 0));
}

double escape_agent::teleport_cost(loc pacman_loc) {
    return (TELE_COST/euclidean_distance(pacman_loc,
                                         make_pair(double(UPPER_TELEPORTATION_ROW), 0)) +
            TELE_COST/euclidean_distance(pacman_loc,
                                         make_pair(double(UPPER_TELEPORTATION_ROW),
                                                   SCREEN_WIDTH)) +
            TELE_COST/euclidean_distance(pacman_loc,
                                         make_pair(double(LOWER_TELEPORTATION_ROW), 0)) +
            TELE_COST/euclidean_distance(pacman_loc,
                                         make_pair(double(LOWER_TELEPORTATION_ROW),
                                                   SCREEN_WIDTH)));
}

double escape_agent::pellet_cost(loc pacman_loc, vector<loc> pellet_loc) {
    double total_cost = 0.0;
    for (int i = 0; i < pellet_loc.size(); ++i)
    {
        total_cost += PELLET_COST/euclidean_distance(pacman_loc, pellet_loc[i]);
    }
    // cout << "Pellet: " << total_cost << ", ";
    return total_cost;
}

Action escape_agent::get_action(pacman_image p_image, vector<loc> object_locations, vector<loc> edible_ghosts) {
    loc pacman_loc = object_locations[0];
    if (!std::isnan(pacman_loc.first) && !std::isnan(pacman_loc.second)){
        vector<direction> valid_moves = p_image.get_valid_moves(pacman_loc);
        direction least_cost_dir = NULL_DIR;
        double cost = GHOST_COST * 4 + TELE_COST * 4 + CORNER_COST;
        for (size_t i = 0; i < valid_moves.size(); ++i) {
            direction dir = valid_moves[i];
            loc next_pacman_loc;
            if (dir == UP_DIR) {
                next_pacman_loc = make_pair(pacman_loc.first - 1, pacman_loc.second);
            }
            else if (dir == DOWN_DIR) {
                next_pacman_loc = make_pair(pacman_loc.first + 1, pacman_loc.second);
            }
            else if (dir == LEFT_DIR) {
                next_pacman_loc = make_pair(pacman_loc.first, pacman_loc.second - 1);
            }
            else if (dir == RIGHT_DIR) {
                next_pacman_loc = make_pair(pacman_loc.first, pacman_loc.second + 1);
            }
            double dir_cost = 0;
            if (edible_ghosts.size() > 0) {
                for (size_t edible_ghost = 0; edible_ghost < edible_ghosts.size(); ++edible_ghost) {
                    dir_cost -= ghost_cost(next_pacman_loc, edible_ghosts[edible_ghost]);
                }
            }
            else {
                dir_cost += corner_cost(next_pacman_loc) + teleport_cost(next_pacman_loc) +
                            pellet_cost(next_pacman_loc, p_image.get_pellet_pos());
                for (int ghost = 0; ghost < 4; ++ghost) {
                    double c = ghost_cost(next_pacman_loc, object_locations[ghost + 1]);
                    dir_cost += (std::isnan(c) ? 0: c);
                }
            }
            if (dir_cost < cost) {
                cost = dir_cost;
                least_cost_dir = dir;
            }
        }
        // cout << "\n";
        if (least_cost_dir == NULL_DIR) return PLAYER_A_NOOP;
        else if (least_cost_dir == UP_DIR) return PLAYER_A_UP;
        else if (least_cost_dir == DOWN_DIR) return PLAYER_A_DOWN;
        else if (least_cost_dir == LEFT_DIR) return PLAYER_A_LEFT;
        else if (least_cost_dir == RIGHT_DIR) return PLAYER_A_RIGHT;
        else return PLAYER_A_NOOP;
    }
    else return PLAYER_A_NOOP;
}
