#include "mainwindow.h"
#include "youtubedl.h"

#include <QtGlobal>
#include <QApplication>
#include <QTextEdit>
#include <QFontDatabase>

Q_DECLARE_METATYPE(QList<VideoFormat>)

#ifndef QT_NO_DEBUG
#include <iostream>
#include <thread>
#endif

// An easy to use log. qDebug() and family redirected to it.
QTextEdit* g_logWidget;

void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    // Since this handler can be called from non-GUI threads, we cannot directly
    // access GUI objects.
    QMetaObject::invokeMethod(g_logWidget, "append", Qt::QueuedConnection, Q_ARG(QString, msg));

#ifndef QT_NO_DEBUG
    std::cout << std::this_thread::get_id() << ": " << msg.toStdString() << std::endl;
#endif
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<QList<VideoFormat>>();

    QCoreApplication::setOrganizationName("organization");
    QCoreApplication::setOrganizationDomain("organization.com");
    QCoreApplication::setApplicationName("qt-youtube-dl");

    QApplication a(argc, argv);

    g_logWidget = new QTextEdit();
    g_logWidget->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    g_logWidget->resize(400, 600);
    g_logWidget->setWindowTitle("Log");
    qInstallMessageHandler(messageOutput);

    MainWindow* window = new MainWindow(nullptr, g_logWidget);
    window->show();
    const int status = a.exec();

    // Reset the message handler. We are about to get rid of the
    // main window and the log widget. Since our message handler
    // redirected messages to the log widget, we need to do this in
    // this particular order.
    qInstallMessageHandler(nullptr);
    delete window;
    delete g_logWidget;

    return status;
}
