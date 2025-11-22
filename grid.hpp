#pragma once // Make sure the file is included only once

#include <vector>
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
}; // End of cell class

class grid_2d {
private:
  vector<cell> grid_pts; // vector of grid pts, each is a cell
  int m , n; // m rows and n columns
public:
  // Constructor
  grid_2d( int m , int n ) : m(m) , n(n) , grid_pts(m*n) {};

  // Overloading
  cell& operator () ( int i , int j ) { return get_cell( i*n + j); } // Reference so we can modify the grid
  
  // Methods
  cell& get_cell( int i , int j ) { return grid_pts.at( i*n + j ); } // Reference so we can modify the grid
  
  cell_type get_cell_type( int i , int j ) { return grid_pts.at( i*n + j ).get_cell_type(); }
  
  void set_cell_type( int i , int j , cell_type t ) { grid_pts.at( i*n + j ).set_cell_type(t); }
}; // End defining 2d grid class
