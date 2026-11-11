#ifndef ESCHEALTHSTATUS_H
#define ESCHEALTHSTATUS_H

#include <QObject>
#include "Definclude.h"
#include "Structdef.h"
#include <QDialog>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDataStream>
#include <QPointer>
class ESCHealthstatus : public QObject
{
    Q_OBJECT
public:
    explicit ESCHealthstatus(QObject *parent = nullptr);
    void eschealthDialog();

void processESCHealthData(const QByteArray &data);
private slots:
void updateSPBHealthStatus(quint16 healthStatus);
void updateGPGPUHealthStatus(quint16 healthStatus);

private:
// SPB: 10 normal indicators
QPointer<QRadioButton> spbButtons[10];

// SPB temperature
QPointer<QRadioButton> spbTempButton;

// GPU: 8 normal indicators
QPointer<QRadioButton> gpuButtons[8];

// GPU temperature
QPointer<QRadioButton> gpuTempButton;

QString radioStyle;



signals:
};

#endif // ESCHEALTHSTATUS_H
