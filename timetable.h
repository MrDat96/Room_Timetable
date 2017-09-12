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
    void    insertInforMorning(int32_t row, int32_t col, QString subject, QString teacher_name, QString class_section);
    void    insertInforAfternoon(int32_t row, int32_t col, QString subject, QString teacher_name, QString class_section);
    void    move(QString move);
    void    getPressButton(Timetable w);
    static void *listening(void *vargp);
    void    waiting();
    ~Timetable();

private slots:
    void onResult(QNetworkReply* reply);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Timetable *ui;
    QTableWidget *table;
};

#endif // TIMETABLE_H
