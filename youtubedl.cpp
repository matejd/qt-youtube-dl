#include "youtubedl.h"

#include <QDebug>
#include <QProcess>
#include <QTextCodec>
#include <QRegularExpression>
#include <QtGlobal>
#include <QScopedPointer>

QList<VideoFormat> getVideoFormats(const QString& url)
{
    /// Run youtube_dl as a script in another process,
    /// capture all of the stdout and parse it.
    /// Returns a list of video formats, which are just
    /// pairs of strings (format id, description).
#ifdef Q_OS_WIN
    const QString binPath = "youtube_dl_frozen\\__main__.exe";
    QStringList args;
    args << "--list-formats" << url;
#else
    const QString binPath = "python";
    QStringList args;
    args << "youtube_dl/__main__.py" << "--list-formats" << url;
#endif
    QScopedPointer<QProcess> pythonProcess(new QProcess());
    pythonProcess->setProcessChannelMode(QProcess::MergedChannels);
    pythonProcess->start(binPath, args);
    const bool success = pythonProcess->waitForFinished();
    if (!success) {
        qDebug() << "getVideoFormats FAILED:" << pythonProcess->errorString();
        return QList<VideoFormat>();
    }

    // Parse stdout lines for video formats.
    const QByteArray processStdoutEncoded = pythonProcess->readAllStandardOutput();
    const QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    const QString processStdout = utfCodec->toUnicode(processStdoutEncoded);
    qDebug() << processStdout;
    const QStringList lines = processStdout.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    bool linesWithFormats = false;
    QList<VideoFormat> formats;
    for (const QString& line: lines) {
        if (linesWithFormats) {
            VideoFormat newVideoFormat;
            newVideoFormat.id = line.split(" ").at(0);
            newVideoFormat.description = line.mid(12);
            formats.prepend(newVideoFormat);
            continue;
        }
        if (line.startsWith("format"))
            linesWithFormats = true;
    }

    return formats;
}

void downloadVideo(const QString& url, const QString& format, const QString& destFolder, QObject* watcher)
{
    /// Starts another process which executes youtube_dl as a script.
    /// This functions captures stdout as it is created (stdout consists of
    /// download progress updates). updateDownloadProgress(url, progressPercent) is called
    /// on the watcher object.
    /// In case of an error, progressPercent = -1 is reported.
    Q_ASSERT(watcher != nullptr);
#ifdef Q_OS_WIN
    const QString binPath = "youtube_dl_frozen\\__main__.exe";
    const QString outTemplate = destFolder + "\\%(title)s-%(id)s.%(ext)s";
    QStringList args;
    args << "--format" << format << "--output" << outTemplate << url;
#else
    const QString binPath = "python";
    const QString outTemplate = destFolder + "/%(title)s-%(id)s.%(ext)s";
    QStringList args;
    args << "youtube_dl/__main__.py" << "--format" << format << "--output" << outTemplate << url;
#endif
    QScopedPointer<QProcess> pythonProcess(new QProcess());
    pythonProcess->setProcessChannelMode(QProcess::MergedChannels);
    pythonProcess->start(binPath, args);

    const QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    const QRegularExpression downloadRE("\\[download\\]\\s+(\\d+)\\.");
    QString data;
    int unmatchedOffset = 0;
    while (pythonProcess->waitForReadyRead()) {
        const QString newData = utfCodec->toUnicode(pythonProcess->readAll());
        data.append(newData);
        qDebug() << newData;
        QRegularExpressionMatchIterator matchIterator = downloadRE.globalMatch(data, unmatchedOffset);
        if (!matchIterator.hasNext()) {
            if (data.indexOf("has already been downloaded") != -1) {
                // In this case we can simply return success and call it a day.
                QMetaObject::invokeMethod(watcher, "updateDownloadProgress", Q_ARG(QString, url), Q_ARG(int, 100));
                break;
            }
            if (data.indexOf("ERROR:") != -1) {
                QMetaObject::invokeMethod(watcher, "updateDownloadProgress", Q_ARG(QString, url), Q_ARG(int, -1));
                break;
            }

            // We couldn't find a progress update (percentage) or errors or anything else,
            // let's wait for more stdout.
            continue;
        }

        // Get only the last match (sometimes youtube_dl prints out multiple lines).
        QRegularExpressionMatch match;
        while (matchIterator.hasNext()) {
            match = matchIterator.next();
        }

        // Found progress update. Report it.
        unmatchedOffset = match.capturedEnd(1);
        Q_ASSERT(unmatchedOffset != -1);
        const QString percentString = match.captured(1);
        bool ok = false;
        const int percent = percentString.toInt(&ok);
        qDebug() << "PROGRESS UPDATE:" << percent;
        if (ok)
            QMetaObject::invokeMethod(watcher, "updateDownloadProgress", Q_ARG(QString, url), Q_ARG(int, percent));
        else
            qDebug() << "FAILED TO PARSE percentString.";
    }

    pythonProcess->waitForFinished();
    qDebug() << "DOWNLOAD DONE:" << url;
}
