#include "editorwidget.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>

EditorWidget::EditorWidget(QWidget* parent) : QPlainTextEdit(parent) {
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &EditorWidget::onCursorChanged);
    connect(document(), &QTextDocument::modificationChanged, this, &EditorWidget::onDocChanged);
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
    return true;
}

bool EditorWidget::saveToFile(const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
	    *error = file.errorString();
	}
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << toPlainText();
    out.flush();
    if (file.error() != QFile::NoError) {
        if (error) {
	    *error = file.errorString();
	}
        return false;
    }
    document()->setModified(false);
    m_dirty = false;
    setFilePath(path);
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