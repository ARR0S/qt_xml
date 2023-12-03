#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QTreeView>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "QtTreeModel.h"



class QtXML : public QMainWindow
{
	Q_OBJECT

public:
	QtXML(QWidget* parent = nullptr);

private:
	static int tabcount;
	QtTreeModel* model;
	QTreeView* view;
	QMenu* menu;
	QTabWidget* tabWgt;
	void setupMenu();
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;

private slots:

	void open();
	void closeFile();
	void clearAll();
	void newTab();
	void exit();
	void customMenu(const QPoint& pos);
};

