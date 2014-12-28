#ifndef YOUTUBEDL_H
#define YOUTUBEDL_H

#include <QMetaType>
#include <QList>
#include <QString>

struct VideoFormat
{
    QString id;
    QString description;
};

Q_DECLARE_METATYPE(QList<VideoFormat>)

// Should be called in a background thread.
QList<VideoFormat> getVideoFormats(const QString& url);
void downloadVideo(const QString& url, const QString& format, const QString& destFolder, QObject* watcher);

#endif // YOUTUBEDL_H
