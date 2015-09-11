#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QScrollArea>

#include "ui_mainwindow.h"
#include "checkergame.h"
#include "checkerstate.h"
#include "optionsdialog.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void startNewGame();
    void setState(CheckersState *);
    void endGame();
    void option();
    void about();
    void help();
    void gameEnded(uint8 status);
    void saveSettings();
    void loadSettings();
    void timeChanged();

private:
    CheckersGame * game;
    OptionsDialog *optionsdialog;
    int myColor;
    QString greeting;
    QLabel * timeLabel;
    QTimer * time;
    QLabel * whiteIcon;
    QLabel * whiteLabel;
    QLabel * blackIcon;
    QLabel * blackLabel;
};

#endif // MAINWINDOW_H
