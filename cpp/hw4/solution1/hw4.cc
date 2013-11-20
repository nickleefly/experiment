// home work for c++ week 4
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

enum class eColor {NONE, WHITE, BLACK};

class cHexPoint {

    int    x;     // x coordinate of a valid hex point
    int    y;     // y coordinate of a valid hex point
    eColor color; // color of the valid hex pont

public:
    cHexPoint(int ix, int iy, eColor ic): x(ix), y(iy), color(ic){ };
    void setColor(eColor c) { color = c; };
    eColor getColor() { return color; };
    int  getX() { return x; };
    bool match(int ix, int iy) { return (ix == x && iy == y); };
    bool isNeighbor(cHexPoint& candidate)
    {
         if ((abs(x-candidate.x) <= 1) && (abs(y-candidate.y) <= 1)) return true;
         return false;
    }
};

class cHexMap {

    vector< vector <cHexPoint> > hexMap;
    int size;

public:
    cHexMap(int isize): size(isize)
    {
        // create  an array of cHexPoint vectors, each vector will hold
        // a row of cHexPoint in the x-y matrix presentation
        hexMap.resize(isize);
    }

    // add a hex point to the row index by iy
    void addPoint (int iy, int ix, eColor ic)
    {
        cHexPoint hexP(ix, iy, ic);
        hexMap[iy].push_back (hexP);
    }
    // set point color
    bool setPoint( int ix, int  iy, eColor ic)
    {
        // first check if (ix, iy) is on the map
        if ( iy < 0 || iy > (size -1)) return false;
        // the starting x in row y has an offset equal to the y
        ix += iy;
        vector<cHexPoint>::iterator it;
        for (it = hexMap[iy].begin(); it != hexMap[iy].end(); it++)
        {
            if (it->match(ix, iy)) break;
        }
        if ( it == hexMap[iy].end() ) return false;
        // check if already been taken
        if (it->getColor() != eColor::NONE) return false;
        it->setColor(ic);
        return true;
    }
    // check if a color form a line
    bool checkWin(eColor ic)
    {
        // for BLACK the line go north - south
        // 0. push points in row 0 that are marked BLACK to a vector preV
        //    if the size of preV is zero -> NO WIN
        // 1. check each point in row i that is marked BLACK
        //    if the BLACK point has neighbor in preV, push to a vector curV
        //    if size of curV is zero -> NO WIN
        // 2. if row i is the last row -> BLACK WIN
        //    otherwise, copy curV to preV, increase row i to i+1, goto step 1
        if (ic == eColor::BLACK)
        {
             vector<cHexPoint> preV;
             // step 0
             int row = 0;
             for (vector<cHexPoint>::iterator it = hexMap[row].begin(); it != hexMap[row].end(); it++)
             {
                 if (it->getColor() == ic) preV.push_back(*it);
             }
             if (preV.size() == 0) return false;
             // step 1
             row = 1;
             while ( row < size )
             {
                  vector<cHexPoint> curV;
                  for (vector<cHexPoint>::iterator curit = hexMap[row].begin(); curit != hexMap[row].end(); curit++)
                  {
                       if (curit->getColor() == ic)
                       {
                              for (vector<cHexPoint>::iterator preit = preV.begin(); preit != preV.end(); preit++)
                              {
                                    if (curit->isNeighbor(*preit))
                                    {
                                         curV.push_back(*curit);
                                         break;
                                     }
                              }

                       }
                  }
                  if (curV.size() == 0) return false;
                  // step 2
                  preV = curV;
                  row++;
             }
             return true;
        }
        // for WHITE the line go east - west
        // 0. push points in column 0 that are marked WHITE to a vector preV
        //    if the size of preV is zero -> NO WIN
        // 1. check each point in column j that is marked WHITE
        //    if the WHITE point has neighbor in preV, push to a vector curV
        //    if size of curV is zero -> NO WIN
        // 2. if column j is the last row -> WHITE WIN
        //    otherwise, copy curV to preV, increase column j to j+1, goto step 1
        else if (ic == eColor::WHITE)
        {
             vector<cHexPoint> preV;
             // step 0
             int column = 0;
             for (int row=0; row<size; row++)
             {
                  if ((hexMap[row].at(column)).getColor() == ic)
               preV.push_back(hexMap[row].at(column));
             }
             if (preV.size() == 0) return false;
             // step 1
             column = 1;
             while ( column < size )
             {
                  vector<cHexPoint> curV;
                  for (int row=0; row<size; row++)
                  {
                       if ((hexMap[row].at(column)).getColor() == ic)
                       {
                              for (vector<cHexPoint>::iterator preit = preV.begin(); preit != preV.end(); preit++)
                              {
                                    if ((hexMap[row].at(column)).isNeighbor(*preit))
                                    {
                                         curV.push_back(hexMap[row].at(column));
                                         break;
                                     }
                              }

                       }
                  }
                  if (curV.size() == 0) return false;
                  // step 2
                  preV = curV;
                  column++;
             }
             return true;
        }
        // only two player is allowed
        else
        {
            return false;
        }
    }

    void drawMap ()
    {
        cout << endl;

        for(int row = 0; row < size; row++)
        {
            for (vector<cHexPoint>::iterator it = hexMap[row].begin(); it != hexMap[row].end(); it++)
            {
               char mark;
               if      (it->getColor() == eColor::BLACK)      mark = 'B';
               else if (it->getColor() == eColor::WHITE)      mark = 'W';
               else                                           mark = '.';

               // for first element, shift the plot point to x offset
               if ( it == hexMap[row].begin())
               {
                    for (int sp = 0; sp < it->getX(); sp++) cout << "  ";
               }
               // no dash for last point
               if ( (it+1) == hexMap[row].end()) cout << mark << endl;
               else                              cout << mark << "-";
            }

            // now draw symbol between rows when row is not the last row
            if (row != size -1)
            {
        for (vector<cHexPoint>::iterator it = hexMap[row].begin(); it != hexMap[row].end(); it++)
        {
           // for first element, shift the plot point to x offset
           if ( it == hexMap[row].begin())
           {
                cout << " ";
                for (int sp = 0; sp < it->getX(); sp++) cout << "  ";
           }
           // no dash for last point
           if ( (it+1) == hexMap[row].end()) cout << "\\" << endl;
           else                              cout << "\\" << "|";
        }
            }
        }
    }
};


int main(int argc, char *argv[])
{
   int size;

   if (argc != 2)
   {
        cout << "Usage: " << argv[0] << " board_size" << endl;
        return 0;
   }

   size = atoi(argv[1]);
   if ( size <= 0)
   {
        cout << "wrong size " << argv[1] << endl;
        return 0;
   }

   // create map equal to a matrix (rows of cHexPoint vector)
   cHexMap myMap(size);

   // hex point needs to be on the cross points of the follow 3 lines
   // 1. x = cx
   // 2. y = cy
   // 3. x - y = coff
   // where cx, cy and coff range from 0 to size-1

   for (int cy = 0; cy < size; cy++)
   {
        for (int coff = 0; coff < size; coff++)
        {
             int cx = cy + coff;
             myMap.addPoint(cy, cx, eColor::NONE);
        }
   }

   myMap.drawMap();

   eColor player = eColor::BLACK;
   int ix, iy;
   while(1)
   {
      // get input (x, y) from current player
      if (player == eColor::BLACK) cout << " It is BLACK turn ( x y ) => ";
      else                         cout << " It is WHITE turn ( x y ) => ";
      cin >> ix >> iy;
      // check if input is valid and no player claim it yet
      if (!myMap.setPoint(ix, iy, player))
      {
          cout << "invalid input, try again" << endl;
          continue;
      }
      // draw new map
       myMap.drawMap();
      // check if current player win, if yes, end the game
       if (myMap.checkWin(player))
       {
            if (player == eColor::BLACK) cout << "Player BLACK win" << endl;
            else                         cout << "Player WHITE win" << endl;
            break;
       }
      // otherwise, switch player
      if (player == eColor::BLACK) player = eColor::WHITE;
      else                         player = eColor::BLACK;
   }
   return 0;
}