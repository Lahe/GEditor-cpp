#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QFontDatabase>
#include "include/processhandler.h"
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QFontDatabase::addApplicationFont("resources/ProximaNova_rg.ttf");
}

MainWindow::~MainWindow() {
    delete ui;
}

static void ffmpegSubprocess(QStringList &paramList) {
    ProcessHandler handler;
    handler.startProcess("bin/ffmpeg.exe", paramList);
    qDebug() << "   _____ ____  __  __ _____  _      ______ _______ ______ _____  \n"
                "  / ____/ __ \\|  \\/  |  __ \\| |    |  ____|__   __|  ____|  __ \\ \n"
                " | |   | |  | | \\  / | |__) | |    | |__     | |  | |__  | |  | |\n"
                " | |   | |  | | |\\/| |  ___/| |    |  __|    | |  |  __| | |  | |\n"
                " | |___| |__| | |  | | |    | |____| |____   | |  | |____| |__| |\n"
                "  \\_____\\____/|_|  |_|_|    |______|______|  |_|  |______|_____/ ";
}

static void ffplaySubprocess(QStringList &paramList) {
    ProcessHandler handler;
    handler.startProcess("bin/ffplay.exe", paramList);
}

void MainWindow::appendLutParams(QStringList &params) {
    QString lutPreviewPath = ffmpegInputs.value("lut");
    params << QString("lut3d=%1").arg(lutPreviewPath.replace(0, 1, R"(C\\)"));
}

void MainWindow::appendSpeedParams(QStringList &params) {
    float percent = ffmpegInputs.value("speed").toFloat();
    float frames = 1 / (percent / 100);
    params << QString("setpts=%1*PTS").arg(frames);
}

void MainWindow::appendExtractParams(QStringList &params) {
    int startTime = ffmpegInputs.value("tStart").toInt();
    int endTime = ffmpegInputs.value("tEnd").toInt();
    int duration = endTime - startTime;
    params << "-ss" << QString::number(startTime) << "-t" << QString::number(duration);
}

void MainWindow::appendScaleParams(QStringList &params) {
    params << QString("scale=%1:%2").arg(ffmpegInputs.value("scaleW"), ffmpegInputs.value("scaleH"));
}

void MainWindow::changeCodec() {
    QString vCodecText = ui->vCodecDrop->currentText();
    QString aCodecText = ui->aCodecDrop->currentText();
    vCodec.clear();
    aCodec.clear();
    if (vCodecText == "default") {
        extension = QFileInfo(inPath).suffix();
        vCodec << "-c:v" << "copy";
    } else if (vCodecText == "x264 (mp4) (optimal)" or vCodecText == "x265 (mp4)") {
        QString codec;
        if (vCodecText.contains("x264")) {
            codec = "libx264";
        } else if (vCodecText.contains("x265")) {
            codec = "libx265";
        }
        extension = "mp4";
        if (ui->x264Preset->currentText() == "medium (Default)") {
            preset = "medium";
        } else {
            preset = ui->x264Preset->currentText();
        }
        vCodec << "-c:v" << codec << "-preset" << preset << "-crf" << QString::number(ui->x264Var->value())
               << "-pix_fmt" << "yuv420p";
    } else if (vCodecText == "VP8 (webm)") {
        extension = "webm";
        vCodec << "-c:v" << "libvpx" << "-deadline" << "realtime" << "-b:v" << QString("%1k").arg(ui->VP8Var->value())
               << "-pix_fmt" << "yuv420p";
    } else if (vCodecText == "Xvid (avi)") {
        extension = "avi";
        vCodec << "-c:v" << "libxvid" << "-qscale:v" << QString::number(ui->XvidVar->value());
    } else if (vCodecText == "H.264 NVENC (mp4)" or vCodecText == "HEVC NVENC (mp4)") {
        QString codec;
        if (vCodecText.contains("H.264")) {
            codec = "h264_nvenc";
        } else if (vCodecText.contains("HEVC")) {
            codec = "hevc_nvenc";
        }
        extension = "mp4";
        preset = ui->NVENCPresets->currentText().split(" ")[0];
        vCodec << "-c:v" << codec << "-preset" << preset << "-b:v" << QString("%1k").arg(ui->NVENCVar->value())
               << "-pix_fmt" << "yuv420p";
    }

    // Audio codecs
    if (aCodecText == "default") {
        aCodec << "-c:a" << "copy";
    } else if (aCodecText == "AAC") {
        aCodec << "-c:a" << "aac" << "-ac" << "2" << "-b:a" << QString("%1k").arg(ui->AACBitrateVar->value());
    } else if (aCodecText == "MP3") {
        aCodec << "-c:a" << "libmp3lame" << "-qscale:a" << QString::number(ui->mp3Value->value());
    } else if (aCodecText == "Vorbis") {
        aCodec << "-c:a" << "libvorbis" << "-qscale:a" << QString::number(ui->vorbisValue->value());
    }
    if (vCodec.contains("libvpx") and
        (aCodec.contains("aac") or aCodec.contains("libmp3lame") or aCodec.contains("copy")))
        QMessageBox::warning(this, "Error", "VP8 video codec can only use Vorbis audio codec.");
    else {
        QString filter;
        if (extension == "mp4")
            filter = "MP4 files (*.mp4)";
        else if (extension == "avi")
            filter = "AVI files (*.avi)";
        else if (extension == "webm")
            filter = "WebM files (*.webm)";
        outPath = QFileDialog::getSaveFileName(this, "Save File", "", filter);
        QStringList params = {"-hwaccel", "auto", "-y", "-loglevel", "debug", "-i", inPath};
        params << vCodec << aCodec << outPath;
        ffmpegSubprocess(params);
    }
}

void MainWindow::importFile() {
    qDebug() << "Import File clicked";
    inPath = QFileDialog::getOpenFileName(this, "Import File", "",
                                          "All supported formats (*.mp4 *.webm *.avi *.mp3 *.ts *.mkv);;"
                                          "MP4 files (*.mp4);;"
                                          "WebM files (*.webm);;"
                                          "AVI files (*.avi)");
    ui->importField->setText(inPath);
    qDebug() << inPath;
}

void MainWindow::importLut() {
    qDebug() << "Import LUT clicked";
    lutPath = QFileDialog::getOpenFileName(this, "Import LUT", "", "LUT Files (*.cube)");
    ui->lutPath->setText(lutPath);
    qDebug() << lutPath;
}

void MainWindow::applyFunction() {
    qDebug() << "Apply function clicked";
    if (ui->importField->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Import not specified!");
    } else {
        QString currentTab = ui->functionTabs->currentWidget()->objectName();
        qDebug() << currentTab;
        if (currentTab == "lutTab") {
            if (lutPath.isEmpty()) {
                QMessageBox::warning(this, "Error", "LUT not specified!");
                return;
            }
            ffmpegInputs.insert("lut", lutPath);
            ui->confirmationLabel->setText("LUT changes applied.");
        } else if (currentTab == "speedTab") {
            ffmpegInputs.insert("speed", QString::number(ui->speedVar->value()));
            ui->confirmationLabel->setText("Speed changes applied.");
        } else if (currentTab == "extractTab") {
            ffmpegInputs.insert("tStart",
                                QString::number(
                                        ui->extractStartMinVar->value() * 60 + ui->extractStartSecVar->value()));
            ffmpegInputs.insert("tEnd",
                                QString::number(ui->extractEndMinVar->value() * 60 + ui->extractEndSecVar->value()));
            ui->confirmationLabel->setText("Extract changes applied.");
        } else if (currentTab == "scaleTab") {
            ffmpegInputs.insert("scaleW", QString::number(ui->widthVar->value()));
            ffmpegInputs.insert("scaleH", QString::number(ui->heightVar->value()));
            ui->confirmationLabel->setText("Scale changes applied.");
        }
    }
}

void MainWindow::previewOutput() {
    qDebug() << "Preview output clicked";
    if (ui->importField->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Import not specified!");
    } else {
        if (!ffmpegInputs.isEmpty()) {
            QString currentTab = ui->functionTabs->currentWidget()->objectName();
            QStringList params = {"-i", "-x", "1280", "-y", "720", inPath};
            qDebug() << currentTab;
            if (currentTab == "lutTab") {
                if (lutPath.isEmpty()) {
                    QMessageBox::warning(this, "Error", "LUT not specified!");
                    return;
                }
                params << "-vf";
                appendLutParams(params);
            } else if (currentTab == "speedTab") {
                params << "-vf";
                appendSpeedParams(params);
            } else if (currentTab == "extractTab") {
                appendExtractParams(params);
            } else if (currentTab == "scaleTab") {
                params << "-vf";
                appendScaleParams(params);
            }
            ffplaySubprocess(params);
        } else QMessageBox::warning(this, "Error", "Click apply before previewing.");
    }
}


void MainWindow::convertToMp3() {
    qDebug() << "Convert to mp3 clicked";
    outPath = QFileDialog::getSaveFileName(this, "Save File", "", "MP3 Files (*.mp3)");
    QStringList params = {"-hwaccel", "auto", "-y", "-loglevel", "debug", "-i", inPath,
                          outPath, "-b:a", "192K", "-vn"};
    ffmpegSubprocess(params);
}

void MainWindow::saveAsGif() {
    qDebug() << "Save as GIF clicked";
    outPath = QFileDialog::getSaveFileName(this, "Save File", "", "Gif files (*.gif)");
    QStringList params = {"-hwaccel", "auto", "-y", "-loglevel", "debug", "-i", inPath,
                          "-filter_complex",
                          "fps=10,scale=320:-1:flags=lanczos,split[o1][o2];[o1]palettegen[p];[o2]fifo[o3];[o3][p]paletteuse",
                          outPath};
    ffmpegSubprocess(params);
}

void MainWindow::stabilizeVideo() {
    qDebug() << "Stabilize video clicked";
    outPath = QFileDialog::getSaveFileName(this, "Save File", "", "MP4 Files (*.mp4)");
    QStringList detectParams = {"-hwaccel", "auto", "-y", "-loglevel", "debug", "-i", inPath,
                                "-vf",
                                "vidstabdetect=stepsize=32:shakiness=10:accuracy=10:result=external/transform_vectors.trf",
                                "-f",
                                "null", "-"};
    ffmpegSubprocess(detectParams);
    QStringList transformParams = {"-hwaccel", "auto", "-y", "-loglevel", "debug", "-i", inPath,
                                   "-vf",
                                   "vidstabtransform=input=external/transform_vectors.trf:zoom=0:smoothing=10,unsharp=5:5:0.8:3:3:0.4",
                                   "-vcodec", "libx264", "-acodec", "-copy", outPath};
    ffmpegSubprocess(transformParams);
    std::remove("external/transform_vectors.trf");
}

void MainWindow::saveToFile() {
    qDebug() << "Export clicked";
    if (!ffmpegInputs.isEmpty()) {
        outPath = QFileDialog::getSaveFileName(this, "Save File", "", "MP4 Files (*.mp4)");
        qDebug() << ffmpegInputs;
        QStringList params = {"-hwaccel", "auto", "-y", "-loglevel", "debug", "-i", inPath};
        if (ffmpegInputs.contains("lut") or ffmpegInputs.contains("speed") or
            ffmpegInputs.contains("scaleW")) {
            params << "-vf";
            if (ffmpegInputs.contains("lut")) {
                appendLutParams(params);
            } else if (ffmpegInputs.contains("speed")) {
                appendSpeedParams(params);
            } else if (ffmpegInputs.contains("scaleW")) {
                appendScaleParams(params);
            }
        } else if (ffmpegInputs.contains("tStart")) {
            appendExtractParams(params);
        }
        params << outPath;
        ffmpegSubprocess(params);
    } else {
        QMessageBox::warning(this, "Error", "Click apply before exporting.");
    }
}

void MainWindow::executeCustom() {
    qDebug() << "Execute custom clicked";
    QStringList params = ui->customText->toPlainText().split(" ");
    ffmpegSubprocess(params);
}

void MainWindow::tabSelected() {
    ui->confirmationLabel->clear();
    ffmpegInputs.clear();
    if (ui->functionTabs->currentIndex() == 4) {
        ui->actionApply->hide();
        ui->actionPreview->hide();
        ui->actionExecute->hide();
        ui->confirmationLabel->hide();
    } else {
        ui->actionApply->show();
        ui->actionPreview->show();
        ui->actionExecute->show();
        ui->confirmationLabel->show();
    }
}


void MainWindow::uploadToStreamable() {
    if (ui->importField->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Import not specified!");
    } else {
        QProcess process;
        QStringList params = {"https://api.streamable.com/upload", "-u",
                              "hilefab320@mailcupp.com:hilefab320@mailcupp.com", "-F", QString("file=@%1").arg(inPath)};
        process.start("curl", params);
        if (!process.waitForFinished())
            qDebug() << process.errorString();
        else {
            QString shortCode = process.readAllStandardOutput();
            shortCode = shortCode.split("\"")[3];
            QString url = QString("https://streamable.com/%1").arg(shortCode);
            qDebug().noquote() << "Video uploaded at url: " << url;
            QDesktopServices::openUrl(QUrl(url));
        }
    }
}
