#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWizard>
#include <QMap>
#include <QTextEdit>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    // An easy to use log. qDebug() and family redirected to it.
    static QTextEdit* s_logWidget;

private slots:
    void on_downloadButton_clicked();
    void on_actionAbout_triggered();
    void on_actionLog_triggered();

    void onDownloadWizardFinished(int result);
    void updateDownloadProgress(QString url, int progressPercent);

private:
    Ui::MainWindow *m_ui;
    QWizard* m_downloadWizard;
    QMap<QString, int> m_urlRow; // Mapping URL (being downloaded) => row in statusTableWidget.
};

#endif // MAINWINDOW_H
