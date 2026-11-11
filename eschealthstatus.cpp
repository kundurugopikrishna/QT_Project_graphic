
#include "eschealthstatus.h"
#include "Structdef.h"

#include <QDebug>

ESCHealthstatus::ESCHealthstatus(QObject *parent)
    : QObject{parent}
{
    // Initialize SPB buttons
    for (int i = 0; i < 10; ++i)
        spbButtons[i] = nullptr;

    spbTempButton = nullptr;

    // Initialize GPU buttons
    for (int i = 0; i < 8; ++i)
        gpuButtons[i] = nullptr;

    gpuTempButton = nullptr;

    // Normal radio button style
    radioStyle =
        "QRadioButton {"
        " color: white;"
        " font-size: 13px;"
        " spacing: 8px;"
        "}"
        "QRadioButton::indicator {"
        " width: 16px;"
        " height: 16px;"
        " border-radius: 8px;"
        " border: 2px solid #D32F2F;"
        " background-color: #D32F2F;"
        "}"
        "QRadioButton::indicator:checked {"
        " background-color: #00C853;"
        " border: 2px solid #00C853;"
        "}"
        "QRadioButton::indicator:unchecked {"
        " background-color: #D32F2F;"
        " border: 2px solid #D32F2F;"
        "}";



}


// =====================================================
//                  HEALTH DIALOG
// =====================================================

void ESCHealthstatus::eschealthDialog()
{
    QDialog *dlghealth = new QDialog();

    dlghealth->setWindowTitle("Health Status");
    dlghealth->setAttribute(Qt::WA_DeleteOnClose);
    dlghealth->setMinimumSize(450, 400);

    dlghealth->setStyleSheet(
        "QDialog { background-color: #0B1F3A; }"
        "QLabel { color: white; font-size: 14px; font-weight: bold; }"
        "QPushButton { background-color: #1976D2; color: white; }"
        );

    QVBoxLayout *lay = new QVBoxLayout(dlghealth);

    QGridLayout *grid = new QGridLayout();

    grid->setHorizontalSpacing(40);
    grid->setVerticalSpacing(8);


    // =================================================
    //                       SPB
    // =================================================

    QLabel *lblSPB = new QLabel("SPB", dlghealth);
    lblSPB->setAlignment(Qt::AlignCenter);

    grid->addWidget(lblSPB, 0, 0);

    QString spbNames[10] =
        {
            "DAB",
            "Ethernet",
            "RIO",
            "PCIe_Line",
            "ADC_FPGA",
            "DDR_3_1",
            "DDR_3_2",
            "PCIe_Device",
            "Flash",
            "EPROM"
        };

    for (int i = 0; i < 10; ++i)
    {
        spbButtons[i] = new QRadioButton(spbNames[i], dlghealth);

        spbButtons[i]->setStyleSheet(radioStyle);
        spbButtons[i]->setAutoExclusive(false);

        // Prevent mouse clicks
        spbButtons[i]->setAttribute(Qt::WA_TransparentForMouseEvents);

        grid->addWidget(spbButtons[i], i + 1, 0);
    }

    // SPB temperature
    spbTempButton = new QRadioButton("TEMP", dlghealth);

    spbTempButton->setAutoExclusive(false);
    spbTempButton->setStyleSheet(radioStyle);
    spbTempButton->setAttribute(Qt::WA_TransparentForMouseEvents);

    grid->addWidget(spbTempButton, 11, 0);


    // =================================================
    //                       GPU
    // =================================================

    QLabel *lblGPU = new QLabel("GPU", dlghealth);
    lblGPU->setAlignment(Qt::AlignCenter);

    grid->addWidget(lblGPU, 0, 1);

    QString gpuNames[8] =
        {
            "SPB",
            "GPGPU",
            "Ethernet",
            "PCIe_Device",
            "Onboard_Hard_disk",
            "SATA",
            "Flash",
            "GPU_ConnectX"
        };

    for (int i = 0; i < 8; ++i)
    {
        gpuButtons[i] = new QRadioButton(gpuNames[i], dlghealth);

        gpuButtons[i]->setStyleSheet(radioStyle);
        gpuButtons[i]->setAutoExclusive(false);

        // Prevent mouse clicks
        gpuButtons[i]->setAttribute(Qt::WA_TransparentForMouseEvents);

        grid->addWidget(gpuButtons[i], i + 1, 1);
    }

    // GPU temperature
    gpuTempButton = new QRadioButton("TEMP", dlghealth);

    gpuTempButton->setAutoExclusive(false);
    gpuTempButton->setStyleSheet(radioStyle);
    gpuTempButton->setAttribute(Qt::WA_TransparentForMouseEvents);

    grid->addWidget(gpuTempButton, 9, 1);


    // =================================================
    //                    ADD LAYOUT
    // =================================================

    lay->addLayout(grid);



    quint16 health_states_rfps = 0x0B55;

    // Test GPGPU health
    quint16 health_states_GPGPU = 0x02A5;

    // Update radio buttons
    updateSPBHealthStatus(health_states_rfps);
    updateGPGPUHealthStatus(health_states_GPGPU);
    dlghealth->exec();
}


// =====================================================
//              PROCESS RECEIVED HEALTH PACKET
// =====================================================

void ESCHealthstatus::processESCHealthData(const QByteArray &data)
{
    // Expected packet size: 13 bytes
    // if (data.size() < 13)
    // {
    //     qDebug() << "Invalid health packet size:" << data.size();
    //     return;
    // }

    // quint8 startMsg = 0;
    // quint8 msgType = 0;

    // quint16 cmdCode = 0;
    // quint16 seqNo = 0;
    // quint16 msgLength = 0;

    // // quint16 health_states_rfps = 0;
    // // quint16 health_states_GPGPU = 0;

    // quint8 endMsg = 0;

    // // Read packet in Big Endian
    // QDataStream stream(data);

    // stream.setByteOrder(QDataStream::BigEndian);

    // stream >> startMsg
    //     >> msgType
    //     >> cmdCode
    //     >> seqNo
    //     >> msgLength
    //     >> health_states_rfps
    //     >> health_states_GPGPU
    //     >> endMsg;

    // if (stream.status() != QDataStream::Ok)
    // {
    //     qDebug() << "Failed to read health packet";
    //     return;
    // }

    // // Validate packet markers
    // if (startMsg != 0xC9 || endMsg != 0xCF)
    // {
    //     qDebug() << "Invalid health packet markers";
    //     return;
    // }

    // // Update SPB

    // updateSPBHealthStatus(health_states_rfps);

    // // Update GPU
    // updateGPGPUHealthStatus(health_states_GPGPU);

    // Test SPB health
    // quint16 health_states_rfps = 0x0B55;

    // // Test GPGPU health
    // quint16 health_states_GPGPU = 0x02A5;

    // // Update radio buttons
    // updateSPBHealthStatus(health_states_rfps);
    // updateGPGPUHealthStatus(health_states_GPGPU);
}


// =====================================================
//                  UPDATE SPB HEALTH
// =====================================================

void ESCHealthstatus::updateSPBHealthStatus(quint16 healthStatus)
{
    qDebug() << "SPB Health received:"
             << Qt::hex << healthStatus;


    // Bits 0 to 9: Normal SPB health
    for (int bit = 0; bit < 10; ++bit)
    {
        bool status = (healthStatus >> bit) & 0x01;

        if (spbButtons[bit])
        {
            spbButtons[bit]->setChecked(status);
        }
    }

    // Bits 10 and 11: SPB Temperature
    int tempStatus = (healthStatus >> 10) & 0x03;

    if (spbTempButton)
    {
        // Restore normal style before updating
        spbTempButton->setStyleSheet(radioStyle);

        if (tempStatus == 0)          // 00 - Not OK
        {
            spbTempButton->setChecked(false);
        }
        else if (tempStatus == 1)     // 01 - OK
        {
            spbTempButton->setChecked(true);
        }
        else if (tempStatus == 2)     // 10 - Alert
        {
            spbTempButton->setChecked(false);

            spbTempButton->setStyleSheet(
                "QRadioButton { color: white; }"
                "QRadioButton::indicator {"
                " width: 16px;"
                " height: 16px;"
                " border-radius: 8px;"
                " background-color: #FF9800;"
                " border: 2px solid #FF9800;"
                "}"
                );
        }
        else                          // 11 - Undefined
        {
            spbTempButton->setChecked(false);
        }
    }

    qDebug() << "SPB Health:" << Qt::hex << healthStatus;
}


// =====================================================
//                 UPDATE GPGPU HEALTH
// =====================================================

void ESCHealthstatus::updateGPGPUHealthStatus(quint16 healthStatus)
{
    // Bits 0 to 7: Normal GPU health
    for (int bit = 0; bit < 8; ++bit)
    {
        bool status = (healthStatus >> bit) & 0x01;

        if (gpuButtons[bit])
        {
            gpuButtons[bit]->setChecked(status);
        }
    }

    // Bits 8 and 9: GPU Temperature
    int tempStatus = (healthStatus >> 8) & 0x03;

    if (gpuTempButton)
    {
        // Restore normal style before updating
        gpuTempButton->setStyleSheet(radioStyle);

        if (tempStatus == 0)          // 00 - Not OK
        {
            gpuTempButton->setChecked(false);
        }
        else if (tempStatus == 1)     // 01 - OK
        {
            gpuTempButton->setChecked(true);
        }
        else if (tempStatus == 2)     // 10 - Alert
        {
            gpuTempButton->setChecked(false);

            gpuTempButton->setStyleSheet(
                "QRadioButton { color: white; }"
                "QRadioButton::indicator {"
                " width: 16px;"
                " height: 16px;"
                " border-radius: 8px;"
                " background-color: #FF9800;"
                " border: 2px solid #FF9800;"
                "}"
                );
        }
        else                          // 11 - Undefined
        {
            gpuTempButton->setChecked(false);
        }
    }

    qDebug() << "GPGPU Health:" << Qt::hex << healthStatus;
}
