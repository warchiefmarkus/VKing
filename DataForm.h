#ifndef DATAFORM_H
#define DATAFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QtGui>
#include <QCheckBox>
#include <QComboBox>

class DataForm : public QWidget
{
    Q_OBJECT
public:
    DataForm();

    QLineEdit* login ;
    QLineEdit* parol ;
    QLineEdit* count ;
    QLineEdit* countTo ;
    QLineEdit* user ;
    QLineEdit* group;
    QCheckBox* checkBox;
    QComboBox* comboBox;



signals:
    void run(QString login, QString parol, QString user, QString count, QString countTo,  QString group, bool del);
public slots:
    void send();
};

#endif
