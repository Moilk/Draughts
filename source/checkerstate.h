#ifndef CHECKERSTATE_H
#define CHECKERSTATE_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

typedef unsigned char uint8;

enum checker {
    WHITE = 1,			// 白棋
    BLACK = 2,			// 黑棋
    WHITEKING = 3,			// 白王棋
    BLACKKING = 4,			// 黑王棋
    WHITECELL = 10,			// 没放棋子的格子, i%2 != j%2

    MOVEDFROM = 20,
    MOVEDTO = 21,
    MOVEDTHROUGH = 22,
    DELETED = 23,
    MARKDELETED = 24,
    TOKING = 25
};

class point {
public:
    point() {
        x=0; y=0; type=0;
    }
    point(int x, int y, int type) {
        this->x = x;
        this->y = y;
        this->type = type;
    }
    uint8 x;
    uint8 y;
    uint8 type;
    bool operator == (const point & p ) {
        if( x==p.x && y==p.y && type==p.type )
            return true;
        return false;
    }
};

class CheckersState
{
public:
    CheckersState();
    CheckersState(uint8 size);
    ~CheckersState();
    CheckersState(const CheckersState &source);
    CheckersState(const CheckersState *source);

    CheckersState * genNextState(std::vector <point> & v);
    void setParent(CheckersState * parent);
    std::vector <uint8> & counts();
    CheckersState * parent();
    std::vector < CheckersState * > & childs();
    std::vector < point > & move();
    int & deletedAtMove();
    int & score();
    uint8 size();
    uint8 & at(uint8 i, uint8 j);
    uint8 color(uint8 i, uint8 j);
    bool isWhite(uint8 i, uint8 j);
    bool isBlack(uint8 i, uint8 j);
    bool isKing(uint8 i, uint8 j);
    bool isNull(uint8 i, uint8 j);
private:
    CheckersState * p;
    std::vector < CheckersState * > xchilds;
    std::vector < point > xmove;		// 棋路中的点数
    std::vector < uint8 > xcounts;		// 不同类型棋子的数量，包括空棋
    void allocate(uint8 size);
    uint8 ** data;
    uint8 n;
    uint8 tmp;
    int xscore;
    int deletedatmove;
};

#endif // CHECKERSSTATE_H
