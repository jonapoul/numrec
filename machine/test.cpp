#include <iostream>
#include <chrono>
namespace chr = std::chrono;

#include <mlpack/core.hpp>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>
using namespace mlpack;
using namespace mlpack::neighbor; // NeighborSearch and NearestNeighborSort
using namespace mlpack::metric;   // ManhattanDistance

#include "test.h"
#include "../global.h"

void covariance(const arma::mat& data) {
   STARTING();
   Timer::Start(__FUNCTION__);

   const arma::mat original = trans(data);
   // original.raw_print("Original Data:");
   // data.raw_print("Transposed Data:");

   /* Now compute the covariance.  We assume that the data is already centered.
      Remember, because the matrix is column-major, the covariance operation is
      transposed. */
   const arma::mat covariance = data * trans(data) / data.n_cols;
   covariance.print("Covariance:");

   Timer::Stop(__FUNCTION__);
   printf("%s() ran for %.2e secs\n", __FUNCTION__, 1e-3*chr::duration<double>(Timer::Get(__FUNCTION__)).count());
   ENDING();
}

void nearest_neighbours() {
   STARTING();
   Timer::Start(__FUNCTION__);
   // Load the data from data.csv (hard-coded).  Use CLI for simple command-line
   // parameter handling.
   arma::mat data;
   data::Load("machine/data.csv", data, true);
   data.print("Transposed Data:");
   // Use templates to specify that we want a NeighborSearch object which uses
   // the Manhattan distance.
   NeighborSearch<NearestNeighborSort, ManhattanDistance> nn(data);
   // Create the object we will store the nearest neighbours in.
   arma::Mat<size_t> neighbours;
   arma::mat distances; // We need to store the distance too.
   // Compute the neighbours.
   nn.Search(1, neighbours, distances);
   neighbours.print("Neighbours:");
   distances.print("Distances");
   // Write each neighbor and distance using Log.
   for (size_t i = 0; i < neighbours.n_elem; ++i) {
      printf("i=%2zu neighbour=%2zu distance=%f\n", i, neighbours[i], distances[i]);
   }
   Timer::Stop(__FUNCTION__);
   printf("%s() ran for %.2e secs\n", __FUNCTION__, 1e-3*chr::duration<double>(Timer::Get(__FUNCTION__)).count());
   ENDING();
}