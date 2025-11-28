#pragma once // guard against multiple instances

#include "grid.hpp"
#include <vector>
#include <random>
#include <stdexcept>

using std::vector;

class ocean {
private:
  grid_2d current_grid , last_grid;
  int n_ships , n_turtles , n_garbage , n_cells;
public:
  // creating an ocean of size m and n
  ocean( int n_rows , int n_cols , int n_ships , int n_turtles , int n_garbage ) : current_grid( n_rows , n_cols ) , last_grid( n_rows , n_cols ) , n_ships(n_ships) , n_turtles(n_turtles) , n_garbage(n_garbage) , n_cells(n_rows*n_cols) {};

  // Methods
  void initiate_grid( int ship_count , int turtle_count, int garbage_count ) { // Initiates the very first grid
    total_occupied = ship_count+turtle_count+garbage_count;
    if (total_occupied > n_cells) throw std::runtime_error("More occupied cells than number of cells in the grid. Fix your inputs.");

    // Building a vector with the positions of the objects
    vector<int> grid_locations(total_occupied);
    std::iota(grid_locations.begin(), grid_locations.end(), 0);

    int idx = 0;
    
    for ( int ship=0 ; ship < ship_count ; ship++ ) {
      int i = idx/cols;
      int j = idx%cols;
      
  } // Done initiateing the random grid
  void step_forward(); // Steps forward in time one step
  void simulate( int T ); // Simulates for T time steps
  
  
}; // End defining the ocean class
