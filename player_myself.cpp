#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <algorithm>
using namespace std;

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
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


int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

int score[8][8] = {
    {500, -25, 10, 5, 5, 10, -25, 500},
    {-25, -45, 1, 1, 1, 1, -45, -25},
    {10, 1, 3, 2, 2, 3, 1, 10},
    {5, 1, 2, 1, 1, 2, 1, 5},
    {5, 1, 2, 1, 1, 2, 1, 5},
    {10, 1, 3, 2, 2, 3, 1, 10},
    {-25, -45, 1, 1, 1, 1, -45, -25},
    {500, -25, 10, 5, 5, 10, -25, 500}
};


class State{
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    static const int SIZE = 8;
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;


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

    State() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        for(int i=0;i<3;i++){
            disc_count[i] = 0;
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


};

/*int cnt_value(State rhs){
    State tmp;
    tmp.reset();
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            tmp.board[i][j] = rhs.board[i][j];
        }
    }
    for(int i=0;i<3;i++){
        tmp.disc_count[i] = rhs.disc_count[i];
    }
    tmp.cur_player = 3-player;
    int value;
    value = score[point.x][point.y] * 3;
    tmp.set_disc(point, player);
    tmp.disc_count[player]++;
    tmp.disc_count[tmp.EMPTY]--;
    tmp.flip_discs(point);
    tmp.next_valid_spots = tmp.get_valid_spots();
    int tmpsize = tmp.next_valid_spots.size();
    value = value - 5*tmpsize;

    tmp.cur_player = player;
    tmp.next_valid_spots = tmp.get_valid_spots();
    int value=0;
    for(Point p : tmp.next_valid_spots){
        value = value + score[p.x][p.y] + 10;
    }
//    if()
    return value;
}

int cnt_value2(Point &point){
    return score[point.x][point.y];
}*/

int cnt_value3(State rhs){
    int value=0;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(rhs.board[i][j]==player)
                value += score[i][j] * 1;
            else if(rhs.board[i][j]==3-player)
                value += score[i][j] * -1;
        }
    }
    int tmpsize = rhs.next_valid_spots.size();
    value += (tmpsize*10);
    return value;
}


void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
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

int minimax_AB(int dep, int alpha, int beta, int mamiplayer, State &S, Point &point_to_move){      //mamiplayer : 1=me  0=opponent
    if(dep>=6)
    {
        return cnt_value3(S);
    }
    if(mamiplayer) //me
    {
        int maxeval = INT_MIN;
        State tmpstate;
        tmpstate.reset();
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                tmpstate.board[i][j] = S.board[i][j];
            }
        }
        for(int i=0;i<3;i++){
            tmpstate.disc_count[i] = S.disc_count[i];
        }
        tmpstate.cur_player = player;
        tmpstate.set_disc(point_to_move, 3-player);
        tmpstate.disc_count[3-player]++;
        tmpstate.disc_count[tmpstate.EMPTY]--;
        tmpstate.flip_discs(point_to_move);
        tmpstate.next_valid_spots = tmpstate.get_valid_spots();
        if(tmpstate.next_valid_spots.size()==0)
        {
            tmpstate.cur_player = tmpstate.get_next_player(tmpstate.cur_player);
            tmpstate.next_valid_spots = tmpstate.get_valid_spots();
            if(tmpstate.next_valid_spots.size()==0)
            {    //game ends
                int white_disc = tmpstate.disc_count[tmpstate.EMPTY];
                int black_disc = tmpstate.disc_count[tmpstate.BLACK];
                if(white_disc == black_disc) return 0;
                else if(player==tmpstate.WHITE)
                {
                    if(white_disc > black_disc) return 999999;
                    else if(white_disc < black_disc) return -999999;
                }
                else if(player==tmpstate.BLACK)
                {
                    if(white_disc > black_disc) return -999999;
                    else if(white_disc < black_disc) return 999999;
                }
            }
            else return -999999;
        }
        else
        {
            Point p;
            for(p : tmpstate.next_valid_spots){
                int eval;
                eval = minimax_AB(dep+1, alpha, beta, 1-mamiplayer, tmpstate, p);
                maxeval = max(maxeval, eval);
                alpha = max(alpha, eval);
                if(beta <= alpha) break;
            }
            return maxeval;
        }
    }
    else   //opponent
    {
        int mineval = INT_MAX;
        State tmpstate;
        tmpstate.reset();
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                tmpstate.board[i][j] = S.board[i][j];
            }
        }
        for(int i=0;i<3;i++){
            tmpstate.disc_count[i] = S.disc_count[i];
        }
        tmpstate.cur_player = 3-player;
        tmpstate.set_disc(point_to_move, player);
        tmpstate.disc_count[player]++;
        tmpstate.disc_count[tmpstate.EMPTY]--;
        tmpstate.flip_discs(point_to_move);
        tmpstate.next_valid_spots = tmpstate.get_valid_spots();
        if(tmpstate.next_valid_spots.size()==0)
        {
            tmpstate.cur_player = tmpstate.get_next_player(tmpstate.cur_player);
            tmpstate.next_valid_spots = tmpstate.get_valid_spots();
            if(tmpstate.next_valid_spots.size()==0)
            {    //game ends
                int white_disc = tmpstate.disc_count[tmpstate.WHITE];
                int black_disc = tmpstate.disc_count[tmpstate.BLACK];
                if(white_disc == black_disc) return 0;
                else if(player==tmpstate.WHITE)
                {
                    if(white_disc > black_disc) return 999999;
                    else if(white_disc < black_disc) return -999999;
                }
                else if(player==tmpstate.BLACK)
                {
                    if(white_disc > black_disc) return -999999;
                    else if(white_disc < black_disc) return 999999;
                }
            }
            else return 999999;
        }
        else
        {
            Point p;
            for(p : tmpstate.next_valid_spots){
                int eval;
                eval = minimax_AB(dep+1, alpha, beta, 1-mamiplayer, tmpstate, p);
                mineval = min(mineval, eval);
                beta = min(beta, eval);
                if(beta <= alpha) break;
            }
            return mineval;
        }
    }
}


void write_valid_spot(std::ofstream& fout) {
    Point i, p;
    int maxvalue = INT_MIN;
    State tmpstate;
    tmpstate.reset();
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            tmpstate.board[i][j] = board[i][j];
            if(board[i][j]==0) tmpstate.disc_count[0]++;
            else if(board[i][j]==1) tmpstate.disc_count[1]++;
            else if(board[i][j]==2) tmpstate.disc_count[2]++;
        }
    }
    //p.x = 9, p.y = 9;
    //Point p(next_valid_spots[0]);
    for(i : next_valid_spots){
        int val = minimax_AB(2, INT_MIN, INT_MAX, 0, tmpstate, i);
        if(val > maxvalue)
        {
            maxvalue = val;
            p = i;
        }
    }
    if(next_valid_spots.size()==1) p = next_valid_spots.front();
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
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

