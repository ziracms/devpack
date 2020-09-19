#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QClipboard>
#include <QTextStream>

const QString PHP_TMP_DIR = "tmp";
const QString BIN_DIR = "bin";
const QString GIT_CORE_DIR = "git-core";
const QString GIT_TEMPLATES_DIR = "templates";
const QString TERMINFO_DIR = "terminfo";
const QString MC_DIR = "mc";
const QString MC_THEMES_SUBDIR = "skins";
const QString MC_SYNTAX_SUBDIR = "syntax";
const QString APP_DATA_HOME = "/data/data/com.github.ziracms.devpack/files";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList stddirs = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if (stddirs.size()>0) dataDir = stddirs.at(0);
    else dataDir = "";

    binFiles << "php" << "sqlite3" << "git" << "git-receive-pack" << "git-upload-archive" << "git-upload-pack" << "sassc" << "nano" << "mc" << "vim" << "termux-elf-cleaner";
    gitFiles << "git" << "git-remote" << "git-remote-http" << "git-remote-https" << "git-receive-pack" << "git-upload-archive" << "git-upload-pack";
    termFiles << "linux" << "screen" << "screen-256color" << "vt100" << "xterm" << "cygwin";
    mcFiles << "filehighlight.ini" << "mc.charsets";
    mcThemeFiles << "default.ini" << "standard.ini";
    mcSyntaxFiles << "Syntax" << "unknown.syntax";
    otherFiles << "php.ini" << "gitconfig" << "cacert.pem" << "nanorc";

    if (!isPackInstalled()) {
        ui->checkButton->hide();
        ui->uninstallButton->hide();
        ui->infoButton->hide();
        ui->envLabel->hide();
        ui->envTextEdit->hide();
        ui->copyButton->hide();
    } else {
        ui->installButton->hide();
    }

    ui->versionsLabel->hide();

    ui->envTextEdit->setText(
                "export ZIRA_HOME=\""+APP_DATA_HOME+"\"\n" +
                "export PATH=\"$ZIRA_HOME/"+BIN_DIR+":$PATH\"\n" +
                "export TERMINFO=\"$ZIRA_HOME/"+TERMINFO_DIR+"\"\n" +
                "export TERM=\"linux\"\n" +
                "export LC_CTYPE=\"en_US.UTF-8\"\n" +
                "cd ~"
                );

    connect(ui->installButton, SIGNAL(clicked(bool)), this, SLOT(installButtonClicked(bool)));
    connect(ui->uninstallButton, SIGNAL(clicked(bool)), this, SLOT(uninstallButtonClicked(bool)));
    connect(ui->checkButton, SIGNAL(clicked(bool)), this, SLOT(checkButtonClicked(bool)));
    connect(ui->infoButton, SIGNAL(clicked(bool)), this, SLOT(infoButtonClicked(bool)));
    connect(ui->copyButton, SIGNAL(clicked(bool)), this, SLOT(copyPathClicked(bool)));

    applyStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::installButtonClicked(bool)
{
    QString installButtonText = ui->installButton->text();
    ui->installButton->setText(tr("Please wait")+"...");
    ui->installButton->repaint();
    QApplication::processEvents();

    if (!installPack(true)) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(QObject::tr("Error"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText("Installation failed :(");
        msgBox.exec();
    } else {
        QSize size = geometry().size();

        ui->installButton->hide();
        ui->checkButton->show();
        ui->infoButton->show();
        ui->uninstallButton->show();
        ui->envLabel->show();
        ui->envTextEdit->show();
        ui->copyButton->show();

        setFixedSize(size);

        /*
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(QObject::tr("Success"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText("Installation completed :)");
        msgBox.exec();
        */
    }

    ui->installButton->setText(installButtonText);
}

void MainWindow::uninstallButtonClicked(bool)
{
    if (QMessageBox::question(this, tr("Confirmation"), tr("Uninstall pack ?"), QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) return;

    QString uninstallButtonText = ui->uninstallButton->text();
    ui->uninstallButton->setText(tr("Please wait")+"...");
    ui->uninstallButton->repaint();
    QApplication::processEvents();

    if (!uninstallPack()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(QObject::tr("Error"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText("An error occurred :(");
        msgBox.exec();
    } else {
        QSize size = geometry().size();

        ui->checkButton->hide();
        ui->infoButton->hide();
        ui->uninstallButton->hide();
        ui->envLabel->hide();
        ui->envTextEdit->hide();
        ui->copyButton->hide();
        ui->installButton->show();

        if (ui->versionsLabel->isVisible()) {
            ui->versionsLabel->hide();
        }
        if (!ui->ziraLabel->isVisible()){
            ui->ziraLabel->show();
        }

        setFixedSize(size);

        /*
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(QObject::tr("Success"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText("Uninstalled successfully :)");
        msgBox.exec();
        */
    }

    ui->uninstallButton->setText(uninstallButtonText);
}

void MainWindow::showVersionsLabel()
{
    if (ui->versionsLabel->isVisible()) return;
    ui->versionsLabel->show();
    ui->ziraLabel->hide();
}

void MainWindow::checkButtonClicked(bool)
{
    QSize size = geometry().size();
    showVersionsLabel();
    ui->envLabel->show();
    ui->envTextEdit->show();
    ui->copyButton->show();
    showVersions();
    setFixedSize(size);
}

void MainWindow::infoButtonClicked(bool)
{
    QSize size = geometry().size();
    showVersionsLabel();
    ui->envLabel->hide();
    ui->envTextEdit->hide();
    ui->copyButton->hide();
    showPHPInfo();
    setFixedSize(size);
}

bool MainWindow::installPack(bool cleanELF)
{
    if (dataDir.size() == 0) return false;

    QFile f(dataDir);
    setPermissions(f);

    QFileInfo di(dataDir);
    if (!di.isWritable() || !di.isExecutable()) return false;

    QDir d(dataDir);
    if (!createDirectory(d, dataDir+"/"+BIN_DIR)) return false;
    if (!createDirectory(d, dataDir+"/"+BIN_DIR+"/"+GIT_CORE_DIR)) return false;
    if (!createDirectory(d, dataDir+"/"+GIT_TEMPLATES_DIR)) return false;
    if (!createDirectory(d, dataDir+"/"+PHP_TMP_DIR)) return false;
    if (!createDirectory(d, dataDir+"/"+TERMINFO_DIR)) return false;
    if (!createDirectory(d, dataDir+"/"+MC_DIR)) return false;
    if (!createDirectory(d, dataDir+"/"+MC_DIR+"/"+MC_THEMES_SUBDIR)) return false;
    if (!createDirectory(d, dataDir+"/"+MC_DIR+"/"+MC_SYNTAX_SUBDIR)) return false;

    for (QString fileName : binFiles) {
        if (!installBinaryFile(fileName, dataDir+"/"+BIN_DIR)) return false;
    }

    for (QString fileName : gitFiles) {
        if (!installBinaryFile(fileName, dataDir+"/"+BIN_DIR+"/"+GIT_CORE_DIR)) return false;
    }

    for (QString fileName : termFiles) {
        if (fileName.size() == 0) continue;
        QChar c = fileName.at(0);
        if (!createDirectory(d, dataDir+"/"+TERMINFO_DIR+"/"+c)) return false;
        if (!installBinaryFile(fileName, dataDir+"/"+TERMINFO_DIR+"/"+c)) return false;
    }

    for (QString fileName : mcFiles) {
        if (!installBinaryFile(fileName, dataDir+"/"+MC_DIR)) return false;
    }

    for (QString fileName : mcThemeFiles) {
        if (!installBinaryFile(fileName, dataDir+"/"+MC_DIR+"/"+MC_THEMES_SUBDIR)) return false;
    }

    for (QString fileName : mcSyntaxFiles) {
        if (!installBinaryFile(fileName, dataDir+"/"+MC_DIR+"/"+MC_SYNTAX_SUBDIR)) return false;
    }

    for (QString fileName : otherFiles) {
        if (!installBinaryFile(fileName, dataDir)) return false;
    }

    QFile gitConfigHidden(dataDir+"/.gitconfig");
    if (gitConfigHidden.exists()) gitConfigHidden.remove();

    QFile gitConfig(dataDir+"/gitconfig");
    if (gitConfig.exists()) gitConfig.rename(".gitconfig");

    if (cleanELF) {
        QStringList cleanFiles;

        for (QString fileName : binFiles) {
            if (fileName == "termux-elf-cleaner") continue;
            cleanFiles << dataDir+"/"+BIN_DIR+"/"+fileName;
        }

        for (QString fileName : gitFiles) {
            cleanFiles << dataDir+"/"+BIN_DIR+"/"+GIT_CORE_DIR+"/"+fileName;
        }

        QProcess cleanProcess(this);
        cleanProcess.start(dataDir+"/"+BIN_DIR+"/termux-elf-cleaner", cleanFiles);
        if (!cleanProcess.waitForFinished()) return false;
    }

    return true;
}

bool MainWindow::uninstallPack()
{
    if (dataDir.size() == 0) return false;

    for (QString fileName : binFiles) {
        if (!removeFile(dataDir+"/"+BIN_DIR+"/"+fileName)) return false;
    }

    for (QString fileName : gitFiles) {
        if (!removeFile(dataDir+"/"+BIN_DIR+"/"+GIT_CORE_DIR+"/"+fileName)) return false;
    }

    for (QString fileName : termFiles) {
        if (fileName.size() == 0) continue;
        QChar c = fileName.at(0);
        if (!removeFile(dataDir+"/"+TERMINFO_DIR+"/"+c+"/"+fileName)) return false;
    }

    for (QString fileName : mcFiles) {
        if (!removeFile(dataDir+"/"+MC_DIR+"/"+fileName)) return false;
    }

    for (QString fileName : mcThemeFiles) {
        if (!removeFile(dataDir+"/"+MC_DIR+"/"+MC_THEMES_SUBDIR+"/"+fileName)) return false;
    }

    for (QString fileName : mcSyntaxFiles) {
        if (!removeFile(dataDir+"/"+MC_DIR+"/"+MC_SYNTAX_SUBDIR+"/"+fileName)) return false;
    }

    for (QString fileName : otherFiles) {
        if (fileName == "gitconfig") fileName = "."+fileName;
        if (!removeFile(dataDir+"/"+fileName)) return false;
    }

    if (!removeDirectory(dataDir+"/"+BIN_DIR+"/"+GIT_CORE_DIR)) return false;
    if (!removeDirectory(dataDir+"/"+BIN_DIR)) return false;
    if (!removeDirectory(dataDir+"/"+GIT_TEMPLATES_DIR)) return false;
    if (!removeDirectory(dataDir+"/"+PHP_TMP_DIR)) return false;
    if (!removeDirectory(dataDir+"/"+TERMINFO_DIR)) return false;
    if (!removeDirectory(dataDir+"/"+MC_DIR+"/"+MC_THEMES_SUBDIR)) return false;
    if (!removeDirectory(dataDir+"/"+MC_DIR+"/"+MC_SYNTAX_SUBDIR)) return false;
    if (!removeDirectory(dataDir+"/"+MC_DIR)) return false;

    return true;
}

bool MainWindow::createDirectory(QDir rootDir, QString path)
{
    if (!rootDir.mkpath(path)) return false;
    if (!setPermissions(path)) return false;
    return true;
}

bool MainWindow::removeDirectory(QString path)
{
    QDir dir(path);
    if (!dir.exists()) return true;
    if (!dir.removeRecursively()) return false;
    return true;
}

bool MainWindow::installBinaryFile(QString fileName, QString installDir)
{
    QFile f("assets:/"+fileName);
    if (!f.exists()) return false;

    QFile fi(installDir+"/"+fileName);
    if (fi.exists()) fi.remove();

    if (!f.copy(installDir+"/"+fileName)) return false;

    QFile pf(installDir+"/"+fileName);
    if (!setPermissions(pf)) return false;

    return true;
}

bool MainWindow::removeFile(QString fileName)
{
    QFile f(fileName);
    if (!f.exists()) return true;
    if (!f.remove()) return false;
    return true;
}

bool MainWindow::setPermissions(QFile &f)
{
    return f.setPermissions(
        QFileDevice::ReadOwner |
        QFileDevice::ReadGroup |
        QFileDevice::ReadOther |
        QFileDevice::WriteOwner |
        QFileDevice::WriteGroup |
        QFileDevice::WriteOther |
        QFileDevice::ExeOwner |
        QFileDevice::ExeGroup |
        QFileDevice::ExeOther
        );
}

bool MainWindow::setPermissions(QString path)
{
    QFile f(path);
    return setPermissions(f);
}

bool MainWindow::isPackInstalled()
{
    for (QString fileName : binFiles) {
        QFileInfo fileInfo(dataDir+"/"+BIN_DIR+"/"+fileName);
        if (!fileInfo.exists() || !fileInfo.isExecutable()) return false;
    }

    for (QString fileName : gitFiles) {
        QFileInfo fileInfo(dataDir+"/"+BIN_DIR+"/"+GIT_CORE_DIR+"/"+fileName);
        if (!fileInfo.exists() || !fileInfo.isExecutable()) return false;
    }

    return true;
}

void MainWindow::copyPathClicked(bool)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->envTextEdit->toPlainText());

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(QObject::tr("Message"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setText("Copied to clipboard");
    msgBox.exec();
}

void MainWindow::showVersions()
{
    QString versions = "<div>";

    QProcess phpProcess(this);
    phpProcess.start(dataDir+"/"+BIN_DIR+"/php", QStringList() << "-v");
    if (!phpProcess.waitForFinished()) return;
    QByteArray phpResult = phpProcess.readAllStandardError();
    phpResult += phpProcess.readAllStandardOutput();

    versions += "<p>"+QString(phpResult).trimmed().split("\n").at(0) + "</p>";

    QProcess gitProcess(this);
    gitProcess.start(dataDir+"/"+BIN_DIR+"/git", QStringList() << "--version");
    if (!gitProcess.waitForFinished()) return;
    QByteArray gitResult = gitProcess.readAllStandardError();
    gitResult += gitProcess.readAllStandardOutput();

    versions += "<p>"+QString(gitResult).trimmed().split("\n").at(0) + "</p>";

    QProcess sasscProcess(this);
    sasscProcess.start(dataDir+"/"+BIN_DIR+"/sassc", QStringList() << "--version");
    if (!sasscProcess.waitForFinished()) return;
    QByteArray sasscResult = sasscProcess.readAllStandardError();
    sasscResult += sasscProcess.readAllStandardOutput();

    versions += "<p>"+QString(sasscResult).trimmed().split("\n").at(0) + "</p>";

    QProcess sqlite3Process(this);
    sqlite3Process.start(dataDir+"/"+BIN_DIR+"/sqlite3", QStringList() << "--version");
    if (!sqlite3Process.waitForFinished()) return;
    QByteArray sqlite3Result = sqlite3Process.readAllStandardError();
    sqlite3Result += sqlite3Process.readAllStandardOutput();

    versions += "<p>SQLite " + QString(sqlite3Result).trimmed().split("\n").at(0) + "</p>";

    QProcess nanoProcess(this);
    nanoProcess.start(dataDir+"/"+BIN_DIR+"/nano", QStringList() << "--version");
    if (!nanoProcess.waitForFinished()) return;
    QByteArray nanoResult = nanoProcess.readAllStandardError();
    nanoResult += nanoProcess.readAllStandardOutput();

    versions += "<p>"+QString(nanoResult).trimmed().split("\n").at(0) + "</p>";

    QProcess vimProcess(this);
    vimProcess.start(dataDir+"/"+BIN_DIR+"/vim", QStringList() << "--version");
    if (!vimProcess.waitForFinished()) return;
    QByteArray vimResult = vimProcess.readAllStandardError();
    vimResult += vimProcess.readAllStandardOutput();

    versions += "<p>"+QString(vimResult).trimmed().split("\n").at(0) + "</p>";

    QProcess mcProcess(this);
    mcProcess.start(dataDir+"/"+BIN_DIR+"/mc", QStringList() << "--version");
    if (!mcProcess.waitForFinished()) return;
    QByteArray mcResult = mcProcess.readAllStandardError();
    mcResult += mcProcess.readAllStandardOutput();

    versions += "<p>"+QString(mcResult).trimmed().split("\n").at(0) + "</p>";

    versions += "</div>";

    ui->versionsLabel->setText(versions);
}

void MainWindow::showPHPInfo()
{
    QProcess phpProcess(this);
    phpProcess.start(dataDir+"/"+BIN_DIR+"/php", QStringList() << "-r" << "phpinfo();");
    if (!phpProcess.waitForFinished()) return;
    QByteArray phpResult = phpProcess.readAllStandardOutput();
    QStringList phpResultList = QString(phpResult).trimmed().split("\n");
    QString phpResultStr = "<div>";
    for (QString phpResultListItem : phpResultList) {
        QStringList phpResultListItemParts = phpResultListItem.split("=>");
        if (phpResultListItemParts.size() == 1) {
            phpResultStr += "<h2>" + phpResultListItemParts.at(0) + "</h2>";
        } else if (phpResultListItemParts.size() > 1) {
            phpResultStr += "<p><b>" + phpResultListItemParts.at(0) + ":</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + "<span>" + phpResultListItemParts.at(1) + "</span></p>";
        }
    }
    phpResultStr += "</div>";

    ui->versionsLabel->setText(phpResultStr);
}

void MainWindow::applyStyle()
{
    QFile f(":/styles/dark");
    if (!f.exists()) return;
    QString style = "";
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    style += in.readAll() + "\n";
    f.close();
    setStyleSheet(style);
}
