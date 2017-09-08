#include "timetable.h"
#include "ui_timetable.h"

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
    header<<"Morning"<<"8:00 AM"<<"9:00 AM"<<"10:00 AM"<<"11:00 AM"<<"12:00 AM" ;
    ui->tableWidget->setHorizontalHeaderLabels(header);

    QStringList header2;
    header2<<"Afternoon"<<"1:00 PM"<<"2:00 PM"<<"3:00 PM"<<"4:00 PM"<<"5:00 PM" ;
    ui->tableWidget_2->setHorizontalHeaderLabels(header2);

    //ui->tableWidget->setFixedWidth(ui->tableWidget->width() / 6);
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //table = new QTableWidget(this);
    //    table->setRowCount(3);
    //    table->setColumnCount(6);

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

    QTextEdit *text = new QTextEdit("Mr \r\n Dat", NULL);
    QString str = "<div>Mr Dat <br> ESO</div>";
    text->setText(str);
    text->setAlignment(Qt::AlignHCenter);

    ui->tableWidget->setCellWidget(0,1,text);
    ui->tableWidget->setCellWidget(0,4, new QTextEdit("Mr<br><br> Dat"));
    ui->tableWidget->resizeRowToContents(0);
    ui->tableWidget->setSpan(0,1,1,2);
}

void Timetable::initNetWork() {
    QString url = "http://188.166.247.154/atk-ble/api/web/index.php/v1/venue/timetable?venue=%2708-06-09%27&semester=3";
    QString url2 = "http://genk.vn/";
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
              this, SLOT(onResult(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl(url)));
//
//    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

//    QNetworkReply* rep = nam->get(QNetworkRequest(QUrl(url)));

//    while (!rep->isFinished()) {
//        qApp->processEvents();
//    }
//    QByteArray byteData = rep->readAll();
//    QString data = byteData;
//    qDebug() << "Finished";
//    qDebug() << "Finished" << data;

//    QNetworkAccessManager networkManager;
//    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));

//    QUrl url("http://188.166.247.154/atk-ble/api/web/index.php/v1/venue/timetable?venue=%2708-06-09%27&semester=3");
//    QNetworkRequest request;
//    request.setUrl(url);
//    QNetworkReply *reply = networkManager.get(request);
//    QString data = (QString) reply->readAll();
//    qDebug() << "Requested" << data;
}

void Timetable::insertInfor(int32_t row, int32_t col, QString subject, QString teacher_name, QString class_section) {
    QString str = "<b>" + subject + "</b> <br>" + teacher_name + "<br>" + class_section;
    QTextEdit *text = new QTextEdit(str);
    text->setAlignment(Qt::AlignHCenter);
    ui->tableWidget->setCellWidget(row, col, text);
}

void Timetable::onResult(QNetworkReply *reply) {
    qDebug() << "Request done start";

    QByteArray bytes = reply->readAll();
    QString data = bytes;
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << reply->isFinished();
    qDebug() << reply->error();
    qDebug() << QVariant(statusCode).toString();
    qDebug() << "Str : " << QVariant(str).toString();

    QTextStream out(stdout);
    out << "MrDat : " <<  bytes  << endl;

    //ui->tableWidget->setCellWidget(0,0, new QTextEdit(data));
    qDebug() << "Request done";

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonArray jsonArray = doc.array();
    for (auto it = jsonArray.constBegin(); it != jsonArray.constEnd(); ++it)
    {
        const QJsonValue &val = *it;
        QJsonObject o = val.toObject();
        QVariantMap json_map = o.toVariantMap();
        if (json_map["ldate"] == "2017-10-31") {
            qDebug()<<json_map["ldate"].toString();
            qDebug()<<json_map["name"].toString();
            qDebug()<<json_map["facility"].toString();
            qDebug()<<json_map["subject_area"].toString();
            qDebug()<<json_map["start_time"].toString();
            qDebug()<<json_map["end_time"].toString();
            insertInfor(0,0,json_map["subject_area"].toString(), "MR.DAT",json_map["class_section"].toString());
        }
        // Iterate over all sub-objects. They all have string values.
//        for (auto oIt = o.constBegin(); oIt != o.constEnd(); ++oIt)
//        {
//            // "MCC_Dealer":'test'
//            qDebug() << "Key:" << oIt.key() << ", Value:" << oIt.value().toString();
//        }
    }

//        // check validity of the document
//        if(!doc.isNull())
//        {
//            if(doc.isArray())
//            {
//                qDebug() << "Array";
//                QJsonArray jsonArray = doc.array();
//                qint32 i = 0;
//                foreach (const QJsonValue & value, jsonArray)
//                    {
//                        //qDebug() << "Name :" << value.toString();
//                        QJsonObject obj = value.toObject();
//                        qDebug()<< "Name : \""<< i++ << obj["name"].toString() << "\"DAT" << obj["catalog_number"].toString();
//                    }
//            }
//            else
//            {
//                qDebug() << "Document is not an object" <<endl;
//            }
//        }
//        else
//        {
//            qDebug() << "Invalid JSON...\n" << "HIHI" << endl;
//        }

    reply->deleteLater();
}

Timetable::~Timetable()
{
    delete ui;
}
