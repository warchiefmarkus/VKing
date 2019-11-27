#include "downloadmanager.h"

#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

DownloadManager::DownloadManager(QObject *parent, QString folder) :
    QObject(parent),mCurrentReply(0),mFile(0),mDownloadSizeAtPause(0)
{
    saveFolder = folder;
}

void DownloadManager::download(QUrl url, QString name)
{
    NAME = name;
    mDownloadSizeAtPause = 0;
    mCurrentRequest = QNetworkRequest(url);
    mFile = new QFile();


    dir = new QDir(QCoreApplication::applicationDirPath().split("/").at(0)+"/");
    // if save dir doesnt exist
    if(!dir->exists(saveFolder)){

        // if created
        if(dir->mkdir(saveFolder)){
            qDebug() << "DIR DOESNT EXIST CREATE ";
        }else{
             qFatal("Cannt create Save Folder");
        }
    }else
    {
        qDebug() << " FOLDER EXIST " << dir->path();
    }

    //mFile->setFileName(QCoreApplication::applicationDirPath().split("/").at(0)+"/"+saveFolder+"/"+NAME+".part");
    mFile->setFileName(QCoreApplication::applicationDirPath().split("/").at(0)+"/"+NAME+".part");
    // if file downloaded
    qDebug() << " FILENAME  << " << mFile->fileName();
    //if(QFile::exists(QCoreApplication::applicationDirPath().split("/").at(0)+"/"+saveFolder+"/"+NAME+".mp3")){
    if(QFile::exists(QCoreApplication::applicationDirPath().split("/").at(0)+"/"+NAME+".mp3")){
        emit downloadComplete();
         qDebug() << " FILE EXIST ";
        return;
    }
    // else if file exist continue loading
    else if (mFile->exists()){
        qDebug() << " FILE PART EXIST LOADING ";
        mFile->open(QIODevice::ReadWrite);
        mDownloadSizeAtPause = mFile->size();
        mFile->seek(mDownloadSizeAtPause);
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(mDownloadSizeAtPause) + "-";
        mCurrentRequest.setRawHeader("Range",rangeHeaderValue);
        download(mCurrentRequest);
    }
    // else download new
    else{
        qDebug() << " FILE LOAD NEW ";
        qDebug() << mFile->fileName();
        qDebug() << mFile->open(QIODevice::ReadWrite);
        download(mCurrentRequest);
    }
}

void DownloadManager::pause()
{
    if( mCurrentReply == 0 ) {
        return;
    }
    disconnect(mCurrentReply,SIGNAL(finished()),this,SLOT(finished()));
    disconnect(mCurrentReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    disconnect(mCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    mCurrentReply->abort();
    mFile->write( mCurrentReply->readAll());
    mCurrentReply = 0;
}

void DownloadManager::resume()
{
    mDownloadSizeAtPause = mFile->size();
    mFile->seek(mDownloadSizeAtPause);
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(mDownloadSizeAtPause) + "-";
    mCurrentRequest.setRawHeader("Range",rangeHeaderValue);
    download(mCurrentRequest);
}

void DownloadManager::download( QNetworkRequest& request )
{
    mCurrentReply = mManager->get(request);
    connect(mCurrentReply,SIGNAL(finished()),this,SLOT(finished()));
    connect(mCurrentReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(mCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    prevBytesRec = 0;
    bytesRec = 0;
    bytesTot = 0;
    connect(&prgUpdate, SIGNAL(timeout()), this, SLOT(Update()));
    prgUpdate.start(1000);
}

void DownloadManager::finished()
{
    mFile->close();
    //mFile->rename(QCoreApplication::applicationDirPath().split("/").at(0)+"/"+saveFolder+"/"+NAME+".part",QCoreApplication::applicationDirPath().split("/").at(0)+"/"+saveFolder+"/"+NAME+".mp3");
    mFile->rename(QCoreApplication::applicationDirPath().split("/").at(0)+"/"+NAME+".part",QCoreApplication::applicationDirPath().split("/").at(0)+"/"+NAME+".mp3");
    mFile = 0;
    mCurrentReply = 0;
    prgUpdate.stop();
    emit downloadComplete();
}

void DownloadManager::Update()
{
    if (bytesTot>0){
        title.clear();
        title.append(" | ");
        title.append( QString::number(((((float)(mDownloadSizeAtPause+bytesRec))/1024)/1024),'f',3));
        title.append("Mb | ");
        title.append(QString::number(((((float)(mDownloadSizeAtPause+bytesTot))/1024)/1024),'f',3));
        title.append("Mb | ");
        title.append(QString::number(((bytesRec-prevBytesRec)/1024)));
        title.append(" Kb/S");

        sum1 = ((mDownloadSizeAtPause+bytesRec)*100);
        sum2 = mDownloadSizeAtPause+bytesTot;
        div = (sum1 / sum2);
        percent = (qint64) div;

        emit prgUpdateSig((int)div, title);
        prevBytesRec = bytesRec;
    }
}

void DownloadManager::downloadProgress ( qint64 bytesReceived, qint64 bytesTotal )
{
    bytesRec = bytesReceived;
    bytesTot = bytesTotal;
    mFile->write( mCurrentReply->readAll() );
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{

}
