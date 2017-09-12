#include "timetable.h"
#include <QApplication>
extern "C" {
#include <wiringPi.h>
#include <pthread.h>
}

static void *listening(void *vargp) {
    //Timetable *w = (Timetable *) vargp;
    int prevValueBack = 0;
    int prevValueNext = 0;
    while(1) {
        //qDebug() << "Running";
        int valueBack = digitalRead(0);
        if (valueBack && valueBack != prevValueBack) {
           // qDebug() << "Press";
           // (*w).move("previous");
            ((Timetable *)vargp)->move("previous");
        }
        prevValueBack = valueBack;

        int valueNext = digitalRead(0);
        if (valueNext && valueNext != prevValueNext) {
            qDebug() << "Press";
           // (*w).move("next");
        }
        prevValueNext = valueNext;

       // delay(100);
    }
}


void getPressButton(Timetable w) {
    qDebug() <<"Before thread";
    qDebug() << "Waiting for button press";
    if (wiringPiSetup() == -1) {
        qDebug() << "Setup wire fail";
        return;
    }
    pinMode(0, INPUT);
    w.move("next");
    //pthread_t tid;
    //pthread_create(&tid, NULL, listening, &w);
    //pthread_join(tid, NULL);
    qDebug() << "After Thread";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Timetable w;
    w.initTable();
    w.initNetWork();
    w.show();
    w.waiting();
    //w.move("next");
    //getPressButton(w);
    return a.exec();
}
