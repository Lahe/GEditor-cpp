#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void importFile();
    void importLut();
    void applyFunction();
    void previewOutput();
    void convertToMp3();
    void saveAsGif();
    void stabilizeVideo();
    void executeCustom();
    void changeCodec();
    void saveToFile();
    void tabSelected();

private:
    Ui::MainWindow *ui;
    QHash<QString, QString> ffmpegInputs;
    QString inPath;
    QString lutPath;
    QString outPath;
    QStringList vCodec;
    QStringList aCodec;
    QString extension;
    QString preset;

private:
    void appendLutParams(QStringList &params);
    void appendSpeedParams(QStringList &params);
    void appendExtractParams(QStringList &params);
    void appendScaleParams(QStringList &params);
};

#endif // MAINWINDOW_H
