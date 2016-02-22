#ifndef GRIDSQUARE_HH
#define GRIDSQUARE_HH

class GridSquare {
    public:
    bool isBlocked;
    char mark;
    GridSquare* parent;
    int g_score;
    int f_score;
};
#endif
