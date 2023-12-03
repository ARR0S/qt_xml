#include "QtXML.h"

int QtXML::tabcount = 1;

QtXML::QtXML(QWidget* parent)
	: QMainWindow(parent)
{

	model = new QtTreeModel(this);
	view = new QTreeView(this);
	view->setModel(model);
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	tabWgt = new QTabWidget();
	tabWgt->addTab(view, tr("Tab 1"));
	tabWgt->setCurrentIndex(tabWgt->indexOf(view));
	setCentralWidget(tabWgt);

	setupMenu();

	resize(800, 600);
	setWindowTitle("XMLTreeViewer");

	connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenu(QPoint)));
}

void QtXML::setupMenu() {
	menu = new QMenu(this);
	menu->setTitle("File");
	QAction* openAction = new QAction(this);
	openAction->setText("Open");
	QAction* clearallAction = new QAction(this);
	clearallAction->setText("Close all");
	QAction* exitAction = new QAction(this);
	exitAction->setText("Exit");
	menu->addAction(openAction);
	menu->addAction(clearallAction);
	menu->addAction(exitAction);
	menuBar()->addMenu(menu);
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
	connect(clearallAction, SIGNAL(triggered()), this, SLOT(clearAll()));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));
}

void QtXML::open()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open an .xml file"), "", tr("XML (*.xml)"));

	int currentIndex = tabWgt->currentIndex();
	if (currentIndex >= 0 && currentIndex < tabWgt->count()) {
		auto* currView = qobject_cast<QTreeView*>(tabWgt->widget(currentIndex));
		if (currView) {
			auto* currModel = dynamic_cast<QtTreeModel*>(currView->model());
			if (currModel) {
				currModel->read(fileName);
				currView->reset();
			}
		}
	}
	for (int i = tabWgt->count() - 1; i >= 0; --i) {
		auto* tabView = qobject_cast<QTreeView*>(tabWgt->widget(i));
		if (tabView) {
			auto* tabModel = dynamic_cast<QtTreeModel*>(tabView->model());
			if (tabModel) {
				tabModel->addFile(fileName);
			}
		}
	}
}




void QtXML::closeFile() {
	auto* currView = qobject_cast<QTreeView*>(tabWgt->currentWidget());
	QString fileName;
	if (currView) {
		auto* currModel = dynamic_cast<QtTreeModel*>(currView->model());
		if (currModel) {
			auto currIndex = currView->currentIndex();
			if (currIndex.isValid() && currIndex.row() >= 0 && currIndex.row() < currModel->rowCount()) {
				fileName = currModel->data(currIndex, Qt::DisplayRole).toString();
				currModel->removeRow(currIndex.row());
				view->reset();

				// Проверяем, есть ли еще файлы в данном табе
				bool anyFilesLeft = false;
				for (int row = 0; row < currModel->rowCount(); ++row) {
					if (currModel->data(currModel->index(row, 0), Qt::DisplayRole).toString() != fileName) {
						anyFilesLeft = true;
						break;
					}
				}

				// Если нет больше файлов в текущем табе, закрываем его (кроме первого таба)
				if (!anyFilesLeft && tabWgt->currentIndex() > 0) {
					tabWgt->removeTab(tabWgt->currentIndex());
					delete currModel;
				}
			}
		}
	}
	for (int i = tabWgt->count() - 1; i >= 0; --i) {
		auto* tabView = qobject_cast<QTreeView*>(tabWgt->widget(i));
		if (tabView) {
			auto* tabModel = dynamic_cast<QtTreeModel*>(tabView->model());
			if (tabModel) {
				tabModel->removeFile(fileName);
			}
		}
	}
}



void QtXML::clearAll() {
	auto* firstView = qobject_cast<QTreeView*>(tabWgt->widget(0));
	if (firstView) {
		auto* firstModel = dynamic_cast<QtTreeModel*>(firstView->model());
		if (firstModel) {
			while (firstModel->rowCount() > 0) {
				auto fileNames = firstModel->getFileList();
				for (const auto& fileName : fileNames) {
					firstModel->removeFile(fileName);
				}
				firstModel->removeRow(0);
			}
		}
	}

	// Удаляем табы кроме первого
	for (int i = tabWgt->count() - 1; i > 0; --i) {
		auto* tabView = qobject_cast<QTreeView*>(tabWgt->widget(i));
		if (tabView) {
			auto* tabModel = dynamic_cast<QtTreeModel*>(tabView->model());
			if (tabModel) {
				delete tabModel;
			}
			tabWgt->removeTab(i);
		}
	}
	tabcount = 1;
	// Очищаем список файлов в основной модели
	model->clearFileList();
	view->reset();
}





void QtXML::newTab()
{
	auto* currView = qobject_cast<QTreeView*>(tabWgt->currentWidget());
	auto* currModel = dynamic_cast<QtTreeModel*>(currView->model());
	if (currModel->rowCount() > 1) { // Проверяем, есть ли более одного файла в текущей вкладке
		++tabcount;
		auto currIndex = currView->currentIndex();
		auto* newView = new QTreeView(tabWgt);
		auto* newModel = new QtTreeModel(newView);
		QStringList list = currModel->getFileList();
		newModel->setFileList(currModel->getFileList());
		newView->setModel(newModel);
		tabWgt->addTab(newView, "Tab " + QString::number(tabcount));
		tabWgt->setCurrentIndex(tabWgt->indexOf(newView));
		auto* tmp = dynamic_cast<QtTreeModel*>(newView->model());
		tmp->append(currIndex);
		currModel->removeRow(currIndex.row());
		currModel->setFileList(list);
		newView->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(newView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenu(QPoint)));
	}
}


void QtXML::exit()
{
	close();
}

void QtXML::customMenu(const QPoint& pos)
{
	if (view->currentIndex().parent() == view->rootIndex())
	{
		QMenu* menu = new QMenu(this);
		QAction* newTabAction = new QAction("Open in a new tab", menu);
		QAction* closefileAction = new QAction("Close", menu);

		connect(newTabAction, SIGNAL(triggered()), this, SLOT(newTab()));
		connect(closefileAction, SIGNAL(triggered()), this, SLOT(closeFile()));

		menu->addAction(newTabAction);
		menu->addAction(closefileAction);
		menu->exec(view->viewport()->mapToGlobal(pos));
	}
}
