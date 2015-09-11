#include "checkerstate.h"

/*四种构造函数*/
CheckersState::CheckersState()
{
    allocate(10);
}

CheckersState::CheckersState(uint8 size)
{
    allocate(size);
}

CheckersState::CheckersState(const CheckersState &source) {
    allocate(source.n);
    for(int i=0; i<n; i++)
        for(int j=0; j<n/2; j++)
            data[i][j] = source.data[i][j];
    xcounts = source.xcounts;
}

CheckersState::CheckersState(const CheckersState *source) {
    allocate(source->n);
    for(int i=0; i<n; i++)
        for(int j=0; j<n/2; j++)
            data[i][j] = source->data[i][j];
    xcounts = source->xcounts;
}

CheckersState::~CheckersState() {
    for(int i=0; i<n; i++)
        delete[] data[i];
    delete[] data;
}

/*空间分配并且完成部分初始化*/
void CheckersState::allocate(uint8 size) {
    n = size;
    data = new uint8 * [n];
    for(int i=0; i<n; i++)
    {
        data[i] = new uint8[n/2];
        memset( data[i], 0, n/2*sizeof(uint8) );
    }
    tmp = 0;
    p = NULL;
    deletedatmove = 0;
}


uint8 CheckersState::size() {
    return n;
}

std::vector <uint8> & CheckersState::counts() {
    return xcounts;
}

/*生成一条棋路后的状态*/
CheckersState * CheckersState::genNextState(std::vector <point> & v) {
    CheckersState * state = new CheckersState(this);
    uint8 tmp;
    for(unsigned k=0; k<v.size(); k++) {
        uint8 i = v.at(k).x;
        uint8 j = v.at(k).y;
        switch(v.at(k).type) {
        case(MOVEDFROM):
            tmp = state->at(i,j);
            state->at(i,j) = 0;
            break;
        case(MOVEDTO):
            state->at(i,j) = tmp;
            break;
        case(MARKDELETED):
            state->at(i,j) = MARKDELETED;
            break;
        case(DELETED):
            state->at(i,j) = 0;
            break;
        case(TOKING):
            if( tmp == WHITE )
                state->at(i,j) = WHITEKING;
            if( tmp == BLACK )
                state->at(i,j) = BLACKKING;
            break;
        default:
            break;
        }
    }
    state->move() = v;
    state->counts().clear();
    return state;
}

/*设置父结点*/
void CheckersState::setParent(CheckersState * parent) {
    p = parent;
}

/*返回父结点*/
CheckersState * CheckersState::parent() {
    return p;
}

/*返回子树容器*/
std::vector < CheckersState * > & CheckersState::childs() {
    return xchilds;
}

/*返回棋路中点容器*/
std::vector < point > & CheckersState::move() {
    return xmove;
}

/*返回棋路的评估分*/
int & CheckersState::score() {
    return xscore;
}

/*返回棋路的删除动作数*/
int & CheckersState::deletedAtMove() {
    return deletedatmove;
}

/*返回点属性*/
uint8 & CheckersState::at(uint8 i, uint8 j) {
    if(i%2 != j%2) {
        tmp = WHITECELL;
        return tmp;
    }
    return data[i][j/2];
}

/*返回棋子颜色*/
uint8 CheckersState::color(uint8 i, uint8 j) {
    if( at(i,j) == WHITE || at(i,j) == WHITEKING)
        return WHITE;
    if( at(i,j) == BLACK || at(i,j) == BLACKKING)
        return BLACK;
    return false;
}

bool CheckersState::isWhite(uint8 i, uint8 j) {
    if(at(i,j)==WHITE || at(i,j)==WHITEKING)
        return true;
    return false;
}

bool CheckersState::isBlack(uint8 i, uint8 j) {
    if(at(i,j)==BLACK || at(i,j)==BLACKKING)
        return true;
    return false;
}

bool CheckersState::isKing(uint8 i, uint8 j) {
    if(at(i,j)==WHITEKING || at(i,j)==BLACKKING)
        return true;
    return false;
}

/*判断是否没放棋子*/
bool CheckersState::isNull(uint8 i, uint8 j) {
    if( at(i,j) == WHITECELL )
        return true;
    return !at(i,j);
}

