#include <QtGui>
#include "DataForm.h"
#include "mainwindow.h"
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QApplication>


void myMessageHandler(QtMsgType type, const char *msg)
{
    QString path = QCoreApplication::applicationDirPath().split("/").at(0);
    path.append( "/");

        QString txt;
        switch (type) {
        case QtDebugMsg:
                txt = QString("Debug: %1").arg(msg);
                break;
        case QtWarningMsg:
                //txt = QString("Warning: %1").arg(msg);
        break;
        case QtCriticalMsg:
                txt = QString("Critical: %1").arg(msg);
        break;
        case QtFatalMsg:
                txt = QString("Fatal: %1").arg(msg);
                abort();
        }
        QFile outFile(path+"VKlog.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qInstallMsgHandler(myMessageHandler);
    MainWindow window;
    //window.showFullScreen();
    window.showMaximized();

    return app.exec();
}
