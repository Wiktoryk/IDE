#pragma once
#include <QString>
#include <QStringView>
#include <vector>
#include "textSnapshot.h"

class ITextBuffer {
public:
	virtual ~ITextBuffer() = default;

	virtual void clear() = 0;
	virtual qsizetype size() const = 0;
	virtual void insert(qsizetype pos, QStringView stringview) = 0;
	virtual void erase(qsizetype pos, qsizetype len) = 0;

	virtual QString slice(qsizetype pos, qsizetype len) const = 0;
	virtual QString toString() const = 0;
	virtual qsizetype lineCount() const = 0;
	virtual qsizetype lineStart(qsizetype line) const = 0;
	virtual qsizetype positionFromLineCol(qsizetype line, qsizetype col) const = 0;

	virtual TextSnapshot snapshot() const = 0;
	virtual void beginEdit() {}
	virtual void endEdit() {}
};