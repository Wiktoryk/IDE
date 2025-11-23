#pragma once
#include <QString>
#include <QtGlobal>
#include <vector>

class TextSnapshot {
public:
    TextSnapshot() = default;
    TextSnapshot(QString text, std::vector<qsizetype> lineStarts, qsizetype version = 0) : m_text(std::move(text)), m_lineStarts(std::move(lineStarts)), m_version(version) {
        if (m_lineStarts.empty() || m_lineStarts.front() != 0) m_lineStarts.insert(m_lineStarts.begin(), 0);
        if (!m_text.isEmpty() && m_lineStarts.back() > m_text.size()) m_lineStarts.back() = m_text.size();
    }
    const QString& text() const { return m_text; }
    qsizetype size() const { return m_text.size(); }
    qsizetype version() const { return m_version; }
    qsizetype lineCount() const { return static_cast<qsizetype>(m_lineStarts.size()); }
    qsizetype lineStart(qsizetype line) const {
        if (m_lineStarts.empty()) return 0;
        if (line < 0) line = 0;
        if (line >=static_cast<qsizetype>(m_lineStarts.size())) line =  static_cast<qsizetype>(m_lineStarts.size()) - 1;
        return m_lineStarts[static_cast<std::size_t>(line)];
    }
    qsizetype positionFromLineCol(qsizetype line, qsizetype col) const {
        const qsizetype start = lineStart(line);
        const qsizetype end = (line + 1 < lineCount()) ? lineStart(line + 1) : size();
        qsizetype pos = start + col;
        if (pos < start) pos = start;
        if (pos > end) pos = end;
        return pos;
    }
    QString slice(qsizetype pos, qsizetype len) const {
        if (pos < 0) pos = 0;
        if (pos > size()) pos = size();
        if (len < 0) len = 0;
        if (pos + len > size()) len = size() - pos;
        return m_text.mid(pos, len);
    }
private:
    QString m_text;
    std::vector<qsizetype> m_lineStarts;
    qsizetype m_version = 0; 
};