#ifndef CHECKERPICTURE_H
#define CHECKERPICTURE_H

#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>

#include "checkerstate.h"

class CheckersPicture : public QWidget
{
    Q_OBJECT
public:
    CheckersPicture(QWidget * parent = 0);
    ~CheckersPicture();
    CheckersState * state();
    void setComputerColor(uint8 color);
    void setShowHourglass(bool show)
    {
        showHourglass = show;
    }
public slots:
    void setState(CheckersState *state);
    void setVector(std::vector <point> & v);
    void deleteVector();
    void setSize(int n);
    void clear();
    void startThinking();
    void stopThinking();
signals:
    void mouseClicked(int, int);
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent * event);
private:
    QRect pixelRect(int i, int j) const;

    CheckersState * curstate;
    std::vector <point> v;
    QPoint p;   /*棋盘的原点*/
    int side;       /*棋盘的边长*/
    int zoom;
    int n;
    bool thinking;
    QImage hourglass;
    bool showHourglass;

    uint8 color;
};

#endif // CHECKERSPICTURE_H

