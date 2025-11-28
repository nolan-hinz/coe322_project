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
}; // End of cell class

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
    std::random_device rd;
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // KEEPING SEED SAME FOR TESTING REMEMBER TO REMOVE
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    std::mt19937 generator(1000);
    // Shuffle using the generator we created
    std::shuffle(grid_pts.begin(), grid_pts.end(), generator);
  } // End of shuffle grid

  void print_grid() {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
	  cell_type t = get_cell_type(i,j);

            char symbol;
            switch (t) {
                case cell_type::water_only: symbol = ' ';  break;
                case cell_type::turtle:     symbol = 'O';  break;
                case cell_type::ship:       symbol = '|';  break;
                case cell_type::garbage:    symbol = 'X';  break;
                default:                    symbol = '?';  break;
            }

            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }
    std::cout << "---------------------------------" << '\n';
  }
  cell& get_cell( int i , int j ) { return grid_pts.at( i*n + j ); } // Reference so we can modify the grid

  pair<int,int> random_motion( int i , int j ) {
    // Takes indicies of a grid and will return updated indicies for where the cell went after

    // Random generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,7);
    
    // No need to move water or garbage
    if ( get_cell_type(i,j) == cell_type::water_only ) return {i,j};
    if ( get_cell_type(i,j) == cell_type::garbage ) return {i,j};
    if ( get_cell_type(i,j) == cell_type::turtle || get_cell_type(i,j) == cell_type::ship ) {
      // Need to check if the move is a valid one
      int new_i, new_j;
      bool valid_cell_move = false;
      while ( !valid_cell_move ) {
	// Get a cell to move to
	int move_to_cell = distribution(generator);
	auto [new_i,new_j] = get_new_indicies(i, j, move_to_cell);
	valid_cell_move = is_valid_move(i, j, new_i, new_j);
      }
      return {new_i,new_j};
    }
    return {-1,-1}; // User will know something is wrong
  }

  pair<int,int> get_new_indicies(int i, int j, int move_direction) {
    int new_i = i;
    int new_j = j;

    switch(move_direction) {
      case 0: new_i -= 1; new_j -= 1; break; // top-left
      case 1: new_i -= 1; new_j += 0; break; // top
      case 2: new_i -= 1; new_j += 1; break; // top-right
      case 3: new_i += 0; new_j += 1; break; // right
      case 4: new_i += 1; new_j += 1; break; // bottom-right
      case 5: new_i += 1; new_j += 0; break; // bottom
      case 6: new_i += 1; new_j -= 1; break; // bottom-left
      case 7: new_i += 0; new_j -= 1; break; // left
      default: break; // invalid direction, stay in place
    }

    return {new_i, new_j};
  } // End getting new indicies

  bool is_valid_move(int i, int j, int new_i, int new_j) {
    if (new_i < 0 || new_j < 0) return false;
    if (new_i > m || new_j > n) return false;
    //if ( get_cell_type(new_i,new_j) == cell_type::ship || get_cell_type(new_i,new_j) == cell_type::turtle ) return false;
    return true;
  } // Done checking if the move is valid
  
  vector<cell> get_surroundings(  ) {
    // Diagram of surroundings. The number represents the index in the return vector:
    // 0   1   2
    // 7  CELL 3
    // 6   5   4
    
  } // End finding the surroundings
  
  cell_type get_cell_type( int i , int j ) { return grid_pts.at( i*n + j ).get_cell_type(); }
  
  void set_cell_type( int i , int j , cell_type t ) { grid_pts.at( i*n + j ).set_cell_type(t); }
}; // End defining 2d grid class
