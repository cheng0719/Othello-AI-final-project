#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <climits>
#include <algorithm>
#define infinite 98766666
using namespace std;
int boardscores[8][8] = {
    {500,-25,10,5,5,10,-25,500},
    {-25,-45,1,1,1,1,-45,-25},
    {10,1,3,2,2,3,1,10},
    {5,1,2,1,1,2,1,5},
    {5,1,2,1,1,2,1,5},
    {10,1,3,2,2,3,1,10},
    {-25,-45,1,1,1,1,-45,-25},
    {500,-25,10,5,5,10,-25,500}
};


struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};


class BoardState {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;

    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    //翻中間的祺子
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }

    BoardState() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }

    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
//        if(!is_spot_valid(p)) {
//            winner = get_next_player(cur_player);
//            done = true;
//            return false;
//        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
//        cur_player = get_next_player(cur_player);
//        next_valid_spots = get_valid_spots();
        // Check Win
//        if (next_valid_spots.size() == 0) {
//            cur_player = get_next_player(cur_player);
//            next_valid_spots = get_valid_spots();
//            if (next_valid_spots.size() == 0) {
//                // Game ends
//                done = true;
//                int white_discs = disc_count[WHITE];
//                int black_discs = disc_count[BLACK];
//                if (white_discs == black_discs) winner = EMPTY;
//                else if (black_discs > white_discs) winner = BLACK;
//                else winner = WHITE;
//            }
//        }
//        return true;
    }
};



int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}


int MiniMax(Point &p, int depth, int alpha, int beta, int maximizingplayer, BoardState &laststate){
    if(depth == 0){
        return boardscores[p.x][p.y];
    }

    BoardState curstate;
    curstate.reset();
    curstate.cur_player = maximizingplayer;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            curstate.board[i][j] = laststate.board[i][j];
        }
    }

    curstate.set_disc(p, maximizingplayer);
    curstate.disc_count[maximizingplayer]++;
    curstate.disc_count[curstate.EMPTY]--;
    curstate.flip_discs(p);

    curstate.next_valid_spots = curstate.get_valid_spots();

    if (curstate.next_valid_spots.size() == 0) {
        curstate.cur_player = curstate.get_next_player(curstate.cur_player);
        curstate.next_valid_spots = curstate.get_valid_spots();
        if (curstate.next_valid_spots.size() == 0) {
            int white_discs = curstate.disc_count[curstate.WHITE];
            int black_discs = curstate.disc_count[curstate.BLACK];
            if (white_discs == black_discs)
                return 0;
            else if (black_discs > white_discs){
                if(player == 1){
                    return infinite;
                }
                else
                    return -infinite;
            }
            else if (black_discs < white_discs){
                if(player == 2){
                    return infinite;
                }
                else
                    return -infinite;
            }
        }
        else{
            if(maximizingplayer == player)
                return -infinite;
            else
                return infinite;
        }
    }


    if(maximizingplayer == player){
        int maxEval = -infinite;
        for(Point p : curstate.next_valid_spots){
            int eval = MiniMax(p, depth - 1, alpha, beta, 3-maximizingplayer, curstate);
            maxEval = max(maxEval, eval);
            alpha = max(eval, alpha);
            if(beta <= alpha)
                break;
        }
        return maxEval;
    }
    else{
        int minEval = infinite;
        for(Point p : curstate.next_valid_spots){
            int eval = MiniMax(p, depth - 1, alpha, beta, 3-maximizingplayer, curstate);
            minEval = min(minEval, eval);
            beta = min(eval, beta);
            if(beta >= alpha)
                break;
        }
        return minEval;
    }
}


void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
//    int n_valid_spots = next_valid_spots.size();
//    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
//    int index = (rand() % n_valid_spots);
//    Point p = next_valid_spots[index];
    Point ans(next_valid_spots[0]);
    BoardState curstate;
    curstate.reset();
    int maxEval = -infinite;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            curstate.board[i][j] = board[i][j];
        }
    }
    ans.x=9, ans.y=9;
    for(Point p : next_valid_spots){
        int eval = MiniMax(p, 10, -infinite, infinite, 3 - player, curstate);
        if(eval > maxEval)
            ans = p;
        maxEval = max(maxEval, eval);
    }
    // Remember to flush the output to ensure the last action is written to file.
    fout << ans.x << " " << ans.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

