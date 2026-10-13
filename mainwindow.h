#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <cstdint>
#include<QPlainTextEdit>
#include "conn.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QDialog>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QGuiApplication>
#include <QApplication>
#include <cstring>

#include "Structdef.h"

class QMenuBar;
class QMenu;
class QAction;
class QLabel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void fileCount(uint16_t count);
    void fileCopyCompleted();

private slots:
    void sendHealthCheck();
    void sendTimeSynchronization();
    void selectFile();
    void Tigger_ESC_filecout(uint16_t filecount);
    void sendMissionFileMessage();
    void LoginDetails();
    void LogoutDetails();

    void showDebugMessage(const QString &message,
                          const QColor &color);

private:
    void initUI();
    void updateProfileStyle();

    Ui::MainWindow *ui;
    Conn *connection;

    // Left menu bar
    QMenuBar *bar = nullptr;
    QMenu *menu = nullptr;
    QMenu *fileMenu = nullptr;
    QMenu *viewMenu = nullptr;

    QAction *healthcheck = nullptr;
    QAction *setTime = nullptr;
    QAction *exit_1 = nullptr;
    QAction *newAction = nullptr;
    QAction *openAction = nullptr;
    QAction *exitAction = nullptr;

    // Right menu bar (Profile)
    QMenuBar *profileBar = nullptr;
    QMenu *profileMenu = nullptr;
    QAction *loginAction = nullptr;
    QAction *logoutAction = nullptr;
    QAction *profileExitAction = nullptr;

    // Other
     QPlainTextEdit *select = nullptr;     // shows the selected file names
    QString inputFiles;

    bool isLoggedIn = false;
    QString currentUser;
    QString correctUsername = "admin";
};

#endif // MAINWINDOW_H