#pragma once
#include "point.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>


namespace game {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Point> Graph;
    class PointManager {
        public:
            PointManager(): graph(Graph()) {}

            ~PointManager() = default;

            // Updates the position of all points in the simulation
            void update();

            // Add a point to the graph
            void add_point(Point point);
        private:
            Graph graph;
    };
}