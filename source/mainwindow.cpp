#include "mainwindow.h"
#include <QStackedLayout>
#include <QLabel>
#include <QSettings>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    whiteIcon = new QLabel;
    statusbar->addWidget(whiteIcon);
    whiteIcon->setPixmap(QPixmap(":/icons/whitelabel.png"));

    whiteLabel = new QLabel;
    statusbar->addWidget(whiteLabel);

    blackIcon = new QLabel;
    statusbar->addWidget(blackIcon);
    blackIcon->setPixmap(QPixmap(":/icons/blacklabel.png"));

    blackLabel = new QLabel;
    statusbar->addWidget(blackLabel);

    QWidget * spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    statusbar->addWidget(spacer,1);

    timeLabel = new QLabel;
    timeLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    statusbar->addWidget(timeLabel);
    timeLabel->setStyleSheet("color: black;");

    time = new QTimer;
    connect(time, SIGNAL(timeout()), SLOT(timeChanged()));
    time->start(1000);
    timeChanged();

    game = new CheckersGame;
    optionsdialog=new OptionsDialog(this);

    connect(actionNewGame, SIGNAL(triggered()), this, SLOT(startNewGame()));
    connect(actionOption, SIGNAL(triggered()), this, SLOT(option()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    connect(optionsdialog,SIGNAL(accepted()),this,SLOT(saveSettings()));
    connect(optionsdialog,SIGNAL(accepted()),this,SLOT(startNewGame()));


    connect(picture, SIGNAL(mouseClicked(int, int)), game, SLOT(setClicked(int, int)));
    connect(game, SIGNAL(stateChanged(CheckersState *)), picture, SLOT(setState(CheckersState *)));
    connect(game, SIGNAL(stateChanged(CheckersState *)), SLOT(setState(CheckersState *)));
    connect(game, SIGNAL(vectorChanged(std::vector <point> &)), picture, SLOT(setVector(std::vector <point> &)));
    connect(game, SIGNAL(vectorDeleted()), picture, SLOT(deleteVector()));

    connect(game, SIGNAL(gameEnded(uint8)), SLOT(gameEnded(uint8)));
    connect(game, SIGNAL(startThinking()), picture, SLOT(startThinking()));
    connect(game, SIGNAL(stopThinking()), picture, SLOT(stopThinking()));

    startNewGame();
}

MainWindow::~MainWindow()
{
    delete game;
}

/*开始新的游戏*/
void MainWindow::startNewGame()
{
    loadSettings();

    QSettings settings("IoT1301", "Checkers");
    int color = myColor = settings.value("color",WHITE).toInt();
    int level = settings.value("level",EASY).toInt();
    int show=settings.value("showHourglass").toBool();
    if (color == WHITE)
        color = BLACK;
    else
        color = WHITE;
    picture->setComputerColor(color);
    picture->setShowHourglass(show);
    game->setMaxLevel(level);
    game->startNewGame(color);

}

/*更新棋子计数器*/
void MainWindow::setState(CheckersState * state)
{
    if (state->counts().size())
    {
        int whiteCount = state->counts().at(0) + state->counts().at(1);
        int blackCount = state->counts().at(4) + state->counts().at(5);
        whiteLabel->setText(QString("<b><font color=red>%1</font></b>").arg(whiteCount));
        blackLabel->setText(QString("<b><font color=red>%1</font></b>").arg(blackCount));
    }
}

/*程序结束*/
void MainWindow::endGame()
{
    game->endGame();
    picture->stopThinking();
    close();
}

/*游戏结束*/
void MainWindow::gameEnded(uint8 status)
{
    QString str;
    if(status == myColor)
        str=QString("恭喜，你赢了！\n再来一局？");
    else
        str=QString("很遗憾，你输了！\n再来一局？");
    int r=QMessageBox::information(this,tr("结果"),str,QMessageBox::Yes|QMessageBox::No);
    if(r==QMessageBox::Yes)
        startNewGame();
    else
        endGame();
}

/*打开选项对话框*/
void MainWindow::option()
{
    if (!optionsdialog) {
        optionsdialog= new OptionsDialog(this);
    }
    optionsdialog->show();
    optionsdialog->raise();
    optionsdialog->activateWindow();
}

/*保存设置*/
void MainWindow::saveSettings()
{
    QSettings settings("IoT1301", "Checkers");
    int color, level;

    if( optionsdialog->colorcomboBox->currentIndex() == 0)
        color = WHITE;
    else
        color = BLACK;

    if(optionsdialog->levelcomboBox->currentIndex()==0)
        level=EASY;
    else if(optionsdialog->levelcomboBox->currentIndex()==1)
        level=MEDIUM;
    else
        level=DIFFICULT;

    settings.setValue("color",color);
    settings.setValue("level",level);
    settings.setValue("showHourglass", optionsdialog->showcheckBox->isChecked());
}

/*载入设置*/
void MainWindow::loadSettings()
{
    QSettings settings("IoT1301", "Checkers");

    int color = settings.value("color",WHITE).toInt();
    if(color != WHITE && color !=BLACK)
        color = WHITE;
    int level = settings.value("level",EASY).toInt();
    if( level < EASY || level > DIFFICULT)
        level = EASY;

    if( color == BLACK )
        optionsdialog->colorcomboBox->setCurrentIndex(1);
    else
        optionsdialog->colorcomboBox->setCurrentIndex(0);
    if(level==MEDIUM)
        optionsdialog->levelcomboBox->setCurrentIndex(1);
    else if(level==DIFFICULT)
        optionsdialog->levelcomboBox->setCurrentIndex(2);
    else
        optionsdialog->levelcomboBox->setCurrentIndex(0);

    bool showHourglass= settings.value("showHourglass", true).toBool();
    optionsdialog->showcheckBox->setChecked(showHourglass);
}

void MainWindow::timeChanged()
{
    timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
}

void MainWindow::about() {
    QMessageBox::about(this, trUtf8("关于国际跳棋"),
                       trUtf8(
                           "<h3 align=center>关于国际跳棋</h3>"
                           "<P>开发者"
                           "<P align=center>张云远"
                           "<P align=center>- HUST, IOT1301, 2014 -"));
}

void MainWindow::help(){
    QMessageBox *messageRecord = new QMessageBox(this);
    messageRecord->setStyleSheet("background-color:white");
    messageRecord->setWindowTitle("国际跳棋规则");
    QString str = QString("<h3>棋盘</h3><p>棋盘是由深浅两色间隔排列的一百个小方格组成的正方形，（即：10小方格X10小"
                          "方格）深色的小方格里都有阿拉伯数字的号码叫做棋位，号码是作为棋局记录使用的。</p>"
                          "<h3>棋子</h3><p>棋子是圆柱型的，黑白棋子各二十枚，棋子表面上有罗纹，这种棋子叫“兵”<br>"
                          "把兵翻过来（或两兵叠起来）就是“王”（兵跳到对方的底线升变为“王”或称为王棋）<br>"
                          "行棋前，把棋盘摆在对弈者中间，双方面对棋盘的左下角是黑格，黑兵摆在1至20的棋位上，白兵摆在"
                          "31至50的棋位上，对局开始执白棋者先行。</p><h3>走法</h3>"
                          "<p>所有棋子均在黑格子中行走。<br><br>1、兵的走法<br>兵的走法是：只能向前斜走一格，不能后退。"
                          "<br><br>2、兵的跳吃<br>兵的跳吃是：黑白两枚棋子紧连在一条斜线上，如轮到某一方行棋时，对方"
                          "棋子的前后正好有一空棋位能跳过对方的棋子，那么就可以跳过对方的棋子把被跳过的棋子吃掉，并从棋盘上取下。"
                          "<br><br>3、兵的连跳<br>兵的连跳是是跳过对方的棋子以后，又遇上可以跳过的棋子，那么就可以连续"
                          "跳过去，把被跳过的棋子吃掉，并且从棋盘上一次取下（见下图）。兵的走法是不能后退，但是遇到跳吃或"
                          "连续跳时，可以退跳或吃子。<br><br>4、兵的升变<br>对局开始前双方在棋盘上摆的棋子都是兵，兵在"
                          "对局过程中，走到或跳到对方底线停下，即可升变为“王”刚升变的王要到下一步才能享有王的走法的权利。"
                          "<br>兵在对局过程中，走到或跳到对方底线没停下（即中途经过），不可以升变为“王”。<br><br>"
                          "5、王的走法<br>王的走法是：王在其位于任何一条斜线上均可进退，并且不限格数。（类似国际象棋的象"
                          "的走法）<br><br>6、王的跳吃<br>王的跳吃是王与对方棋子遇在同一斜线上，不管相距有几个棋位，对方"
                          "棋子的前后只要有空棋位，那么王棋就可以跳过去吃掉对方的棋子，而且跳吃时要跳过的对方棋子前后面的那"
                          "一个空位里。<br><br>7、王的连跳<br>王的连跳与兵连跳的情况基本上相同，只是不限距离。</p>"
                          "<h3>吃子规定</h3><p>1、有吃必吃<br>凡有跳吃或连跳机会时，不管对自己是否有利都必须连续跳吃或跳"
                          "过，尤其是王。如果有连跳的局面，必须将对方所有的棋子跳完，直到无可再跳时才能停下。<br><br>"
                          "2、吃多数棋子（必须吃多不能吃少）<br>如果有两条路线或2枚棋子都能吃对方的棋子，那么不管是否对自己"
                          "有利，必须吃多的棋子。例如：同时在两条路线上可以吃对方的棋子，一条路线上能吃3枚棋子，另一条路线上"
                          "能吃2枚棋子，必须先跳吃3枚棋子。<br><br>3、土耳其打击<br>在连跳时，王或兵都必须将对方所有可能"
                          "跳过的棋子跳完以后，才可以将对方被跳过的棋子从棋盘上一次性取下。一着棋连跳中即不允许跳一枚（棋子）"
                          "取一枚（棋子），也不允许重复地两次跳过对方的同一枚棋子。利用这条规定形成的吃子方法叫做土耳其打击。</p>"
                          "<h3>棋局结束</h3><p>1、所有的棋子都被对方吃掉为负棋。<br>2、残留在棋盘上的棋子，被对方封锁，"
                          "无子可动为负棋。<br>3、棋局进行到最后无任何可能战胜对方时为和棋。</p>");

    QLabel *label = new QLabel(str);
    label->setWordWrap(true);
    QScrollArea *area = new QScrollArea();
    area->setMinimumSize(530, 550);
    area->setWidget(label);
    area->setWidgetResizable(true);
    area->setStyleSheet("border:0");
    area->setBackgroundRole(QPalette::Midlight);
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(area);
    (dynamic_cast<QGridLayout *>(messageRecord->layout()))->addLayout(grid, 0, 0);
    QPushButton *buttonOK = messageRecord->addButton("确 认", QMessageBox::AcceptRole);
    buttonOK->setStyleSheet("QPushButton{width:65px solid; height: 22px; border-radius:4px; "
                            "background-color:qconicalgradient(cx:0.5, cy:0.522909, angle:179.9, stop:0.494318 rgba(214, 214, 214, 255), "
                            "stop:0.5 rgba(236, 236, 236, 255));}"
                            "QPushButton:hover{ background-color: qconicalgradient(cx:0.5, cy:0.522909, angle:179.9,"
                            " stop:0.494318 rgba(181, 225, 250, 255), stop:0.5 rgba(222, 242, 251, 255));border-radius:5px;"
                            " border: 1px solid #3C80B1}"
                            "QPushButton:pressed{background-color: qconicalgradient(cx:0.5, cy:0.522909, angle:179.9, "
                            "stop:0.494318 rgba(134, 198, 233, 255), stop:0.5 rgba(206, 234, 248, 255));border-radius:5px;"
                            "border: 1px solid #5F92B2;  }");
    messageRecord->exec();
}
