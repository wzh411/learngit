#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
using namespace cv;

const int N = 15, CELL = 40;  
int Map[N][N] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},  
    {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1},  
    {0,0,0,0,1,0,0,1,0,0,0,0,1,0,0},  
    {0,0,0,0,1,0,0,1,0,0,0,0,1,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,1,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,1,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0,1,0,0},  
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};
struct P {
    int x, y;
    int g, h, f;    
    P* fa;    
    P(int a, int b) : x(a), y(b) { g = h = f = 0; fa = nullptr; }
};
struct Cmp {
    bool operator()(P* a, P* b) { return a->f > b->f; }
};
int dis(int x, int y, int tx, int ty) { return abs(tx - x) + abs(ty - y); }
int main() {
    int sx = 0, sy = 0, tx = 14, ty = 14;
    Mat img(N * CELL, N * CELL, CV_8UC3);
    img.setTo(Scalar(255, 255, 255));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (Map[i][j])
                rectangle(img, Rect(j * CELL, i * CELL, CELL, CELL), Scalar(60, 60, 60), FILLED);
    priority_queue<P*, vector<P*>, Cmp> open;  
    bool close[N][N] = {false}; 
    int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};
    
    P* s = new P(sx, sy);
    s->h = dis(sx, sy, tx, ty);
    s->f = s->h;
    open.push(s);

    P* end = nullptr;
    while (!open.empty()) {
        P* u = open.top();
        open.pop();
        if (close[u->x][u->y]) continue;
        close[u->x][u->y] = true;

        rectangle(img, Rect(u->y * CELL, u->x * CELL, CELL, CELL), Scalar(0, 200, 255), FILLED);
        imshow("A*", img);
        waitKey(10);

        if (u->x == tx && u->y == ty) { end = u; break; } 
        for (int i = 0; i < 4; i++) { 
            int nx = u->x + dx[i], ny = u->y + dy[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue; 
            if (Map[nx][ny] || close[nx][ny]) continue;
            P* v = new P(nx, ny);
            v->g = u->g + 1;
            v->h = dis(nx, ny, tx, ty);
            v->f = v->g + v->h;
            v->fa = u;
            open.push(v);
        }
    }

    if (!end) { cout << "NO PATH!" << endl; return 0; }

    
    vector<P*> path;
    for (P* p = end; p; p = p->fa) path.push_back(p);
    reverse(path.begin(), path.end());

    
    for (P* p : path) {
        rectangle(img, Rect(p->y * CELL + 4, p->x * CELL + 4, CELL - 8, CELL - 8), Scalar(0, 0, 255), FILLED);
        imshow("A*", img);
        waitKey(30);
    }
    
    rectangle(img, Rect(sy * CELL + 4, sx * CELL + 4, CELL - 8, CELL - 8), Scalar(0, 200, 0), FILLED);
    rectangle(img, Rect(ty * CELL + 4, tx * CELL + 4, CELL - 8, CELL - 8), Scalar(0, 0, 255), FILLED);

    cout << "path len = " << path.size() << endl;
    imwrite("astar_result.png", img);
    imshow("A*", img);
    waitKey(0);
    return 0;
}
