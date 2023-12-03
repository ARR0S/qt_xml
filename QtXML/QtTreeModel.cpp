#include "QtTreeModel.h"

QtTreeModel::QtTreeModel(QObject* parent) : QAbstractItemModel(parent)
{
	root = new QStandardItem();
}

void QtTreeModel::append(const QModelIndex& index)
{
	root->setColumnCount(2);
	if (index.isValid())
	{
		beginResetModel();
		auto* tmp = static_cast<QStandardItem*>(index.internalPointer())->clone();
		if (tmp)
		{
			if (columnCount(index) == 2) {
				auto sibling = index.model()->index(index.row(), 1, index);
				auto sibling_tmp = static_cast<QStandardItem*>(sibling.internalPointer())->clone();
				root->appendRow({ tmp, sibling_tmp });
			}
			else {
				root->appendRow(tmp);
			}
			append(index, tmp);
			endResetModel();
		}
	}
}

void QtTreeModel::read(const QString& name)
{

	QFile file(name);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning("Can't open the file!");
		return;
	}

	QDomDocument doc;
	if (!doc.setContent(&file)) {
		file.close();
		qWarning("XML reading error!");
		return;
	}
	QFileInfo fileInfo(file.fileName());
	QString filename(fileInfo.fileName());
	file.close();
	QDomElement rootElement = doc.documentElement();

	for (const QString& fileName : files) {
		if (fileName == filename) {
			qWarning("XML-file has been already opened!");
			return;
		}
	}

	QStandardItem* first = new QStandardItem(filename);
	root->appendRow(first);
	root->setColumnCount(2);

	parseDomNode(rootElement, first);
}

void QtTreeModel::parseDomNode(const QDomNode& node, QStandardItem* parentItem)
{
	QDomNode currentNode = node.firstChild();
	while (!currentNode.isNull()) {
		if (currentNode.isElement()) {
			QDomElement currentElement = currentNode.toElement();

			QStandardItem* item = new QStandardItem(currentElement.tagName());
			if (currentElement.tagName() == "array" || currentElement.tagName() == "employee") {
				QDomNodeList childNodes = currentElement.childNodes();
				if (childNodes.count() == 1 && childNodes.at(0).isText()) {
					QString text = childNodes.at(0).nodeValue();
					QStandardItem* textItem = new QStandardItem(text);
				}
				else {
					parseDomNode(currentElement, item);
				}
			}
			else {
				if (currentElement.tagName() == "ShortName" ||
					currentElement.tagName() == "Name" ||
					currentElement.tagName() == "name" ||
					currentElement.tagName() == "Kind")
				{
					QString text = currentElement.text();
					parentItem->setText(text);
				}
				if (currentElement.hasAttributes()) {
					QDomNamedNodeMap attributes = currentElement.attributes();
					for (int i = 0; i < attributes.count(); ++i) {
						QDomAttr attr = attributes.item(i).toAttr();
						QString attrName = attr.name();
						QString attrValue = attr.value();

						QStandardItem* attrItemName = new QStandardItem(attrName);
						QStandardItem* attrItemValue = new QStandardItem(attrValue);

						item->appendRow({ attrItemName, attrItemValue });
					}
				}

				if (currentElement.hasChildNodes()) {
					QDomNodeList childNodes = currentElement.childNodes();
					if (childNodes.count() == 1 && childNodes.at(0).isText()) {
						QString text = childNodes.at(0).nodeValue();
						QStandardItem* textItem = new QStandardItem(text);
						item->setColumnCount(2); // Устанавливаем два столбца
						item->appendRow({ new QStandardItem(), textItem }); // Добавляем второй элемент второго столбца
					}
					else {
						parseDomNode(currentElement, item);
					}
				}
				else {
					QString text = currentElement.text();
					if (!text.isEmpty()) {
						QStandardItem* textItem = new QStandardItem(text);
						item->setColumnCount(2); // Устанавливаем два столбца
						item->appendRow({ new QStandardItem(), textItem }); // Добавляем второй элемент второго столбца
					}
				}
			}
			parentItem->appendRow(item);
		}
		currentNode = currentNode.nextSibling();
	}
}




int QtTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return static_cast<QStandardItem*>(parent.internalPointer())->rowCount();
	return root->rowCount();
}

int QtTreeModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return static_cast<QStandardItem*>(parent.internalPointer())->columnCount();
	return root->columnCount();
}

QModelIndex QtTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (hasIndex(row, column, parent))
	{
		QStandardItem* parentItem;
		if (!parent.isValid())
			parentItem = root;
		else
			parentItem = static_cast<QStandardItem*>(parent.internalPointer());
		QStandardItem* descendantItem = parentItem->child(row, column);
		if (descendantItem)
			return createIndex(row, column, descendantItem);
	}
	return QModelIndex();
}

QModelIndex QtTreeModel::parent(const QModelIndex& index) const
{
	QStandardItem* child = static_cast<QStandardItem*>(index.internalPointer());
	QStandardItem* par = child->parent();
	if (par == root || child == root) return QModelIndex();
	else
	{
		return createIndex(par->row(), 0, par);
	}
}

QVariant QtTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) return QVariant();
	if (role != Qt::DisplayRole) return QVariant();
	auto* item = static_cast<QStandardItem*>(index.internalPointer());
	return item->data(0);
}

QVariant QtTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
		return QString();
	return QVariant();
}

bool QtTreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
	QStandardItem* item = nullptr;
	QModelIndex index;
	if (parent.isValid())
	{
		item = static_cast<QStandardItem*>(parent.internalPointer());
		index = parent;
	}
	else
	{
		item = root;
		index = QModelIndex();
	}
	if (item)
	{
		auto first = row;
		beginRemoveRows(index, first, first + count - 1);
		item->removeRows(first, count);
		endRemoveRows();
		for (int i = 0; i < count; ++i) {
			if (row < files.size()) {
				files.removeAt(row);
			}
		}
		return true;
	}
	return false;
}

void QtTreeModel::append(const QModelIndex& sourceIndex, QStandardItem* item)
{
	item->setColumnCount(2);
	if (sourceIndex.isValid()) {
		for (int i = 0; i < rowCount(sourceIndex); ++i) {
			auto child = sourceIndex.model()->index(i, 0, sourceIndex);
			auto* clonedItem = static_cast<QStandardItem*>(child.internalPointer())->clone();
			if (clonedItem) {
				if (columnCount(sourceIndex) == 2) {
					auto second = sourceIndex.model()->index(i, 1, sourceIndex);
					if (second.isValid())
					{
						auto secondClone = static_cast<QStandardItem*>(second.internalPointer())->clone();
						item->appendRow({ clonedItem, secondClone });
					}
				}
				else {
					item->appendRow(clonedItem);
				}
				append(child, clonedItem);
			}
		}
	}
}


void QtTreeModel::removeFile(const QString& fileName) {
	for (auto it = files.begin(); it != files.end();) {
		if (it->contains(fileName)) {
			it = files.erase(it);
		}
		else {
			++it;
		}
	}
}


void QtTreeModel::clearFileList() {
	files.clear();
}

QStringList QtTreeModel::getFileList() const {
	return files;
}

void QtTreeModel::setFileList(const QStringList& newfiles)
{
	files = newfiles;
}

void QtTreeModel::addFile(const QString& filePath)
{
	QFile file(filePath);
	QFileInfo fileInfo(file.fileName());
	QString filename(fileInfo.fileName());
	files.append(filename);
}