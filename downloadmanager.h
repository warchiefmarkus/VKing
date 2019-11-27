#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QUrl>
#include <QDir>
#include <QTimer>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0, QString folder = "");
    QNetworkAccessManager* mManager;

    QString NAME;
    QString saveFolder;
    QDir* dir;
    QString text;

    // Speed Counter
    int prevBytesRec;
    int bytesRec;
    int bytesTot;
    qint64 percent;
    qint64 sum1;
    qint64 sum2;
    float div;
    QString title;
    QTimer prgUpdate;


signals:
    void downloadComplete();
    void prgUpdateSig(int value, QString text); // emited once on 1 sec with received bytes and percents

public slots:
    void download(QUrl url, QString name); // for simple files
    void pause();
    void resume();
    void Update();

private slots:
    void download( QNetworkRequest& request );
    void finished();
    void downloadProgress ( qint64 bytesReceived, qint64 bytesTotal );
    void error ( QNetworkReply::NetworkError code );

private:
    QNetworkRequest mCurrentRequest;
    QNetworkReply* mCurrentReply;
    QFile* mFile;
    int mDownloadSizeAtPause;
};

#endif // DOWNLOADMANAGER_H
