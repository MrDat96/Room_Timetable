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
    currentDate = QDate::currentDate();
    clickedTimeStamp = -1;
    //currentDate = QDate::fromString("2016-09-21","yyyy-MM-dd");
}

//format name of user to standard
QString Timetable::formatPersonalName(QString name) {
    name = name.toLower();
    for(int i = 0; i < name.length(); i++) {
        if (i == 0 || name.at(i - 1) == ' ' || name.at(i-1) == '-')
            name[i] = name[i].toUpper();
    }
    return name;
}

//init table every time receive new data
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

    //set column width equals each other
    QHeaderView* Qheader = ui->tableWidget->horizontalHeader();
    Qheader->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);

    QHeaderView* Qheader2 = ui->tableWidget_2->horizontalHeader();
    Qheader2->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->verticalHeader()->setVisible(false);

    //set style for header of tab;le
    QString styleSheet = "::section {"
                         "border: 1px solid red;"
                         "border-radius: 5;"
                         "margin: 1px;"
                         "text-align: right;"
                         "font-family: arial;"
                         "font-size: 12px; }";
    ui->tableWidget->horizontalHeader()->setStyleSheet(styleSheet);
    ui->tableWidget_2->horizontalHeader()->setStyleSheet(styleSheet);
}

//init api and get data from it, create a sinal when it done, it will call onresult function
void Timetable::initNetWork() {
    ui->lineEdit->setText(currentDate.toString("yyyy-MMM-dd (dddd)"));
    QString url = "https://f0vafga1w2.execute-api.ap-southeast-1.amazonaws.com/api/venue/08-06-09/date/";
    QString curStrDate = currentDate.toString("yyyy-MM-dd");
    url.append(curStrDate);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
              this, SLOT(onResult(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(url)));
}

//insert data to one cell
void Timetable::insertInforMorning(int32_t row, int32_t col, QString subject, QString teacher_name) {
    QString str = "<b>" + subject + "</b>" + "<br><font color=green size=1>" + formatPersonalName(teacher_name) + "</font>";
    QTextEdit *text = new QTextEdit(str);
    text->setAlignment(Qt::AlignHCenter);
    ui->tableWidget->setCellWidget(row, col, text);
    ui->tableWidget->resizeRowToContents(0);
}
void Timetable::insertInforAfternoon(int32_t row, int32_t col, QString subject, QString teacher_name) {
    QString str = "<b>" + subject + "</b>" + "<br><font color=green size=1>" + formatPersonalName(teacher_name) + "</font>";
    QTextEdit *text = new QTextEdit(str);
    text->setAlignment(Qt::AlignHCenter);
    ui->tableWidget_2->setCellWidget(row, col, text);
    ui->tableWidget_2->resizeRowToContents(0);
}

void Timetable::onResult(QNetworkReply *reply) {
    //Remove first row of tables
    ui->tableWidget->removeRow(0);
    ui->tableWidget_2->removeRow(0);
    initTable();

    //Read data return on reply
    QByteArray bytes = reply->readAll();
    QString data = bytes;
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << QVariant(statusCode).toString();

    if (statusCode != 200) {
        //connection error
        ui->label_3->setText("<font color=red> Connection error </font>");
    } else {
        ui->label_3->setText("");
    }

    QTextStream out(stdout);
    out << "MrDat : " <<  bytes  << endl;

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonArray jsonArray = doc.array();


    insertInforMorning(0,0,"MORNING","");
    insertInforAfternoon(0, 0, "AFTERNOON", "");
    // array time[i] = j mean the arrangement from i 'oclock to j 'oclock
    int time[24];
    for(int i = 0; i < 24; i++) time[i] = i + 1;
    bool isFirst = true;
    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();

        if (isFirst) {
            //QString strDate = obj["date"].toString();
            //QDate date = QDate::fromString(strDate,"yyyy-MM-dd");
            //qDebug() << date.toString("yyyy-MMM-dd (dddd)");
            //ui->lineEdit->setText(date.toString("yyyy-MMM-dd (dddd)"));
            ui->lineEdit_2->setText(obj["venue_name"].toString());
            isFirst = false;
        }

        QTime startTime = QTime::fromString(obj["start_time"].toString(), "h:mm:ss");
        QTime endTime = QTime::fromString(obj["end_time"].toString(), "h:mm:ss");

        time[startTime.hour()] = endTime.hour();
        qDebug() << "Time : " <<startTime.hour() << " " << endTime.hour();

        //if the arrangment throught the noon
        if (startTime.hour() <= 12 && endTime.hour() > 12 && endTime.hour() > time[13]) {
            time[13] = endTime.hour();
            insertInforAfternoon(0,1,obj["purpose"].toString(),obj["user"].toString());
        }

        if (startTime.hour() <= 12) {
            insertInforMorning(0,startTime.hour() - 7,obj["purpose"].toString(),obj["user"].toString());
        } else
            insertInforAfternoon(0,startTime.hour() - 12,obj["purpose"].toString(),obj["user"].toString());

    }

    //setSpan tables when arrangement >= 2 hours
    int index = 8;
    while( index < 18) {
        qDebug() << QVariant(index).toString() << " " << QVariant(time[index]).toString();

        if (index <= 12) {
            if (time[index] - index > 1)
                ui->tableWidget->setSpan(0,index-7,1,time[index]-index);
            index = time[index];
            if (index > 12) index = 13;
        } else {
            if (time[index] - index > 1)
            ui->tableWidget_2->setSpan(0, index - 12, 1, time[index] - index);
            index = time[index];
        }
    }
    reply->deleteLater();
}

//listening for user press button
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
        bool press = false;
        int valueBack = digitalRead(0);
        if (valueBack && valueBack != prevValueBack) {
            //qDebug() << "Press";
            press = true;
            move("previous");
        }
        prevValueBack = valueBack;

        int valueNext = digitalRead(1);
        if (valueNext && valueNext != prevValueNext) {
            //qDebug() << "Press";
            press = true;
            move("next");
        }
        if (!press) checkMove();
        prevValueNext = valueNext;
        QApplication::processEvents();
    }
}

//move the day (next || previous)
void Timetable::move(QString move) {
    if (move == "next") {
        qDebug() << "Move next";
        on_pushButton_2_clicked();
    } else if (move == "previous") {
        qDebug() << "Previous";
        on_pushButton_clicked();
    }
}

Timetable::~Timetable()
{
    delete ui;
}

//move next date
void Timetable::on_pushButton_2_clicked()
{
    currentDate = currentDate.addDays(1);
    clickedTimeStamp = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "MrDAT";
    qDebug() <<"Time clicked : " << clickedTimeStamp;
    initNetWork();
}
//move previous date
void Timetable::on_pushButton_clicked()
{
    currentDate = currentDate.addDays(-1);
    clickedTimeStamp = QDateTime::currentMSecsSinceEpoch();
    qDebug() <<"Time clicked : " << clickedTimeStamp;
    initNetWork();
}

//Check for come back to current date
void Timetable::checkMove() {
    if (clickedTimeStamp == -1) return;
    else {
        qint64 currentTimeStamp = QDateTime::currentMSecsSinceEpoch();
        if (currentTimeStamp - clickedTimeStamp > 20 * 1000) {
            // 20s from clicked
            clickedTimeStamp = -1;
            currentDate = QDate::currentDate();
            initNetWork();
        }
    }
}
