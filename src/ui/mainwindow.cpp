#include "mainwindow.h"
#include <QTextEdit>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    auto* editor = new QTextEdit;
    setCentralWidget(editor);
    statusBar()->showMessage("Ready");
    resize(900, 600);
    setWindowTitle("IDE");
}
