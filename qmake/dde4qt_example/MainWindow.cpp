#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    urlProtocolHandler("dde4qt"),
    ui(new Ui::MainWindow)
{
    connect(&urlProtocolHandler, &win32::QUrlProtocolHandler::activate, this, &MainWindow::onProtocolActivate);
    urlProtocolHandler.install();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  urlProtocolHandler.uninstall();
  delete ui;
}

void MainWindow::onProtocolActivate(const QUrl& url) {
    QColor color(url.path());
    setPalette(QPalette(color));
}
