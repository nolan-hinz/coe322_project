#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cmath>
#include <tuple>
#include "ocean.hpp"
#include "cxxopts.hpp"

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
    ("r,turtle_rate","<double> rate of turtle population increase per timestep. For example, if there are 10 turtles and turtle_rate=1.1, 1 turtle will be added to the ocean the next timestep.",
     cxxopts::value<int>()->default_value("1"));
  options.add_options()
    ("T,timesteps","<int> number of timesteps to run for the ocean simulation.",
     cxxopts::value<int>()->default_value("50"));
  options.add_options()
    ("N,n_simulations","<int> number of simulations to be executed.",
     cxxopts::value<int>()->default_value("10000"));

  // Parse the input
  auto result = options.parse(argc,argv);

  if (result.count("help")) {
    std::cout << option.help() << '\n';
    exit(0);
  } // End giving help if user asks for it
  
  // Setting default values
  int n_rows 		= 20;
  int n_cols 		= 20;
  int n_turtles 	= 10;
  int n_ships 		= 5;
  int n_garbage 	= 15;
  double turtle_rate 	= 1.0;
  int timesteps 	= 50;
  int n_sims 		= 10000;
	
  // Get users inputs
  std::vector<int> v = result["size"].as<std::vector<int>>();
  if ( v.size==2 ) {
    n_rows = v[0];
    n_cols = v[1];
  }
  n_turtles = result["turtles"].as<int>();
  n_ships = result["boats"].as<int>();
  n_garbage = result["garbage"].as<int>();
  turtle_rate = result["turtle_rate"].as<double>();
  timesteps = result["timesteps"].as<int>();
  n_sims = result["n_simulations"].as<int>();

  // Init vectors to hold how many turtles, ships, and garbage are left at the end
  std::vector<int> end_turtles(n_sims);
  std::vector<int> end_ships(n_sims);
  std::vector<int> end_garbage(n_sims);

  // Loop over and run the simulation n_sims times
  for ( int i=0 ; i<n_sims ; i++ ) {
    ocean test_ocean(n_rows,n_cols);
    test_ocean.initiate_grid(n_ships,n_turtles,n_garbage);
    test_ocean.simulate(timesteps);
    auto [sim_turtle_count,sim_ship_count,sim_garbage_count] = test_ocean.count_items();
    end_turtles[i] = sim_turtle_count;
    end_ships[i] = sim_ship_count;
    end_garbage[i] = sim_garbage_count;
  } // Looping over the number of simulations to run

  auto compute_mean = []( const std::vector<int> &v ) {
    if ( v.size==1 ) { return v[0]; }
    auto sum  = std::ranges::fold_left(v, 0.0, std::plus<>());
    double mean = sum/v.size();
    return mean;
  }; // End computing the mean

  auto compute_std = [compute_mean]( std::vector<int> v ) {
    if ( v.size() == 1 ) { return 0.0; }
    auto mean = compute_mean(v);
    // Transform the array to store the squared difference with the mean
    std::transform(v.begin(),
		   v.end(),
		   v.begin(),
		   [&](int &n) {
		     double diff = n-mean;
		     return = diff*diff;
		   });
    auto sum = std::ranges::fold_left(v, 0.0, std::plus<>());
    double std = std::sqrt( sum/(v.size()-1) );
    return std;
  }; // End computing the standard deviation

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

  // Return the turtle vector, ship vector, and the garbage vector
  std::tuple<std::vector<int>,std::vector<int>,std::vector<int>> turtles_ships_garbage{end_turtles,end_ships,end_garbage};
  return turtles_ships_garbage;
} // End of int main
