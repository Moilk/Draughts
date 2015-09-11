#include "checkergame.h"
#include <time.h>
#include <QDebug>

CheckersGame::CheckersGame()
{
    srand(time(NULL));
    cleared = 0;
    created = 0;
    n = 10;
    k_start = 4;
    current = NULL;
    first = NULL;
    current = NULL;
}

CheckersGame::~CheckersGame() {
    endGame();
}

/*设置最大搜索深度*/
void CheckersGame::setMaxLevel(int level) {
    if(level >=3 && level <=7)
        maxlevel = level;
    else
        maxlevel = 3;
}

/*开始新的游戏*/
void CheckersGame::startNewGame(uint8 color) {
    computercolor = color;
    if(color == WHITE)
        humancolor = BLACK;
    else
        humancolor = WHITE;
    first = new CheckersState(n);
    created++;

    /*初始化各个能放棋子的方格的棋子类型*/
    for(int i=0; i<n; i++) {
        for(int j=0; j<k_start; j++) {
            if(i%2 == j%2)
                first->at(i,j) = WHITE;
        }
        for(int j=n-k_start; j<n; j++) {
            if(i%2 == j%2)
                first->at(i,j) = BLACK;
        }
    }
    if(current)
        delete current;
    current = new CheckersState(first);
    created++;
    click = 0;
    /* 旁路的安装方向*/
    ix[0] = -1; ix[1] = 1; ix[2] = -1; ix[3] = 1;   /*左下；右下；左上；右上*/
    jx[0] = 1; jx[1] = 1; jx[2] = -1; jx[3] = -1;
    gamerunning = true;
    if(computercolor == WHITE) {
        go();
    } else {
        emit stateChanged(current);
        qDebug() << "sc1";
        pp(current,humancolor);
    }
}

/*结束游戏*/
void CheckersGame::endGame() {
    cleared = 0;
    created = 0;
    gamerunning = false;
    if(current) {
        clearTree(current, true, true);
        current = NULL;
    }
    first = NULL;
}

/*把state下的color棋子所有可行棋路的状态设为state的子树*/
void CheckersGame::pp(CheckersState * state, uint8 color) {
    if (!gamerunning)/*如果游戏没有开始则返回*/
        return;
    std::vector <point> vp;
    capturefound = false;/*捕获为假*/
    for(unsigned i=0; i<n; i++) {
        for(unsigned j=0; j<n; j++) {
            if(i%2 == j%2 && state->color(i,j) == color) {
                searchMove(state, i, j, vp);    /*把所有该色点的所有棋路存进movesearch中*/
                clearTree(state, false, true);  /* 删除孩子保留根*/
            }
        }
    }
    /*如果捕获为真，清除movesearch中的非捕获棋路*/
    if(capturefound) {
        for(unsigned i=0; i<movesearch.size(); i++) {
            if( movesearch.at(i)->move().size() == 2 ) {
                delete movesearch.at(i);
                movesearch.erase( movesearch.begin()+i );
                i--;
            } else {
                break;
            }
        }
    }

    /*如果是国际版，则找出最大吃子路线，删除其它棋路
     * PS：俄罗斯版本没有有多吃必多吃的规定*/
        int max_deleted = 0;
        for(unsigned i=0; i<movesearch.size(); i++) {
            if( movesearch.at(i)->deletedAtMove() > max_deleted )
                max_deleted = movesearch.at(i)->deletedAtMove();
        }
        for(unsigned i=0; i<movesearch.size(); i++) {
            if( movesearch.at(i)->deletedAtMove() < max_deleted ) {
                delete movesearch.at(i);
                movesearch.erase( movesearch.begin()+i );
                i--;
            }
        }
    /*把当前可用的棋路设为state的子树*/
    for(unsigned i=0; i<movesearch.size(); i++) {
        std::vector<point>::iterator it;            /*声明一个迭代器*/
        for ( it=movesearch.at(i)->move().begin() ; it < movesearch.at(i)->move().end(); it++ ) {
            if( it->type == DELETED ) /*把类型为DELETED的点置为零*/
                movesearch.at(i)->at( it->x, it->y ) = 0;
        }
        movesearch.at(i)->setParent(state);
        state->childs().push_back( movesearch.at(i) );
    }
    movesearch.clear();
}


/**/
int CheckersGame::movesCount(CheckersState * state, int i, int j) {
    uint8 color = state->color(i,j);
    int moves = 0;
    if(!color)
        return 0;
    int sidx, eidx, pidx = 1;
    if(color == WHITE) {
        sidx = 0; eidx = 1;     /*方向为左下和右下*/
    }
    if(color == BLACK) {
        sidx = 2; eidx = 3;     /*方向为左上和右上*/
    }
    if( state->isKing(i,j) ) {
        sidx = 0; eidx = 3; pidx = n; /*方向全激活，攻击距离全开*/
    }
    /**/
    for(char k=sidx; k<=eidx; k++) {
        for(char pk=1; pk <= pidx; pk++) {
            char xi = i + pk*ix[(int)k];
            char xj = j + pk*jx[(int)k];
            if( !checkCoordinate(xi) || !checkCoordinate(xj) ) /*如果坐标超出棋盘则跳出该方向*/
                break;
            if(state->isNull(xi,xj)) {/*如果该点没放棋子，moves加1，否则跳出该方向*/
                moves ++;
            } else {
                break;
            }
        }
    }
    bool captureflag;
    for(int k=0; k<=3; k++) {
        captureflag = false;
        for(char pk=1; pk <= pidx+1; pk++) {/*考虑捕获攻击距离要加1*/
            char xi = i + pk*ix[k];
            char xj = j + pk*jx[k];
            if( !checkCoordinate(xi) || !checkCoordinate(xj) )
                break;
            if( !captureflag && state->isNull(xi,xj) )/*如果预捕获为假而且无棋子，跳至该方向下一点*/
                continue;
            if( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
                break;/*如果该点棋子颜色是本色或者该点属性为目标删除，则跳出该方向*/
            if( !captureflag && state->color(xi, xj)!= color ) {
                captureflag = true; /*如果预捕获为假且棋子颜色为反色，则标记预捕获，跳至该方向下一点*/
                continue;
            }
            if(captureflag) {
                if( !state->isNull(xi,xj) ) {
                    captureflag = false;
                    break;
                }
                moves++;
            }
        }
    }
    return moves;
}


/*函数功能：把点(i,j)出发的棋路全部存进movesearch*/
int CheckersGame::searchMove(CheckersState *state, int i, int j, std::vector <point> & vp) {
    std::vector < std::vector <point> > vpp; /*装点容器的容器*/
    std::vector <point> tmp_vp;
    point cp;
    int normmoves = 0, delmoves = 0;

    uint8 color = state->color(i,j);
    bool isking = state->isKing(i,j);
    if(!color)
        return false;

    int sidx, eidx, pidx = 1; /*sidx与eidx用来限定旁路方向，pidx用来限定步数*/
    if(color == WHITE) {
        sidx = 0; eidx = 1;         /*方向限定在左下和右下*/
    }
    if(color == BLACK) {
        sidx = 2; eidx = 3;         /*方向限定在左上和右上*/
    }
    if( state->isKing(i,j) ) {
        sidx = 0; eidx = 3; pidx = n; /*四个方向全部激活，最大步数是棋盘格数*/
    }
    /*没有捕获的情况下*/
    for(char k=sidx; k<=eidx && !capturefound; k++) {       /*没有捕获的情况下，白棋只能朝下，黑棋只能朝上移*/
        for(char pk=1; pk <= pidx; pk++) {                                          /*普通棋子最多只能移动一步，王棋n步*/
            char xi = i + pk*ix[(int)k];
            char xj = j + pk*jx[(int)k];
            if( !checkCoordinate(xi) || !checkCoordinate(xj) )      /*如果该点超出棋盘则跳过该方向*/
                break;
            if(state->isNull(xi,xj)) {  /*如果该格没放棋子*/
                tmp_vp.clear();     /*清除临时点容器中所有数据*/
                tmp_vp.push_back(point(i,j,MOVEDFROM));     /*在尾部插入一条数据*/
                /*如果黑棋下一步到顶或者白棋到底，下一格属性为王棋*/
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==n-1 && !isking)  ) {
                    tmp_vp.push_back(point(xi,xj,TOKING));
                } else {
                    tmp_vp.push_back(point(xi,xj,MOVEDTO));     /*否则属性为目标格*/
                }
                vpp.push_back(tmp_vp);          /*存入一条棋路*/
                normmoves ++;
            } else {
                break;
            }
        }
    }
    bool captureflag;       /*预捕获标记*/
    for(int k=0; k<=3; k++) {
        captureflag = false;
        for(char pk=1; pk <= pidx+1; pk++) {
            char xi = i + pk*ix[k];
            char xj = j + pk*jx[k];
            if( !checkCoordinate(xi) || !checkCoordinate(xj) )	/*如果超出棋盘则跳过此方向*/
                break;
            if( !captureflag && state->isNull(xi,xj) )				/*如果没有预捕获标记且没棋子则进入该方向下一格*/
                continue;
            /*如果该格棋子颜色与出发点颜色相同或者该点属性为目标删除，则跳过该方向*/
            if( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
                break;
            /*如果该点没有预捕获且颜色与出发点不同，则标记为预捕获，
             * 缓存该点数据，类型记为目标删除，且进入该方向下一格*/
            if( !captureflag && state->color(xi, xj)!= color ) {
                captureflag = true;
                cp.x = xi; cp.y = xj; cp.type = MARKDELETED;
                continue;
            }
            /*如果有预捕获标记*/
            if(captureflag) {
                /*如果该格有棋子则取消预捕获标记，跳出该方向*/
                if( !state->isNull(xi,xj) ) {
                    captureflag = false;
                    break;
                }
                /*否则预捕获成功，标记为捕获，删除步加1*/
                capturefound = true;
                delmoves++;

                /*临时容器清空存入起点和目标删除点*/
                tmp_vp.clear();
                tmp_vp.push_back(point(i,j,MOVEDFROM));
                tmp_vp.push_back(cp);
                /*如果达成成王条件则属性为王棋，否则仅为终点*/
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==n-1 && !isking)  )
                    tmp_vp.push_back(point(xi,xj,TOKING));
                else
                    tmp_vp.push_back(point(xi,xj,MOVEDTO));

                CheckersState *tmpstate = state->genNextState(tmp_vp);		/*把该条棋路后的状态设置为临时状态*/
                created++;

                /*把vp赋给历史容器*/
                std::vector <point> history_vector = vp;
                if( vp.size() ) {           /*如果vp非空，则原终点变成经过点，目标删除点变成删除点，删除操作加1*/
                    (history_vector.end()-1)->type = MOVEDTHROUGH;          /*end()指向的是最后一条数据的下一个*/
                    (history_vector.end()-2)->type = DELETED;
                    tmpstate->deletedAtMove() = state->deletedAtMove()+1;
                } else {
                    history_vector.push_back( tmp_vp.at(0) );   /*否则将临时容器中的起点放入历史容器，删除操作为1*/
                    tmpstate->deletedAtMove() = 1;
                }

                /*注意如果历史容器非空则末端就是临时容器的起点*/
                cp.type = DELETED;
                history_vector.push_back( cp );         /*把临时容器中的目标删除点和终点按格式放入历史容器中*/
                history_vector.push_back( tmp_vp.at(2) );
                tmpstate->move() = history_vector;      /*把临时状态的xmove设为历史容器*/
                tmpstate->setParent(state);                   /*把临时状态设为state的子结点*/
                state->childs().push_back(tmpstate);    /*注意递归调用中临时状态的父节点设为了上一层的临时状态*/
                if(! searchMove(tmpstate, xi, xj, history_vector )) /*自身递归调用，把点(i,j)所有该方向的棋路都存入movesearch*/
                    movesearch.push_back(tmpstate); /*注意如果一条路没走到尽头，棋路总值就不会等于零，该语句就不会执行*/
            }
        }
    }       /*循环四次，如果都非捕获，则vpp保存有所有可能的非捕获棋路的初始点状态
              *如果存在捕获，则所有捕获棋路都已存入movesearch*/

    if(capturefound) {          /*如果捕获成立，则正常移动为否*/
        normmoves = 0;
    } else {    /*否则把vpp中全部非捕获棋路存入movesearch*/
        for(unsigned i=0; i<vpp.size(); i++) {
            CheckersState * tmpstate = state->genNextState(vpp.at(i));
            created++;
            tmpstate->setParent(state);
            state->childs().push_back(tmpstate);
            movesearch.push_back(tmpstate);
        }
    }
    return normmoves + delmoves;        /*返回正常棋路和捕获棋路的总值*/
}


void CheckersGame::go()
{
    goRecursive(current, 1, -9999, 9999);
    int xmax = -9999;
    int id = 0;
    for(unsigned i=0; i<current->childs().size(); i++) {
        if( current->childs().at(i)->score() > xmax ) {
            xmax = current->childs().at(i)->score();
        }
    }
    std::vector <CheckersState *> tmp;
    for(unsigned i=0; i<current->childs().size(); i++) {
        if( current->childs().at(i)->score() == xmax ) {
            tmp.push_back( current->childs().at(i) );
        }
    }
    id = rand() % tmp.size();
    move( tmp.at(id)->move().front(), tmp.at(id)->move().back() );
    pp(current,humancolor);
}


/*搜索算法，阿尔法-贝塔剪枝*/
int CheckersGame::goRecursive(CheckersState *state, int level, int alpha, int beta)
{
    bool max = true;
    if(computercolor == WHITE)
        max = false;/*白方max为true*/
    uint8 color = humancolor;
    if(level%2) {/*深度为奇数则搜索电脑方*/
        color = computercolor;
        max = !max;
    }
    /*如果达到最终的节点或者深度达到限度，则使用评估函数返回局面得分*/
    if(level == maxlevel || checkTerminatePosition(state)) {
        state->score() = evaluation(state);
        return state->score();
    }
    /*把state下的color棋子所有可行棋路的状态设为state的子树*/
    pp(state, color);
    unsigned i;
    for(i=0; i< state->childs().size(); i++) {
        alpha = std::max( alpha, - goRecursive( state->childs().at(i), level+1 , -beta, -alpha ) );
        if ( beta < alpha ) {
            break;
        }
    }
    int xmax=-9999, xmin=9999;
    for(unsigned j=0; j<state->childs().size(); j++) {
        if(max) {
            if( state->childs().at(j)->score() > xmax )
                xmax = state->childs().at(j)->score();
        } else {
            if( state->childs().at(j)->score() < xmin )
                xmin = state->childs().at(j)->score();
        }
    }
    if(max)
        state->score() = xmax;
    else
        state->score() = xmin;
    return alpha;
}

/*检查坐标*/
bool CheckersGame::checkCoordinate(char x) {
    if(x>=0 && x<n)
        return true;
    return false;
}

/*统计各类棋子数目以及可移动数*/
void CheckersGame::calcCounts(CheckersState * state)
{
    state->counts().clear();
    state->counts().resize(8,0);/*xcounts容量为8，初始值为0*/

    int movescount;
    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<n; j++)
        {
            if(i%2!=j%2)
                continue;   /*如果是不能放棋子的点，直接跳过*/
            movescount = movesCount(state, i, j);/*点(i,j)上所有的移动*/
            switch(state->at(i, j))
            {
            case WHITE:
            {
                state->counts()[0]++;/*白色普通棋子个数*/
                state->counts()[2] += movescount;/*白方的移动数*/
                break;
            }
            case WHITEKING:
            {
                state->counts()[1]++;/*白色王棋的个数*/
                state->counts()[2] += movescount;
                break;
            }
            case BLACK:
            {
                state->counts()[4]++;/*黑色普通棋子的个数*/
                state->counts()[6] += movescount;/*黑方的移动数*/
                break;
            }
            case BLACKKING:
            {
                state->counts()[5]++;/*黑色王棋的个数*/
                state->counts()[6] += movescount;
                break;
            }
            }
        }
    }
}


/*  评估函数*/
int CheckersGame::evaluation(CheckersState * state) {
    if(!state->counts().size()) /*如果xcounts为空则构建*/
        calcCounts(state);
    int evaluation = 0;
    /*评估策略*/
    evaluation += 4 * ( state->counts()[0] - state->counts()[4] );
    evaluation += 6 * ( state->counts()[1] - state->counts()[5] );
    evaluation += ( state->counts()[2] - state->counts()[6] );

    return evaluation;
}


/*判断是否达到最终节点*/
bool CheckersGame::checkTerminatePosition(CheckersState * state)
{
    if(!state->counts().size())
        calcCounts(state);
    if ( !(state->counts()[0]+state->counts()[1]) ||
         !(state->counts()[4]+state->counts()[5]) )
        return true;
    if( !state->counts()[2] || !state->counts()[6] )
        return true;
    return false;
}


/*判断输赢*/
uint8 CheckersGame::whoWin(CheckersState *state) {
    if ( !(state->counts()[0]+state->counts()[1]) || !state->counts()[2] )
        return BLACK;
    if ( !(state->counts()[4]+state->counts()[5]) || !state->counts()[6] )
        return WHITE;
    return -1;
}


/* 清除树*/
void CheckersGame::clearTree(CheckersState * state, bool clearlists, bool onlychilds)
{
    if (onlychilds) {		/*只清除孩子*/
        for(unsigned i =0; i < state->childs().size(); i++) {
            clearTreeRecursive( state->childs().at(i), clearlists ); /*清除全部孩子节点*/
        }
        state->childs().clear();
    } else {
        clearTreeRecursive( state, clearlists ); /*全部清除*/
    }
}


/*清除树*/
void CheckersGame::clearTreeRecursive(CheckersState * state, bool clearlists) {
    /*如果没有孩子则递归返回*/
    if( state->childs().size() == 0 && clearlists ) {
        cleared ++;
        delete state;
    } else {
        for(unsigned i =0; i < state->childs().size(); i++) {
            clearTreeRecursive( state->childs().at(i), clearlists );
        }
        state->childs().clear();
        if(clearlists) {
            cleared ++;
            delete state;
        }
    }
}

/*鼠标点击设置*/
void CheckersGame::setClicked(int i, int j) {
    if(i>=0 && i<n && j>=0 && j<n && i%2==j%2 && gamerunning) {
        if(click == 0)
            firstClick(i, j);
        else
            secondClick(i, j);
    } else {
        emit vectorDeleted();
        click = 0;
    }
}

/*鼠标点击第一下*/
void CheckersGame::firstClick(int i, int j) {
    if( (humancolor == current->color(i,j))) {
        tmppoint.x = i; tmppoint.y = j; tmppoint.type = MOVEDFROM;
        tmpvector.clear();
        for(unsigned ii=0; ii< current->childs().size(); ii++ ) {
            if( current->childs().at(ii)->move().at(0) == tmppoint ) {
                for( unsigned jj=0; jj<current->childs().at(ii)->move().size(); jj++ ) {
                    tmpvector.push_back( current->childs().at(ii)->move().at(jj) );
                }
            }
        }
        if(tmpvector.size()) {
            click = 1;
            emit vectorChanged(tmpvector);
            return;
        }
    }
    emit vectorDeleted();
}

/*鼠标点击第二下*/
void CheckersGame::secondClick(int i, int j)
{
    bool move = false;
    if( current->isNull(i,j) || (tmppoint.x == i && tmppoint.y == j))
        move = this->move( tmppoint , point(i,j,MOVEDTO) );
    if( !move ) {
        click =0;
        firstClick(i, j);
    } else {
        if(gamerunning)
        {
            emit startThinking();
            go();
        }
        click = 0;
    }
}

/*棋子移动时伴随的操作*/
bool CheckersGame::move(point p1, point p2)
{
    for(unsigned i=0; i< current->childs().size(); i++ )
    {
        if( current->childs().at(i)->move().front() == p1 &&
                current->childs().at(i)->move().back().x == p2.x &&
                current->childs().at(i)->move().back().y == p2.y )
        {
            CheckersState * tmpstate = current->childs().at(i);
            current->childs().erase(current->childs().begin()+i);
            clearTree(current,true);
            clearTree(tmpstate,true,true);
            current = tmpstate;
            tmp = new CheckersState(current);
            tmp->childs().clear();
            emit stateChanged(current);
            emit stopThinking();

            if(checkTerminatePosition(current))
            {
                gamerunning = false;
                emit gameEnded( whoWin(current) );
            }
            return true;
        }
    }
    return false;
}



