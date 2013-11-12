//g++ prim.cc -std=c++11
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

//function used for sorting the priority queu
inline bool myf (pair<pair<int,int>,double> i, pair<pair<int,int>,double> j){
    return (i.second>j.second);
}

//funtion to verify if a vertex is on the closed set
inline bool _closed(vector<int>* v, int n){
    for(int i =0; i<(*v).size(); ++i){
        if((*v)[i] == n)
            return true;
    }
    return false;
}

class Graph{
public:
    double *g;

    Graph(){};
    //contructor to initialize graph with file
    Graph(string txt){
        vector<char> line(200);
        auto p = line.begin();
        ifstream myfile (txt, ios::in);
        if(myfile.is_open()){
            //getting the number of vertices from the beginning of the file
            myfile >> vertices;
            g = new double[vertices*vertices];
            //first, filling with 0's
            for(int i =0; i<vertices;++i)
                for(int j =0; j<vertices;++j)
                    g[i*vertices+j] = 0;
            //building graph
            while (true) {
                myfile >> start >> end >> c;
                g[start*vertices+end] = c;
                if (myfile.eof()) break;
            }
        }
        else{
            cout<<"Couldn't open file"<<endl;
        }
        myfile.close();
    }

    inline int getSize(){ return vertices;}//method that return the number of vertices in the graph

private:
    int r, start, end;
    double c;
    int vertices;

};

//priority queu to determine with vertex to explore (represents the open set)
class PriorityQueue{
public:
    //first pair is the pair of nodes connected and the second part is the cost of the edge
    vector< pair<pair<int,int>,double> > list;

    //funtion to add a path with it's cost in the open set
    void add(pair<pair<int,int>,double>* pa){
        list.push_back(*pa);
        sort(list.begin(), list.end(), myf);
    }

    //returns the edge with the lowest cost on the open set
    pair<pair<int,int>,double> getVertex(){
        pair<pair<int, int>,double> aux;
        aux =list.back();
        list.pop_back();
        return aux;
    }
    //funtion to verify if the open set is empty
    bool isempty(){
        return (list.size() == 0);
    }
    //function to verify if an vertice is already in the open set and if so if the new path found to it is better or worse
    void ishere(pair<pair<int,int>, double>* n){
        for(int i =0; i<list.size(); ++i){
            if(list.at(i).first.second==(*n).first.second) {
                    if(list.at(i).second>(*n).second){
                        list.erase(list.begin()+i);
                        add(n);
                        return; //new path is better
                    } else {
                        return; //new path is worse
                    }
            }

        }
        add(n); //didn't have a path
        return;
    }
};


//class where the algorithm is written
class Prim{
public:
    //constructor
    Prim(string name):cost(0), current(0), t(0){
        Graph graph(name);
        g = graph;
        close.push_back(0);
        //start on vertex 0
        p.first.first=current;
        p.first.second=current;
        p.second=cost;
        open.ishere(&p);
    }

    void run(){
        while(t<g.getSize()){
            //test if open set is empty
            if (open.isempty()){
                cout<<"Graph not connected"<<endl;
                break;
            }
            //get next edge
            p = open.getVertex();
            current=p.first.second;
            cost +=p.second;
            close.push_back(current);
            //creating the string with the tree
            if(p.first.first != p.first.second)
                tree.append(to_string(p.first.first)+"\t"+to_string(p.first.second)+"\t"+to_string(p.second)+"\n");
            //analyzing the new vertex for new edges
            for(int a =1; a<g.getSize(); a++){
                if(g.g[current*g.getSize()+a]!=0  && !_closed(&close, a)){
                    p.first.first=current;
                    p.first.second=a;
                    p.second=g.g[current*g.getSize()+a];
                    open.ishere(&p);
                }
            }
            ++t;
        }
        cout<<cost<<endl;
        cout<<tree<<endl;
    }
private:
    Graph g;
    PriorityQueue open;
    vector<int> close;
    //auxiliar variables to create and handle the algorithm
    pair<pair<int,int>, double> p;
    int t;
    double cost; //length
    int current; //vertex being currently explored
    string tree;
};


int main()
{
    string path;
    cout <<"Input path to file: ";
    cin>> path;
    Prim p(path);
    p.run();
    return 0;
}

/*I used the Prim algorithm with a graph implemented as a matrix. When the program runs it asks for the path of the file. The first line of the output is the total cost of the MST and the next lines have three numbers, the two firsts represents the edge explored and the third is the cost of the edge.
The output of the sample test data is this:
30
0       2       2.000000
2       9       1.000000
9       13      3.000000
9       12      3.000000
12      17      1.000000
17      1       1.000000
1       6       1.000000
6       5       1.000000
5       18      1.000000
18      14      1.000000
12      11      1.000000
12      3       1.000000
5       16      2.000000
16      10      3.000000
10      15      2.000000
15      19      2.000000
15      4       2.000000
4       8       1.000000
4       7       1.000000*/