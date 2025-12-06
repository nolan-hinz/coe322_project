#pragma once // Guaring multiple instance

#include "cell_lib.hpp"
#include <vector>
using std::vector;

class ocean {
private:
  vector<cell> grid;
  int m{10} , n{10};
public:
  // Constructor
  ocean( int n , int m ) : m(m) , n(n) {
    int n_elements = m*n;
    grid = vector<cell>(n_elements);
  };

  // Overloading
  cell operator () ( int i , int j ) { return get_cell(i,j); }
  
  // Methods
  cell get_cell( int i , int j ) { return grid.at( i*n + j ); }

  void set_cell( int i , int j , cell_type t ) { grid.at( i*n + j ) = t };
}; // End of ocean class
