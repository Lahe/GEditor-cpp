#include "include/processhandler.h"

void ProcessHandler::StartProcess(QString process, QStringList& params) {
    qDebug() << process;
    qDebug() << params;
    myProcess = new QProcess(this);
    connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    myProcess->start(process, params);
    myProcess->waitForFinished();
}

void ProcessHandler::readStandardError() {
    QByteArray processOutput;
    processOutput = myProcess->readAllStandardError();

    qDebug().noquote() << "Conversion successful" << QString(processOutput);
}