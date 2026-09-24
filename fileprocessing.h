#ifndef FILEPROCESSING_H
#define FILEPROCESSING_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class FileProcessing;
}

class FileProcessing : public QWidget
{
    Q_OBJECT

public:
    static FileProcessing& getInstance();

    static QString getPreprocessingPath();
    void renameTiffUsingMeta(const QString &extractedFolderPath);
    explicit FileProcessing(QWidget *parent = nullptr);
    ~FileProcessing();

    void readCalibrationConstants(const QString &metaFilePath);

    // ✅ MAKE THESE PUBLIC
    double getSigmaHH() const;
    double getSigmaHV() const;

    double getGammaHH() const;
    double getGammaHV() const;

    double getBetaHH() const;
    double getBetaHV() const;

private slots:
    void on_btnUnzip_clicked();
    void onUnzipFinished(int exitCode, QProcess::ExitStatus status);
    void on_openCalBtn_clicked();

private:
    static QString preprocessingPath;

    FileProcessing(const FileProcessing&) = delete;
    FileProcessing& operator=(const FileProcessing&) = delete;

    void process(const QString &zipPath, const QString &outputPath);
    QString extractValue(const QString &line);
    QString getUniqueFolderPath(const QString &basePath);
    bool zipContainsBandMeta(const QString &zipPath);
    QString getBaseFolderFromTiff(const QString &inputPath);

    // constants
    double sigmaHH = 0.0;
    double sigmaHV = 0.0;

    double gammaHH = 0.0;
    double gammaHV = 0.0;

    double betaHH = 0.0;
    double betaHV = 0.0;

private:
    Ui::FileProcessing *ui;
};

#endif
