#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../../src/QDynamicDataExchange.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private Q_SLOTS:
    void onProtocolActivate(const QUrl& url);
private:
    Ui::MainWindow *ui;
    win32::QUrlProtocolHandler urlProtocolHandler;
};

#endif // MAINWINDOW_H
