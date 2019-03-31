/*
 * File:   SimulatorData.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef SIMULATORDATA_H
#define SIMULATORDATA_H

#include <QObject>

// This class is the QObject interface to simulator data received from the network.
class SimulatorData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int p_networkStatus MEMBER m_networkStatus NOTIFY networkStatusChanged())
    Q_PROPERTY(QString p_networkCallsign MEMBER m_networkCallsign NOTIFY networkCallsignChanged())

    Q_PROPERTY(int p_xpdrCodeA MEMBER m_xpdrCodeA NOTIFY xpdrCodeAChanged())
    Q_PROPERTY(int p_xpdrMode MEMBER m_xpdrMode NOTIFY xpdrModeChanged())
    Q_PROPERTY(QString p_com1freq MEMBER m_com1freq NOTIFY com1FreqChanged())
    Q_PROPERTY(QString p_com1name MEMBER m_com1name NOTIFY com1NameChanged())
    Q_PROPERTY(QString p_com2freq MEMBER m_com2freq NOTIFY com2FreqChanged())
    Q_PROPERTY(QString p_com2name MEMBER m_com2name NOTIFY com2NameChanged())
    Q_PROPERTY(int p_radioActive MEMBER m_radioActive NOTIFY radioActiveChanged())

    Q_PROPERTY(QString p_softwareVersion MEMBER m_softwareVersion NOTIFY softwareInfoChanged())
    Q_PROPERTY(QString p_softwareBranch MEMBER m_softwareBranch NOTIFY softwareInfoChanged())

    Q_PROPERTY(bool p_ivaoUseWeather MEMBER m_ivaoUseWeather NOTIFY ivaoUseWeatherChanged())

    Q_PROPERTY(bool p_tagging MEMBER m_tagging NOTIFY taggingChanged())
public:
    SimulatorData();
    ~SimulatorData();

    QString GetActiveFreq();

    void    SetNetworkStatus(int status);
    QString GetNetworkCallsign();
    void    SetNetworkCallsign(QString callsign);

    void    SetXPDRCodeA(int code);
    void    SetXPDRMode(int mode);
    void    SetCOM1Freq(QString freq);
    void    SetCOM1Name(QString name);
    void    SetCOM2Freq(QString freq);
    void    SetCOM2Name(QString name);
    void    SetRadioActive(int radioId);

    void    SetSoftwareInfo(QString version, QString branch);

    void    SetIVAOUseWeather(bool useWeather);

    void    SetTagging(bool useTagging);
signals:
    void    networkStatusChanged();
    void    networkCallsignChanged();
    void    xpdrCodeAChanged();
    void    xpdrModeChanged();
    void    com1FreqChanged();
    void    com1NameChanged();
    void    com2FreqChanged();
    void    com2NameChanged();
    void    radioActiveChanged();
    void    softwareInfoChanged();
    void    ivaoUseWeatherChanged();
    void    taggingChanged();
private:
    int     m_networkStatus = 0;
    QString m_networkCallsign;

    int     m_xpdrCodeA = 0;
    int     m_xpdrMode = 0;
    int     m_radioActive = 1;

    QString m_com1freq = "000.000";
    QString m_com1name = "";
    QString m_com2freq = "000.000";
    QString m_com2name = "";

    QString m_softwareVersion;
    QString m_softwareBranch;

    bool    m_ivaoUseWeather = false;
    bool    m_tagging = false;
};

extern SimulatorData* sSimulatorData;

#endif
