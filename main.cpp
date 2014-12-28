#include "mainwindow.h"
#include "youtubedl.h"

#include <QtGlobal>
#include <QApplication>
#include <QTextEdit>

void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    if (MainWindow::s_logWidget != nullptr)
        QMetaObject::invokeMethod(MainWindow::s_logWidget, "append", Qt::QueuedConnection, Q_ARG(QString, msg));
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<QList<VideoFormat>>();

    QCoreApplication::setOrganizationName("organization");
    QCoreApplication::setOrganizationDomain("organization.com");
    QCoreApplication::setApplicationName("qt-youtube-dl");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qInstallMessageHandler(messageOutput);
    const int status = a.exec();
    return status;
}
