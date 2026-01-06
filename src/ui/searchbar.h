#pragma once
#include <QWidget>
#include <QLineEdit>
class SearchBar : public QWidget {
	Q_OBJECT
	QLineEdit* m_input;
public:
	explicit SearchBar(QWidget* parent = nullptr);
signals:
	void searchChanged(const QString&);
	void next();
	void previous();
};