#include <vector>
#include <random>
#include <map>
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

typedef vector<int> IntArray;

class State{

public:
    IntArray state;

    explicit State(const IntArray& arr){
        state.resize(9);
        state = arr;
    }

    /// find the index of the empty slot
    inline int findZeroIndex() const{
        for (unsigned int i = 0; i < state.size(); ++i)
            if (state[i] == 0) return i;

        return (int)state.size();
    }

    inline int getManhattanCost(){
        int cost = 0;
        for (unsigned int i = 0; i < state.size(); ++i){
            int v = state[i];
            if (v != 0) {
                // actual index of v should be v-1
                v = v - 1;
                int gx = v % 3;
                int gy = v / 3;

                int x = i % 3;
                int y = i / 3;

                int mancost = abs(x - gx) + abs(y - gy);
                cost += mancost;
            }
        }
        return cost;
    }

    void print() const{
        for (int i = 0; i < 3; ++i){
            for (int j = 0; j < 3; ++j){
                int index = i * 3 + j;
                cout << state[index] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

};

class Node{

public:
    Node(const State& state, Node *parent, int depth = 0) : state1(state) ,depth1(depth){
        parent1 = parent;
        mc1 = state1.getManhattanCost();
    }

public:
    State state1;
    Node *parent1;
    int mc1;
    int depth1;
};

class Neighbours{

public:
    typedef map<int, vector<int> > IndexNeighbourMap;
    IndexNeighbourMap _edges;

    Neighbours(){
        CreateGraphFor8Puzzle();
    }

    const vector<int>& getNeighbors(int id) const{
        IndexNeighbourMap::const_iterator itr(_edges.find(id));
        if (itr != _edges.end()) return itr->second;
        static vector<int> s;
        return s;
    }

private:
    void CreateGraphFor8Puzzle(){
        /*
        0 1 2
        3 4 5
        6 7 8
        */
        _edges.insert(make_pair(0, vector<int>{ 1, 3 }));
        _edges.insert(make_pair(1, vector<int>{ 0, 2, 4 }));
        _edges.insert(make_pair(2, vector<int>{ 1, 5 }));
        _edges.insert(make_pair(3, vector<int>{ 4, 0, 6 }));
        _edges.insert(make_pair(4, vector<int>{ 3, 5, 1, 7 }));
        _edges.insert(make_pair(5, vector<int>{ 4, 2, 8 }));
        _edges.insert(make_pair(6, vector<int>{ 7, 3 }));
        _edges.insert(make_pair(7, vector<int>{ 6, 8, 4 }));
        _edges.insert(make_pair(8, vector<int>{ 7, 5 }));
    }
};

inline bool isInArray(const State& state, const vector<Node*> &li){
    unsigned int i = 0;
    for (; i < li.size(); ++i){
        if (state.state == li[i]->state1.state)
            return true;
    }
    return false;
}

bool CompOp(Node * i, Node * current){
    return ((i->mc1 + i->depth1) > (current->mc1 + current->depth1));
}

typedef vector<Node *> NodeList;
NodeList _openlist;
NodeList _closedlist;
Neighbours g;
State goal(vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 0 });
State start(vector<int>{ 2, 3, 7, 1, 4, 8, 0, 5, 6 });

Node * GetNextNode(){
    if (_openlist.empty()) return 0;
    Node * current;

    sort(_openlist.begin(), _openlist.end(),CompOp);
    current = _openlist.back();

    _openlist.pop_back();
    _closedlist.push_back(current);
    return current;
}

void ExpandNode(Node * current, const Neighbours& graph, bool &_solved){
    if (current->state1.state == goal.state){
        _solved = true;
        return;
    }

    int zero = current->state1.findZeroIndex();
    const IntArray& neighbours = graph.getNeighbors(zero);

    for (int next : neighbours){
        State state_temp = current->state1;

        int tmp = state_temp.state[zero];
        state_temp.state[zero] = state_temp.state[next];
        state_temp.state[next] = tmp;

        if (!isInArray(state_temp, _closedlist)){
            Node * n(new Node(state_temp, current, current->depth1 + 1));
            _openlist.push_back(n);
        }
    }
}

int main(int argc, char* argv[])
{
    Node * node;
    //Solver solver(start, goal);
    int count = 0;

    bool _solved;

    Node * root(new Node(start, 0, 0));
    _openlist.push_back(root);


    while (!_solved){
        node = GetNextNode();
        ExpandNode(node, g, _solved);
        count++;
    }

    // accumulate the nodes for the solution.
    vector<Node*> solution;
    Node *s = node;
    do{
        solution.push_back(s);
        s = s->parent1;
    } while (s != NULL);

    cout << "solved in " << solution.size() - 1 << " steps\n";
    for (int i = (int)solution.size() - 1; i >= 0; i--){
        solution[i]->state1.print();
    }

    return 0;
}