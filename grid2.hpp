#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <utility>
#include <iostream>
#include "random_gen.cpp"

using std::pair;
using std::vector;

// Define the enum class for cell contents
enum class cell_type { water_only = 0, turtle = 1, ship = 2, garbage = 3 };

class cell {
private:
    cell_type this_cell_type{cell_type::water_only};

public:
    cell() = default;
    cell(cell_type t) : this_cell_type(t) {}

    bool operator==(cell_type t) { return this_cell_type == t; }
    void operator=(cell_type t) { this_cell_type = t; }

    cell_type get_cell_type() const { return this_cell_type; }
    void set_cell_type(cell_type t) { this_cell_type = t; }

    friend std::ostream& operator<<(std::ostream &os, const cell &c);
};

// Pretty-print the grid cells
std::ostream& operator<<(std::ostream &os, const cell &c) {
    switch (c.this_cell_type) {
    case cell_type::water_only: os << " "; break;
    case cell_type::ship:       os << "|"; break;
    case cell_type::turtle:     os << "O"; break;
    case cell_type::garbage:    os << "X"; break;
    }
    return os;
}

class grid_2d {
private:
    vector<cell> grid_pts;
    int m, n;

public:
    grid_2d(int m, int n) : m(m), n(n), grid_pts(m * n) {}

    cell& operator()(int i, int j) { return get_cell(i, j); }

    void shuffle_grid() {
        auto &generator = engine();
        std::shuffle(grid_pts.begin(), grid_pts.end(), generator);
    }

    int get_num_cell_type(cell_type ct) const {
        int count = 0;
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                if (get_cell_type(i, j) == ct)
                    count++;
        return count;
    }

    void print_grid() const {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++)
                std::cout << get_cell_type(i, j);
            std::cout << "\n";
        }
        for (int i = 0; i < n; i++) std::cout << "-";
        std::cout << "\n";
    }

    cell& get_cell(int i, int j) { return grid_pts.at(i * n + j); }
    cell_type get_cell_type(int i, int j) const { return grid_pts.at(i * n + j).get_cell_type(); }
    void set_cell_type(int i, int j, cell_type t) { grid_pts.at(i * n + j).set_cell_type(t); }

    bool is_move_valid(pair<int,int> new_ij, cell_type ct, grid_2d &g) {
        auto [new_i, new_j] = new_ij;

        // Bounds check
        if (!(new_i >= 0 && new_j >= 0 && new_i < m && new_j < n))
            return false;

        cell_type last_destination = get_cell_type(new_i, new_j);
        cell_type destination      = g.get_cell_type(new_i, new_j);

        switch (ct) {
        case cell_type::water_only:
            return true;

        case cell_type::garbage:
            return !(last_destination == cell_type::garbage ||
                     destination      == cell_type::garbage ||
		     last_destination == cell_type::ship ||
		     destination == cell_type::ship ||
		     destination == cell_type::turtle ||
		     last_destination == cell_type::turtle );

        case cell_type::turtle:
            if (last_destination == cell_type::ship ||
                last_destination == cell_type::turtle ||
                destination      == cell_type::ship ||
                destination      == cell_type::turtle)
                return false;
            return true;

        case cell_type::ship:
            if (last_destination == cell_type::ship ||
                last_destination == cell_type::turtle ||
                destination      == cell_type::ship ||
                destination      == cell_type::turtle)
                return false;
            return true;
        }

        return false; // fallback
    }

    pair<int,int> random_cell(int i, int j) {
        auto &generator = engine();
        std::uniform_int_distribution<int> zero_to_seven(0, 7);

        vector<int> delta_i{1,1,1,0,-1,-1,-1,0};
        vector<int> delta_j{-1,0,1,1,1,0,-1,-1};

        int r = zero_to_seven(generator);
        return {i + delta_i[r], j + delta_j[r]};
    }

    pair<int,int> get_valid_random_move(int i, int j, cell_type ct, grid_2d &g) {
        int tries_to_move = 0;

        while (tries_to_move < 100) {
            auto [new_i, new_j] = random_cell(i, j);

            if (is_move_valid({new_i, new_j}, ct, g))
                return {new_i, new_j};

            tries_to_move++;
        }

        return {i, j}; // couldn't move
    }

    pair<int,int> smart_ship_move(int i, int j, grid_2d &g) {
        for (auto [ii, jj] : neighbors(i, j)) {
            if (g.get_cell_type(ii, jj) == cell_type::garbage)
                return {ii, jj};
        }
        return get_valid_random_move(i, j, cell_type::ship, g);
    }

    void random_motion(int i, int j, grid_2d &g, bool smart_ships, bool ocean_currents) {
        cell_type ct = get_cell_type(i, j);

        if (ct == cell_type::water_only) return;

        // Garbage movement only if currents enabled
        if (ct == cell_type::garbage && !ocean_currents)
            return;

        if (ct == cell_type::garbage && ocean_currents) {
	  auto [new_i, new_j] = get_valid_random_move(i, j, ct, g);

	  // If garbage can't move, just stay in place
	  if (new_i == i && new_j == j) {
	    g(i, j) = cell_type::garbage;
	    return;
	  }

	  cell_type dest = g.get_cell_type(new_i, new_j);
	  cell_type old_dest = get_cell_type(i,j);

	  // Garbage should never overwrite a ship
	  if (dest == cell_type::ship || old_dest == cell_type::ship) {
	    g(new_i,new_j) = cell_type::ship;  // stay in place
	    g(i,j) = cell_type::water_only;
	  } else {
	    // Move garbage
	    g(new_i, new_j) = cell_type::garbage;
	    g(i, j) = cell_type::water_only;
	  }
	  return;
	}        

        if (ct == cell_type::turtle) {
            auto [new_i, new_j] = get_valid_random_move(i, j, ct, g);
            if (new_i == i && new_j == j) {
                g(i, j) = cell_type::turtle;
                return;
            }

            cell_type dest = g.get_cell_type(new_i, new_j);

            if (dest == cell_type::garbage) {
                g(new_i, new_j) = cell_type::garbage; // turtle dies
            } else {
                g(new_i, new_j) = cell_type::turtle;
            }

            g(i, j) = cell_type::water_only;
            return;
        }

        if (ct == cell_type::ship) {
            auto [new_i, new_j] =
                smart_ships ? smart_ship_move(i, j, g)
                            : get_valid_random_move(i, j, ct, g);

            if (new_i == i && new_j == j) {
                g(i, j) = cell_type::ship;
                return;
            }

            if (g.get_cell_type(new_i, new_j) == cell_type::garbage) {
                g(new_i, new_j) = cell_type::ship; // picking up garbage
            } else {
                g(new_i, new_j) = cell_type::ship;
            }

            g(i, j) = cell_type::water_only;
            return;
        }
    }

    // ----------------------------
    // FIXED: neighbors() RETURNS
    // ----------------------------
    vector<pair<int,int>> neighbors(int i, int j) {
        vector<pair<int,int>> indices;

        vector<int> delta_i{1,1,1,0,-1,-1,-1,0};
        vector<int> delta_j{-1,0,1,1,1,0,-1,-1};

        for (int k = 0; k < 8; k++) {
            int ii = i + delta_i[k];
            int jj = j + delta_j[k];

            if (ii < 0 || ii >= m || jj < 0 || jj >= n)
                continue;

            indices.emplace_back(ii, jj);
        }

        return indices;
    }

    int count_around(int i, int j, cell_type ct) {
        int count = 0;
        for (auto [ii, jj] : neighbors(i, j))
            if (get_cell_type(ii, jj) == ct)
                count++;
        return count;
    }
};
