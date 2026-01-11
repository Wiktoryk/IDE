#pragma once
#include <QWidget>
class QLineEdit;
class QPushButton;

class SearchBar : public QWidget {
	Q_OBJECT
	QLineEdit* m_input;
	QPushButton* m_next;
	QPushButton* m_prev;
	QPushButton* m_close;
public:
	explicit SearchBar(QWidget* parent = nullptr);
signals:
	void searchChanged(const QString& text);
	void next();
	void previous();
protected:
	void keyPressEvent(QKeyEvent* event) override;
};