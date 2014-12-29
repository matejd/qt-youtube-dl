#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWizard>
#include <QMap>
#include <QTextEdit>
#include <QProcess>

namespace Ui {
    class MainWindow;
}

class Youtubedl;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr, QTextEdit* logWidget = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent*) override;

private slots:
    void on_downloadButton_clicked();
    void on_actionAbout_triggered();
    void on_actionLog_triggered();

    void onDownloadWizardFinished(int result);
    void onDownloadProgressUpdated(QString url, int progressPercent);

private:
    Ui::MainWindow *m_ui;
    QWizard* m_downloadWizard;
    QMap<QString, int> m_urlRow; // Mapping URL (being downloaded) => row in statusTableWidget.
    QTextEdit* m_logWidget;
    QList<Youtubedl*> m_runningDownloadProcesses;
};

#endif // MAINWINDOW_H
