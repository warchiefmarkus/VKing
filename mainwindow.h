#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QList>
#include <QAction>
#include <QPushButton>
#include <QListView>
#include <QStringListModel>
#include <QFile>
#include <QNetworkReply>
#include "json.h"
#include <QAbstractItemModel>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "downloadmanager.h"
#include "DataForm.h"
#include <QMenuBar>
#include <QLabel>

class Progress: public QProgressBar
{
    Q_OBJECT
public:
    QString format;
public slots:

    void progress(int value, QString text){
        setValue(value);
        format.clear();
        format = "%p%";
        format.append(" ");
        format.append(text);
        setFormat(format);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QNetworkAccessManager * headerManager;
    QNetworkAccessManager * manager;
    DownloadManager* downloadManager;

    DataForm* dataForm;

    QMessageBox messageBox;
    int ERROR(int type, QString msg);

    QAction *exitAction;
    QAction *aboutAction;
    QAction *downloadAction;

    QString m_token;
    QString m_login;
    QString m_parol;
    QString m_user;
    QString m_count;
    QString m_countTo;
    QString m_userGroup;
    bool m_del;

    QListView list;
    QStringListModel items;

    struct Audio{
        QString id;
        QString title;
        QString artist;
        QString duration;
        QString url;
        QString owner;
    };

    QList<Audio> audios;
    QString path;
    QByteArray GET(QUrl);
    QNetworkReply* m_pNetworkReply;
    QFile* m_file;

    QStandardItemModel *model;
    QTableView* tableView;

    QListView* listView;

    int FILESIZE;
    int loadCounter;
    Progress* progressBar;

    void saveDownloadList(); // save downloaded audio ids
    void loadDownloadList(QString filename); // load downloaded audio ids
    void addAudios(QString id, QString count); // add count audios of ID to download list
    void fillTable();

signals:
    void fileSize(int);
    void progress(int);
    void loadNext();

public slots:

    void slotSuccess(QString login, QString parol, QString user, QString count, QString countTo, QString userGroup, bool del);
    void download();
    void slotDisconnected();
    void sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & errlist);

protected slots:
    void about();
    void close(){QCoreApplication::exit();}
    void slotDownload();

protected:
    void setupActions();
    void setupMenus();
    void setupUi();    
};

#endif
