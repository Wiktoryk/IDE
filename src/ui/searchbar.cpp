#include "searchbar.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QKeyEvent>

SearchBar::SearchBar(QWidget* parent) : QWidget(parent) {
	setWindowFlags(Qt::Tool);
	setAttribute(Qt::WA_DeleteOnClose, false);

	m_input = new QLineEdit(this);
	m_next = new QPushButton("v", this);
	m_prev = new QPushButton("^", this);
	m_close = new QPushButton("X", this);

	auto* layout = new QHBoxLayout(this);
	layout->setContentsMargins(8,8,8,8);
	layout->addWidget(m_input);
	layout->addWidget(m_prev);
	layout->addWidget(m_next);
	layout->addWidget(m_close);

	connect(m_input, &QLineEdit::textChanged, this, &SearchBar::searchChanged);
	connect(m_next, &QPushButton::clicked, this, &SearchBar::next);
	connect(m_prev, &QPushButton::clicked, this, &SearchBar::previous);
	connect(m_close, &QPushButton::clicked, this,[this]() {
		hide();
		emit searchClosed();
	});
	connect(m_input, &QLineEdit::returnPressed, this, &SearchBar::next);
}

void SearchBar::keyPressEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key_Escape) {
		m_input->clear();
		hide();
		emit searchClosed();
		event->accept();
		return;
	}
	QWidget::keyPressEvent(event);
}

void SearchBar::setSearchText(const QString& text) {
	m_input->setText(text);
	m_input->selectAll();
	m_input->setFocus();
	emit searchChanged(text);
}