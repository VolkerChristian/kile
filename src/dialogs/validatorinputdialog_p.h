/*
 *  Copyright 2003  Nadeem Hasan <nhasan@kde.org>
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */


#ifndef KILEVALIDATORINPUTDIALOG_P_H
#define KILEVALIDATORINPUTDIALOG_P_H

#include <QDialog>

class QDialogButtonBox;
class QLineEdit;
class QString;
class QWidget;
class QValidator;

namespace KileDialog {
class ValidatorInputDialogHelper : public QDialog
{
	Q_OBJECT

public:
	ValidatorInputDialogHelper(const QString &caption, const QString &label,
		const QString &value, QWidget *parent,
		QValidator *validator, const QString &mask);

	QLineEdit * lineEdit() const;

public Q_SLOTS:
	void slotEditTextChanged(const QString &value);

private:
	QLineEdit *m_lineEdit;
	QDialogButtonBox *m_buttonBox;
};
}

#endif
