#include <QMainWindow>
#include <QApplication>
#include <QTextEdit>
#include <QStatusBar>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        auto *editor = new QTextEdit;
        setCentralWidget(editor);
        statusBar()->showMessage("Ready");
        resize(900, 600);
        setWindowTitle("IDE");
    }
};

#include "mainwindow.moc"
