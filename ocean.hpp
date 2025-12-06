#pragma once // guard against multiple instances

#include "grid.hpp"
#include <vector>
#include <random>
#include <stdexcept>
#include <utility>
#include <tuple>

using std::vector;

class ocean {
private:
  grid_2d current_grid , last_grid;
  int n_cells, n_rows , n_cols;
public:
  // creating an ocean of size m and n
    ocean( int n_rows , int n_cols ) : current_grid( n_rows , n_cols ) , last_grid( n_rows , n_cols ) , n_cells(n_rows*n_cols) , n_rows(n_rows) , n_cols(n_cols) {};

  // Methods
  void initiate_grid( int ship_count , int turtle_count, int garbage_count ) { // Initiates the very first grid
    int total_occupied = ship_count+turtle_count+garbage_count;
    if (total_occupied > n_cells) throw std::runtime_error("More occupied cells than number of cells in the grid. Fix your inputs.");
    
    // Building a vector with the positions of the objects
    vector<int> grid_locations(total_occupied);
    std::iota(grid_locations.begin(), grid_locations.end(), 0);

    // Fill in sequentially we will shuffle list later
    int idx = 0;
    for ( int ship=0 ; ship < ship_count ; ship++ ) {
      int i = idx/n_cols;
      int j = idx%n_cols;
      last_grid(i,j) = cell_type::ship;
      idx++;
    }
    
    for ( int turtle=0 ; turtle < turtle_count ; turtle++ ) {
      int i = idx/n_cols;
      int j = idx%n_cols;
      last_grid(i,j) = cell_type::turtle;
      idx++;
    }
    
    for ( int garbage=0 ; garbage < garbage_count ; garbage++ ) {
      int i = idx/n_cols;
      int j = idx%n_cols;
      last_grid(i,j) = cell_type::garbage;
      idx++;
    }

    // Now we are done filling grid with correct number of ships, garbage and turtles, we should randomly shuffle it before we start the simulation
    last_grid.shuffle_grid();
    
    // Now we got a shuffled grid we can start simulation
      
  } // Done initiateing tshe random grid
  void print_grid() { last_grid.print_grid(); }; // printout of the grid
  std::tuple<int,int,bool> get_new_position( int i, int j, int move, const grid_2d &grid ) {
    // these vecs are how the row and the column of the object change
    vector<int> delta_i = { -1, -1, -1, 0, 1, 1, 1, 0 };
    vector<int> delta_j = { -1, 0, 1, 1, 1, 0, -1, -1 };
    
    // Get the new position
    int new_i = i+delta_i[move];
    int new_j = j+delta_j[move];

    // Check if the move is in bounds
    bool is_inbounds = false;
    if (new_i >= 0 && new_j >= 0 && new_i < grid.m && new_j < grid.n) {
      is_inbounds = true;
    }
        
    return {new_i,new_j,is_inbounds};
  }
  
  void step_forward() { // Steps forward in time one step
    // Below is the diagram for how are ships will pick to move
    // 0  1  2
    // 7  S  3
    // 6  5  4
    // Ship will pick a random square around it assuming it is not
    // the edge and then it will move it there
    for ( int i=0 ; i < grid.m ; i++ ) {
      for ( int j=0 ; j < grid.n ; j++ ) {
	
  }

  void simulate( int T ); // Simulates for T time steps
    
}; // End defining the ocean class
