#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "introwizardpage.h"
#include "videoformatspage.h"
#include "folderwizardpage.h"

#include <QWizard>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>

QTextEdit* MainWindow::s_logWidget = nullptr;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_ui->statusTableWidget->setColumnCount(3);
    QStringList headerLabels;
    headerLabels << "URL" << "Downloaded" << "Folder";
    m_ui->statusTableWidget->setHorizontalHeaderLabels(headerLabels);
    m_ui->statusTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_ui->statusTableWidget->setColumnWidth(0, 400);

    m_downloadWizard = new QWizard();
    m_downloadWizard->setPage(0, new IntroWizardPage());
    m_downloadWizard->setPage(1, new VideoFormatsWizardPage());
    m_downloadWizard->setPage(2, new FolderWizardPage());
    m_downloadWizard->setWindowTitle("Download Wizard");
    m_downloadWizard->setFixedSize(QSize(800, 400));
    m_downloadWizard->setModal(true);
    connect(m_downloadWizard, SIGNAL(finished(int)), this, SLOT(onDownloadWizardFinished(int)));

    s_logWidget = new QTextEdit();
    s_logWidget->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    s_logWidget->resize(400, 600);
    s_logWidget->setWindowTitle("Log");
}

MainWindow::~MainWindow()
{
    delete s_logWidget;
    delete m_downloadWizard;
    delete m_ui;
}

void MainWindow::on_downloadButton_clicked()
{
    m_downloadWizard->restart();
    m_downloadWizard->show();
}

void MainWindow::onDownloadWizardFinished(int result)
{
    qDebug() << "DOWNLOAD WIZARD FINISHED:" << result;
    if (result != 1)
        return;

    const QString url    = m_downloadWizard->field("url").toString();
    const QString folder = m_downloadWizard->field("folder").toString();
    const QString format = m_downloadWizard->field("format").toString();
    if (m_urlRow.contains(url)) {
        QMessageBox box;
        box.setText("This URL is already being downloaded!");
        box.exec();
        return;
    }

    const int destRow = m_ui->statusTableWidget->rowCount();
    m_ui->statusTableWidget->setRowCount(destRow+1);
    m_ui->statusTableWidget->setItem(destRow, 0, new QTableWidgetItem(url));
    m_ui->statusTableWidget->setItem(destRow, 1, new QTableWidgetItem("0%"));
    m_ui->statusTableWidget->setItem(destRow, 2, new QTableWidgetItem(folder));

    // Disable editing. We really don't need it.
    for (int i = 0; i < 3; ++i) {
        QTableWidgetItem* item = m_ui->statusTableWidget->item(destRow, i);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    qDebug() << "DOWNLOADING:" << url << " " << format << " " << folder;
    m_urlRow[url] = destRow;
    QtConcurrent::run([=] {
        // Run in a background thread.
        downloadVideo(url, format, folder, this);
    });
}

void MainWindow::updateDownloadProgress(QString url, int progressPercent)
{
    if (!m_urlRow.contains(url)) {
        qDebug() << "UNKNOWN URL:" << url;
        return;
    }
    const int destRow = m_urlRow.value(url);
    QTableWidgetItem* item = m_ui->statusTableWidget->item(destRow, 1);

    QString progressStr = QString::number(progressPercent) + "%";
    if (progressPercent < 0) {
        progressStr = "Error";
        m_urlRow.remove(url);
    }
    if (progressPercent == 100) {
        progressStr = "Done";
        m_urlRow.remove(url);
    }
    item->setText(progressStr);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox about;
    about.setText("Version 2014-12-28. Send bugs to matej.drame@gmail.com");
    about.exec();
}

void MainWindow::on_actionLog_triggered()
{
    if (s_logWidget->isVisible())
        s_logWidget->hide();
    else
        s_logWidget->show();
}
