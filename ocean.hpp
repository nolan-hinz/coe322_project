#pragma once // guard against multiple instances

#include "grid.hpp"
#include <vector>
#include <random>
#include <stdexcept>

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
  void step_forward(); // Steps forward in time one step
  void simulate( int T ); // Simulates for T time steps
    
}; // End defining the ocean class
