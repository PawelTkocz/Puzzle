struct PuzzleSide{
    int type; //
    //-1 wklesly 0 bok 1 wypukly
    int width; //
    int points_num; //
    struct Position* positions;
    double angle_left; //na razie nie uwzgledniam
    double angle_right; // na razie nie uwzgledniam
    int heighest; //
    int lowest; //
    int left_shape_ind; //
    int right_shape_ind; //
};

struct Position{
    int x;
    int y;
};

struct CornerCandidate{
    int ind;
    int x;
    int y;
    int end_left_x;
    int end_left_y;
    int end_right_x;
    int end_right_y;
    double angle;
};


struct Puzzle{
    struct PuzzleSide sides[4];
};

