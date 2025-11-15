#pragma once
#include <QString>
#include <vector>
#include <chrono>

class ITextBuffer;

struct Edit {
	enum Type { Insert, Erase } type;
	qsizetype pos = 0;
	QString text;
	qsizetype cursorAfter = 0;
};

class UndoStack {
public:
	void clear();
	void push(const Edit& edit);

	bool canUndo() const { return !m_done.empty(); }
	bool canRedo() const { return !m_redo.empty(); }
	qsizetype undo(ITextBuffer& buf);
	qsizetype redo(ITextBuffer& buf);

	void enableCoalescing(bool on) { m_coalesce = on; }
private:
	bool tryCoalesce(const Edit& edit);

	std::vector<Edit> m_done;
	std::vector<Edit> m_redo;
	bool m_coalesce = true;
	std::chrono::steady_clock::time_point m_lastTime{};
};