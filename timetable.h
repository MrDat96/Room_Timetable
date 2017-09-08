#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <QMainWindow>
#include <QTableWidget>
#include <QtNetwork>
namespace Ui {
class Timetable;
}

class Timetable : public QMainWindow
{
    Q_OBJECT

public:
    explicit Timetable(QWidget *parent = 0);
    void    initTable();
    void    initNetWork();
    void    insertInfor(int32_t row, int32_t col, QString subject, QString teacher_name, QString class_section);
    ~Timetable();

private slots:
    void onResult(QNetworkReply* reply);

private:
    Ui::Timetable *ui;
    QTableWidget *table;
};

#endif // TIMETABLE_H
