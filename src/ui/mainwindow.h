#pragma once
#include <QMainWindow>
class EditorWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    bool maybeSave();
    bool doSaveAs(QString* outPath=nullptr);
    void addToRecent(const QString& path);
    void rebuildRecentMenu();

    EditorWidget* m_editor = nullptr;
    QStringList m_recent;
    QMenu* m_recentMenu = nullptr;

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* ev) override;

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void openRecent();

    void updateStatusLineCol(int line,int col);
    void updateWindowModified(bool dirty);
};