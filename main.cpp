#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>
#include <ctime>

using namespace std;

// Function declarations
void bfs_serial(const vector<vector<int>>& graph, int start);
void bfs_parallel(const vector<vector<int>>& graph, int start);

// Function to generate a random graph
vector<vector<int>> generate_random_graph(int n, int edges_per_node) {
    vector<vector<int>> graph(n);
    srand(time(0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < edges_per_node; ++j) {
            int neighbor = rand() % n;
            if (neighbor != i) {
                graph[i].push_back(neighbor);
                graph[neighbor].push_back(i);
            }
        }
    }
    return graph;
}

// Function to perform BFS on a graph (serial version)
void bfs_serial(const vector<vector<int>>& graph, int start) {
    int n = graph.size();
    vector<bool> visited(n, false);
    queue<int> q;

    // Start with the source node
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        cout << node << " ";

        // Visit all the neighbors of the current node
        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    cout << endl;
}

// Function to perform parallel BFS on a graph
void bfs_parallel(const vector<vector<int>>& graph, int start) {
    int n = graph.size();
    vector<bool> visited(n, false);
    queue<int> q;

    // Start with the source node
    visited[start] = true;
    q.push(start);

    #pragma omp parallel
    {
        queue<int> local_q;
        #pragma omp single
        {
            local_q.push(start);
        }

        while (!local_q.empty() || !q.empty()) {
            int node = -1;

            #pragma omp critical
            {
                if (!q.empty()) {
                    node = q.front();
                    q.pop();
                }
            }

            if (node == -1) continue;

            cout << node << " ";

            #pragma omp parallel for
            for (int i = 0; i < graph[node].size(); ++i) {
                int neighbor = graph[node][i];
                if (!visited[neighbor]) {
                    #pragma omp critical
                    {
                        if (!visited[neighbor]) {
                            visited[neighbor] = true;
                            local_q.push(neighbor);
                        }
                    }
                }
            }

            #pragma omp barrier

            #pragma omp critical
            {
                while (!local_q.empty()) {
                    q.push(local_q.front());
                    local_q.pop();
                }
            }
        }
    }
    cout << endl;
}

int main() {
    int n = 1000; // Number of nodes
    int edges_per_node = 10; // Average number of edges per node
    vector<vector<int>> graph = generate_random_graph(n, edges_per_node);

    double start_time, end_time;

    // Serial BFS
    start_time = omp_get_wtime();
    bfs_serial(graph, 0);
    end_time = omp_get_wtime();
    cout << "Serial BFS time: " << end_time - start_time << " seconds" << endl;

    // Parallel BFS
    start_time = omp_get_wtime(); // Точка старта
    bfs_parallel(graph, 0);
    end_time = omp_get_wtime(); // Точка окончания
    cout << "Parallel BFS time: " << end_time - start_time << " seconds" << endl;

    return 0;
}

