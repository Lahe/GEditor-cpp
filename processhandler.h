#ifndef GEDITOR_PROCESSHANDLER_H
#define GEDITOR_PROCESSHANDLER_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class ProcessHandler : public QObject {
Q_OBJECT
public:
    ProcessHandler() : QObject() {};

    void StartProcess(QString process, QStringList& params);

private slots:

    void readStandardError();

private:
    QProcess *myProcess{};
};


#endif //GEDITOR_PROCESSHANDLER_H
