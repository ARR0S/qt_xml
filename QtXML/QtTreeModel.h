#pragma once
#include <QAbstractItemModel>
#include <QStandardItem>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QAction>

class QtTreeModel :
	public QAbstractItemModel
{
	Q_OBJECT

public:
	QtTreeModel(QObject* parent = nullptr);
	void append(const QModelIndex& index);
	void read(const QString& fileName);
	void parseDomNode(const QDomNode& node, QStandardItem* parentItem);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& index) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	void removeFile(const QString& fileName);
	QStringList getFileList() const;
	void setFileList(const QStringList& newfiles);
	void addFile(const QString& fileName);
	void clearFileList();


private:
	QStringList files{};
	QStandardItem* root{};
	QModelIndex currIndex{};
	void append(const QModelIndex& sourceIndex, QStandardItem* item);

};

