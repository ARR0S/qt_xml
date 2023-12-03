#include "QtXML.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include <QDebug>
void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	Q_UNUSED(context); // Используется для избежания предупреждения о неиспользуемой переменной

	switch (type) {
	case QtDebugMsg:
		qDebug() << msg;
		break;
	case QtInfoMsg:
		qInfo() << msg;
		break;
	case QtWarningMsg:
		QMessageBox::warning(nullptr, "Warning", msg);
		break;
	case QtCriticalMsg:
		QMessageBox::critical(nullptr, "Critical Error", msg);
		break;
	case QtFatalMsg:
		QMessageBox::critical(nullptr, "Fatal Error", msg);
		abort();
	}
}

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	qInstallMessageHandler(customMessageHandler);
	QtXML w;
	w.show();
	return a.exec();
}
