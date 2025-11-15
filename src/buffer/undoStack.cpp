#include "undoStack.h"
#include "textBuffer.h"
#include <algorithm>

void UndoStack::clear() {
	m_done.clear();
	m_redo.clear();
}

bool UndoStack::tryCoalesce(const Edit& edit) {
	if (!m_coalesce || m_done.empty()) return false;
	auto now = std::chrono::steady_clock::now();
	if (m_lastTime.time_since_epoch().count() == 0) {
		m_lastTime = now;
	}
	bool timeOk = (now - m_lastTime) < std::chrono::milliseconds(600);
	if (!timeOk) return false;
	m_lastTime = now;

	Edit& last = m_done.back();
	if (last.type == Edit::Insert && edit.type == Edit::Insert) {
		if (edit.pos == last.pos +last.text.size()) {
			last.text +=edit.text;
			last.cursorAfter = edit.cursorAfter;
			return true;
		}
	}
	if (last.type == Edit::Erase && edit.type == Edit::Erase) {
        if (edit.pos + edit.text.size() == last.pos) {
            last.pos = edit.pos;
            last.text = edit.text + last.text;
            last.cursorAfter = edit.cursorAfter;
            return true;
        }
    }
    return false;
}

void UndoStack::push(const Edit& edit) {
	if (!tryCoalesce(edit)) {
		m_done.push_back(edit);
	}
	m_redo.clear();
}

static inline void apply(ITextBuffer& buf, const Edit& edit) {
	if (edit.type == Edit::Insert) {
		buf.insert(edit.pos, edit.text);
	} else {
		buf.erase(edit.pos, edit.text.size());
	}
}

static inline Edit inverseOf(const Edit& edit) {
    Edit inv;
    if (edit.type == Edit::Insert) {
        inv.type = Edit::Erase;
        inv.pos = edit.pos;
        inv.text = edit.text;
        inv.cursorAfter = edit.pos;
    } else {
        inv.type = Edit::Insert;
        inv.pos = edit.pos;
        inv.text = edit.text;
        inv.cursorAfter = edit.pos + edit.text.size();
    }
    return inv;
}

qsizetype UndoStack::undo(ITextBuffer& buf) {
    if (m_done.empty()) return 0;
    Edit edit = m_done.back();
    m_done.pop_back();
    Edit inv = inverseOf(edit);
    apply(buf, inv);
    m_redo.push_back(edit);
    return inv.cursorAfter;
}

qsizetype UndoStack::redo(ITextBuffer& buf) {
    if (m_redo.empty()) return 0;
    Edit edit = m_redo.back();
    m_redo.pop_back();
    apply(buf, edit);
    m_done.push_back(edit);
    return edit.cursorAfter;
}