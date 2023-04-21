// Lakelon Bailey - Project 5: Word Dice - 04/21/2023
#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>

using namespace std;

const int MAXN = 200; // Maximum number of nodes in the graph
const int INF = 1e9;  // Large constant to represent infinity

vector<int> adj[MAXN]; // Adjacency list to store the graph
int res[MAXN][MAXN];   // Residual capacities of the edges
int parent[MAXN];      // Parent array for storing the augmenting path during BFS


// Perform a breadth-first search (BFS) from source s to sink t
// Return the bottleneck capacity of the augmenting path found, or false if no path is found
bool bfs(int s, int t) {
    memset(parent, -1, sizeof(parent));
    parent[s] = -2;
    queue<pair<int, int> > q;
    q.push({s, INF});

    while (!q.empty()) {
        int cur = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int next : adj[cur]) {
            if (parent[next] == -1 && res[cur][next]) {
                parent[next] = cur;
                int new_flow = min(flow, res[cur][next]);
                if (next == t)
                    return new_flow;
                q.push({next, new_flow});
            }
        }
    }
    return false;
}


// Find the maximum flow in the graph from source s to sink t using the Edmonds-Karp algorithm
int maxflow(int s, int t) {
    int flow = 0;
    int new_flow;
    while ((new_flow = bfs(s, t))) {
        flow += new_flow;
        int cur = t;
        while (cur != s) {
            int prev = parent[cur];
            res[prev][cur] -= new_flow;
            res[cur][prev] += new_flow;
            cur = prev;
        }
    }
    return flow;
}


// Add an edge to the graph with capacity 1
void add_edge(int u, int v) {
    adj[u].push_back(v);
    adj[v].push_back(u);
    res[u][v] = 1;
}


// Return a formatted string with the dice order and word if it can be spelled, or an empty string otherwise
string can_spell_word(const vector<string> &dice, const string &word) {
    int n = dice.size();
    int s = 2 * n;
    int t = 2 * n + 1;

    // Clear the adjacency list and reset the residual capacities
    for (int i = 0; i < MAXN; ++i)
        adj[i].clear();
    memset(res, 0, sizeof(res));

    // Build the bipartite graph representing the problem
    for (int i = 0; i < n; ++i) {
        add_edge(s, i);
        add_edge(i + n, t);
        for (size_t j = 0; j < word.size(); ++j) {
            if (dice[i].find(word[j]) != string::npos)
                add_edge(i, j + n);
        }
    }

    // Check if the word can be spelled
    if (maxflow(s, t) == (int)word.size()) {
        string result;
        for (size_t j = 0; j < word.size(); ++j) {
            for (int i = 0; i < n; i++) {
                if (res[j + n][i] == 1) {
                    result += to_string(i) + ",";
                    break;
                }
            }
        }
        result.pop_back(); // Remove the trailing comma
        result += ": " + word;
        return result;
    }
    return "";
}

int main(int argc, char *argv[]) {


    // Error check command line args and file opening
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <dice_file> <words_file>" << endl;
        return 1;
    }
    ifstream dice_file(argv[1]);
    ifstream words_file(argv[2]);
    if (!dice_file || !words_file) {
        cerr << "Error opening input files." << endl;
        return 1;
    }

    // Determine and output result
    vector<string> dice;
    string line;
    while (getline(dice_file, line))
        dice.push_back(line);

    while (getline(words_file, line)) {
        string result = can_spell_word(dice, line);
        if (!result.empty())
            cout << result << endl;
        else
            cout << "Cannot spell " << line << endl;
    }

    return 0;
}