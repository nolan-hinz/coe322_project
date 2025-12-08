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

  bool is_move_valid(pair<int,int> new_ij, cell_type ct, grid_2d &g) {
    // Unpack the variables
    auto [new_i,new_j] = new_ij;

    // First check if in bounds, return false if no
    if ( !(new_i>=0 && new_j>=0 && new_i<m && new_j<n) ) { return false; }
    
    // Check the old and the new grid
    cell_type last_destination = get_cell_type(new_i,new_j);
    cell_type destination = g.get_cell_type(new_i,new_j);

    // Give the options for the current cell types available
    switch (ct) {
    case cell_type::water_only :
      return true; break; // Water can go anywhere, though this isn't really needed, random_motion() should never let a water cell get here.
    case cell_type::garbage :
      if ( last_destination == cell_type::garbage || destination == cell_type::garbage ) { return false; break; }
      else { return true; break; } // Garbage can go anywhere except onto other garbage
    case cell_type::turtle :
      if ( last_destination == cell_type::ship || last_destination == cell_type::turtle ||
	   destination == cell_type::ship || destination == cell_type::turtle ) {
	return false;
      } // If current cell is a turtle do not let it move onto another turtle or ship ( prevents overwriting )
      else { return true; }
      break;
    case cell_type::ship :
      if ( last_destination == cell_type::ship || last_destination == cell_type::turtle ||
	   destination == cell_type::ship || destination == cell_type::turtle ) {
	return false;
      }  // If current cell is a ship do not let it move onto another turtle or ship ( prevents overwriting )
      else { return true; }
      break;
    } // End checking the if move is valid for particular cell_type
  } // End checking if move is valid

  pair<int,int> random_cell(int i, int j) {
    // Call up the engine 
    auto &generator = engine();
    std::uniform_int_distribution<int> zero_to_seven(0,7);

    // Cell deltas
    auto delta_i = vector<int> {1,1,1,0,-1,-1,-1,0};
    auto delta_j = vector<int> {-1,0,1,1,1,0,-1,-1};

    // Getting random cell
    int rand_cell = zero_to_seven(generator);

    // Return the random cell
    return {i+delta_i[rand_cell] , j+delta_j[rand_cell]};
  } // end getting a random cell

  pair<int,int> get_valid_random_move(int i, int j, cell_type ct, grid_2d &g) {
    // Counter and bool for our loop
    int is_valid = false;
    int tries_to_move = 0;

    // Values we need to get, we must init
    int new_i, new_j;
    
    // Allows for up to 100 attempts
    while ( !is_valid && tries_to_move<100 ) {
      auto [tmp_i,tmp_j] = random_cell(i,j);
      new_i = tmp_i;
      new_j = tmp_j;
      is_valid = is_move_valid({new_i,new_j}, ct, g);
      tries_to_move++;
    } // End loop over trying to move

    if ( !is_valid ) {
      return {i,j}; // Do not move
    } // End checking if we were able to move
    else {
      return {new_i,new_j};
    } // End returning valid indicies
  } // End get_valid_random_move

  pair<int,int> smart_ship_move(int i, int j, grid_2d &g) { 
    auto delta_i = vector<int> {1,1,1,0,-1,-1,-1,0};
    auto delta_j = vector<int> {-1,0,1,1,1,0,-1,-1};

    for ( auto [ii,jj] : neighbors(i,j) ) {
      if ( g.get_cell_type(ii,jj) == cell_type::garbage ) { std:: cout << ii << "|" << jj << '\n'; return {ii,jj}; }
      else { continue; }
    } // End loop over the neighbors

    // If no trash return random move
    return get_valid_random_move(i,j,cell_type::ship,g);    
  } // End getting smart move for a ship
   
  
  void random_motion( int i , int j , grid_2d &g , bool smart_ships , bool ocean_currents ) {
    // Takes indicies of a grid, determines what is at that grid point.
    // If it's a ship or a turtle it picks a new location for it in an adjacent cell.
    // It then makes that move on a different grid which is passed to the function (g)
    // The function checks the new grid (g), to make sure the move is valid.

    // 0    1     2
    // 7  CELL    3
    // 6    5     4
    
    // No need to move water
    if ( get_cell_type(i,j) == cell_type::water_only ) return;

    // Only move garbage if ocean currents are a thing
    if ( get_cell_type(i,j) == cell_type::garbage && ocean_currents ) {
      auto [new_i,new_j] = get_valid_random_move(i,j,cell_type::garbage,g);

      // Check if it actually moved
      if ( new_i == i && new_j == j ) {	g(i,j) = cell_type::garbage; return; }
      
      cell_type destination = g.get_cell_type(new_i,new_j);
      // If its a turtle, the trash wins, if its a ship, the ship wins
      if ( destination == cell_type::turtle ) {
	g(new_i,new_j) = cell_type::garbage;
	g(i,j) = cell_type::water_only;
      }
      else if ( destination == cell_type::ship ) {
	g(new_i,new_j) = cell_type::ship;
	g(i,j) = cell_type::water_only;
      }
      else {
	g(new_i,new_j) = cell_type::garbage;
	g(i,j) = cell_type::water_only;
      } // End moving trash
    } // End moving trash

    // No need to move trash if there are no currents
    else if ( get_cell_type(i,j) == cell_type::garbage && !ocean_currents ) return;

    else if ( get_cell_type(i,j) == cell_type::turtle ) {
      auto [new_i,new_j] = get_valid_random_move(i,j,cell_type::turtle,g);
      cell_type destination = g.get_cell_type(new_i,new_j);

      if ( new_i == i && new_j == j ) { g(i,j) = cell_type::turtle; return; }
      
      if ( destination == cell_type::garbage ) {
	g(new_i,new_j) = cell_type::garbage;
	g(i,j) = cell_type::water_only; // Turtle dies
      }
      else {
	g(new_i,new_j) = cell_type::turtle;
	g(i,j) = cell_type::water_only;
      } // End moving turtle if it goes to water
    } // End moving turtles

    else if ( get_cell_type(i,j) == cell_type::ship && smart_ships ) {
      auto [new_i,new_j] = smart_ship_move(i,j,g);

      if ( new_i == i && new_j == j ) { g(i,j) = cell_type::ship; return; }

      cell_type destination = g.get_cell_type(new_i,new_j);

      if ( destination == cell_type:: garbage ) {
	g(new_i,new_j) = cell_type::ship;
	g(i,j) = cell_type:: water_only;
      } // Ship picks up the trash
      else {
	g(new_i,new_j) = cell_type::ship;
	g(i,j) = cell_type::water_only;
      } // Ship moves to water	
    } // End moving smart ship
    
    else if ( get_cell_type(i,j) == cell_type::ship && !smart_ships ) {
      auto [new_i,new_j] = get_valid_random_move(i,j,cell_type::ship,g);
      cell_type destination = g.get_cell_type(new_i,new_j);

      if ( new_i == i && new_j == j ) { g(i,j) = cell_type::ship; return; }

      if ( destination == cell_type:: garbage ) {
	g(new_i,new_j) = cell_type::ship;
	g(i,j) = cell_type:: water_only;
      } // Ship picks up the trash
      else {
	g(new_i,new_j) = cell_type::ship;
	g(i,j) = cell_type::water_only;
      } // Ship moves to water
    } // End moving the ship
  } // End moving objedcts in the grid
  
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
