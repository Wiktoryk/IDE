struct SearchResult {
    int start;
    int length;
};

class DocumentSearcher {
	QString m_text;
public:
	void setText(const QString& text) {
		m_text = text;
	}
	QVector<SearchResult> findAll(const QString& search, Qt::CaseSensitivity cs) {
		QVector<SearchResult> results;
		if (search.isEmpty()) return results;
		int pos = 0;
		while ((pos = m_text.indexOf(search, pos , cs)) != -1) {
			results.push_back({pos, search.length()});
			pos += search.length();
		}
		return results;
	}
};