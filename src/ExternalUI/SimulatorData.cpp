/*
 * File:   SimulatorData.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "SimulatorData.h"

#include <QDebug>

SimulatorData* sSimulatorData = 0;

SimulatorData::SimulatorData()
{

}

SimulatorData::~SimulatorData()
{

}

void SimulatorData::SetNetworkStatus(int status)
{
    m_networkStatus = status;
    emit networkStatusChanged();
}

QString SimulatorData::GetNetworkCallsign()
{
    return m_networkCallsign;
}

void SimulatorData::SetNetworkCallsign(QString callsign)
{
    m_networkCallsign = callsign;
    emit networkCallsignChanged();
}

void SimulatorData::SetXPDRCodeA(int code)
{
    m_xpdrCodeA = code;
    emit xpdrCodeAChanged();
}

void SimulatorData::SetXPDRMode(int mode)
{
    m_xpdrMode = mode;
    emit xpdrModeChanged();
}

void SimulatorData::SetCOM1Freq(QString freq)
{
    m_com1freq = freq;
    emit com1FreqChanged();
}

void SimulatorData::SetCOM1Name(QString name)
{
    m_com1name = name != m_com1freq ? name : "";
    emit com1NameChanged();
}

void SimulatorData::SetCOM2Freq(QString freq)
{
    m_com2freq = freq;
    emit com2FreqChanged();
}

void SimulatorData::SetCOM2Name(QString name)
{
    m_com2name = name != m_com2freq ? name : "";
    emit com2NameChanged();
}

void SimulatorData::SetRadioActive(int radioId)
{
    m_radioActive = radioId;
    qDebug() << "radio id " << m_radioActive;
    emit radioActiveChanged();
}

QString SimulatorData::GetActiveFreq()
{
    if(m_radioActive == 1)
        return m_com1freq;
    else
        return m_com2freq;
}

void SimulatorData::SetSoftwareInfo(QString version, QString branch)
{
    m_softwareVersion = version;
    m_softwareBranch = branch;

    emit softwareInfoChanged();
}

void SimulatorData::SetIVAOUseWeather(bool useWeather)
{
    m_ivaoUseWeather = useWeather;

    emit ivaoUseWeatherChanged();
}

void SimulatorData::SetTagging(bool useTagging)
{
    m_tagging = useTagging;

    emit taggingChanged();
}
