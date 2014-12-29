#ifndef YOUTUBEDL_H
#define YOUTUBEDL_H

#include <QList>
#include <QString>
#include <QProcess>

struct VideoFormat
{
    QString id;
    QString description;
};

class Youtubedl : public QObject
{
    Q_OBJECT

public:
    Youtubedl();
    ~Youtubedl();

    void getAvailableFormats(const QString& url);
    void downloadVideo(const QString& url, const QString& format, const QString& destFolder);

signals:
    void formatsAvailable(QList<VideoFormat> formats);
    void downloadProgressUpdated(QString url, int progressPercent);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessReadyRead();

private:
    void killIfRunning();
    void processFormats();

    QProcess* m_process;
    QString m_processOutput;
    QString m_url;
    int m_unmatchedOffset;
};

#endif // YOUTUBEDL_H
