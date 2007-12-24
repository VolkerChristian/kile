/***************************************************************************
    begin                : Wed Aug 14 2002
    copyright            : (C) 2003 by Jeroen Wijnhout
    email                :

from Kate (C) 2001 by Matt Newell

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// 2007-03-12 dani
//  - use KileDocument::Extensions

#include "kilefileselect.h"

#include <QAbstractItemView>
#include <qlayout.h>
#include <qlabel.h>
#include <q3strlist.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QFocusEvent>

#include <kactioncollection.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kprotocolinfo.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kcharsets.h>
#include "kiledebug.h"

#include "kileconfig.h"

KileFileSelect::KileFileSelect(KileDocument::Extensions *extensions, QWidget *parent, const char *name ) : QWidget(parent,name)
{
  Q3VBoxLayout* lo = new Q3VBoxLayout(this);

  KToolBar *toolbar = new KToolBar(this, "fileselectortoolbar");
  lo->addWidget(toolbar);

	cmbPath = new KUrlComboBox(KUrlComboBox::Directories, this);
	cmbPath->setEditable(true);
  cmbPath->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ));
  cmpl = new KUrlCompletion(KUrlCompletion::DirCompletion);
  cmbPath->setCompletionObject( cmpl );
  lo->addWidget(cmbPath);

	dir = new KDirOperator(KUrl(), this);
  connect(dir, SIGNAL(fileSelected(const KFileItem*)), this, SIGNAL(fileSelected(const KFileItem*)));
  dir->setView(KFile::Simple);
  dir->setMode(KFile::Files);

	// KileFileSelect filter for sidebar 
	QString filter =  extensions->latexDocuments() 
	                    + ' ' + extensions->latexPackages() 
	                    + ' ' + extensions->bibtex() 
	                    + ' ' +  extensions->metapost();
	filter.replace(".","*.");
	dir->setNameFilter(filter);

	KActionCollection *coll = dir->actionCollection();
#ifdef __GNUC__
#warning Action->setShortcut still needs to be ported!
#endif
//FIXME: port for KDE4
/*
	// some shortcuts of diroperator that clashes with Kate
	coll->action("delete")->setShortcut(KShortcut(Qt::ALT + Qt::Key_Delete));
	coll->action("reload")->setShortcut(KShortcut(Qt::ALT + Qt::Key_F5));
	coll->action("back")->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_Left));
	coll->action("forward")->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_Right));
	// some consistency - reset up for dir too
	coll->action("up")->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_Up));
	coll->action("home")->setShortcut(KShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Home));
*/
	toolbar->addAction(coll->action("home"));
	toolbar->addAction(coll->action("up"));
	toolbar->addAction(coll->action("back"));
	toolbar->addAction(coll->action("forward"));

#ifdef __GNUC__
#warning A file open action still needs to be added!
#endif
//FIXME: port for KDE4
// 	toolbar->insertButton("fileopen", 0, true , i18n( "Open selected" ));
  connect(toolbar, SIGNAL(clicked(int)), this, SLOT(clickedToolbar(int)));

  lo->addWidget(dir);
  lo->setStretchFactor(dir, 2);

  m_comboEncoding = new KComboBox( false, this );
  m_comboEncoding->setObjectName( "comboEncoding" );
  QStringList availableEncodingNames(KGlobal::charsets()->availableEncodingNames());
  m_comboEncoding->setEditable( true );
  m_comboEncoding->insertStringList( availableEncodingNames );
  QToolTip::add(m_comboEncoding, i18n("Set encoding"));
  lo->addWidget(m_comboEncoding);

  connect( cmbPath, SIGNAL( urlActivated( const KUrl&  )),this,  SLOT( cmbPathActivated( const KUrl& ) ));
  connect( cmbPath, SIGNAL( returnPressed( const QString&  )), this,  SLOT( cmbPathReturnPressed( const QString& ) ));
  connect(dir, SIGNAL(urlEntered(const KUrl&)), this, SLOT(dirUrlEntered(const KUrl&)) );
}

KileFileSelect::~KileFileSelect()
{
  delete cmpl;
}

void KileFileSelect::readConfig()
{
	QString lastDir = KileConfig::lastDir();
	QFileInfo ldi(lastDir);
	if ( ! ldi.isReadable() ) dir->home();
	else setDir(KUrl::fromPathOrUrl(lastDir));
}

void KileFileSelect::writeConfig()
{
	KileConfig::setLastDir(dir->url().path());
}

void KileFileSelect::setView(KFile::FileView view)
{
 dir->setView(view);
}

void KileFileSelect::cmbPathActivated( const KUrl& u )
{
	dir->setUrl( u, true );
}

void KileFileSelect::cmbPathReturnPressed( const QString& u )
{
   dir->setFocus();
	dir->setUrl(KUrl(u), true);
}

void KileFileSelect::dirUrlEntered( const KUrl& u )
{
	cmbPath->removeUrl(u);
   QStringList urls = cmbPath->urls();
   urls.prepend( u.url() );
   while ( urls.count() >= (uint)cmbPath->maxItems() )
      urls.remove( urls.last() );
	cmbPath->setUrls(urls);
}

void KileFileSelect::focusInEvent(QFocusEvent*)
{
   dir->setFocus();
}

void KileFileSelect::setDir( KUrl u )
{
	dir->setUrl(u, true);
}

void KileFileSelect::clickedToolbar(int i)
{
	if (i == 0)
	{
		KFileItemList itemList = dir->selectedItems();
		for(KFileItemList::iterator it = itemList.begin(); it != itemList.end(); ++it)
		{
			emit(fileSelected(*it));
			++it;
		}

		dir->view()->clearSelection();
	}
}

#include "kilefileselect.moc"