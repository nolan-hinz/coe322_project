#pragma once // Make sure the file is included only once

// Define our enum class which holds the values a cell may hold
enum class cell_type { water_only=0 , turtle=1 , ship=2 , garbage=3 };

// Define the cell class which will make up the ocean
class cell {
private:
  // Default cell is open water
  cell_type this_cell_type{cell_type::water_only}; 
public:
  // Constructors
  cell() = default;
  cell( cell_type t ) : this_cell_type(t) {};

  // Begin methods
  cell_type get_cell_type() { return this_cell_type; }

  void set_cell_type( cell_type t ) { this_cell_type = t; }
}; // End of cell class
