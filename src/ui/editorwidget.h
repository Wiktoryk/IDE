#pragma once
#include <QPlainTextEdit>
#include <QFileSystemWatcher>
#include <QTimer>

class EditorWidget : public QPlainTextEdit {
    Q_OBJECT
private:
    void updateWindowTitle();
	void startWatching(const QString& path);
	void stopWatching();
    void reloadIfExternalChange();

    QString m_path;
    bool m_dirty = false;
	QFileSystemWatcher* m_watcher = nullptr;
	bool m_saving = false;
    bool m_reloading = false;
    QTimer* m_watchReset = nullptr;

public:
    explicit EditorWidget(QWidget* parent=nullptr);

    bool loadFromFile(const QString& path, QString* error=nullptr);
    bool saveToFile(const QString& path, QString* error=nullptr);
    QString filePath() const { return m_path; }
    void setFilePath(const QString& p) { m_path = p; updateWindowTitle(); }

signals:
    void cursorPosChanged(int line, int col);
    void dirtyChanged(bool isDirty);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private slots:
    void onCursorChanged();
    void onDocChanged();
	void onFileChanged(const QString& path);
	void delayedWatchReset();
};