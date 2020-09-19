#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    bool installPack(bool cleanELF = true);
    bool uninstallPack();
    bool createDirectory(QDir rootDir, QString path);
    bool removeDirectory(QString path);
    bool installBinaryFile(QString fileName, QString installDir);
    bool removeFile(QString fileName);
    bool setPermissions(QFile &f);
    bool setPermissions(QString path);
    bool isPackInstalled();
    void showVersionsLabel();
    void showVersions();
    void showPHPInfo();
    void applyStyle();
private:
    Ui::MainWindow *ui;
    QString dataDir;
    QStringList binFiles;
    QStringList gitFiles;
    QStringList termFiles;
    QStringList mcFiles;
    QStringList mcThemeFiles;
    QStringList mcSyntaxFiles;
    QStringList otherFiles;
private slots:
    void installButtonClicked(bool);
    void uninstallButtonClicked(bool);
    void checkButtonClicked(bool);
    void infoButtonClicked(bool);
    void copyPathClicked(bool);
};
#endif // MAINWINDOW_H
