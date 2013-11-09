// dijkstra.cpp : Defines the entry point for the console application.
/////
//s->v  1
//s->w 4
//v->w 2
//v->T 6
//w->T 3

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std ;

//object of this class will be passed to priority_queue so that min element is returned, not max element.
class Comweight{
    public:
    bool operator()(int& t1, int& t2) // Returns true if t2 is smallerthan t1
    {
       if (t1  >  t2 ) return true;
      return false;
    }
} ;
priority_queue<int, vector<int>,Comweight > pq ; //the vector<int> will be vecor of edge weights

map<  pair<char, char>  , int> edge_map ; // maps a pair of nodes(chars ) i.e. an edge to it's weight (an int )
map<char, int> A  ;// this map maps a node (a char) to it's shortest distance (an int ) from start node 's'
map<int, pair<char, char> > rev_edge_map ;// maps a weight (an int )  to a pair of nodes(chars )

vector< pair<char, char> > edge_vec ; // vector of pairs of nodes i.e. of edges
vector<char> nodes(4); //vector of nodes (chars)
vector<char> X(4); //vector of nodes ( chars ) whose shortest distance from source has been computed

int main() {

vector<char>::iterator n; //iterator for vector nodes
vector< pair<char, char> > ::iterator e;  //iterator for vector edge_vec

A['S'] = 0 ; // 's' is the name of the start node ; shortest path from start node to itself is zero.
X.push_back('S');// 's' is the name of the start node ; it's shortest path from itself has been noted & therefore, it is marked  as seen

//Making graph

//nodes
nodes.push_back('S');
nodes.push_back('V');
nodes.push_back('W');
nodes.push_back('T');
//edges
edge_vec.push_back(  make_pair('S', 'V')   );  edge_vec.push_back(  make_pair('V', 'T')   ) ;
edge_vec.push_back(  make_pair('V', 'W')   ); edge_vec.push_back(  make_pair('S', 'W')   );
edge_vec.push_back(  make_pair('W', 'T')   ) ;
//edge weights
edge_map  [make_pair('S', 'V')  ] = 1 ;
edge_map [make_pair('V', 'W')  ] = 2 ;
edge_map [make_pair('S', 'W')  ] = 4 ;
edge_map [make_pair('V', 'T' )  ] = 6 ;
edge_map [make_pair('W', 'T' )  ] = 3 ;



//cout<<"Edge (s,v) has weight "<<edge_map[make_pair('S', 'V')  ]<<endl ;
//cout<<"Edge (v,w) has weight "<<edge_map[make_pair('V', 'W')  ]<<endl ;
//cout<<"Edge (s,w) has weight "<<edge_map[make_pair('S', 'W')  ]<<endl ;
//cout<<"Edge (v,t) has weight "<<edge_map[make_pair('V', 'T')  ]<<endl ;
//cout<<"Edge (w, t) has weight "<<edge_map[make_pair('W', 'T')  ]<<endl ;

 //cout<<edge_vec.size() ;

pair <char, char > shortest_edge  ;

while(X !=  nodes){

while (! pq.empty()) {
//cout<< pq.top() << endl ;
pq.pop() ;//cleaning pq for next iteration
}
for(e = edge_vec.begin(); e != edge_vec.end() ; e++){
if (  (  binary_search(X.begin(), X.end(), e -> first)  && !(binary_search(X.begin(), X.end(), e -> second) )  )  )
{
//put every candidate  wieght on the min heap
pq.push(   edge_map[ (*e) ] + A[ e->first ]   ) ;
rev_edge_map [ edge_map[ (*e) ] + A[ e->first ]  ] = *e ;
}
}
//extract the candidate with minimum weight
//cout<< pq.top() << endl ;
shortest_edge =  rev_edge_map  [ pq.top() ] ;
cout<< shortest_edge.first << shortest_edge.second<<endl ;
pq.pop() ;

X .push_back( shortest_edge.second  ) ;
A[ shortest_edge.second   ] =   edge_map[  shortest_edge ]    + A[ shortest_edge.first ]  ;
}

return 0 ;
}

// binary_search(u.begin(), u.end(), 5)