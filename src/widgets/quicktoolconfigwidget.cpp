/********************************************************************************
*   Copyright (C) 2007, 2008 by Michel Ludwig (michel.ludwig@kdemail.net)       *
*********************************************************************************/

/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "widgets/quicktoolconfigwidget.h"

#include "kiletoolmanager.h"

QuickToolConfigWidget::QuickToolConfigWidget(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	m_notSpecifiedString = i18n("Not Specified");
	connect(m_pshbAdd, SIGNAL(clicked()), this, SLOT(add()));
	connect(m_pshbRemove, SIGNAL(clicked()), this, SLOT(remove()));
	connect(m_pshbUp, SIGNAL(clicked()), this, SLOT(up()));
	connect(m_pshbDown, SIGNAL(clicked()), this, SLOT(down()));
}

QuickToolConfigWidget::~QuickToolConfigWidget()
{
}

void QuickToolConfigWidget::updateSequence(const QString &sequence)
{
	QStringList toollist = KileTool::toolList(KGlobal::config().data(), true);
	toollist.sort();
	m_cbTools->clear();
	m_cbTools->addItems(toollist);

	updateConfigs(m_cbTools->currentText());
	connect(m_cbTools, SIGNAL(activated(const QString&)), this, SLOT(updateConfigs(const QString&)));

	m_sequence = sequence;
	QStringList list = sequence.split(',', QString::SkipEmptyParts);
	QString tl, cfg;
	m_lstbSeq->clear();
	for(QStringList::iterator i = list.begin(); i != list.end(); ++i) {
		KileTool::extract(*i, tl, cfg);
		if(!cfg.isEmpty()) {
			m_lstbSeq->addItem(tl + " (" + cfg + ')');
		}
		else {
			m_lstbSeq->addItem(tl);
		}
	}
}

void QuickToolConfigWidget::updateConfigs(const QString &tool)
{
	m_cbConfigs->clear();
	m_cbConfigs->addItem(m_notSpecifiedString);
	m_cbConfigs->addItems(KileTool::configNames(tool, KGlobal::config().data()));
}

void QuickToolConfigWidget::down()
{
	QList<QListWidgetItem*> selectedItems = m_lstbSeq->selectedItems();
	if(selectedItems.isEmpty()) {
		return;
	}
	QListWidgetItem *selectedItem = selectedItems.first();
	int row = m_lstbSeq->row(selectedItem);
	if(row < m_lstbSeq->count() - 1) {
		QListWidgetItem *nextItem = m_lstbSeq->item(row + 1);
		QString text = selectedItem->text();
		selectedItem->setText(nextItem->text());
		nextItem->setText(text);
		nextItem->setSelected(true);
		changed();
	}
}

void QuickToolConfigWidget::up()
{
	QList<QListWidgetItem*> selectedItems = m_lstbSeq->selectedItems();
	if(selectedItems.isEmpty()) {
		return;
	}
	QListWidgetItem *selectedItem = selectedItems.first();
	int row = m_lstbSeq->row(selectedItem);
	if(row > 0) {
		QListWidgetItem *previousItem = m_lstbSeq->item(row - 1);
		QString text = selectedItem->text();
		selectedItem->setText(previousItem->text());
		previousItem->setText(text);
		previousItem->setSelected(true);
		changed();
	}
}

void QuickToolConfigWidget::remove()
{
	QList<QListWidgetItem*> selectedItems = m_lstbSeq->selectedItems();
	if(selectedItems.isEmpty()) {
		return;
	}
	QListWidgetItem *selectedItem = selectedItems.first();
	delete selectedItem;
}

void QuickToolConfigWidget::add()
{
	QString entry = m_cbTools->currentText();
	if(m_cbConfigs->currentText() != m_notSpecifiedString) {
		entry += " (" + m_cbConfigs->currentText() + ')';
	}
	if(!m_lstbSeq->findItems(entry, Qt::MatchExactly).isEmpty()) {
		return;
	}
	m_lstbSeq->addItem(entry);
	changed();
}


void QuickToolConfigWidget::changed()
{
	QString sequence, tool, cfg;
	for(int i = 0; i < m_lstbSeq->count(); ++i) {
	    KileTool::extract(m_lstbSeq->item(i)->text(), tool, cfg);
	    sequence += KileTool::format(tool, cfg) + ',';
	}
	if(sequence.endsWith(',')) {
		sequence = sequence.left(sequence.length()-1);
	}
	m_sequence = sequence;
	emit sequenceChanged(m_sequence);
}

#include "quicktoolconfigwidget.moc"
