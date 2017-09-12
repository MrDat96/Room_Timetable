#include "timetable.h"
#include "ui_timetable.h"
extern "C" {
#include <wiringPi.h>
#include <pthread.h>
}
#include <QApplication>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QPushButton>
#include <QtNetwork>
#include <QDialog>
#include <QDebug>
#include <QTextStream>

Timetable::Timetable(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Timetable)
{
    ui->setupUi(this);
}

void Timetable::initTable() {
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(1);
    ui->tableWidget_2->setColumnCount(6);
    ui->tableWidget_2->setRowCount(1);

    QStringList header;
    header<<"TIME"<<"8:00 AM"<<"9:00 AM"<<"10:00 AM"<<"11:00 AM"<<"12:00 AM" ;
    ui->tableWidget->setHorizontalHeaderLabels(header);

    QStringList header2;
    header2<<"TIME"<<"1:00 PM"<<"2:00 PM"<<"3:00 PM"<<"4:00 PM"<<"5:00 PM" ;
    ui->tableWidget_2->setHorizontalHeaderLabels(header2);

    //set column width equals
    QHeaderView* Qheader = ui->tableWidget->horizontalHeader();
    Qheader->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);


    QHeaderView* Qheader2 = ui->tableWidget_2->horizontalHeader();
    Qheader2->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->verticalHeader()->setVisible(false);

    QString styleSheet = "::section {"
                         "border: 1px solid red;"
                         "border-radius: 5;"
                         "margin: 1px;"
                         "text-align: right;"
                         "font-family: arial;"
                         "font-size: 12px; }";
    ui->tableWidget->horizontalHeader()->setStyleSheet(styleSheet);
    ui->tableWidget_2->horizontalHeader()->setStyleSheet(styleSheet);

    //ui->tableWidget->setCellWidget(0,1,text);
    //ui->tableWidget->setCellWidget(0,4, new QTextEdit("Mr<br><br> Dat"));
    ui->tableWidget->resizeRowToContents(0);
    //ui->tableWidget->setSpan(0,1,1,2);
}

void Timetable::initNetWork() {
    QString url = "http://188.166.247.154/atk-ble/api/web/index.php/v1/venue/timetable?venue=%2708-06-09%27&semester=3";
    QString url2 = "http://genk.vn/";
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
              this, SLOT(onResult(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(url)));
}

void Timetable::insertInforMorning(int32_t row, int32_t col, QString subject, QString teacher_name, QString class_section) {
    QString str = "<b>" + subject + "</b> <br>" + teacher_name + "<br>" + class_section;
    QTextEdit *text = new QTextEdit(str);
    text->setAlignment(Qt::AlignHCenter);
    ui->tableWidget->setCellWidget(row, col, text);
    ui->tableWidget->resizeRowToContents(0);
}
void Timetable::insertInforAfternoon(int32_t row, int32_t col, QString subject, QString teacher_name, QString class_section) {
    QString str = "<b>" + subject + "</b> <br>" + teacher_name + "<br>" + class_section;
    QTextEdit *text = new QTextEdit(str);
    text->setAlignment(Qt::AlignHCenter);
    ui->tableWidget_2->setCellWidget(row, col, text);
    ui->tableWidget_2->resizeRowToContents(0);
}

void Timetable::onResult(QNetworkReply *reply) {
    qDebug() << "Request done start";

    QByteArray bytes = reply->readAll();
    QString data = bytes;
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << QVariant(statusCode).toString();

    QTextStream out(stdout);
    out << "MrDat : " <<  bytes  << endl;

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonArray jsonArray = doc.array();


    insertInforMorning(0,0,"MORNING","", "");
    insertInforAfternoon(0, 0, "AFTERNOON", "", "");
    int time[24];
    for(int i = 0; i < 24; i++) time[i] = i;
    bool isFirst = true;
    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();
      if (obj["ldate"].toString() == "2017-09-19" || obj["ldate"].toString() == "2017-10-31") {
        if (isFirst) {
            QString strDate = obj["ldate"].toString();
            QDate date = QDate::fromString(strDate,"yyyy-MM-dd");
            qDebug() << date.toString("yyyy-MMM-dd (dddd)");
            ui->lineEdit->setText(date.toString("yyyy-MMM-dd (dddd)"));
            ui->lineEdit_2->setText(obj["location"].toString());
        }
        //qDebug()<< "Name : \""<< i++ << obj["name"].toString() << "\"DAT" << obj["catalog_number"].toString();
        QTime startTime = QTime::fromString(obj["start_time"].toString(), "hh:mm:ss");
        QTime endTime = QTime::fromString(obj["end_time"].toString(), "hh:mm:ss");

        time[startTime.hour()] = endTime.hour();

        if (startTime.hour() < 12 && endTime.hour() > 12)
            time[12] = endTime.hour();
        if (startTime.hour() < 12)
            insertInforMorning(0,startTime.hour() - 7,obj["subject_area"].toString(),"MrDAT",obj["class_section"].toString());
        else
            insertInforAfternoon(0,startTime.hour() - 12,obj["subject_area"].toString(),"MrDAT",obj["class_section"].toString());
      }
    }

    //setSpan
    int index = 8;
    while( index < 24) {
        qDebug() << QVariant(index).toString() << " " << QVariant(time[index]).toString() <<endl;
        if (index < 12) {
            ui->tableWidget->setSpan(0,index-7,1,time[index]-index);
            index = time[index] + 1;
            if (index > 12) index = 12;
        } else {
            ui->tableWidget_2->setSpan(0, index - 12, 1, time[index] - index);
            index = time[index] + 1;
        }
    }
    reply->deleteLater();
}

void Timetable::waiting() {
    if (wiringPiSetup() == -1) {
        qDebug() << "Setup wire fail";
        return;
    }
    pinMode(0, INPUT);
    int prevValueBack = 0;
    int prevValueNext = 0;
    while(1) {
        //qDebug() << "Running";
        int valueBack = digitalRead(0);
        if (valueBack && valueBack != prevValueBack) {
            //qDebug() << "Press";
            move("previous");
        }
        prevValueBack = valueBack;

        int valueNext = digitalRead(1);
        if (valueNext && valueNext != prevValueNext) {
            //qDebug() << "Press";
            move("next");
        }
        prevValueNext = valueNext;
        QApplication::processEvents();
    }
}

void Timetable::move(QString move) {
    if (move == "next") {
        qDebug() << "Move next";
    } else if (move == "previous") {
        qDebug() << "Previous";
    }
}

Timetable::~Timetable()
{
    delete ui;
}

void Timetable::on_pushButton_2_clicked()
{
    move("next");
}

void Timetable::on_pushButton_clicked()
{
    move("previous");
}
