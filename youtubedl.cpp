#include "youtubedl.h"

#include <QDebug>
#include <QTextCodec>
#include <QRegularExpression>
#include <QtGlobal>

Youtubedl::Youtubedl():
    m_process(nullptr),
    m_unmatchedOffset(0)
{
}

Youtubedl::~Youtubedl()
{
    killIfRunning();
}

void Youtubedl::killIfRunning()
{
    if (m_process && m_process->state() == QProcess::Running) {
        qDebug() << "KILLING process" << m_process->processId();
        m_process->close();
    }
    delete m_process;

    m_processOutput.clear();
    m_url.clear();
    m_unmatchedOffset = 0;
}

void Youtubedl::getAvailableFormats(const QString& url)
{
    /// Runs youtube_dl as a script in another process,
    /// captures all of the stdout and parses it.
    /// Emits a formatsAvailable(formats) signal with the list of available
    /// video formats, which are just pairs of strings (format id, description).

    // If download/format query already in progress, kill it.
    // Instantiate another Youtubedl if you want multiple concurrent queries.
    killIfRunning();

#ifdef Q_OS_WIN
    const QString binPath = "youtube_dl_frozen\\__main__.exe";
    QStringList args;
    args << "--list-formats" << url;
#else
    const QString binPath = "python";
    QStringList args;
    args << "youtube_dl/__main__.py" << "--list-formats" << url;
#endif
    m_process = new QProcess();
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start(binPath, args);
}

void Youtubedl::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    processFormats();
}

void Youtubedl::processFormats()
{
    const int exitCode = m_process->exitCode();
    if (exitCode != 0) {
        qDebug() << "getVideoFormats FAILED:" << m_process->errorString();
        emit formatsAvailable(QList<VideoFormat>());
        return;
    }

    // Parse stdout lines for video formats.
    const QByteArray processStdoutEncoded = m_process->readAllStandardOutput();
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

    emit formatsAvailable(formats);
}

void Youtubedl::downloadVideo(const QString& url, const QString& format, const QString& destFolder)
{
    killIfRunning();

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
    m_process = new QProcess();
    connect(m_process, SIGNAL(readyRead()), this, SLOT(onProcessReadyRead()));
    m_url = url;
    m_unmatchedOffset = 0;
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start(binPath, args);
}

void Youtubedl::onProcessReadyRead()
{
    /// Starts another process which executes youtube_dl as a script.
    /// Captures stdout as it is created (stdout consists of
    /// download progress updates). downloadProgressUpdated(url, progressPercent)
    /// signal is emitted on updates.
    /// In case of an error, progressPercent = -1 is reported.
    ///
    const QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    const QRegularExpression downloadRE("\\[download\\]\\s+(\\d+)\\.");
    const QString newData = utfCodec->toUnicode(m_process->readAll());
    m_processOutput.append(newData);
    qDebug() << newData;
    QRegularExpressionMatchIterator matchIterator = downloadRE.globalMatch(m_processOutput, m_unmatchedOffset);
    if (!matchIterator.hasNext()) {
        if (m_processOutput.indexOf("has already been downloaded") != -1) {
            // In this case we can simply return success and call it a day.
            emit downloadProgressUpdated(m_url, 100);
        }
        if ((m_processOutput.indexOf("ERROR:") != -1) || (m_processOutput.indexOf("[Errno") != -1)) {
            emit downloadProgressUpdated(m_url, -1);
        }

        // We couldn't find a progress update (percentage) or errors or anything else,
        // let's wait for more stdout.
        return;
    }

    // Get only the last match (sometimes youtube_dl prints out multiple lines).
    QRegularExpressionMatch match;
    while (matchIterator.hasNext()) {
        match = matchIterator.next();
    }

    // Found progress update. Report it.
    m_unmatchedOffset = match.capturedEnd(1);
    Q_ASSERT(m_unmatchedOffset != -1);
    const QString percentString = match.captured(1);
    bool ok = false;
    const int percent = percentString.toInt(&ok);
    qDebug() << "PROGRESS UPDATE:" << percent;
    if (ok)
        emit downloadProgressUpdated(m_url, percent);
    else
        qDebug() << "FAILED TO PARSE percentString.";
}
