#include "gapBuffer.h"
#include <QVector>
#include <algorithm>
#include <cassert>

static inline bool isNewLine (QChar c) {
	return c == u'\n';
}

GapBuffer::GapBuffer() {
	m_buf.resize(256);
	m_gapBegin = 0;
	m_gapEnd = qsizetype(m_buf.size());
	m_lines .clear();
	m_lines.push_back(0);
}

GapBuffer::GapBuffer(QStringView initial) : GapBuffer() {
	insert(0, initial);
}

void GapBuffer::clear() {
	m_buf.assign(256, QChar());
	m_gapBegin = 0;
	m_gapEnd = qsizetype(m_buf.size());
	m_lines.clear();
	m_lines.push_back(0);
	m_version = 0;
}

qsizetype GapBuffer::size() const {
	return qsizetype(m_buf.size()) - (m_gapEnd - m_gapBegin);
}

qsizetype GapBuffer::logicalToPhysical(qsizetype pos) const {
	assert(pos >= 0 && pos <= size());
	const qsizetype gapSize = m_gapEnd - m_gapBegin;
	return (pos < m_gapBegin) ? pos : pos + gapSize;
}

void GapBuffer::growGap(qsizetype minExtra) {
    const qsizetype oldGap = m_gapEnd - m_gapBegin;
    qsizetype need = std::max<qsizetype>(minExtra, oldGap ? oldGap : 32);
    qsizetype newCap = qsizetype(m_buf.size()) + need;

    std::vector<QChar> nb;
    nb.resize(newCap);

    std::copy(m_buf.begin(), m_buf.begin() + m_gapBegin, nb.begin());
    qsizetype tailCount = qsizetype(m_buf.size()) - m_gapEnd;
    std::copy(m_buf.begin() + m_gapEnd, m_buf.end(), nb.begin() + newCap - tailCount);

    m_gapEnd = newCap - tailCount;
    m_buf.swap(nb);
}

void GapBuffer::moveGapTo(qsizetype at) {
    if (at == m_gapBegin) return;
    if (at < m_gapBegin) {
        qsizetype delta = m_gapBegin - at;
        std::move_backward(m_buf.begin() + at, m_buf.begin() + m_gapBegin, m_buf.begin() + m_gapEnd);
        m_gapBegin -= delta;
        m_gapEnd   -= delta;
    } else {
        qsizetype delta = at - m_gapBegin;
        std::move(m_buf.begin() + m_gapEnd, m_buf.begin() + m_gapEnd + delta, m_buf.begin() + m_gapBegin);
        m_gapBegin += delta;
        m_gapEnd   += delta;
    }
}

void GapBuffer::ensureGap(qsizetype at, qsizetype minExtra) {
    moveGapTo(at);
    if (m_gapEnd - m_gapBegin < minExtra) {
        growGap(minExtra - (m_gapEnd - m_gapBegin));
        moveGapTo(at);
    }
}

void GapBuffer::insert(qsizetype pos, QStringView stringview) {
    assert(pos >= 0 && pos <= size());
    if (stringview.isEmpty()) return;

    ensureGap(pos, stringview.size());
    std::copy(stringview.begin(), stringview.end(), m_buf.begin() + m_gapBegin);
    m_gapBegin += stringview.size();

    updateLinesForInsert(pos, stringview);
	++m_version;
}

void GapBuffer::collectRemoved(qsizetype pos, qsizetype len, QString& out) const {
    if (len <= 0) { out.clear(); return; }
    out.reserve(len);
    const qsizetype p0 = logicalToPhysical(pos);
    if (p0 + len <= m_gapBegin) {
        out = QString(readRange(p0, p0 + len));
        return;
    }
    QString left = readRange(p0, m_gapBegin);
    qsizetype leftLen = left.size();
    qsizetype rightNeed = len - leftLen;
    const qsizetype afterGap = m_gapEnd + (p0 + len - m_gapBegin);
    QString right = readRange(m_gapEnd, afterGap);
    out = left + right.left(rightNeed);
}

void GapBuffer::erase(qsizetype pos, qsizetype len) {
    assert(pos >= 0 && pos + len <= size());
    if (len <= 0) return;

    QString removed;
    collectRemoved(pos, len, removed);
    moveGapTo(pos);
    m_gapEnd += len;

    updateLinesForErase(pos, len, removed);
	++m_version;
}

QString GapBuffer::readRange(qsizetype physStart, qsizetype physEnd) const {
    if (physStart >= physEnd) return {};
    QString out;
    out.resize(physEnd - physStart);
    for (qsizetype i = 0; i < out.size(); ++i) {
        out[i] = m_buf[physStart + i];
    }
    return out;
}


QString GapBuffer::slice(qsizetype pos, qsizetype len) const {
    assert(pos >= 0 && pos + len <= size());
    if (len <= 0) return {};
    const qsizetype p0 = logicalToPhysical(pos);
    if (p0 + len <= m_gapBegin) {
        return readRange(p0, p0 + len);
    }
    if (p0 >= m_gapEnd) {
        return readRange(p0, p0 + len);
    }
    QString left = readRange(p0, m_gapBegin);
    qsizetype leftLen = left.size();
    qsizetype rightNeed = len - leftLen;
    QString right = readRange(m_gapEnd, m_gapEnd + rightNeed);
    return left + right;
}


QString GapBuffer::toString() const {
    QString left  = readRange(0, m_gapBegin);
    QString right = readRange(m_gapEnd, qsizetype(m_buf.size()));
    return left + right;
}

void GapBuffer::rebuildLineIndex() {
    m_lines.clear();
    m_lines.push_back(0);
    const qsizetype N = size();
    auto addChunk = [this](qsizetype p0, qsizetype p1) {
        for (qsizetype p = p0; p < p1; ++p) {
            if (isNewLine(m_buf[p])) {
                qsizetype gapOff = (p < m_gapBegin) ? 0 : (m_gapEnd - m_gapBegin);
                m_lines.push_back(p - gapOff + 1);
            }
        }
    };
    addChunk(0, m_gapBegin);
    addChunk(m_gapEnd, qsizetype(m_buf.size()));
    if (m_lines.empty() || m_lines[0] != 0) m_lines.insert(m_lines.begin(), 0);
    if (m_lines.back() > N) m_lines.back() = N;
}

void GapBuffer::updateLinesForInsert(qsizetype at, QStringView stringview) {
    if (m_lines.size() == 1 && m_lines[0] == 0 && size() == stringview.size()) {
        rebuildLineIndex();
        return;
    }

    auto it = std::upper_bound(m_lines.begin(), m_lines.end(), at);
    qsizetype lineIdx = qsizetype(std::distance(m_lines.begin(), it));
    qsizetype delta = stringview.size();

    for (qsizetype i = lineIdx; i < static_cast<qsizetype>(m_lines.size()); ++i) {
        m_lines[i] += delta;
    }

    for (qsizetype i = 0; i < stringview.size(); ++i) {
        if (isNewLine(stringview[i])) {
            m_lines.insert(m_lines.begin() + lineIdx + 1, at + i + 1);
            ++lineIdx;
        }
    }
}

void GapBuffer::updateLinesForErase(qsizetype at, qsizetype len, QStringView removed) {
    const qsizetype end = at + len;

    auto newEnd = std::remove_if(m_lines.begin() + 1, m_lines.end(), [&](qsizetype start){
        return start >= at && start < end;
    });
    m_lines.erase(newEnd, m_lines.end());

    for (qsizetype& start : m_lines) {
        if (start >= end) start -= len;
    }
    if (m_lines.empty() || m_lines.front() != 0) {
		m_lines.insert(m_lines.begin(), 0);
	}
    if (!m_lines.empty() && m_lines.back() > size()) {
		m_lines.back() = size();
	}
}

qsizetype GapBuffer::lineCount() const {
    return qsizetype(m_lines.size());
}

qsizetype GapBuffer::lineStart(qsizetype line) const {
    if (line < 0) {
		line = 0;
	}
    if (line >= static_cast<qsizetype>(m_lines.size())) {
		line = qsizetype(m_lines.size()) - 1;
	}
    return m_lines[line];
}

qsizetype GapBuffer::positionFromLineCol(qsizetype line, qsizetype col) const {
    const qsizetype start = lineStart(line);
    const qsizetype end   = (line + 1 < lineCount()) ? lineStart(line + 1) : size();
    return std::clamp<qsizetype>(start + col, start, end);
}

TextSnapshot GapBuffer::snapshot() const {
	QString txt = toString();
	std::vector<qsizetype> starts = m_lines;
	return TextSnapshot(std::move(txt), std::move(starts), m_version);
}