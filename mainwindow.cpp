#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connection(new Conn(this))
{
    ui->setupUi(this);
    initUI();

    qDebug() << "Qt platform:" << QGuiApplication::platformName();

    // Menu actions
    connect(exit_1,      &QAction::triggered, this, &QWidget::close);
    connect(healthcheck, &QAction::triggered, this, &MainWindow::sendHealthCheck);
    connect(setTime,     &QAction::triggered, this, &MainWindow::sendTimeSynchronization);

    // File actions
    connect(exitAction,  &QAction::triggered, this, &QWidget::close);
    connect(openAction,  &QAction::triggered, this, &MainWindow::selectFile);

    // Profile actions
    connect(loginAction,       &QAction::triggered, this, &MainWindow::LoginDetails);
    connect(logoutAction,      &QAction::triggered, this, &MainWindow::LogoutDetails);
    connect(profileExitAction, &QAction::triggered, this, &QWidget::close);

    // Signals
    connect(this, &MainWindow::fileCount,         this, &MainWindow::Tigger_ESC_filecout);
    connect(this, &MainWindow::fileCopyCompleted, this, &MainWindow::sendMissionFileMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =========================================================
//  UI
// =========================================================
void MainWindow::initUI()
{
    QWidget *header = new QWidget(this);
    header->setFixedHeight(40);
    header->setStyleSheet("QWidget { background-color: yellow; }");

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 10, 0);
    headerLayout->setSpacing(0);

    // ---------- Common styles ----------
    const QString barStyle =
        "QMenuBar { background-color: yellow; color: black; }"
        "QMenuBar::item { background-color: yellow; color: black; padding: 5px 15px; }"
        "QMenuBar::item:selected { background-color: orange; }";

    const QString menuStyle =
        "QMenu { background-color: yellow; color: black; border: 1px solid black; }"
        "QMenu::item { background-color: yellow; color: black; padding: 6px 25px; }"
        "QMenu::item:selected { background-color: orange; color: black; }";

    // =====================================================
    //              LEFT MENU BAR (Menu / File / View)
    // =====================================================
    bar = new QMenuBar(header);
    bar->setNativeMenuBar(false);
    bar->setStyleSheet(barStyle);
    bar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    menu = bar->addMenu("Menu");
    menu->setStyleSheet(menuStyle);
    healthcheck = new QAction("Health", this);
    setTime     = new QAction("SetTime", this);
    exit_1      = new QAction("Exit", this);
    menu->addAction(healthcheck);
    menu->addAction(setTime);
    menu->addAction(exit_1);

    fileMenu = bar->addMenu("File");
    fileMenu->setStyleSheet(menuStyle);
    newAction  = new QAction("New", this);
    openAction = new QAction("Open", this);
    exitAction = new QAction("Exit", this);
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(exitAction);

    viewMenu = bar->addMenu("View");
    viewMenu->setStyleSheet(menuStyle);

    // =====================================================
    //          RIGHT MENU BAR (Profile: Login / Logout)
    // =====================================================
    profileBar = new QMenuBar(header);
    profileBar->setNativeMenuBar(false);
    profileBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    profileMenu = profileBar->addMenu("Profile");
    profileMenu->setStyleSheet(menuStyle);

    loginAction       = new QAction("Login", this);
    logoutAction      = new QAction("Logout", this);
    profileExitAction = new QAction("Exit", this);

    profileMenu->addAction(loginAction);
    profileMenu->addAction(logoutAction);
    profileMenu->addSeparator();
    profileMenu->addAction(profileExitAction);

    updateProfileStyle();

    // Left bar, stretch, right bar
    headerLayout->addWidget(bar);
    headerLayout->addStretch();
    headerLayout->addWidget(profileBar);

    // =====================================================
    //                     SPLITTER
    // =====================================================
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    // ---------- Top panel ----------
    QWidget *topPanel = new QWidget();
    topPanel->setStyleSheet("background-color: #3498db;");

    QVBoxLayout *topPanelLayout = new QVBoxLayout(topPanel);
    topPanelLayout->setContentsMargins(10, 10, 10, 10);
    topPanelLayout->setSpacing(0);

    // Debug text area
    select = new QPlainTextEdit(topPanel);

    select->setStyleSheet(
        "QPlainTextEdit {"
        "    background-color: #0B1F3A;"
        "    color: #FFFFFF;"
        "    border: 1px solid #42A5F5;"
        "    selection-background-color: #42A5F5;"
        "    selection-color: #FFFFFF;"
        "}"
        );

    select->setReadOnly(true);

    // Allow the text area to expand in both directions
    select->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );

    // Give the text area all available vertical space
    topPanelLayout->addWidget(select, 1);
    //topPanelLayout->addStretch();

    // ---------- Bottom panel ----------
    QWidget *bottomPanel = new QWidget();
    bottomPanel->setStyleSheet("background-color: #2ecc71;");

    QVBoxLayout *bottomPanelLayout = new QVBoxLayout(bottomPanel);
    bottomPanelLayout->setContentsMargins(0, 0, 0, 0);
    bottomPanelLayout->setSpacing(0);

    QWidget *bottomHeader = new QWidget();
    bottomHeader->setFixedHeight(40);
    bottomHeader->setStyleSheet("background-color: #27ae60;");

    bottomPanelLayout->addWidget(bottomHeader);
    bottomPanelLayout->addStretch();

    splitter->addWidget(topPanel);
    splitter->addWidget(bottomPanel);
    splitter->setSizes({300, 300});
    splitter->setChildrenCollapsible(false);

    // =====================================================
    //               MAIN WINDOW LAYOUT
    // =====================================================
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(header);
    mainLayout->addWidget(splitter);

    setCentralWidget(centralWidget);
}

// Green "Log" when logged in, yellow "Profile" when not
void MainWindow::updateProfileStyle()
{
    const QString textColor = isLoggedIn ? "green" : "black";

    profileMenu->setTitle(isLoggedIn ? "Log" : "Profile");

    profileBar->setStyleSheet(
        QString("QMenuBar { background-color: yellow; }"
                "QMenuBar::item { background-color: yellow; color: %1;"
                "                 padding: 5px 15px; font-weight: %2; }"
                "QMenuBar::item:selected { background-color: orange; }")
            .arg(textColor, isLoggedIn ? "bold" : "normal"));

    profileBar->adjustSize();

    loginAction->setEnabled(!isLoggedIn);
    logoutAction->setEnabled(isLoggedIn);
}

// =========================================================
//  Login / Logout
// =========================================================
void MainWindow::LoginDetails()
{
    constexpr int kUserNameSize = 10;

    QDialog dialog(this);
    dialog.setWindowTitle("Login");
    dialog.setFixedSize(350, 160);

    QGridLayout *layout = new QGridLayout(&dialog);

    QLabel *usernameLabel = new QLabel("Username:", &dialog);

    QLineEdit *usernameEdit = new QLineEdit(&dialog);
    usernameEdit->setPlaceholderText("Enter username");
    usernameEdit->setMaxLength(kUserNameSize);

    layout->addWidget(usernameLabel, 0, 0);
    layout->addWidget(usernameEdit, 0, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", &dialog);
    okButton->setDefault(true);
    QPushButton *closeButton = new QPushButton("Close", &dialog);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout, 1, 0, 1, 2);

    connect(okButton, &QPushButton::clicked, &dialog, [&]()
            {
                const QString username = usernameEdit->text().trimmed();

                // 1) Empty username
                if (username.isEmpty())
                {
                    QMessageBox::warning(&dialog, "Login Failed", "Please enter a username.");

                    showDebugMessage("Login Failed, Please enter a username.", Qt::red);
                    usernameEdit->setFocus();
                    return;
                }

                // 2) Too long (needs room for '\0' in char[10])
                if (username.toUtf8().size() >= kUserNameSize)
                {
                    QMessageBox::warning(&dialog, "Username Too Long",
                                         "Username should be less than 10 characters.");


                    showDebugMessage("Username Too Long Username should be less than 10 characters", Qt::red);

                    usernameEdit->clear();
                    usernameEdit->setFocus();
                    return;
                }

                // 3) Check username
                if (username == correctUsername)
                {
                    isLoggedIn = true;
                    currentUser = username;

                    QMessageBox::information(&dialog, "Login Successful",
                                             "Welcome, " + username + "!");

                    updateProfileStyle();
                    dialog.accept();
                    qDebug() << "Login successful";

                    showDebugMessage("Login successful", Qt::red);
                }
                else
                {
                    QMessageBox::warning(&dialog, "Login Failed",
                                         "Incorrect username. Please try again.");
                    usernameEdit->clear();
                    usernameEdit->setFocus();
                }
            });

    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}

void MainWindow::LogoutDetails()
{
    if (!isLoggedIn)
    {
        QMessageBox::information(this, "Logout", "You are not logged in.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Logout", "Are you sure you want to log out?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        isLoggedIn = false;
        currentUser.clear();
        updateProfileStyle();
        qDebug() << "Logged out";
        showDebugMessage("Logged Out successfully", Qt::red);

    }
}

void MainWindow::showDebugMessage(const QString &message,const QColor &color)
{
    QTextCursor cursor = select->textCursor();

    cursor.movePosition(QTextCursor::End);

    QTextCharFormat format;
    format.setForeground(color);

    cursor.insertText(
        QDateTime::currentDateTime().toString("[hh:mm:ss] ")
            + message + "\n",
        format
        );

    select->setTextCursor(cursor);
    select->ensureCursorVisible();
}

// =========================================================
//  Messages
// =========================================================
void MainWindow::sendTimeSynchronization()
{
    SetDataMessage timeSyncMsg{};

    QDateTime currentTime = QDateTime::currentDateTime();
    qDebug() << "System time:" << currentTime.toString("yyyy-MM-dd HH:mm:ss");

    timeSyncMsg.elapsedSeconds =
        static_cast<uint64_t>(currentTime.toSecsSinceEpoch());

    qDebug() << "Unix time:" << timeSyncMsg.elapsedSeconds;

    QByteArray data(reinterpret_cast<const char *>(&timeSyncMsg),
                    sizeof(SetDataMessage));
    connection->writeData(data);
}

void MainWindow::sendHealthCheck()
{
    HealthCheckMessage healthCheckMsg{};

    QByteArray data(reinterpret_cast<const char *>(&healthCheckMsg),
                    sizeof(HealthCheckMessage));

    qDebug() << "Health check is being sent";
    qDebug() << "Data:" << data.toHex(' ').toUpper();

    connection->writeData(data);
}

void MainWindow::selectFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Select Input Files", "", "All Files (*.*)");

    if (fileNames.isEmpty())
        return;

    QStringList selectedNames;
    const QString destinationPath = "/home/sana/project";

    if (!QDir().mkpath(destinationPath))
    {
        qDebug() << "Failed to create destination folder:" << destinationPath;
        return;
    }

    bool allCopied = true;

    for (const QString &filePath : fileNames)
    {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        selectedNames << fileName;

        QString destinationFile = QDir(destinationPath).filePath(fileName);

        if (QFile::exists(destinationFile) && !QFile::remove(destinationFile))
        {
            qDebug() << "Failed to remove existing file:" << destinationFile;
            allCopied = false;
            continue;
        }

        if (QFile::copy(filePath, destinationFile))
            qDebug() << "File copied:" << destinationFile;
        else
        {
            qDebug() << "Failed to copy:" << filePath;
            allCopied = false;
        }
    }

    // Set before emitting, because sendMissionFileMessage() reads inputFiles
    inputFiles = selectedNames.join(", ");
  //  select->setText(inputFiles);

    if (allCopied)
    {
        qDebug() << "All files copied successfully.";
        emit fileCopyCompleted();
    }

    uint16_t count = static_cast<uint16_t>(fileNames.count());
    emit fileCount(count);

    qDebug() << "Input files:" << inputFiles;
    qDebug() << "File count:" << count;
}

void MainWindow::Tigger_ESC_filecout(uint16_t filecount)
{
    qDebug() << "Tigger_ESC_filecout" << filecount;

    TriggerMessage triggerMsg{};
    triggerMsg.inputFileCount = filecount;

    // TODO: send triggerMsg with connection->writeData(...) when needed
}

void MainWindow::sendMissionFileMessage()
{
    RawFileMessage missionMsg{};

    QByteArray fileName = inputFiles.toUtf8();

    if (static_cast<size_t>(fileName.size()) >= sizeof(missionMsg.missionIdFileName))
    {
        QMessageBox::warning(this, "Filename Too Long",
                             "File name must be less than 100 bytes.");
        return;
    }

    std::memcpy(missionMsg.missionIdFileName,
                fileName.constData(),
                fileName.size());
    missionMsg.missionIdFileName[fileName.size()] = '\0';

}