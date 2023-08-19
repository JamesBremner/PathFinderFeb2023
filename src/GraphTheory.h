#pragma once
#include <set>
#include <functional>
#include "cGraph.h"

namespace raven
{
    namespace graph
    {
        enum class graph_calc
        {
            none,
            cost,
            cycle,
            astar,
            tour,
            obs,
            spans,
            sales,
            cliques,
            flows,
            multiflows,
            allpaths,
            probs,
            alloc,
            euler,
            cover,
            explore,
            cuts,
        };

        struct sGraphData
        {
            cGraph g;
            std::string fname;
            std::vector<double> edgeWeight;
            std::string startName;
            std::vector<std::string> multiStart;
            std::string endName;
            graph_calc option;
        };

        class cTourNodes
        {
        public:
            // cTourNodes(const cGraph &theGraph)
            //     : g(theGraph)
            // {
            // }

            void calculate(sGraphData &gd);

            std::vector<int> getTour() const
            {
                return tour;
            }

            std::vector<std::pair<int, int>>
            spanTree_get() const;

            int unvisitedCount() const
            {
                return unvisited;
            }
            int revisitedCount() const
            {
                return revisited.size();
            }

        private:
            const cGraph *g;
            std::vector<double> myEdgeWeights;
            cGraph spanTree;

            int dfsStart;
            int unvisited;
            std::vector<int> vleaf;
            std::vector<bool> spanVisited;
            std::vector<int> revisited;

            std::vector<int> tour;

            void tourNodesAdd(int v);

            bool visitor(int v);

            /// @brief find leaf to jump to from leaf
            /// @param v
            /// @return >= 0 node index to jump to
            /// @return -1 no jump required
            /// @return -2 no unvisited leaves

            int isLeafJump(int v);

            int PathToUnvisited();

            std::vector<int> vectorgraphIndexFromSpanIndex(
                const std::vector<int> &visp);
        };

        class cSpanningTree
        {
        public:
            cGraph mySpanningTree;
            std::set<int> myVertexSet;

            void add(
                const cGraph &g,
                int v, int w);

            int vertexCount() const
            {
                return myVertexSet.size();
            }
        };

        /// @brief Travelling Salesman Problem using Branch and Bound.
        class cTSP
        {
        public:
            cTSP(raven::graph::cGraph &inputGraph,
                 const std::vector<double> &vEdgeWeight);

            std::vector<int> calculate();

            int TotalPathEdgeWeight() const
            {
                return final_res;
            }

        private:
            // final_path[] stores the final solution ie, the
            // path of the salesman.
            std::vector<int> final_path;

            std::vector<int> curr_path;

            // visited[] keeps track of the already visited nodes
            // in a particular path
            std::vector<bool> visited;

            // Stores the final minimum weight of shortest tour.
            int final_res;

            raven::graph::cGraph &g;
            const std::vector<double> &myEdgeWeight;

            void TSPRec(int curr_bound,
                        int curr_weight,
                        int level);

            int firstMin(int i);
            int secondMin(int i);

            int edgeWeight(int i, int j) const;
        };

        /// @brief Find articulation points in an undirected graph
        /// An articulation point is a vertes whose removal 
        /// increases the number of connected components in the graph.

        class cTarjan
        {
        public:
            /// @brief Find articulation points with Tarjan's algorithm
            /// @param gd undirected graph description ( directed will raise exception )
            /// @return vector of articulation point names

            std::vector<std::string> ArticulationPoints(sGraphData &gd);

        private:
            std::vector<bool> visited;  // true if vertex index has been visited
            std::vector<int> disc;      // discovery times of visited vertices
            std::vector<int> low;       // earliest visited vertex (the vertex with minimum
                                        // discovery time) that can be reached from subtree
                                        // rooted with vertex
            std::set<int> sAP;          // set of articulation point vertex indices

            /// @brief Recursive search graph for articulation points
            /// @param gd graph description
            /// @param time current time
            /// @param parent vertex index to start search from

            void APRecurse(
                sGraphData &gd,
                int &time, int parent );
        };

        /// @brief read input file
        /// @param[in/out] graphData to store input

        void readfile(sGraphData &graphData);

        /// @brief read edge attribute as integer
        /// @param g graph
        /// @param i source vertex index
        /// @param j destination verted index
        /// @param ai attribute index
        /// @return integer value
        int rEdgeAttrInt(const cGraph &g, int i, int j, int ai);

        /// @brief find shortest path from start node to every other
        /// @param g
        /// @param start vertex index
        /// @param[out] dist shortest distance from start to each node
        /// @param[out] pred previous node on shortest path to each node

        void dijsktra(
            const cGraph &g,
            const std::vector<double> &edgeWeight,
            int start,
            std::vector<double> &dist,
            std::vector<int> &pred);

        /// @brief find shortest path from start to end node
        /// @param gd  graph data
        /// @return pair: vector of node indices on the path, path cost
        /// @return pair: empty vector, -1 when end is not reachable from start

        std::pair<std::vector<int>, double>
        path(sGraphData &gd);

        /// @brief find all paths between two nodes
        /// @param gd  graph data
        /// @return vector of vectors of node indices on paths

        std::vector<std::vector<int>>
        allPaths(sGraphData &gd);

        std::pair<std::vector<int>, double>
        path(sGraphData &gd);

        /// @brief find spanning tree
        /// @param gd  graph data
        /// @return graph - a tree rooted at start and visiting every node

        cGraph
        spanningTree(sGraphData &gd);

        /// @brief depth first search
        /// @param g
        /// @param startIndex
        /// @param visitor function to call when a new node is reached
        /// visitor should return true, but false if the search should stop

        void dfs(
            const cGraph &g,
            int startIndex,
            std::function<bool(int v)> visitor);

        /// @brief cycle finder
        /// @param gd  graph data
        /// @return vector of cycles

        std::vector<std::vector<int>>
        dfs_cycle_finder(sGraphData &gd);

        /// @brief find all paths between 2 vertices
        /// @param gd  graph data
        /// @return vector of path vectors

        std::vector<std::vector<int>>
        dfs_allpaths(sGraphData &gd);

        /// @brief path visiting every node

        std::vector<int>
        tourNodes(
            const cGraph &g);

        void cliques(
            const cGraph &g,
            std::string &results);

        /// @brief Maximum flow between two vertices
        /// @param gd  graph data
        /// @param[out] vEdgeFlow flow through each edge
        /// @return total flow

        double flows(
            sGraphData &gd,
            std::vector<int> &vEdgeFlow);

        /// @brief Maximum flow through graph with multiple sources
        /// @param gd  graph data
        /// @return maximum flow

        double multiflows(sGraphData &gd);

        /// @brief Find connected source and sinks
        /// @param g the graph
        /// @return A vector of vectors containing a source index and the connected sink indices
        ///
        /// A source has a zero in-degree, a sink has a zero out-degree

        std::vector<std::vector<int>> sourceToSink(
            const cGraph &g,
            const std::vector<double> &edgeWeight);

        double probs(sGraphData &gd);

        std::vector<std::string> alloc(sGraphData &gd);

        /// @brief  Find euler path through graph
        /// @param g graph
        /// @return vector of vertex indices on path
        /// If no euler path, exception thrown

        std::vector<int> euler(const cGraph &g);

        /// @brief find set of vertices that cover every link
        /// @param g
        /// @return vector of vertex indices
        std::vector<int>
        vertexCover(const cGraph &g);

        /// @brief Graph description in graphviz dot format, multiple paths
        /// @param g
        /// @param vpath
        /// @param pathColor path colors
        /// @param all
        /// @return string in graphviz dot format

        std::string multiPathViz(
            cGraph &g,
            const std::vector<std::vector<int>> &vpath,
            const std::vector<std::string> &pathColor,
            bool all);

        /// @brief Graph description in graphviz dot format, one path
        /// @param g
        /// @param vpath
        /// @param all
        /// @return string in graphviz dot format

        std::string pathViz(
            cGraph &g,
            const std::vector<int> &vpath,
            bool all);

        /// @brief Run the graphviz dot layout program
        /// @param[in] g
        /// @param[in] pathViz string in graphviz dot format

        void RunDOT(
            cGraph &g,
            const std::string &pathViz);

        /// @brief Implement the A* algorithm with constant edge weights
        /// @param gd graph to be searched https://github.com/JamesBremner/PathFinder
        /// @param edgeWeight function calculates edge weight based on edge index
        /// @param heuristic function calculates distance estimate from vertex to goal
        /// @return vector of vertex indices on path from start to goal

        std::vector<int> astar(
            raven::graph::sGraphData &gd,
            std::function<double(int)> edgeWeight,
            std::function<double(int)> heuristic);

        /// @brief Implement the A* algorithm with dynamic edge weights
        /// @param g graph to be searched https://github.com/JamesBremner/PathFinder
        /// @param dynWeight function calculates edge weight based on path so far
        /// @param start vertex index
        /// @param goal vertex index
        /// @param heuristic function calculates distance estimate from vertex to goal
        /// @return vector of vertex indices on path from start to goal

        std::vector<int> astarDynWeights(
            raven::graph::cGraph &g,
            std::function<double(int, const std::vector<int> &)> dynWeight,
            int start, int goal,
            std::function<double(int)> heuristic);

    }
}
