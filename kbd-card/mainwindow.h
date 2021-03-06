#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mwworker.h"
#include <string>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void AddCardMsg(string &s);
    void GetSerial(string &s);
    void ShowErrCnt(int err, int sus);
    void GetMode(int &mode);
    void UpdateSerial();
    void GetCntLimit(int &cnt);
    void GetInterval(int &sec);
    bool GetFileOut(void);
    void GetErrorCode(int &err);

private slots:
    void ShowStatus(string s);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_error_clicked();

private:
    Ui::MainWindow *ui;
    mwWorker *worker;
    int msgcnt;
};

#endif // MAINWINDOW_H
