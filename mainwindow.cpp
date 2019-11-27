#include <QtGui>
#include <QtNetwork>
#include <QTextStream>
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QProgressBar>
#include "downloadmanager.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QStatusBar>

MainWindow::MainWindow()
{ 
    //--------------- SSL ERRORS DECIDER
    QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
    QList<QSslCertificate> ca_list = sslCfg.caCertificates();
    QList<QSslCertificate> ca_new = QSslCertificate::fromData("CaCertificates");
    ca_list += ca_new;
    sslCfg.setCaCertificates(ca_list);
    sslCfg.setProtocol(QSsl::AnyProtocol);
    QSslConfiguration::setDefaultConfiguration(sslCfg);
    //---------------

    this->setGeometry(0,0,240,320);

    //---Form Data--
    dataForm = new DataForm();
    setCentralWidget(dataForm);
    setWindowTitle("Data Form");
    connect(dataForm,SIGNAL(run(QString,QString,QString,QString,QString,QString,bool)), this, SLOT(slotSuccess(QString,QString,QString,QString,QString,QString,bool)));
}


QByteArray MainWindow::GET(QUrl r)
{
        QNetworkReply * reply = manager->get(QNetworkRequest(r));
        QEventLoop wait;
        connect(manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
        QTimer::singleShot(30000, &wait, SLOT(quit()));
        wait.exec();
        QByteArray answer = reply->readAll();
        return answer;
}

void MainWindow::slotDisconnected()
{
     // DELETE AUDIO FROM VK
    if (m_del==true){
            QUrl * current = new QUrl("https://api.vk.com/method/audio.delete");
            current->addQueryItem("access_token",m_token);
            current->addQueryItem("aid",model->item(0,3)->text());
            current->addQueryItem("oid",m_user);
            GET(*current);
    }

    model->removeRow(0);
    audios.removeFirst();
    //delete current;
    emit loadNext();
}

void MainWindow::download()
{
        //saveDownloadList(); // save audios list
        //loadDownloadList(QCoreApplication::applicationDirPath().split("/").at(0)+"/downloadList.xml");
        loadCounter = 0;

        //!! Check END OF LIST
        if (audios.size()>0){       
            downloadManager->download(QUrl(audios.at(0).url.split("?extra").at(0)), audios.at(0).title);
        }
        //!! else download from reserve group
        else{          
            setWindowTitle("Reserve Load");
            slotSuccess("","","","300","3000","Group",false);
        }
}

void MainWindow::slotDownload()
{
    addAudios(m_user,m_count);
    fillTable();

    connect(this, SIGNAL(loadNext()),this, SLOT(download()));
    emit loadNext();
}

void MainWindow::about()
{
    QMessageBox msgBox;
    msgBox.setText("");
    msgBox.exec();
}

void MainWindow::setupActions()
{
    downloadAction = new QAction(tr("Download"), this);
    exitAction = new QAction(tr("E&xit"), this);
    aboutAction = new QAction(tr("A&bout"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(downloadAction, SIGNAL(triggered()), this, SLOT(slotDownload()));
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(downloadAction);
    fileMenu->addAction(aboutAction);
    fileMenu->addAction(exitAction);
}

void MainWindow::setupUi()
{
    QWidget *widget = new QWidget;
    model = new QStandardItemModel(1,1,this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Title")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Artist")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Length")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("ID")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString("URL")));

    listView = new QListView(widget);
    listView->setGeometry(0,0,240,220);
    listView->setModel(model);

    progressBar->setParent(widget);
    progressBar->setGeometry(0,220,240,20);

    this->setCentralWidget(widget);
    setWindowTitle("VKing");
}

void MainWindow::slotSuccess(QString login, QString parol, QString user, QString count, QString countTo, QString userGroup, bool del)
{
    m_login = login;
    m_parol = parol;
    m_user = user;
    m_count = count;
    m_countTo = countTo;
    m_userGroup = userGroup;
    m_del = del;


    //-------CONTINUE
    centralWidget()->deleteLater();
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )), this, SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));

    downloadManager = new DownloadManager(this,"VKingMusic");

    path = QCoreApplication::applicationDirPath().split("/").at(0);
    path.append( "/");

    progressBar = new Progress();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100); // in percent
    progressBar->setTextVisible(true);

    connect(downloadManager, SIGNAL(prgUpdateSig(int,QString)), progressBar, SLOT(progress(int,QString)));
    connect(downloadManager,SIGNAL(downloadComplete()),this, SLOT(slotDisconnected()));
    downloadManager->mManager = manager;

    setupActions();
    setupMenus();
    setupUi();

    //--------------------
    QByteArray answer = GET(QUrl("https://oauth.vk.com/token?grant_type=password&client_id=3697615&client_secret=AlVXZFMUqyrnABp8ncuU&username="+m_login+"&password="+m_parol+"&v=5.28"));
    if (answer==""){
        ERROR(0," Token GET return '' ");
    }else{
        m_token = JSON::parse(answer).toMap().value("access_token").toString(); // parse to list of objects Audio
        slotDownload();
    }
}

int MainWindow::ERROR(int type, QString msg)
{
            switch (type) {
            case 0:{
                messageBox.setText(msg);
                messageBox.exec();
                return 0;
            }
            case 1:{
                break;
            }
            case 2:{
                break;
            }
            case 3:{
                break;
            }
            }
}

void MainWindow::sslErrorHandler(QNetworkReply *qnr, const QList<QSslError> &errlist)
{
    qnr->ignoreSslErrors();
}

void MainWindow::saveDownloadList()
{
    QFile* f = new QFile();
    f->setFileName(QCoreApplication::applicationDirPath().split("/").at(0)+"/downloadList.xml");
    f->open(QIODevice::ReadWrite);

    QXmlStreamWriter* stream = new QXmlStreamWriter(f);
    stream->setAutoFormatting(true);
    stream->writeStartDocument();
    stream->writeStartElement("downloads");
    for (int i=0; i<audios.count(); i++){
        stream->writeStartElement("download");
        stream->writeAttribute("id",audios.at(i).id);
        //stream.writeAttribute("title",audios.at(i).title);
        //stream.writeAttribute("artist",audios.at(i).artist);
        //stream.writeAttribute("duration",audios.at(i).duration);
        //stream.writeAttribute("url",audios.at(i).url);
        stream->writeAttribute("owner",audios.at(i).owner);
        stream->writeEndElement();
    }
    stream->writeEndElement();
    stream->writeEndDocument();
    f->close();
    f->deleteLater();
    delete stream;
}

void MainWindow::loadDownloadList(QString filename)
{
    QFile* f = new QFile();
    f->setFileName(filename);
    f->open(QIODevice::ReadWrite);
    QXmlStreamReader* stream = new QXmlStreamReader(f);
    while (!stream->atEnd()) {
             stream->readNextStartElement();
             if (stream->isStartElement()){
                 Audio audio;
                 audio.id = stream->attributes().value("id").toString();
                 audio.owner = stream->attributes().value("id").toString();
                 audios.append(audio);
             }
       }
       if (stream->hasError()) {

       }
    f->close();
    f->deleteLater();
    delete stream;
}

void MainWindow::addAudios(QString id, QString count)
{
    QUrl * current = new QUrl("https://api.vk.com/method/audio.get");
    current->addQueryItem("v","5.2");
    current->addQueryItem("access_token",m_token);
    if (m_userGroup=="Group"){
            current->addQueryItem("gid",m_user);
        }
    else if (m_userGroup=="User") {
        current->addQueryItem("uid",m_user);
    }

    current->addQueryItem("count",count);
    current->addQueryItem("offset",m_countTo);

    QVariantList List = JSON::parse(GET(*current)).toMap().value("response").toMap().value("items").toList(); // parse to list of objects Audio
    delete current;

    for (int i=0; i<List.size(); i++)
    {
        QVariantMap current = List[i].toMap();
        Audio au;
        au.id = current.value("id").toString();
        au.title = current.value("title").toString();
        au.artist = current.value("artist").toString();
        au.duration = current.value("duration").toString();
        au.url = current.value("url").toString();
        au.owner = m_user;
        audios.append(au);

        //current.~QMap();
    }
    //List.~QList();
}

void MainWindow::fillTable()
{
    /* for table
    for (int i = 0; i<audios.count(); i++){
        QStandardItem *firstRow ;
        firstRow = new QStandardItem(audios.at(i).title);
        model->setItem(i,0,firstRow);
        firstRow = new QStandardItem(audios.at(i).artist);
        model->setItem(i,1,firstRow);
        firstRow = new QStandardItem(audios.at(i).duration);
        model->setItem(i,2,firstRow);
        firstRow = new QStandardItem(audios.at(i).id);
        model->setItem(i,3,firstRow);
        firstRow = new QStandardItem(audios.at(i).url);
        model->setItem(i,4,firstRow);
    }
    */
    QString text;
    for (int i = 0; i<audios.count(); i++){
        QStandardItem *firstRow ;
        text.append(QString::number(i+1));
        text.append(". ");
        text.append(audios.at(i).artist);
        text.append(" - ");
        text.append(audios.at(i).title);

        firstRow = new QStandardItem(text);
        model->setItem(i,0,firstRow);
        text.clear();

        /*
        model->setItem(i,0,firstRow);
        firstRow = new QStandardItem(audios.at(i).artist);
        model->setItem(i,1,firstRow);
        firstRow = new QStandardItem(audios.at(i).duration);
        model->setItem(i,2,firstRow);
        firstRow = new QStandardItem(audios.at(i).id);
        model->setItem(i,3,firstRow);
        firstRow = new QStandardItem(audios.at(i).url);
        model->setItem(i,4,firstRow);
        */
    }


}
