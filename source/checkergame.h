#ifndef CHECKERGAME_H
#define CHECKERGAME_H

#include <QObject>
#include <QVector>
#include <vector>
#include "checkerstate.h"

enum GameLevel{
    EASY=3,
    MEDIUM=4,
    DIFFICULT=5
};

class CheckersGame : public QObject
{
    Q_OBJECT
public:
    CheckersGame();
    ~CheckersGame();
    bool setGameType(uint8 type);
    void setMaxLevel(int level);
    void startNewGame(uint8 color);
    void endGame();
    CheckersState * currentState() { return current; }
public slots:
    void setClicked(int i, int j);
signals:
    void stateChanged(CheckersState *);
    void vectorChanged(std::vector <point> & v);
    void vectorDeleted();
    void gameEnded(uint8);
    void startThinking();
    void stopThinking();
private:
    void firstClick(int i, int j);
    void secondClick(int i, int j);

    void go();
    int goRecursive(CheckersState * state, int level, int alpha, int beta);
    void calcCounts(CheckersState * state);
    int evaluation(CheckersState * state);
    void pp(CheckersState * state, uint8 color);
    bool checkTerminatePosition(CheckersState * state);
    int movesCount(CheckersState * state, int i, int j);
    int searchMove(CheckersState * state, int i, int j, std::vector <point> & vp);
    bool checkCoordinate(char x);
    void clearTree(CheckersState * state, bool clearlists = true, bool onlychilds = false);
    void clearTreeRecursive(CheckersState * state, bool clearlists = false);
    bool move(point p1, point p2);
    uint8 whoWin(CheckersState * state);

    std::vector <CheckersState *> movesearch;
    std::vector < point > tmpvector;	// 临时容器
    point tmppoint;

    CheckersState * first;			// 游戏的初始状态
    CheckersState * current;		// 游戏的当前状态
    CheckersState * tmp;
    uint8 n;						// 棋盘格数
    uint8 k_start;						// 棋子的行数
    uint8 type;						// 游戏的类型
    uint8 computercolor;			// 电脑颜色
    uint8 humancolor;				// 玩家颜色

    int level;						// 树的深度
    uint8 click;
    char ix[4];				/*定义方向*/
    char jx[4];

    bool capturefound;
    bool calccounts;
    bool gamerunning;
    int maxlevel;				// 搜索的最大深度

    // 状态记录器
    int cleared;
    int created;
};

#endif // CHECKERSGAME_H
