/**************************************************************************************
  Copyright (C) 2003 by Jeroen Wijnhout (Jeroen.Wijnhout@kdemail.net)
                2011 by Michel Ludwig (michel.ludwig@kdemail.net)
 **************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************/

#ifndef KILETOOLMANAGER_H
#define KILETOOLMANAGER_H

#include <QLinkedList>
#include <QObject>
#include <QQueue>
#include <QStackedWidget>
#include <QStringList>

#include "kiletool.h"

/***********************************************************************************************************
 * CAUTION!!
 * It must be ensured that no event loop is started whenever some tool operation
 * is running. This includes running code inside the tool classes!
 *
 * The reason for that is that an event loop might trigger the deletion a tool object for which code
 * is currently executed, for instance with the 'stopLivePreview' method.
 * An event loop is executed, for example, within the 'documentSave' method of KatePart. Although the event
 * loop doesn't process user events, the document modification timer might still be triggered in such
 * an event loop and 'stopLivePreview' will be called. Now, no document saving is performed inside tool
 * classes anymore (including the tool manager).
 ***********************************************************************************************************/

class QTimer;

class KConfig;
class KTextEdit;
class KAction;
namespace KParts { class PartManager; }

class KileInfo;
namespace KileParser { class Manager; }
namespace KileView { class Manager; }
namespace KileWidget { class LogWidget; class OutputView; }

namespace KileTool
{
	class Factory;
	class LivePreviewManager;

	class QueueItem
	{
	public:
		explicit QueueItem(Base *tool, bool block = false);
		~QueueItem();

		Base* tool() const { return m_tool; }
		bool shouldBlock() { return m_bBlock; }

	private:
		Base *m_tool;
		bool m_bBlock;
	};

	class Queue : public QQueue<QueueItem*>
	{
	public:
		Base* tool() const;
		bool shouldBlock() const;

		void enqueueNext(QueueItem *);
	};

	class Manager : public QObject
	{
		friend class Base;
		Q_OBJECT

	public:
		Manager(KileInfo *ki, KConfig *config, KileWidget::LogWidget *log, KileWidget::OutputView *output, KParts::PartManager *, QStackedWidget* stack, KAction *, uint to);
		~Manager();

	public:
		Base* createTool(const QString& name, const QString &cfg = QString(), bool prepare = false);
		bool configure(Base*, const QString &cfg = QString());
		bool retrieveEntryMap(const QString & name, Config & map, bool usequeue = true, bool useproject = true, const QString & cfg = QString());
		void saveEntryMap(const QString & name, Config & map, bool usequeue = true, bool useproject = true);
		QString currentGroup(const QString &name, bool usequeue = true, bool useproject = true);

		void wantGUIState(const QString &);

		KParts::PartManager * partManager() { return m_pm; }
		QStackedWidget* widgetStack() { return m_stack; }
		KileView::Manager* viewManager();
		KileTool::LivePreviewManager* livePreviewManager();
		KileParser::Manager* parserManager();

		KileInfo * info() { return m_ki; }
		KConfig * config() { return m_config; }

		void setFactory(Factory* fac) { m_factory = fac; }
		Factory* factory() { return m_factory; }

		bool queryContinue(const QString & question, const QString & caption = QString());

		bool shouldBlock();
		int lastResult() { return m_nLastResult; }

	public Q_SLOTS:
		void run(KileTool::Base *tool);

		void stopLivePreview();

	private:
		void setEnabledStopButton(bool state);
		void initTool(Base*);

	private Q_SLOTS:
		int runImmediately(Base *tool, bool insertAtTop = false, bool block = false, Base *parent = NULL);
		int runNextInQueue();
		void enableClear();

		void started(KileTool::Base *tool);
		void done(KileTool::Base *tool, int result);

		void stop(); //should be a slot that stops the active tool and clears the queue
		void stopActionDestroyed();

		// must be used when a child tool is launched from within another tool!
		int runChildNext(Base *parent, Base *tool, bool block = false);

		void toolScheduledAfterParsingDestroyed(KileTool::Base *tool);
		void handleParsingComplete();

	Q_SIGNALS:
		void requestGUIState(const QString &);
		void jumpToFirstError();
		void toolStarted();
		void previewDone();
		// emitted when a tool spawns another tool (parent, child).
		void childToolSpawned(KileTool::Base*,KileTool::Base*);

	private:
		KileInfo			*m_ki;
		KConfig				*m_config;
		KileWidget::LogWidget		*m_log;
		KileWidget::OutputView		*m_output;
		KParts::PartManager		*m_pm;
		QStackedWidget			*m_stack;
		KAction				*m_stop;
		Factory				*m_factory;
		Queue				m_queue;
		QTimer				*m_timer;
		bool				m_bClear;
		int				m_nLastResult;
		uint				m_nTimeout;
		QQueue<Base*>		m_toolsScheduledAfterParsingList;

		void deleteLivePreviewToolsFromQueue();
		void deleteLivePreviewToolsFromRunningAfterParsingQueue();
	};

	QStringList toolList(KConfig *config, bool menuOnly = false);
	QStringList configNames(const QString &tool, KConfig *config);

	QString configName(const QString & tool, KConfig *);
	void setConfigName(const QString & tool, const QString & name, KConfig *);

	QString groupFor(const QString & tool, KConfig *);
	QString groupFor(const QString & tool, const QString & cfg = "Default" );

	void extract(const QString &str, QString &tool, QString &cfg);
	QString format(const QString & tool, const QString &cfg);

	QString menuFor(const QString &tool, KConfig *config);
	QString iconFor(const QString &tool, KConfig *config);

	QString categoryFor(const QString &clss);

	void setGUIOptions(const QString &tool, const QString &menu, const QString &icon, KConfig *config);
}

#endif
