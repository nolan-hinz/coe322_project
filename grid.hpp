#pragma once // Make sure the file is included only once

#include <vector>
#include <random>
#include <algorithm>
#include <utility>
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
    std::random_device r;
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // KEEPING SEED SAME FOR TESTING REMEMBER TO REMOVE
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    std::default_random_engine generator(1000);
    // Shuffle using the generator we created
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
  } // End function for counting cells of a certain type
  
  void print_grid() {
    // Function that prints out the grid
    for (int i=0 ; i<m ; i++) {
      for (int j=0 ; j<n ; j++) {
	std::cout << get_cell(i,j);
      } // End loop over the columns
      std::cout << '\n';
    } // End loop over the rows
    for (int i=0 ; i<m ; i++) { std::cout << "-"; }
    std::cout << '\n'
  } // End printing out the grid
  
  cell& get_cell( int i , int j ) { return grid_pts.at( i*n + j ); } // Reference so we can modify the grid

  void random_motion( int i , int j , grid_2d &g ) {
    // Takes indicies of a grid, determines what is at that grid point.
    // If it's a ship or a turtle it picks a new location for it in an adjacent cell.
    // It then makes that move on a different grid which is passed to the function (g)
    // The function checks the new grid (g), to make sure the move is valid.
    
    // Random generator
    std::random_device r;
    std::default_random_engine generator{r()};
    std::uniform_int_distribution<int> distribution(0,7);
    
    // No need to move water or garbage
    if ( get_cell_type(i,j) == cell_type::water_only ) return {i,j};
    if ( get_cell_type(i,j) == cell_type::garbage ) return {i,j};
    if ( get_cell_type(i,j) == cell_type::turtle || get_cell_type(i,j) == cell_type::ship ) {

      // Delta of the cell moves
      auto delta_i = vector<int> {1,1,1,0,-1,-1,-1,0};
      auto delta_j = vector<int> {-1,0,1,1,1,0,-1,-1};
      
      // Need to check if the move is a valid one
      int new_i, new_j;
      bool valid_cell_move = false;
      int tries_to_move = 0;
      while ( !valid_cell_move && tries_to_move < 100 ) {
	// Get a cell to move to
	int move_to_cell = distribution(generator);
	new_i = i + delta_i[move_to_cell];
	new_j = j + delta_j[move_to_cell];

	// Check if cell CAN be moved to
	valid_cell_move = !(g(new_i,new_j).get_cell_type()==cell_type::ship ||
			    g(new_i,new_j).get_cell_type()==cell_type::turtle);	
       	tries_to_move++;
      } // End while loop over attempting to move
      
      // Check if we were actually able to move
      if (!valid_cell_move) {
	g(i,j) = get_cell_type(i,j);
	return; // exit function now
      } // End checking if a valid move was produced

      // Otherwise we will move it
      if (g(new_i,new_j).get_cell_type()==cell_type::garbage &&
	  get_cell_type()==cell_type::turtle) {
	g(new_i,new_j) = cell_type::garbage;
	g(i,j) = cell_type::water_only;
      } // End checking if turtle dies
      else if (g(new_i,new_j).get_cell_type()==cell_type::garbage &&
	       get_cell_type()==cell_type::ship) {
	g(new_i,new_j) = cell_type::ship;
	g(i,j) = cell_type::water_only;
      } // End checking if ship picks up trash
      else {
	g(new_i,new_j) = get_cell_type(i,j);
	g(i,j) = cell_type::water_only;
      } // End moving if the move to cell is just water
      
    } // End checking if its a turtle or ship
    
  } // End random_motion generation 
  
  vector<cell> get_surroundings(  ) {
    // Diagram of surroundings. The number represents the index in the return vector:
    // 0   1   2
    // 7  CELL 3
    // 6   5   4
    
  } // End finding the surroundings
  
  cell_type get_cell_type( int i , int j ) { return grid_pts.at( i*n + j ).get_cell_type(); }
  
  void set_cell_type( int i , int j , cell_type t ) { grid_pts.at( i*n + j ).set_cell_type(t); }
}; // End defining 2d grid class
