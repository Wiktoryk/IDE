#include "searchbar.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

SearchBar::SearchBar(QWidget* parent) : QWidget(parent) {
	auto* layout = new QHBoxLayout(this);
	layout->setContentsMargins(4,4,4,4);
	m_input = new QLineEdit(this);
	auto* nextButton = new QPushButton("Next", this);
	auto* prevButton = new QPushButton("Prev", this);
	layout->addWidget(new QLabel("Find:"));
	layout->addWidget(m_input);
	layout->addWidget(prevButton);
	layout->addWidget(nextButton);

	connect(m_input, &QLineEdit::textChanged, this, &SearchBar::searchChanged);
	connect(nextButton, &QPushButton::clicked, this, &SearchBar::next);
	connect(prevButton, &QPushButton::clicked, this, &SearchBar::previous);
}