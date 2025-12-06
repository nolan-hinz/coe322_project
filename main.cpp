#include <iostream>
#include "ocean.hpp"

int main() {
  ocean test_ocean(40,40);
  test_ocean.initiate_grid(5,10,15);
  test_ocean.print_grid();
  test_ocean.simulate(1);
  test_ocean.print_grid();
  return 0;
} // End of int main
