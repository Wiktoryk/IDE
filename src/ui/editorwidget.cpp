#include "editorwidget.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>

EditorWidget::EditorWidget(QWidget* parent) : QPlainTextEdit(parent) {
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &EditorWidget::onCursorChanged);
    connect(document(), &QTextDocument::modificationChanged, this, &EditorWidget::onDocChanged);

	m_watcher = new QFileSystemWatcher(this);
	connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &EditorWidget::onFileChanged);

	m_watchReset = new QTimer(this);
    m_watchReset->setSingleShot(true);
    m_watchReset->setInterval(500);
    connect(m_watchReset, &QTimer::timeout, this, &EditorWidget::delayedWatchReset);
}

void EditorWidget::updateWindowTitle() {
    QString name = m_path.isEmpty() ? QStringLiteral("Untitled") : QFileInfo(m_path).fileName();
    if (m_dirty) {
		name.append('*');
    }
    if (auto w = window()) {
		w->setWindowTitle(name);
    }
}

bool EditorWidget::loadFromFile(const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) {
	    	*error = file.errorString();
		}
        return false;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    setPlainText(in.readAll());
    document()->setModified(false);
    m_dirty = false;
    setFilePath(path);
	startWatching(path);
    return true;
}

bool EditorWidget::saveToFile(const QString& path, QString* error) {
	m_saving = true;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
	    	*error = file.errorString();
		}
		m_saving = false;
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << toPlainText();
    out.flush();
	file.close();
	m_saving = false;
    if (file.error() != QFile::NoError) {
        if (error) {
	    	*error = file.errorString();
		}
        return false;
    }
    document()->setModified(false);
    m_dirty = false;
    setFilePath(path);
	startWatching(path);
    return true;
}

void EditorWidget::keyPressEvent(QKeyEvent* e) {
    QPlainTextEdit::keyPressEvent(e);
}

void EditorWidget::onCursorChanged() {
    auto cursor = textCursor();
    emit cursorPosChanged(cursor.blockNumber()+1, cursor.positionInBlock()+1);
}

void EditorWidget::onDocChanged() {
    bool isChanged = document()->isModified();
    if (isChanged != m_dirty) {
        m_dirty = isChanged;
        updateWindowTitle();
        emit dirtyChanged(m_dirty);
    }
}

void EditorWidget::startWatching(const QString& path) {
    stopWatching();
    if (!path.isEmpty()) {
        m_watcher->addPath(path);
	}
}


void EditorWidget::stopWatching()
{
    if (!m_watcher->files().isEmpty()) {
        m_watcher->removePaths(m_watcher->files());
	}
}

void EditorWidget::onFileChanged(const QString& path) {
	if (m_saving || m_reloading) {
		return;
	}
	m_watchReset->start();

    QFileInfo info(path);
    if (!info.exists()) {
        return;
    }

	if (!document()->isModified()) {
        m_reloading = true;
        QString err;
        if (!loadFromFile(path, &err)) {
            QMessageBox::warning(this, "Reload failed", err);
        }
		m_reloading = false;
        return;
    }

    auto ret = QMessageBox::question(this, "File changed",
        QString("The file \"%1\" has changed on disk.\nReload it?")
        .arg(QFileInfo(path).fileName()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

    if (ret == QMessageBox::Yes) {
		m_reloading = true;
        QString err;
        if (!loadFromFile(path, &err)) {
            QMessageBox::warning(this, "Reload failed", err);
			m_reloading = false;
        }
    } else {
        startWatching(path);
    }
}


void EditorWidget::delayedWatchReset()
{
    if (!m_path.isEmpty()) {
        startWatching(m_path);
	}
}