#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cmath>
#include <tuple>
#include "ocean.hpp"
#include "cxxopts.hpp"

double compute_mean( const std::vector<int> &v ) {
  if (v.size()==1) { return v[0]; }
  auto sum = std::ranges::fold_left(v,0.0,std::plus<>());
  double mean = sum/v.size();
  return mean;
}

double compute_std( std::vector<int> v ) {
  if (v.size()==1) { return 0.0; }
  auto mean = compute_mean(v);
  std::transform(v.begin(),v.end(),v.begin(),
		 [&](int &n) {
		   double diff = n-mean;
		   return diff*diff;
		 });
  auto sum = std::ranges::fold_left(v,0.0,std::plus<>());
  double std = std::sqrt( sum/(v.size()-1) );
  return std;
}

int main( int argc, char ** argv ) {
  // Define the options for the user to input
  cxxopts::Options options
    ("Ocean Garbage Cleanup", "Simulate a simplified ocean with ships, turtles, and garbage. Ships clean garbage when it randomly moves onto it. Turtles die when they randomly move onto garbage. Turtles may breed at a specified rate to increase their population.");
  options.add_options()
    ("h,help","Usage Information");
  options.add_options()
    ("s,size","<int,int> n_rows,n_cols wanted in the ocean simulation.",
     cxxopts::value<std::vector<int>>()->default_value("20,20"));
  options.add_options()
    ("t,turtles","<int> number of turtles wanted in the ocean simulation.",
     cxxopts::value<int>()->default_value("10"));
  options.add_options()
    ("b,boats","<int> number of boats/turtles wanted for ocean garbage cleanup.",
     cxxopts::value<int>()->default_value("5"));
  options.add_options()
    ("g,garbage","<int> number of pieces of garbage wanted in the ocean for simulation.",
     cxxopts::value<int>()->default_value("15"));
  options.add_options()
    ("r,turtle_rate","<double,double> rate of turtle population per timestep increment. For example, if <1.1,5.0> were entered, the population of turtles is multiplied by 1.1 every 5 timesteps",
     cxxopts::value<vector<double>>()->default_value("1.0,5.0"));
  options.add_options()
    ("T,timesteps","<int> number of timesteps to run for the ocean simulation.",
     cxxopts::value<int>()->default_value("50"));
  options.add_options()
    ("N,n_simulations","<int> number of simulations to be executed.",
     cxxopts::value<int>()->default_value("10000"));
  options.add_options()
    ("p,printout","<bool> 1 if you want to print out ocean before and after, 0 otherwise.",
     cxxopts::value<bool>()->default_value("1"));
  options.add_options()
    ("i,intelligent_boats","<bool> -i if you want ships to move to grab trash if it is in an adjacent cell, 0 if you want random ship motion.",
     cxxopts::value<bool>()->default_value("0"));
  options.add_options()
    ("o,ocean_currents","<bool> -o if you want trash to randomly move (like there are ocean currents), 0 if you want them in place.",
     cxxopts::value<bool>()->default_value("0"));
  options.add_options()("ts,track_sardines","<bool> -ts if you want to track the sardine population",
			cxxopts::value<bool>()->default_value("0"));
  options.add_options()("sp,sardine_params","<double,double,double>, initial sardine population, rate at which sardines reproduce, how many sardines a turtle eats when the reproduction step is called.",
			cxxopts::value<vector<double>>()->default_value("100,1,0"));
					 
  // Parse the input
  auto result = options.parse(argc,argv);

  if (result.count("help")) {
    std::cout << options.help() << '\n';
    exit(0);
  } // End giving help if user asks for it
  
  // Setting default values
  int n_rows 		= 20;
  int n_cols 		= 20;
  int n_turtles 	= 10;
  int n_ships 		= 5;
  int n_garbage 	= 15;
  double turtle_rate 	= 1.0;
  double reproduction_tsteps = 5.0;
  int timesteps 	= 50;
  int n_sims 		= 10000;
  bool printgrid = true;
  bool smart_ships = false;
  bool ocean_currents = false;
  bool track_sardines = false;
  double init_sardine_pop = 100.0;
  double sardine_birth_rate = 1.0;
  double sardine_eaten_rate = 0.0;
	
  // Get users inputs
  std::vector<int> v = result["size"].as<std::vector<int>>();
  if ( v.size() == 2 ) {
    n_rows = v[0];
    n_cols = v[1];
  }
  n_turtles = result["turtles"].as<int>();
  n_ships = result["boats"].as<int>();
  n_garbage = result["garbage"].as<int>();
  std::vector<double> v2 = result["turtle_rate"].as<std::vector<double>>();
  if (v2.size() == 2) {
    turtle_rate = v2[0];
    reproduction_tsteps = v2[1];
  }
  int turtle_tsteps = reproduction_tsteps;
  timesteps = result["timesteps"].as<int>();
  n_sims = result["n_simulations"].as<int>();
  printgrid = result["printout"].as<bool>();
  smart_ships = result["intelligent_boats"].as<bool>();
  ocean_currents = result["ocean_currents"].as<bool>();
  track_sardines = result["track_sardines"].as<bool>();
  std::vector<double> v3 = result["sardine_params"].as<std::vector<double>>();
  if ( v3.size()==3 ) {
    init_sardine_pop = v3[0];
    sardine_birth_rate = v3[1];
    sardine_eaten_rate = v3[2];
  }

  int sardine_pop = std::round(init_sardine_pop);
  
  // Init vectors to hold how many turtles, ships, and garbage are left at the end
  std::vector<int> end_turtles(n_sims);
  std::vector<int> end_ships(n_sims);
  std::vector<int> end_garbage(n_sims);
  std::vector<int> end_sardines(n_sims);

  // Loop over and run the simulation n_sims times
  for ( int i=0 ; i<n_sims ; i++ ) {
    ocean test_ocean(n_rows,n_cols,sardine_pop);
    test_ocean.initiate_grid(n_ships,n_turtles,n_garbage);
    if (printgrid) { test_ocean.print_grid(); }
    test_ocean.simulate(timesteps, turtle_rate, reproduction_tsteps, smart_ships, ocean_currents,
			track_sardines, sardine_birth_rate, sardine_eaten_rate);
    if (printgrid) { test_ocean.print_grid(); }

    // We can use last_grid_items because last grid is updated after each forward step
    end_turtles[i] = test_ocean.count_last_grid_items(cell_type::turtle);
    end_ships[i] = test_ocean.count_last_grid_items(cell_type::ship);
    end_garbage[i] = test_ocean.count_last_grid_items(cell_type::garbage);
    end_sardines[i] = test_ocean.sardine_count();
  } // Looping over the number of simulations to run

  // Compute the mean ending amounts of each
  double turtle_mean = compute_mean(end_turtles);
  double ship_mean = compute_mean(end_ships);
  double garbage_mean = compute_mean(end_garbage);

  // Compute the standard deviation amounts of each
  double turtle_std = compute_std(end_turtles);
  double ship_std = compute_std(end_ships);
  double garbage_std = compute_std(end_garbage);

  // Tell the user the results
  std::cout << "After " << n_sims << " simulations, with " << timesteps << " timesteps each, theresults are in:" << '\n';
  std::cout << "Listed below is the mean and standard deviation of items left in the ocean at the end of each simulation." << '\n';
  std::cout << "Mean turtles: " << turtle_mean << '\n';
  std::cout << "Standard deviation of turtles: " << turtle_std << '\n';
  std::cout << "Mean ships: " << ship_mean << '\n';
  std::cout << "Standard deviation of ships: " << ship_std << '\n';
  std::cout << "Mean garbage: " << garbage_mean << '\n';
  std::cout << "Standard deviation of garbage: " << garbage_std << '\n';
  if ( track_sardines == true ) {
    std::cout << "Mean sardines: " << compute_mean(end_sardines) << '\n';
    std::cout << "Standard deviation of sardines: " << compute_std(end_sardines) << '\n';
  } // End displaying sardines if asked for 

  // Return the turtle vector, ship vector, and the garbage vector
  std::tuple<std::vector<int>,std::vector<int>,std::vector<int>> turtles_ships_garbage{end_turtles,end_ships,end_garbage};
  return 0;
  //return turtles_ships_garbage;
} // End of int main
