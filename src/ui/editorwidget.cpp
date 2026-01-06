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

    connect(this, &QPlainTextEdit::textChanged, this, &EditorWidget::syncModelFromWidget);
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
	m_model.setText(toPlainText());
	//m_undo.clear();
    document()->setModified(false);
    m_dirty = false;
    setFilePath(path);
	startWatching(path);
    syncModelFromWidget();
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
    syncModelFromWidget();
    return true;
}

void EditorWidget::keyPressEvent(QKeyEvent* e) {
	/*if (e->matches(QKeySequence::Undo)) {
		doUndo();
		return;
	}
    if (e->matches(QKeySequence::Redo)) {
		doRedo();
		return;
	}
	QTextCursor cursor = textCursor();
    const qsizetype pos = cursor.position();
    const qsizetype selectionLen = std::abs(cursor.selectionEnd() - cursor.selectionStart());
    const qsizetype selectionStart = std::min(cursor.selectionStart(), cursor.selectionEnd());
	const QString text = e->text();
    const bool printable = !text.isEmpty() && !text.at(0).isNull() && text.at(0).isPrint();

    if (printable || e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter || e->key() == Qt::Key_Tab) {
        QString ins = text;
        if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) ins = "\n";
        if (e->key() == Qt::Key_Tab) ins = "    ";
        if (selectionLen > 0) {
            applyEraseAt(selectionStart, selectionLen);
            applyInsertAt(selectionStart, ins);
        } else {
            applyInsertAt(pos, ins);
        }
        return;
    }
	if (e->key() == Qt::Key_Backspace) {
        if (selectionLen > 0) {
            applyEraseAt(selectionStart, selectionLen);
        } else if (pos > 0) {
            applyEraseAt(pos - 1, 1);
        }
        return;
    }
	if (e->key() == Qt::Key_Delete) {
        if (selectionLen > 0) {
            applyEraseAt(selectionStart, selectionLen);
        } else {
            if (pos < m_model.size())
                applyEraseAt(pos, 1);
        }
        return;
    }*/
	QPlainTextEdit::keyPressEvent(e);
    syncModelFromWidget();
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

void EditorWidget::syncFromModel(qsizetype newCursorPos) {
	const QString all = m_model.toString();
	const bool blocked = blockSignals(true);
	setPlainText(all);
	blockSignals(blocked);

	QTextCursor cursor = textCursor();
	cursor.setPosition(static_cast<int>(newCursorPos));
	setTextCursor(cursor);
	ensureCursorVisible();
}

void EditorWidget::applyInsertAt(qsizetype pos, const QString& text) {
	if (text.isEmpty()) return;
	m_model.insert(pos, text);
	Edit edit{Edit::Insert, pos, text, pos + text.size()};
	//m_undo.push(edit);
	syncFromModel(edit.cursorAfter);
	document()->setModified(true);
}

void EditorWidget::applyEraseAt(qsizetype pos, qsizetype len) {
	if (len <= 0) return;
	const QString removed = m_model.slice(pos, len);
	m_model.erase(pos, len);
	Edit edit{Edit::Erase, pos, removed, pos};
	//m_undo.push(edit);
	syncFromModel(edit.cursorAfter);
	document()->setModified(true);
}

void EditorWidget::doUndo() {
	/*if (!m_undo.canUndo()) return;
	qsizetype caret = m_undo.undo(m_model);
	syncFromModel(caret);*/
	QPlainTextEdit::undo();
    syncModelFromWidget();
}

void EditorWidget::doRedo() {
	/*if (!m_undo.canRedo()) return;
	qsizetype caret = m_undo.redo(m_model);
	syncFromModel(caret);*/
	QPlainTextEdit::redo();
    syncModelFromWidget();
}

void EditorWidget::syncModelFromWidget() {
    m_model.setText(toPlainText());
}

void EditorWidget::setSearchResults(const QVector<SearchResult>& results) {
	m_results = results;
	QList<QTextEdit::ExtraSelection> selections;
	QTextCharFormat fmt;
	fmt.setBackground(QColor(255, 230, 150));

	for (const auto& result : results) {
		QTextCursor cursor(document());
		cursor.setPosition(result.start);
		cursor.setPosition(result.start + result.length, QTextCursor::KeepAnchor);
		QTextEdit::ExtraSelection selection;
		selection.cursor = cursor;
		selection.format = fmt;
		selections.append(selection);
	}
	setExtraSelections(selections);
}

void EditorWidget::selectSearchResult(int index) {
	if (index < 0 || index >= m_results.size()) {
		return;
	}
	const auto& result = m_results[index];
	QTextCursor cursor(document());
	cursor.setPosition(result.start);
	cursor.setPosition(result.start + result.length, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
	centerCursor();
}