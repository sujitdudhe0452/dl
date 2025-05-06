#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int v, int w) {
        adj[v].push_back(w);
        adj[w].push_back(v);
    }

    void serialBFS(int start);
    void parallelBFS(int start);
    void serialDFS(int start);
    void parallelDFS(int start);
};

void Graph::serialBFS(int start) {
    vector<bool> visited(V, false);
    queue<int> q;
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}

void Graph::parallelBFS(int start) {
    vector<bool> visited(V, false);
    queue<int> q;
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int qSize = q.size();

        #pragma omp parallel for
        for (int i = 0; i < qSize; i++) {
            int node;
            #pragma omp critical
            {
                node = q.front();
                q.pop();
            }

            for (int neighbor : adj[node]) {
                #pragma omp critical
                {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        q.push(neighbor);
                    }
                }
            }
        }
    }
}

void Graph::serialDFS(int start) {
    vector<bool> visited(V, false);
    stack<int> s;
    s.push(start);

    while (!s.empty()) {
        int node = s.top();
        s.pop();

        if (!visited[node]) {
            visited[node] = true;
        }

        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                s.push(neighbor);
            }
        }
    }
}

void Graph::parallelDFS(int start) {
    vector<bool> visited(V, false);
    stack<int> s;
    s.push(start);

    while (!s.empty()) {
        int node;
        #pragma omp critical
        {
            node = s.top();
            s.pop();
        }

        if (!visited[node]) {
            visited[node] = true;
        }

        #pragma omp parallel for
        for (int neighbor : adj[node]) {
            #pragma omp critical
            {
                if (!visited[neighbor]) {
                    s.push(neighbor);
                }
            }
        }
    }
}

int main() {
    int N = 7896;
    Graph g(N);

    cout << "Generating random graph with " << N << " nodes...\n";

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 5; j++) {
            int neighbor = rand() % N;
            if (neighbor != i) {
                g.addEdge(i, neighbor);
            }
        }
    }

    // Serial BFS
    auto start_serial_bfs = high_resolution_clock::now();
    g.serialBFS(0);
    auto end_serial_bfs = high_resolution_clock::now();
    auto duration_serial_bfs = duration_cast<microseconds>(end_serial_bfs - start_serial_bfs);

    // Parallel BFS
    auto start_parallel_bfs = high_resolution_clock::now();
    g.parallelBFS(0);
    auto end_parallel_bfs = high_resolution_clock::now();
    auto duration_parallel_bfs = duration_cast<microseconds>(end_parallel_bfs - start_parallel_bfs);

    // Serial DFS
    auto start_serial_dfs = high_resolution_clock::now();
    g.serialDFS(0);
    auto end_serial_dfs = high_resolution_clock::now();
    auto duration_serial_dfs = duration_cast<microseconds>(end_serial_dfs - start_serial_dfs);

    // Parallel DFS
    auto start_parallel_dfs = high_resolution_clock::now();
    g.parallelDFS(0);
    auto end_parallel_dfs = high_resolution_clock::now();
    auto duration_parallel_dfs = duration_cast<microseconds>(end_parallel_dfs - start_parallel_dfs);

    // Output results
    cout << "\nBREADTH FIRST SEARCH (BFS):\n";
    cout << "Sequential Time : " << duration_serial_bfs.count() << " microseconds\n";
    cout << "Parallel Time   : " << duration_parallel_bfs.count() << " microseconds\n";
    cout << "Speedup Factor  : " << static_cast<double>(duration_serial_bfs.count()) / duration_parallel_bfs.count() << "\n";

    cout << "\nDEPTH FIRST SEARCH (DFS):\n";
    cout << "Sequential Time : " << duration_serial_dfs.count() << " microseconds\n";
    cout << "Parallel Time   : " << duration_parallel_dfs.count() << " microseconds\n";
    cout << "Speedup Factor  : " << static_cast<double>(duration_serial_dfs.count()) / duration_parallel_dfs.count() << "\n";

    return 0;
}
