#pragma once
#include "textBuffer.h"
#include <QChar>
#include <QDebug>

class GapBuffer final : public ITextBuffer {
public:
	GapBuffer();
	explicit GapBuffer(QStringView initial);

	void clear() override;
	qsizetype size() const override;
	void insert(qsizetype pos, QStringview stringview) override;
	void erase(qsizetype pos, qsizetype len) override;
	QString slice(qsizetype pos, qsizetype len) const override;
	QString toString() const override;

	qsizetype lineCount() const override;
	qsizetype lineStart(qsizetype line) const override;
	qsizetype positionFromLineCol(qsizetype line, qsizetype col) const override;

	void setText(QStringView stringview) {
		clear();
		insert(0, stringview);
	}
private:
	std::vector<QChar> m_buf;
	qsizetype m_gapBegin = 0;
	qsizetype m_gapEnd = 0;
	std::vector<qsizetype> m_lines;

	qsizetype logicalToPhysical(qsizetype pos) const ;
	void ensureGap(qsizetype at, qsizetype minExtra);
	void moveGapTo(qsizetype at);
	void growGap(qsizetype minExtra);

	void rebuildLineIndex();
	void updateLinesForInsert(qsizetype at, QStringView stringview);
	void updateLinesForErase(qsizetype at, qsizetype len, QStringView removed);
	QString readRange(qsizetype physStart, qsizetype physEnd) const;
	void collectRemoved(qsizetype pos, qsizetype len, QString& out) const;
};