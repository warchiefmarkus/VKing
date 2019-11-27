#include <QtGui>
#include "DataForm.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QCompleter>

DataForm::DataForm()
{
    setGeometry(0,0,240,320);

    login = new QLineEdit(this);
    parol = new QLineEdit(this);
    count = new QLineEdit(this);
    countTo = new QLineEdit(this);
    user = new QLineEdit(this);
    checkBox = new QCheckBox(this);
    comboBox = new QComboBox(this);

    QStringList wordList;
    wordList << "63000784" << "201789376" << "37376822";
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    user->setCompleter(completer);

    QLabel* label1 = new QLabel(this);
    QLabel* label2 = new QLabel(this);
    QLabel* label3 = new QLabel(this);
    QLabel* label6 = new QLabel(this);
    QLabel* label7 = new QLabel(this);

    label1->setGeometry(5,0,30,25); label1->setText("login");
    label2->setGeometry(5,30,30,25); label2->setText("parol");
    label3->setGeometry(5,60,30,25); label3->setText("count");

    label7->setGeometry(5,90,30,25); label7->setText("offset");

    comboBox->setGeometry(5,120,80,25); comboBox->addItem("User"); comboBox->addItem("Group");

    label6->setGeometry(5,150,30,25); label6->setText("delete");
    checkBox->setGeometry(35,150,200,25); checkBox->setCheckState(Qt::Unchecked);

    login->setGeometry(35,0,200,25); login->setText("380678400259");
    parol->setGeometry(35,30,200,25); parol->setText("warhammer");
    count->setGeometry(35,60,200,25); count->setText("100");

    countTo->setGeometry(35,90,200,25); countTo->setText("0");

    user->setGeometry(90,120,145,25);  user->setText("63000784"); // themarkusfenix - 201789376 - 63000784

    QPushButton* start = new QPushButton(this);
    start->setGeometry(5,180,230,50); start->setText("Start");
    connect(start,SIGNAL(clicked()),this,SLOT(send()));
}

void DataForm::send(){
    emit run(login->text(),parol->text(), user->text(), count->text(), countTo->text(), comboBox->currentText(), checkBox->isChecked());
    //this->hide();
    this->deleteLater();
}
