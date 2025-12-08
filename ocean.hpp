#pragma once // guard against multiple instances

#include "grid.hpp"
#include "random_gen.cpp"
#include <vector>
#include <random>
#include <stdexcept>
#include <utility>
#include <tuple>
#include <cmath>

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
    } // End filling with ships
    
    for ( int turtle=0 ; turtle < turtle_count ; turtle++ ) {
      int i = idx/n_cols;
      int j = idx%n_cols;
      last_grid(i,j) = cell_type::turtle;
      idx++;
    } // End filling with turtles
    
    for ( int garbage=0 ; garbage < garbage_count ; garbage++ ) {
      int i = idx/n_cols;
      int j = idx%n_cols;
      last_grid(i,j) = cell_type::garbage;
      idx++;
    } // End filling with garbage

    // Done filling grid so shuffle it before we begin
    last_grid.shuffle_grid();
  } // Done initiateing tshe random grid
  void print_grid() { last_grid.print_grid(); }; // printout of the grid

  void reproduce_turtles( double rate ) {
    int current_turtle_count = last_grid.get_num_cell_type(cell_type::turtle);

    // Get turtles to add
    int delta_turtles = static_cast<int>(std::round(rate*current_turtle_count - current_turtle_count));

    for ( int i=0 ; i<delta_turtles ; i++ ) {
      for ( auto [ii,jj] : permuted_indicies() ) {
	if (last_grid.get_cell_type(ii,jj) == cell_type::water_only) {
	  last_grid(ii,jj) = cell_type::turtle;
	  break;
	} // Done checking if we can place a turtle here
      } // End looping over indicies of grid
    } // End looping over number of turtles to add to the ocean
  } // End reproducing turtles

  std::vector<std::pair<int,int>> permuted_indicies() {
    // Function returns indicies of our grid in a random order (for random updates)
    std::vector<pair<int,int>> indicies;
    for (int i=0 ; i<n_rows ; i++) {
      for (int j=0 ; j<n_cols ; j++) {
	indicies.push_back({i,j});
      } // End loop over columns
    } // End loop over rows

    // Shuffle the indicies and return them
    auto &generator = engine();                        ;
    std::shuffle(indicies.begin(),indicies.end(),generator);
    return indicies;
  } // End shuffling the indicies of the grid
  
  void step_forward() { // Steps forward in time one step
    // Below is the diagram for how are ships will pick to move
    // 0  1  2
    // 7  S  3
    // 6  5  4
    // Ship will pick a random square around it assuming it is not
    // the edge and then it will move it there
    
    // First loop over and transfer just the garbage to the new grid
    for ( int i=0 ; i < n_rows ; i++ ) {
      for ( int j=0 ; j < n_cols ; j++ ) {
	cell_type cur_type = last_grid(i,j).get_cell_type();
	if ( cur_type == cell_type::garbage ) { current_grid(i,j) = cur_type; }
	else { current_grid(i,j) = cell_type::water_only; }
      } // End loop over columns
    } // End loop over rows

    // Next do loop over whole ocean, this time randomly so change up the order of update
    for ( auto [i,j] : permuted_indicies() ) {
      last_grid.random_motion(i,j,current_grid);
    } // End loop over permuted indicies
    last_grid = current_grid; // Update the last grid to be the current grid
  } // End grid update

  int count_around(int i, int j, cell_type ct) { return last_grid.count_around(i,j,ct); }

  int count_last_grid_items(cell_type ct) { return last_grid.get_num_cell_type(ct); }
  
  void simulate( int T ) { // Simulates for T time steps
    for ( int t=0; t < T; t++ ) {
      step_forward();
    }
  }
}; // End defining the ocean class
