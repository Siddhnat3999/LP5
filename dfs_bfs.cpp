#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <chrono>
#include <cstdlib>  // For rand, srand
#include <ctime>    // For time
#include <omp.h>

using namespace std;

// Graph class representing an undirected graph
class Graph
{
public:
    int vertices; // Number of vertices
    vector<vector<int>> graph; // Adjacency list representation
    vector<bool> visited; // Visited array

    // Constructor: initializes graph with v vertices and e edges
    Graph(int v, int e) : vertices(v)
    {
        graph.resize(vertices);
        generate_random_graph(e); // Generate random edges
    }

    // Generates a random undirected graph with a given number of edges using rand()
    void generate_random_graph(int edges)
    {
        srand(time(0)); // Seed the random number generator with current time

        for (int i = 0; i < edges; i++)
        {
            int u = rand() % vertices;
            int v = rand() % vertices;

            if (u != v) // Avoid self-loops
            {
                graph[u].push_back(v);
                graph[v].push_back(u); // Since it's undirected
            }
        }
    }

    // Resets visited array to all false
    void initialize_visited()
    {
        visited.assign(vertices, false);
    }

    // Parallel Depth-First Search (DFS) using OpenMP
    void parallel_dfs(int start)
    {
        visited[start] = true;
#pragma omp parallel
        {
            stack<int> local_stack;
            local_stack.push(start);

            while (!local_stack.empty())
            {
                int current;
#pragma omp critical
                {
                    if (!local_stack.empty())
                    {
                        current = local_stack.top();
                        local_stack.pop();
                    }
                    else
                    {
                        current = -1;
                    }
                }

                if (current == -1)
                    continue;

                // Visit neighbors
                for (int neighbor : graph[current])
                {
                    if (!visited[neighbor])
                    {
                        visited[neighbor] = true;
#pragma omp critical
                        {
                            local_stack.push(neighbor);
                        }
                    }
                }
            }
        }
    }

    // Parallel Breadth-First Search (BFS) using OpenMP
    void parallel_bfs(int start)
    {
        visited[start] = true;
        queue<int> q;
        q.push(start);

#pragma omp parallel
        {
            queue<int> local_queue;

            while (!q.empty())
            {
                int current;
#pragma omp critical
                {
                    if (!q.empty())
                    {
                        current = q.front();
                        q.pop();
                    }
                    else
                    {
                        current = -1;
                    }
                }

                if (current == -1)
                    continue;

                // Visit neighbors
                for (int neighbor : graph[current])
                {
                    if (!visited[neighbor])
                    {
                        visited[neighbor] = true;
#pragma omp critical
                        {
                            local_queue.push(neighbor);
                        }
                    }
                }

                // Merge local queue into shared queue
#pragma omp critical
                {
                    while (!local_queue.empty())
                    {
                        q.push(local_queue.front());
                        local_queue.pop();
                    }
                }
            }
        }
    }

    // Sequential Depth-First Search (DFS)
    void dfs(int start)
    {
        stack<int> s;
        s.push(start);
        visited[start] = true;

        while (!s.empty())
        {
            int current = s.top();
            s.pop();

            for (int neighbor : graph[current])
            {
                if (!visited[neighbor])
                {
                    s.push(neighbor);
                    visited[neighbor] = true;
                }
            }
        }
    }

    // Sequential Breadth-First Search (BFS)
    void bfs(int start)
    {
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            for (int neighbor : graph[current])
            {
                if (!visited[neighbor])
                {
                    q.push(neighbor);
                    visited[neighbor] = true;
                }
            }
        }
    }
};

// Main function to test graph traversal and measure execution times
int main()
{
    int V = 1000000, E = 10000000; // 1 million nodes, 10 million edges
    Graph g(V, E); // Create graph

    double time_seq_dfs, time_par_dfs, time_seq_bfs, time_par_bfs;

    // Measure sequential DFS
    g.initialize_visited();
    auto start = chrono::high_resolution_clock::now();
    g.dfs(0);
    auto end = chrono::high_resolution_clock::now();
    time_seq_dfs = chrono::duration<double>(end - start).count();
    cout << "Time required for DFS sequential is: " << time_seq_dfs << " seconds" << endl;

    // Measure parallel DFS
    g.initialize_visited();
    start = chrono::high_resolution_clock::now();
    g.parallel_dfs(0);
    end = chrono::high_resolution_clock::now();
    time_par_dfs = chrono::duration<double>(end - start).count();
    cout << "Time required for DFS parallel is: " << time_par_dfs << " seconds" << endl;

    // Measure sequential BFS
    g.initialize_visited();
    start = chrono::high_resolution_clock::now();
    g.bfs(0);
    end = chrono::high_resolution_clock::now();
    time_seq_bfs = chrono::duration<double>(end - start).count();
    cout << "Time required for BFS sequential is: " << time_seq_bfs << " seconds" << endl;

    // Measure parallel BFS
    g.initialize_visited();
    start = chrono::high_resolution_clock::now();
    g.parallel_bfs(0);
    end = chrono::high_resolution_clock::now();
    time_par_bfs = chrono::duration<double>(end - start).count();
    cout << "Time required for BFS parallel is: " << time_par_bfs << " seconds" << endl;

    // Calculate speedup
    cout << "DFS Speedup: " << (time_par_dfs > 0 ? time_seq_dfs / time_par_dfs : 0) << endl;
    cout << "BFS Speedup: " << (time_par_bfs > 0 ? time_seq_bfs / time_par_bfs : 0) << endl;

    return 0;
}
