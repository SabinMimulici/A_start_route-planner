#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 

using std::cout;
using std::vector;
using std::string;
using std::ifstream;
using std::istringstream;
using std::sort;

enum class State {kEmpty, kObstacle, kStart, kGoal, kOpen, kClosed, kPath};
// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine(string line)
{
    istringstream myline(line);
    int n;
    char c;
    vector<State> row;

    while(myline >> n >> c && c ==',')
    {
        if(n == 0)
        {
            row.push_back(State::kEmpty);
        }
        else if (n == 1)
        {
            row.push_back(State::kObstacle);
        }

    }
    return row;
}

vector<vector<State>> ReadBoardFile(string path)
{
    ifstream myfile (path);
    string line;
    vector<vector<State>> myboard;
    while(getline(myfile, line))
    {
        //parse each line and push it into the board matix
        vector<State> row = ParseLine(line);
        myboard.push_back(row);
    }
    return myboard;   
}
string CellToString(State state)
{
    string mystring;
    switch(state)
    {
        // case (State::kEmpty):
        //     mystring = "E ";
        // break;

        case (State::kObstacle):
            mystring = "X ";
        break;

        case (State::kStart):
            mystring = "S ";
        break;

        case(State::kGoal):       
            mystring = "G ";
        break;
        case(State::kPath):       
            mystring = "P ";
        break;

        default:
            mystring = "? ";
        break;
        
    }
    return mystring;
}
int HeuristicFunctionCalculator(int start_x, int start_y, int goal_x, int goal_y)
{
    int h = abs(goal_x - start_x) + abs(goal_y - start_y);
    return h;
}

void AddToOpen(vector<vector<int>> &open, vector<vector<State>> &board, int x, int y, int g, int h)
{

    open.push_back(vector<int>{x, y, g, h});
    board[x][y] = State::kClosed;
}
// compare the F values, returnes true only if f1 > f2, false if otherwise
bool Compare(const vector<int> a, const vector<int> b) 
{
  int f1 = a[2] + a[3]; // f1 = g1 + h1
  int f2 = b[2] + b[3]; // f2 = g2 + h2

  return f1 > f2; 
}
// sort vector in its descending order
void CellSort(vector<vector<int>> *v) 
{
  sort(v->begin(), v->end(), Compare);
}
//check if cell is on the grid
bool CheckValidCell(int x, int y, vector<vector<State>> &board) 
{
    bool on_grid_x = (x >= 0 && x < board.size());
    bool on_grid_y = (y >= 0 && y < board[0].size());

    if (on_grid_x && on_grid_y)
    {
        
        if(board[x][y] == State::kEmpty)
        {
            return true;
        }
        else
        {
            return false;
        }
    }  

    return false;
}
void ExpandNeighbors(const vector<int> &current, int goal[2], 
            vector<vector<int>>&openlist, vector<vector<State>> &board)
{
    // Get current node's data.
    int x = current[0];
    int y = current[1];
    int g = current[2];
    // Loop through current node's potential neighbors.
    for (int i = 0; i < 4; i++) 
    {
        int x2 = x + delta[i][0];
        int y2 = y + delta[i][1];

        if (CheckValidCell(x2, y2, board)) 
        {
            // Increment g value and add neighbor to open list.
            int g2 = g + 1;
            int h2 = HeuristicFunctionCalculator(x2, y2, goal[0], goal[1]);
            AddToOpen(openlist, board, x2, y2, g2, h2);
        }
    }
}

vector<vector<State>> AstarSearchRoute(vector<vector<State>> board, int start[2], int goal[2])
{
    vector<vector<int>> openlist;
    //initialize start coordinates
    board[start[0]][start[1]] = State::kStart;

    //initializing current node as start node
    int x = start[0];
    int y = start[1];
    int g = 0; // start g value
    int h = HeuristicFunctionCalculator(x, y, goal[0], goal[1]);
    // add to open list
    AddToOpen(openlist, board, x, y, g, h);
    while(openlist.size() > 0)
    {  
        CellSort(&openlist);
        auto current = openlist.back();
        openlist.pop_back();
        x = current[0];
        y = current[1];
        board[x][y] = State::kPath;
        // Check if we're done.
        if (x == goal[0] && y == goal[1]) 
        {
            board[start[0]][start[1]] = State::kStart;
            board[goal[0]][goal[1]] = State::kGoal;
            
            return board;
        }
        //expand to its neighbours
        ExpandNeighbors(current, goal, openlist, board);
    }
    cout << "No path found!" << "\n";

    return std::vector<vector<State>>{};
}

void PrintBoard(vector<vector<State>> board)
{
    for(int i =0; i < board.size(); i++)
    {
        for(int j=0; j < board[i].size(); j++)
        {
            cout << CellToString(board[i][j]);
        }
        cout<< "\n";
    }
}
int main()
{
    int start[2] = {0, 0};
    int goal[2] = {4, 5};

    vector<vector<State>>board = ReadBoardFile("1.board");
    auto solvedBoard = AstarSearchRoute(board, start, goal);
    cout<<"Initial board!\n";
    PrintBoard(board);
    cout<<"Solved board!\n";
    PrintBoard(solvedBoard);

    return 0;
}