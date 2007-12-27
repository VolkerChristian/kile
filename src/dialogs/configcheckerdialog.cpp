/***************************************************************************
    begin                : Fri Jun 4 2004
    copyright            : (C) 2004 by Jeroen Wijnout
    email                : Jeroen.Wijnhout@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dialogs/configcheckerdialog.h"

#include <qfileinfo.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include <klocale.h>
#include <kcursor.h>
#include <kprogressdialog.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <k3listbox.h>
#include "kiledebug.h"

#include "widgets/configcheckerwidget.h"

namespace KileDialog
{

ResultItem::ResultItem(K3ListBox *lb, const QString &tool, int status, const QList<ConfigTest> &tests) : Q3ListBoxItem(lb)
{
	QString rt = "<hr><b><font color=\"%1\">%2</font></b> (%3)<br><ul>";
	for (int i = 0; i < tests.count(); ++i) {
		QString itemcolor = "black";
		if ( tests[i].status() == ConfigTest::Failure ) itemcolor = "#FFA201";
		else if ( tests[i].status() == ConfigTest::Critical ) itemcolor = "#AA0000";
		rt += QString("<li><b><font color=\"%1\">%2</font></b>: &nbsp;%3</li>").arg(itemcolor).arg(tests[i].name()).arg(tests[i].resultText());
	}
	rt += "</ul>";

	QString color = "#00AA00", statustr = i18n("Passed");
	if ( status == ConfigTest::Failure )
	{
		color = "#FFA201";
		statustr = i18n("Failed, but not critical");
	}
	else if ( status == ConfigTest::Critical )
	{
		color = "#AA0000";
		statustr = i18n("Critical failure, Kile will not function properly");
	}

	m_richText = new Q3SimpleRichText(rt.arg(color).arg(tool).arg(statustr), listBox()->font());
	m_richText->setWidth(listBox()->width());

	//this is for sorting only
	setText(QString::number(status) + ':' + tool);
}

void ResultItem::paint(QPainter *p)
{
	m_richText->draw(p, 0, 0, QRect(), listBox()->colorGroup());
}

ConfigChecker::ConfigChecker(QWidget* parent) :
	KDialog(parent), m_tester(0L)
{
	setCaption(i18n("System Check"));
	setModal(true);
	setButtons(Ok | Cancel | User1);
	setDefaultButton(Ok);
	showButtonSeparator(true);
	setButtonGuiItem(User1, KGuiItem("&Save Results..."));

	m_widget = new ConfigCheckerWidget(this);
	setMainWidget(m_widget);

	enableButton(Ok, false);
	enableButton(User1, false);

	run();
}

ConfigChecker::~ConfigChecker()
{
}

QProgressBar* ConfigChecker::progressBar()
{
	return m_widget->progressBar();
}

QLabel* ConfigChecker::label()
{
	return m_widget->label();
}

K3ListBox* ConfigChecker::listBox()
{
	return m_widget->listBox();
}

void ConfigChecker::run()
{
	m_tester = new Tester(this);

	connect(m_tester, SIGNAL(started()), this, SLOT(started()));
	connect(m_tester, SIGNAL(percentageDone(int)), this, SLOT(setPercentageDone(int)));
	connect(m_tester, SIGNAL(finished(bool)), this, SLOT(finished(bool)));
	connect(this, SIGNAL(user1Clicked()), this, SLOT(saveResults()));

	m_tester->runTests();
}

void ConfigChecker::slotCancel()
{
	if (m_tester) m_tester->stop();
	finished(false);
	reject();
}

void ConfigChecker::saveResults()
{
	KUrl url = KFileDialog::getSaveUrl();
	if ( !url.isEmpty() ) m_tester->saveResults(url);
}

void ConfigChecker::started()
{
	setCursor(Qt::BusyCursor);
	setPercentageDone(0);
}

void ConfigChecker::finished(bool ok)
{
	setCursor(Qt::ArrowCursor);
	enableButton(Cancel, false);

	if (ok)
	{
		label()->setText(i18n("Finished testing your system..."));

		QStringList tools = m_tester->testedTools();
		QStringList critical, failure;
		for(int i = 0; i < tools.count(); ++i) {
			int status = m_tester->statusForTool(tools[i]);
			if ( status == ConfigTest::Critical ) critical.append(tools[i]);
			else if ( status == ConfigTest::Failure ) failure.append(tools[i]);
			new ResultItem(listBox(), tools[i], status, m_tester->resultForTool(tools[i]));
		}

		listBox()->sort();

		QString cap = i18n("Test Results");
		if ( critical.count() > 0 )
			KMessageBox::error(this, i18n("<qt>The following tools did not pass all <b>critical</b> tests:<br>%1<br>Your system is not ready to use. Please consult the results to find out what to fix.</qt>").arg(critical.join(", ")), cap);
		else if ( failure.count() > 0 )
			KMessageBox::information(this, i18n("The following tools did not pass all tests:\n %1\nYou will still be able to use Kile; however, not all features are guaranteed to work.").arg(failure.join(", ")), cap);
		else
			KMessageBox::information(this, i18n("No problems detected, your system is ready to use."), cap);
	}
	else
		label()->setText(i18n("Tests finished abruptly..."));

	enableButton(Ok, true);
	enableButton(User1, true);
}

void ConfigChecker::setPercentageDone(int p)
{
	progressBar()->setValue(p);
}

}

#include "configcheckerdialog.moc"