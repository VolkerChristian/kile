/**************************************************************************
*   Copyright (C) 2007 by Michel Ludwig (michel.ludwig@kdemail.net)       *
***************************************************************************/

/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef LIBRARYTOOLCONFIGWIDGET_H
#define LIBRARYTOOLCONFIGWIDGET_H

#include <QWidget>

#include "ui_librarytoolconfigwidget.h"

class LibraryToolConfigWidget : public QWidget, public Ui::LibraryToolConfigWidget
{
	Q_OBJECT

	public:
		LibraryToolConfigWidget(QWidget *parent = 0);
		~LibraryToolConfigWidget();
};

#endif