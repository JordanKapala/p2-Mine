// Project Identifier: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F

#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <queue>
#include "P2random.h"

using namespace std;

class MineEscape {
    private:

        bool median_mode  = false;
        double median;
        bool stats_mode   = false;
        int  stats_N;
        bool verbose_mode = false;

        char input_type = '0';
        string buffer;
        int matrix_size;
        int srow;
        int scol;
        int seed = -1;
        int max_rubble = -1;
        int TNT = -1;


        int cleared = 0;
        int total_rubble = 0;

        class Tile {
            private:
                int tile_rubble = 0;
                bool discovered = false;
            
            public:
                Tile() : tile_rubble(0) {} //for resize
                
                Tile(int r) : tile_rubble(r) {}

                int getRubbleMap() {
                    return tile_rubble;
                }

                bool isDiscovered() {
                    return discovered;
                }

                void clearRubble() {
                    tile_rubble = 0;
                }

                void discover() {
                    discovered = true;
                }

        };
        vector<vector<Tile>> Map;

        class ProxTile {
            private:
                int rubble;
                int row;
                int col;

            public:

                class PQComp {
                    public:

                    bool operator() (const ProxTile & lhs, const ProxTile & rhs) const {
                        if (lhs.rubble > rhs.rubble) {
                            return true;
                        }
                        else if (lhs.rubble == rhs.rubble) {
                            if (lhs.col > rhs.col) {
                                return true;
                            }
                            else if (lhs.col == rhs.col) {
                                return lhs.row > rhs.row;
                            }
                        }
                        return false;
                    }
                };

                ProxTile() : 
                rubble(0), row(0), col(0)
                {}
            
                ProxTile(int r, int row, int col) : 
                rubble(r), row(row), col(col)
                {}

                int getRubble() const {
                    return rubble;
                }

                int getRow() const {
                    return row;
                }

                int getCol() const {
                    return col;
                }

                void clearPQTilerubble() {
                    rubble = 0;
                }
                
        };


        void createMap(istream & ss) {
            int current_rubble;
            Map.resize(matrix_size, vector<Tile>(matrix_size));

            for (int row = 0; row < matrix_size; row++) {
                for (int col = 0; col < matrix_size; col++) {
                    ss >> current_rubble;
                    Map[row][col] = Tile(current_rubble);
                }
            }
        }

    // priority_queue<ProxTile, vector<ProxTile>, ProxTile::PQComp> pq;
    // priority_queue<ProxTile, vector<ProxTile>, ProxTile::PQComp> TNTpq;
    priority_queue<int, vector<int>, std::less<int>>    lower;
    priority_queue<int, vector<int>, std::greater<int>> upper;
    vector<vector<int>> stats_vec;
        
    public:

    // mine() : {};

    void GetOptions(int argc, char *argv[])
    {
        int choice;
        int index = 0;
        option long_options[] = {
            {"stats"  , required_argument, nullptr, 's'},
            {"median" , no_argument      , nullptr, 'm'},
            {"verbose", no_argument      , nullptr, 'v'},
            {"help"   , no_argument      , nullptr, 'h'},
            {nullptr, 0, nullptr, '\0'},
        }; // long_options[]

        while ((choice = getopt_long(argc, argv, "s:mvh", long_options, &index)) != -1)
        {
            switch (choice)
            {
            case 'h':
                //printHelp();
                exit(0);

            case 'm':
            {
                median_mode = true;
                upper.push(1000);
                lower.push(0);
                break;
            }

            case 's':
            {
                
                stats_mode = true;
                stats_N = atoi(optarg);
                stats_vec.reserve(stats_N);
                break;
            }

            case 'v':
            {
                verbose_mode = true;
                break;
            }

            default:
                cerr << "Error: Unknown command line option" << endl;
                exit(1);
            } // switch ..choice
        }
    }
    
    void readMine() {
        cin >> input_type;
        cin >> buffer;
        cin >> matrix_size;
        cin >> buffer;
        cin >> srow >> scol;

        stringstream ss;
        if (input_type == 'R') {
            cin >> buffer;
            cin >> seed;
            cin >> buffer;
            cin >> max_rubble;
            cin >> buffer;
            cin >> TNT;
            
            P2random::PR_init(ss, matrix_size, seed, max_rubble, TNT);
        }  // if ..'R'

        istream &inputStream = (input_type == 'M') ? cin : ss;

        if (input_type != 'M' && input_type != 'R') {
            cerr << "Error: Invalid input mode" << endl;
            exit(1);
        }
        else if (srow >= matrix_size || srow < 0) {
            cerr << "Error: Invalid starting row" << endl;
            exit(1);
        }
        else if (scol >= matrix_size || scol < 0) {
            cerr << "Error: Invalid starting column" << endl;
            exit(1);
        }
    
        createMap(inputStream);
    }

    void escape() {
        priority_queue<ProxTile, vector<ProxTile>, ProxTile::PQComp> pq;
        priority_queue<ProxTile, vector<ProxTile>, ProxTile::PQComp> TNTpq;

        Tile start = Map[srow][scol];
        pq.push(ProxTile(start.getRubbleMap(),srow,scol));
        start.discover();

        int row;
        int col;
        int rubble;

        ProxTile next = pq.top();

        while (!pq.empty()) {
            next = pq.top();
            row = next.getRow(); 
            col = next.getCol();
            rubble = next.getRubble();

            Tile& tile = Map[row][col];


            if (rubble != -1) {
                if (rubble != 0 && tile.getRubbleMap() == rubble) {
                    cleared++;
                    total_rubble += rubble;
                    tile.clearRubble();
                    if (verbose_mode) {
                        cout << "Cleared: " << rubble << " at [" << row << "," << col << "]\n";
                    }
                    if (stats_mode) {
                        stats_vec.emplace_back(vector<int>{rubble,col,row});
                    }
                    if (median_mode) {
                            if (rubble >= upper.top()) {
                                upper.push(rubble);
                            }
                            else {
                                lower.push(rubble);
                            }
                            if (upper.size()-1 > lower.size()) {
                                lower.push(upper.top());
                                upper.pop();
                            }
                            else if (lower.size()-1 > upper.size()) {
                                upper.push(lower.top());
                                lower.pop();
                            }
                            if (upper.size() == lower.size()) {
                                median = double((lower.top() + upper.top())) / 2;
                            }
                            else if (upper.size() > lower.size()) {
                                median = upper.top();
                            }
                            else {
                                median = lower.top();
                            }
                            cout << "Median difficulty of clearing rubble is: " << median << "\n";
                        }
                }
                if (row == matrix_size-1 || row == 0 ||
                    col == matrix_size-1 || col == 0) {
                    break;
                }

                pq.pop();
                        Tile& N = Map[row-1][col];
                        Tile& E = Map[row][col+1];
                        Tile& S = Map[row+1][col];
                        Tile& W = Map[row][col-1];
                        
                        if (!N.isDiscovered()) {
                            N.discover();
                            pq.push(ProxTile(N.getRubbleMap(),row-1,col));
                        }
                        if (!E.isDiscovered()) {
                            E.discover();
                            pq.push(ProxTile(E.getRubbleMap(),row,col+1));
                        }
                        if (!S.isDiscovered()) {
                            S.discover();
                            pq.push(ProxTile(S.getRubbleMap(),row+1,col));
                        }
                        if (!W.isDiscovered()) {
                            W.discover();
                            pq.push(ProxTile(W.getRubbleMap(),row,col-1));
                        }
            }
            else if (tile.getRubbleMap() == -1) {
                TNTpq.push(next);
                ProxTile nextTNT = TNTpq.top();
                // int rowt;
                // int colt;
                int TNTrubble;
                pq.pop();
                while (!TNTpq.empty()) {

                    nextTNT = TNTpq.top();
                    row = nextTNT.getRow();
                    col = nextTNT.getCol();
                    TNTrubble = nextTNT.getRubble();

                    Tile& tilet = Map[row][col];
                    
                    tilet.clearRubble();

                    TNTpq.pop();

                    if (TNTrubble == -1) {
                        tilet.discover();
                        if (verbose_mode) {
                            cout << "TNT explosion at [" << row << "," << col << "]!\n";
                        }
                        if (stats_mode) {
                        stats_vec.emplace_back(vector<int>{rubble,col,row});
                        }

                        if (row != 0) {
                            Tile& Nt = Map[row-1][col];
                            int N_rubble = Nt.getRubbleMap();
                            if (N_rubble != 0) {
                            Nt.discover();
                            TNTpq.push(ProxTile(N_rubble, row-1, col));
                            Nt.clearRubble();
                            // pq.push(ProxTile(N_rubble, rowt-1, colt));
                            }
                            else if (!Nt.isDiscovered()) {
                                Nt.discover();
                                pq.push(ProxTile(0,row-1,col));
                            }
                        }
                        if (col+1 != matrix_size) {
                            Tile& Et = Map[row][col+1];
                            int E_rubble = Et.getRubbleMap();
                            if (E_rubble != 0) {
                                Et.discover();
                                TNTpq.push(ProxTile(E_rubble, row, col+1));
                                Et.clearRubble();
                                // pq.push(ProxTile(N_rubble, rowt-1, colt));
                            }
                            else if (!Et.isDiscovered()) {
                                Et.discover();
                                pq.push(ProxTile(0,row,col+1));
                            }
                        }
                        if (row+1 != matrix_size) {
                            Tile& St = Map[row+1][col];
                            int S_rubble = St.getRubbleMap();
                            if (S_rubble != 0) {
                                St.discover();
                                TNTpq.push(ProxTile(S_rubble, row+1, col));
                                St.clearRubble();
                            // pq.push(ProxTile(N_rubble, rowt-1, colt));
                            }
                            else if (!St.isDiscovered()) {
                                St.discover();
                                pq.push(ProxTile(0,row+1,col));
                            }
                        }
                        if (col != 0) {
                            Tile& Wt = Map[row][col-1];
                            int W_rubble = Wt.getRubbleMap();
                            if (W_rubble != 0) {
                                Wt.discover();
                                TNTpq.push(ProxTile(W_rubble, row, col-1));
                                Wt.clearRubble();
                            // pq.push(ProxTile(N_rubble, rowt-1, colt));
                            }
                            else if (!Wt.isDiscovered()) {
                                Wt.discover();
                                pq.push(ProxTile(0,row,col-1));
                            }
                        }
                    }
                    else {
                        total_rubble += TNTrubble;
                        cleared++;
                        pq.push(ProxTile(0,row,col));
                        if (verbose_mode) {
                            cout << "Cleared by TNT: " << TNTrubble << " at [" << row << "," << col << "]\n";
                        }
                        if (stats_mode) {
                            stats_vec.emplace_back(vector<int>{TNTrubble,col,row});
                        }
                        if (median_mode) {
                            if (TNTrubble >= upper.top()) {
                                upper.push(TNTrubble);
                            }
                            else {
                                lower.push(TNTrubble);
                            }
                            if (upper.size()-1 > lower.size()) {
                                lower.push(upper.top());
                                upper.pop();
                            }
                            else if (lower.size()-1 > upper.size()) {
                                upper.push(lower.top());
                                lower.pop();
                            }
                            if (upper.size() == lower.size()) {
                                median = double((lower.top() + upper.top())) / 2;
                            }
                            else if (upper.size() > lower.size()) {
                                median = upper.top();
                            }
                            else {
                                median = lower.top();
                            }
                            cout << "Median difficulty of clearing rubble is: " << median << "\n";
                        }
                    }
                }
            }
            else {
                pq.pop();
            }
        }
    }

    void output() {
        cout << "Cleared " << cleared << " tiles containing " << total_rubble << " rubble and escaped." << endl;
        if (stats_mode) {
            if (int(stats_vec.size()) < stats_N) {
                stats_N = int(stats_vec.size());
            }
            int dist = int(stats_vec.size()-stats_N);
            cout << "First tiles cleared:\n";
            for (int i = 0; i < stats_N; i++) {
                if (stats_vec[i][0] == -1) {
                    cout << "TNT at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
                else {
                    cout << stats_vec[i][0] << " at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
            }

            cout << "Last tiles cleared:\n";
            for (int i = int(stats_vec.size()-1); i >= dist; i--) {
                if (stats_vec[i][0] == -1) {
                    cout << "TNT at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
                else {
                    cout << stats_vec[i][0] << " at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
            }
            
            cout << "Easiest tiles cleared:\n";
            sort(stats_vec.begin(), stats_vec.end());
            for (int i = 0; i < stats_N; i++) {
                if (stats_vec[i][0] == -1) {
                    cout << "TNT at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
                else {
                    cout << stats_vec[i][0] << " at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
            }

            cout << "Hardest tiles cleared:\n";
            for (int i = int(stats_vec.size()-1); i >= dist; i--) {
                if (stats_vec[i][0] == -1) {
                    cout << "TNT at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
                else {
                    cout << stats_vec[i][0] << " at [" << stats_vec[i][2] << "," << stats_vec[i][1] << "]\n";
                }
            }
        }
    }
};

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    cout << std::fixed << std::setprecision(2);
    MineEscape object = MineEscape();
    object.GetOptions(argc, argv);
    object.readMine();
    object.escape();
    object.output();

    // std::vector<int> vec = {1, 2, 3, 4, 5};

    // // Create a reference to the first element
    // const int& first_element = vec.front();

    // // Output the value of the first element before popping
    // std::cout << "Before popping: " << first_element << std::endl;

    // // Pop the first element off the vector
    // vec.erase(vec.begin());

    // // Output the value of the first element after popping
    // std::cout << "After popping: " << first_element << std::endl;

}