#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include "point.hpp"

namespace game {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Point> Graph;
    class PointManager {
        public:
            PointManager();
            ~PointManager();
            // Updates the position of all points in the simulation
            void update();
        private:
            Graph graph;
    }
}