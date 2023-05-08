#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <set>
#include "GraphTheory.h"

namespace raven
{
    namespace graph
    {

        void dijsktra(
            const cGraph &g,
            int start,
            std::vector<double> &dist,
            std::vector<int> &pred)
        {
            // shortest distance from start to each node
            dist.clear();
            dist.resize(g.vertexCount(), INT_MAX);

            // previous node on shortest path to each node
            pred.clear();
            pred.resize(g.vertexCount(), -1);

            std::vector<bool> sptSet(g.vertexCount(), false); // sptSet[i] will be true if vertex i is included in shortest
                                                              // path tree or shortest distance from src to i is finalized

            // Distance of source vertex from itself is always 0
            dist[start] = 0;
            pred[start] = 0;

            // Find shortest path for all vertices
            for (int count = 0; count < g.vertexCount() - 1; count++)
            {
                // Pick the minimum distance vertex from the set of vertices not
                // yet processed. u is always equal to src in the first iteration.
                int min = INT_MAX, uidx;
                for (int vidx = 0; vidx < g.vertexCount(); vidx++)
                    if (sptSet[vidx] == false && dist[vidx] <= min)
                    {
                        min = dist[vidx];
                        uidx = vidx;
                    }
                if (min == INT_MAX)
                {
                    // no more nodes connected to start
                    break;
                }

                // Mark the picked vertex as processed
                sptSet[uidx] = true;

                // Update dist value of the adjacent vertices of the picked vertex.
                for (auto vp : g.adjacentOut(uidx))
                {
                    if (sptSet[vp])
                        continue; // already processed

                    // Update dist[v] only if total weight of path from src to  v through u is
                    // smaller than current value of dist[v]
                    double cost = atof(g.rEdgeAttr(g.find(uidx, vp), 0).c_str());
                    if (dist[uidx] + cost < dist[vp])
                    {
                        dist[vp] = dist[uidx] + cost;
                        pred[vp] = uidx;
                    }
                }
            }
        }

        std::pair<std::vector<int>, double>
        path(
            const cGraph &g,
            const std::string &startName,
            const std::string &endName)
        {
            return path(
                g,
                g.find(startName),
                g.find(endName));
        }

        std::pair<std::vector<int>, double>
        path(
            const cGraph &g,
            int start,
            int end)
        {
            std::vector<int> vpath;

            if (0 > start || start > g.vertexCount() ||
                0 > end || end > g.vertexCount())
                return std::make_pair(vpath, -1);

            // run the Dijsktra algorithm
            std::vector<double> dist;
            std::vector<int> pred;
            dijsktra(g, start, dist, pred);

            // check that end is reachable from start
            if (pred[end] == -1)
                return std::make_pair(vpath, -1);

            vpath.push_back(end);
            int next = end;
            while (1)
            {
                next = pred[next];
                vpath.push_back(next);
                if (next == start)
                    break;
            }
            std::reverse(vpath.begin(), vpath.end());

            return std::make_pair(vpath, dist[end]);
        }

        std::vector<std::vector<int>>
        allPaths(
            const cGraph &g,
            int start,
            int end)
        {
            std::vector<std::vector<int>> ret;

            // copy input graph to working graph
            cGraph work = g;

            bool fnew = true;
            while (fnew)
            {
                // find new path
                auto p = path(work, start, end).first;
                if (!p.size())
                    break;

                // check path is really new
                for (auto &prevPath : ret)
                {
                    if (std::equal(
                            prevPath.begin(), prevPath.end(),
                            p.begin()))
                    {
                        fnew = false;
                        break;
                    }
                }
                // check new path was found
                if (!fnew)
                    break;

                // add new path to return
                ret.push_back(p);

                // increment cost of path links
                for (int k = 1; k < p.size(); k++)
                {
                    int ei = work.find(p[k - 1], p[k]);
                    int cost = atoi(work.rEdgeAttr(ei, 0).c_str()) + 1;
                    work.wEdgeAttr(
                        ei,
                        {std::to_string(cost)});
                }
            }
            return ret;
        }

        void cSpanningTree::add(
            const cGraph &g,
            int v, int w)
        {
            mySpanningTree.add(g.userName(v), g.userName(w));
            myVertexSet.insert(v);
            myVertexSet.insert(w);
        }

        cGraph
        spanningTree(
            const cGraph &g,
            const std::string &startName)
        {
            // std::cout << "spanning Tree " << startName << "\n";

            // copy vertices from input grqaph to spanning tree
            cSpanningTree ST;
            for (int kv = 0; kv < g.vertexCount(); kv++)
                ST.mySpanningTree.add(g.userName(kv));

            int start = g.find(startName);

            // track visited vertices
            std::vector<bool> visited(g.vertexCount(), false);

            // add initial arbitrary link
            int v = start;
            auto va = g.adjacentOut(v);
            if (!va.size())
                throw std::runtime_error(
                    "spanning tree start vertex unconnected");
            auto w = va[0];
            ST.add(g, v, w);

            visited[v] = true;
            visited[w] = true;

            // while nodes remain outside of span
            while (g.vertexCount() > ST.vertexCount())
            {
                double min_cost = INT_MAX;
                std::pair<int, int> bestLink;
                bestLink.first = -1;

                // loop over nodes in span
                for (int v = 0; v < g.vertexCount(); v++)
                {
                    if (!visited[v])
                        continue;

                    // loop over adjacent nodes not in span
                    for (auto w : g.adjacentOut(v))
                    {
                        // std::cout << "try " << g.userName(v) <<" "<< g.userName(w) << "\n";
                        if (visited[w])
                            continue;

                        // check edge exists
                        int ei = g.find(v, w);
                        if (ei < 0)
                            continue;

                        // track cheapest edge
                        double cost = atof(g.rEdgeAttr(ei, 0).c_str());
                        if (cost < min_cost)
                        {
                            min_cost = cost;
                            bestLink = std::make_pair(v, w);
                        }
                    }
                }

                if (bestLink.first == -1)
                {
                    std::cout << "spanning tree starting from " << startName << " cannot reach ";
                    for (int v = 0; v < visited.size(); v++)
                        if (!visited[v])
                            std::cout << g.userName(v) << " ";
                    std::cout << "\n";
                    // std::cout << g.text() << "\nPartial ST\n";
                    // std::cout << ST.mySpanningTree.text();
                    ST.mySpanningTree.clear();
                    return ST.mySpanningTree;
                }

                // add cheapest link between node in tree to node not yet in tree
                ST.add(g, bestLink.first, bestLink.second);

                visited[bestLink.first] = true;
                visited[bestLink.second] = true;
            }

            return ST.mySpanningTree;
        }

        void dfs(
            const cGraph &g,
            int startIndex,
            std::function<bool(int v)> visitor)
        {
            // track visited vertices
            std::vector<bool> visited(g.vertexCount(), false);

            // vertices waiting to be visited
            std::stack<int> wait;

            /*  1 Start by putting one of the graph's vertices on top of a stack.
                2 Take the top vertex of the stack and add it to the visited list.
                3 Add adjacent vertices which aren't in the visited list to the top of the stack.
                4 Keep repeating steps 2 and 3 until the stack is empty.
            */

            wait.push(startIndex);

            while (!wait.empty())
            {
                int v = wait.top();
                if (v < 0)
                    throw std::runtime_error(
                        "dfs bad index 1");
                wait.pop();
                if (visited[v])
                    continue;
                if (!visitor(v))
                    break;
                visited[v] = true;

                for (int w : g.adjacentOut(v))
                {
                    if (w < 0)
                        throw std::runtime_error(
                            "dfs bad index 2");
                    if (!visited[w])
                        wait.push(w);
                }
            }
        }

        std::vector<std::vector<int>>
        dfs_allpaths(
            const cGraph &g,
            int startIndex,
            int destIndex)
        {
            std::vector<std::vector<int>> apaths;
            std::vector<int> path;

            // track visited vertices
            std::vector<bool> visited(g.vertexCount(), false);

            // vertices waiting to be visited
            std::stack<int> wait;

            /*  1 Start by putting one of the graph's vertices on top of a stack.
                2 Take the top vertex of the stack and add it to the visited list.
                3 Add adjacent vertices which aren't in the visited list to the top of the stack.
                4 Keep repeating steps 2 and 3 until the stack is empty.
            */

            wait.push(startIndex);

            while (!wait.empty())
            {
                int v = wait.top();
                if (v < 0)
                    throw std::runtime_error(
                        "dfs bad index 1");
                wait.pop();
                if (visited[v])
                    continue;

                visited[v] = true;

                // add new vertex to current path
                path.push_back(v);

                // check for destination reached
                if (v == destIndex)
                {
                    // store new path
                    apaths.push_back(path);

                    // check for finished
                    if( wait.empty())
                        break;

                    // backtrack along path until last vertex in path
                    // has a connection to the vertex at top of the stack
                    std::vector<int> vadj;
                    do {
                        // mark vertex unvisited
                        visited[path.back()] = false;

                        // remove from path
                        path.erase(path.end() - 1);   
                        
                        vadj = g.adjacentOut(path.back() );
                    } while ( std::find(vadj.begin(),vadj.end(), wait.top()) == vadj.end() ) ;
                }
                else
                {
                        for (int w : g.adjacentOut(v))
                        {
                            if (w < 0)
                                throw std::runtime_error(
                                    "dfs bad index 2");
                            if (!visited[w])
                                wait.push(w);
                        }
                }
            }
            return apaths;
        }

        std::vector<std::vector<int>>
        dfs_cycle_finder(
            const cGraph &g,
            int inputStartIndex)
        {

            // store for found cycles, vertex indices in order reached.
            std::vector<std::vector<int>> ret;

            // store found cycle signatures
            std::vector<std::vector<int>> vfoundCycleSignature;

            // track visited vertices
            std::vector<bool> visited(g.vertexCount(), false);

            /* loop until all vertices have been visited

            This is required for graphs that are not fully connected
            i.e. not every vertex can be reached from every other

            */
            while (true)
            {
                int startIndex;
                if (inputStartIndex < 0)
                {
                    // find unvisited vertex to start the DFS from
                    auto it = std::find(
                        visited.begin(),
                        visited.end(),
                        false);
                    if (it == visited.end())
                    {
                        // all vertices have been visited - done
                        break;
                    }
                    startIndex = it - visited.begin();
                }
                else
                {
                    startIndex = inputStartIndex;
                }

                // vertices waiting to be processed
                std::stack<int> wait;

                // start from an unvisited vertex
                wait.push(startIndex);

                // continue until no more vertices can be reached from the starting vertex
                while (!wait.empty())
                {
                    // visit vertex at top of stack
                    int v = wait.top();
                    wait.pop();
                    visited[v] = true;

                    // loop over vertices reachable with one hop
                    for (int w : g.adjacentOut(v))
                    {
                        if (!visited[w])
                        {
                            // push unvisited vertex onto stack to be visited later
                            wait.push(w);
                            continue;
                        }

                        /* previously visited node

                        before carrying on we need to if this is a novel cycle
                        apply Dijsktra algorithm to find shortest path
                        from w back to the common ancestor and then around to v again

                        */
                        std::vector<int> cycle;
                        if (!g.isDirected())
                        {
                            // for undirected  graphs
                            // remove reverse edge
                            // so the path is forced to go the long way around back to start
                            raven::graph::cGraph temp = g;
                            temp.remove(w, v);
                            cycle = path(temp, w, v).first;
                        }
                        else
                        {
                            cycle = path(g, w, v).first;
                        }

                        // ignore "cycles" that just go back and forth over one edge
                        if (cycle.size() < 2)
                            continue;

                        // create cycle signature
                        // this is a list of the vertex indices in the cycle sorted in numerical order
                        // i.e. the signature is the same no matter where the start begins
                        std::vector<int> signature = cycle;
                        std::sort(signature.begin(), signature.end());

                        // check this is a new cycle
                        // loop over previously found cycles
                        bool fnew = true;
                        for (auto &foundSignature : vfoundCycleSignature)
                        {
                            // check cycle length
                            if (foundSignature.size() != signature.size())
                                continue;

                            // check cycle signature
                            if (std::equal(
                                    foundSignature.begin(),
                                    foundSignature.end(),
                                    signature.begin()))
                            {
                                // this cycle was found previously
                                fnew = false;
                                break;
                            }
                        }
                        if (!fnew)
                            continue;

                        // this is a novel cycle

                        // close the cycle
                        cycle.push_back(w);

                        // store in list of cycles found
                        ret.push_back(cycle);

                        // store the signature to prevent duplicates being stored
                        vfoundCycleSignature.push_back(signature);
                    }
                }
                if (inputStartIndex >= 0)
                {
                    std::vector<std::vector<int>> cycles_with_start;
                    for (auto &c : ret)
                    {
                        if (std::find(c.begin(), c.end(), inputStartIndex) != c.end())
                            cycles_with_start.push_back(c);
                    }
                    ret = cycles_with_start;
                    break;
                }
            }
            return ret;
        }

        void cliques(
            const cGraph &g,
            std::string &results)
        {
            // working copy on input graph
            auto work = g;

            // store for maximal clique collection
            std::vector<std::vector<int>> vclique;

            // true when all vertices have been moved into a clique
            bool finished = false;

            while (!finished)
            {
                std::vector<int> clique;

                while (!finished)
                {
                    // std::cout << "work.nodeCount " << work.nodeCount() << " " << clique.size() << "\n";
                    if (!clique.size())
                    {
                        // start by moving an arbitrary node to the clique from the work graph
                        for (int vi = 0; vi < work.vertexCount(); vi++)
                        {
                            if (work.rVertexAttr(vi, 0) == "deleted")
                                continue;
                            clique.push_back(vi);
                            work.wVertexAttr(vi, {"deleted"});
                            break;
                        }
                        continue;
                    }
                    bool found = false;

                    // loop over nodes remaining in work graph
                    finished = true;
                    for (int u = 0; u < work.vertexCount(); u++)
                    {
                        if (work.rVertexAttr(u, 0) == "deleted")
                            continue;
                        finished = false;

                        // loop over nodes in clique
                        for (int v : clique)
                        {
                            if (work.find(u, v) >= 0 ||
                                work.find(v, u) >= 0)
                            {
                                // found node in work that is connected to clique nodes.
                                // move it to clique
                                std::cout << "add " << work.userName(u) << "\n";
                                clique.push_back(u);
                                work.wVertexAttr(u, {"deleted"});
                                found = true;
                                break;
                            }
                        }
                        if (found)
                            break; // found a node to add to clique
                    }
                    if (!found)
                        break; // no more nodes can be added, the clique is maximal
                }

                if (!clique.size())
                    break; // did not find a clique

                // add to collection of maximal cliques
                vclique.push_back(clique);
            }

            // Display results
            std::stringstream ss;
            for (auto &c : vclique)
            {
                ss << "clique: ";
                for (int n : c)
                    ss << g.userName(n) << " ";
                ss << "\n";
            }
            results = ss.str();
        }

        double
        flows(
            const cGraph &g,
            int start,
            int end,
            std::vector<int> &vEdgeFlow)
        {
            if (!g.isDirected())
                throw std::runtime_error(
                    "Flow calculation needs directed graph ( 2nd input line must be 'g')");

            int totalFlow = 0;

            // copy graph to working graph
            auto work = g;

            while (1)
            {
                // find path
                // std::cout << "links:\n" << linksText() << "\n";
                auto p = path(work, start, end);
                // std::cout << "pathsize " << myPath.size() << " ";
                if (!p.first.size())
                    break;
                // std::cout << "Path " << pathText() << "\n";

                // maximum flow through path
                int maxflow = INT_MAX;
                int u = -1;
                int v;
                for (int v : p.first)
                {
                    if (u >= 0)
                    {
                        double cap = atof(work.rEdgeAttr(work.find(u, v), 0).c_str());
                        if (cap < maxflow)
                        {
                            maxflow = cap;
                        }
                    }
                    u = v;
                }

                // consume capacity of links in path
                u = -1;
                for (int v : p.first)
                {
                    if (u >= 0)
                    {
                        double cap = atof(work.rEdgeAttr(work.find(u, v), 0).c_str()) - maxflow;
                        if (cap <= 0)
                        {
                            // link capacity filled, remove
                            work.remove(u, v);
                        }
                        else
                        {
                            work.wEdgeAttr(
                                u, v,
                                {std::to_string(cap)});
                        }
                    }
                    u = v;
                }

                totalFlow += maxflow;
            }

            vEdgeFlow.clear();
            for (int ei = 0; ei < g.edgeCount(); ei++)
            {
                double f;
                if (work.dest(ei) == -1)
                    f = atof(g.rEdgeAttr(ei, 0).c_str());
                else
                {
                    double oc = atof(g.rEdgeAttr(ei, 0).c_str());
                    double wc = atof(work.rEdgeAttr(ei, 0).c_str());
                    f = oc - wc;
                }

                //         std::cout << g.userName(g.src(ei))
                //   << " " << g.userName(g.dest(ei))
                //   << " " << f << "\n";

                vEdgeFlow.push_back(f);
            }

            return totalFlow;
        }

        double multiflows(
            const cGraph &g,
            const std::vector<int> &vsource,
            int end)
        {
            double totalmultiflow = 0;
            std::vector<int> vEdgeFlow;
            for (int s : vsource)
            {
                totalmultiflow += flows(
                    g,
                    s,
                    end,
                    vEdgeFlow);
            }
            return totalmultiflow;
        }

        std::vector<std::vector<int>> sourceToSink(
            const cGraph &g)
        {
            std::vector<std::vector<int>> ret;

            // find sinks
            std::vector<int> vsink;
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                if (!g.adjacentOut(vi).size())
                    vsink.push_back(vi);
            }

            // loop over vertices
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                // check for source
                if (g.adjacentIn(vi).size())
                    continue;

                // find path to every other vertex
                std::vector<double> dist;
                std::vector<int> pred;
                dijsktra(g, vi, dist, pred);

                // find connected sinks
                std::vector<int> vConnected;
                vConnected.push_back(vi);
                for (int si : vsink)
                {
                    if (pred[si] >= 0)
                        vConnected.push_back(si);
                }
                ret.push_back(vConnected);
            }
            return ret;
        }

        double probs(cGraph &g, int end)
        {
            if (!g.isDirected())
                throw std::runtime_error(
                    "Probability calculation needs directed graph ( 2nd input line must be 'g')");

            // Mark all node probabilities as 'not yet calculated'
            std::string nyc("-1");
            for (int vi = 0; vi < g.vertexCount(); vi++)
                g.wVertexAttr(vi, {nyc});

            // loop over nodes
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                if (vi == end)
                    continue;

                // check for possible starting node
                // i.e one with out edges and no in edges
                if ((!g.adjacentOut(vi).size()) && (!g.adjacentIn(vi).size()))
                    continue;

                // iterate over all paths from starting node to target node
                for (auto &path : allPaths(
                         g,
                         vi,
                         end))
                {
                    // loop over nodes in path
                    for (int n : path)
                    {
                        if (n < 0)
                            continue;

                        // loop over inlinks
                        std::vector<double> vprob;
                        bool fOK = true;
                        for (int m : g.adjacentIn(n))
                        {
                            auto prevNodeProb = g.rVertexAttr(m, 0);
                            if (prevNodeProb == "-1")
                            {
                                // the previous node probability has not been calculated yet
                                // no need to look at any more inlinks
                                fOK = false;
                                break;
                            }
                            // store the probability contribution from this inlink
                            // it is the product of the source node proabability and the link probability
                            vprob.push_back(
                                atof(prevNodeProb.c_str()) *
                                atof(g.rEdgeAttr(g.find(m, n), 0).c_str()));
                        }
                        // check if there is enough information
                        // to calculate the probability for this node
                        if (!fOK)
                            break;

                        // all the previous nodes are calculated
                        // calculate this node's probability
                        double nodeprob = -1;
                        switch (vprob.size())
                        {
                        case 0:
                            // starting node, assume probability of 100%
                            nodeprob = 1;
                            break;

                        case 1:
                            // one inlink, prob is previous node prob times link probability
                            nodeprob = vprob[0];
                            break;

                        case 2:
                            // two inlinks
                            nodeprob =
                                vprob[0] + vprob[1] - vprob[0] * vprob[1];
                            break;

                        default:
                            /*  More then two inlinks
                            The code does not handle this
                            but note that multiple inlinks can alwayd be reduced to a series of
                            nodes with 2 inlinks
                            */
                            throw std::runtime_error(
                                g.userName(n) + " has more than 2 inlinks, please refactor input");
                        }

                        // save node probability
                        g.wVertexAttr(n, {std::to_string(nodeprob)});
                    }
                }
            }

            return atof(g.rVertexAttr(end, 0).c_str());
        }
        std::vector<std::string> alloc(cGraph &g)
        {
            // identify unique agents and tasks
            std::set<int> setAgent, setTask;
            for (int ei = 0; ei < g.edgeCount(); ei++)
            {
                setAgent.insert(g.src(ei));
                setTask.insert(g.dest(ei));
            }

            // add link from start to each agent
            int start = g.add("start_alloc");
            for (int agent : setAgent)
                g.add(start, agent);

            // add link from each task to end
            int end = g.add("end_alloc");
            for (int task : setTask)
                g.add(task, end);

            // set capacity of every link to 1
            for (int ei = 0; ei < g.edgeCount(); ei++)
                g.wEdgeAttr(ei, {"1"});

            // assign agents to tasks by calculating the maximum flow
            std::vector<int> vEdgeFlow;
            flows(g, start, end, vEdgeFlow);

            std::vector<std::string> ret;
            for (int ei = 0; ei < g.edgeCount(); ei++)
            {
                if (vEdgeFlow[ei] <= 0)
                    continue;

                int s = g.src(ei);
                int d = g.dest(ei);
                if (s == start)
                    continue;
                if (d == end)
                    continue;
                ret.push_back(g.userName(s));
                ret.push_back(g.userName(d));
            }
            return ret;
        }

        std::vector<int> euler(const cGraph &g)
        {
            // firewall
            if (!g.isDirected())
                throw std::runtime_error(
                    "euler:  needs directed graph ( 2nd input line must be 'g')");
            for (int vi = 0; vi < g.vertexCount(); vi++)
                if (g.adjacentIn(vi).size() != g.adjacentOut(vi).size())
                    throw std::runtime_error(
                        "euler: every vertex in-degree must equal out-degree");

            // working copy of graph
            cGraph work = g;

            // list to store circuit
            std::vector<int> circuit;

            // start at first vertex
            int curr_v = 0;

            while (1)
            {
                // add vertex to circuit
                circuit.push_back(curr_v);

                // find next vertex along unused edge
                auto vadj = work.adjacentOut(curr_v);
                if (!vadj.size())
                    break;
                int next_v = vadj[0];

                // remove used edge
                work.remove(curr_v, next_v);

                // continue from new vertex
                curr_v = next_v;
            }

            return circuit;
        }

        std::vector<int> vertexCover(const cGraph &g)
        {
            if (g.isDirected())
                throw std::runtime_error(
                    "vertexCover works only on undirected graphs");

            // output store
            std::vector<int> ret;
            std::set<int> vset;

            // working copy of input graph
            auto work = g;

            // The nodes that connect leaf nodes to the rest of the graph must be in cover set
            for (int leaf = 0; leaf < g.vertexCount(); leaf++)
            {
                // check for leaf vertex
                auto ns = g.adjacentOut(leaf);
                if (ns.size() != 1)
                    continue;

                // add to cover set
                vset.insert(ns[0]);
                // std::cout << "added " << work.userName(ns[0]) << "\n";
            }

            // loop over links
            for (auto l : work.edgeList())
            {
                if (vset.find(l.first) != vset.end() ||
                    vset.find(l.second) != vset.end())
                    continue;

                // add node with greatest degree to cover set
                auto sun = work.adjacentOut(l.first);
                auto svn = work.adjacentOut(l.second);
                int v = l.first;
                if (svn.size() > sun.size())
                    v = l.second;
                // std::cout << "added " << work.userName(v) << "\n";
                vset.insert(v);
            }

            for (int v : vset)
                ret.push_back(v);
            return ret;
        }

    }
}
