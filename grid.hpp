#pragma once // Make sure the file is included only once

#include <vector>
#include <random>
#include <algorithm>
#include <utility>
#include "random_gen.cpp"
using std::pair;
using std::vector;

// Define our enum class which holds the values a cell may hold
enum class cell_type { water_only=0 , turtle=1 , ship=2 , garbage=3 };

class cell {
private:
  // Default cell is open water
  cell_type this_cell_type{cell_type::water_only}; 
public:
  // Constructors
  cell() = default;
  cell( cell_type t ) : this_cell_type(t) {};

  // Overloading
  bool operator == ( cell_type t ) { return this_cell_type == t; }

  void operator = ( cell_type t ) { this_cell_type = t; } // Lets us easily set a cell to a value

  // Begin methods
  cell_type get_cell_type() { return this_cell_type; }

  void set_cell_type( cell_type t ) { this_cell_type = t; }

  // Declare friend function so << operator can get private info
  friend std::ostream& operator<<(std::ostream &os, const cell &c);
}; // End of cell class

// Overload << so we can cout a cell directly
std::ostream& operator<<(std::ostream &os, const cell &c) {
  switch (c.this_cell_type) {
  case cell_type::water_only: os << " "; break;
  case cell_type::ship:       os << "|"; break;
  case cell_type::turtle:     os << "O"; break;
  case cell_type::garbage:    os << "X"; break;
  } // Done writing correct cell type out to screen
  return os;
} // End overloading << to cout cells

class grid_2d {
private:
  vector<cell> grid_pts; // vector of grid pts, each is a cell
  int m , n; // m rows and n columns
public:
  // Constructor
  grid_2d( int m , int n ) : m(m) , n(n) , grid_pts(m*n) {};

  // Overloading
  cell& operator () ( int i , int j ) { return get_cell(i,j); } // Reference so we can modify the grid
  
  // Methods
  void shuffle_grid() {
    auto &generator = engine();
    std::shuffle(grid_pts.begin(), grid_pts.end(), generator);
  } // End of shuffle grid

  int get_num_cell_type( const cell_type &ct ) {
    // Takes a cell type and returns the amount of that cell in the grid
    int count = 0;
    for (int i=0 ; i<m ; i++) {
      for (int j=0 ; j<n ; j++) {
	if ( ct == get_cell_type(i,j) ) { count++; }
      } // End loop over columns
    } // End loop over rows
    return count;
  } // End function for counting cells of a certain type
  
  void print_grid() {
    // Function that prints out the grid
    for (int i=0 ; i<m ; i++) {
      for (int j=0 ; j<n ; j++) {
	std::cout << get_cell(i,j);
      } // End loop over the columns
      std::cout << '\n';
    } // End loop over the rows
    for (int i=0 ; i<n ; i++) { std::cout << "-"; }
    std::cout << '\n';
  } // End printing out the grid
  
  cell& get_cell( int i , int j ) { return grid_pts.at( i*n + j ); } // Reference so we can modify the grid

  void random_motion( int i , int j , grid_2d &g , bool smart_ships , bool ocean_currents ) {
    // Takes indicies of a grid, determines what is at that grid point.
    // If it's a ship or a turtle it picks a new location for it in an adjacent cell.
    // It then makes that move on a different grid which is passed to the function (g)
    // The function checks the new grid (g), to make sure the move is valid.

    // 0    1     2
    // 7  CELL    3
    // 6    5     4
    
    // Deltas in position based on which cell around we are picking ordered from 0 to 7 as shown
    auto delta_i = vector<int> {1,1,1,0,-1,-1,-1,0};
    auto delta_j = vector<int> {-1,0,1,1,1,0,-1,-1};
    
    // Random generator
    auto &generator = engine();
    std::uniform_int_distribution<int> distribution(0,7);
    
    // No need to move water or garbage
    if ( get_cell_type(i,j) == cell_type::water_only ) return;
    if ( get_cell_type(i,j) == cell_type::garbage && ocean_currents ) {
      int new_cell = distribution(generator);
      

    else if ( get_cell_type(i,j) == cell_type::garbage && !ocean_currents ) return;
    if ( get_cell_type(i,j) == cell_type::turtle ) {
      // Need to check if the move is a valid one
      int new_i, new_j;
      bool valid_cell_move = false;
      int tries_to_move = 0;
      while ( !valid_cell_move && tries_to_move < 100 ) {
	// Get a cell to move to
	int move_to_cell = distribution(generator);
	new_i = i + delta_i[move_to_cell];
	new_j = j + delta_j[move_to_cell];
	if (!(new_i >= 0 && new_i < m && new_j >= 0 && new_j < n)) {
	  valid_cell_move=false;
	  tries_to_move++;
          continue;
	} // End check if in bounds

	cell_type last_destination = get_cell_type(new_i,new_j);
	cell_type destination = g.get_cell_type(new_i,new_j);
	if (destination == cell_type::ship || destination == cell_type::turtle) {
	  valid_cell_move=false;
	  tries_to_move++;
	  continue;
	} // End check if destination already has something on it

	if (last_destination == cell_type::ship || last_destination == cell_type::turtle) {
	  valid_cell_move=false;
	  tries_to_move++;
	  continue;
	}
	else { valid_cell_move=true; }
      } // End while loop over attempting to move
      
      // Check if we were actually able to move
      if (!valid_cell_move) {
	g(i,j) = get_cell_type(i,j);
	return; // exit function now
      } // End checking if a valid move was produced

      // Otherwise we will move it
      cell_type destination = g.get_cell_type(new_i,new_j);
      cell_type cur_type = get_cell_type(i,j);
      if (destination==cell_type::garbage && cur_type==cell_type::turtle) { g(i,j) = cell_type::water_only; } // Turtle dies, trash remains
      else if (destination==cell_type::garbage && cur_type==cell_type::ship) {
	g(i,j) = cell_type::water_only; // Ship moves away so now its water
	g(new_i,new_j) = cell_type::ship; // Ship replaces the trash
      } // End replacing trash with ship
      else {
	g(new_i,new_j) = cur_type;
	g(i,j) = cell_type::water_only;
      } // If the first two ifs did not trip anything then we know it is water only
    } // End checking if its a turtle or ship
  } // End random_motion generation

  vector<pair<int,int>> neighbors(int i, int j) {
    // Init the return and the deltas in position
    vector<pair<int,int>> indicies;
    auto delta_i = vector<int> {1,1,1,0,-1,-1,-1,0};
    auto delta_j = vector<int> {-1,0,1,1,1,0,-1,-1};

    for ( int k=0 ; k<=7 ; k++ ) {
      int ii = i+delta_i[k];
      int jj = j+delta_j[k];
      if ( ii<0 || ii>=m || jj<0 || jj>=n ) { continue; }
      else { indicies.push_back({ii,jj}); }
    } // End loop over neighbors
  } // End returning neighboring indicies

  int count_around(int i, int j, cell_type ct) {
    int count = 0;
    for ( auto [ii,jj] : neighbors(i,j) ) {
      if ( get_cell_type(ii,jj) == ct ) { count++; }
    } // End loop over the neighboring cells
    return count;
  } // End counting the type of objects around a certain cell
  
  cell_type get_cell_type( int i , int j ) { return grid_pts.at( i*n + j ).get_cell_type(); }
  
  void set_cell_type( int i , int j , cell_type t ) { grid_pts.at( i*n + j ).set_cell_type(t); }
}; // End defining 2d grid class
