/*
 * File:   MainViewModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QObject>

#include "../models/ActiveFrequencyMessagesListModel.h"
#include "../models/ATCListModel.h"
#include "../models/ActivePrivateChatsListModel.h"

class MainViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ActiveFrequencyMessagesListModel* p_activeFrequencyMessagesListModel MEMBER m_activeFrequencyMessagesListModel NOTIFY viewModelReady())
    Q_PROPERTY(ATCListModel* p_atcListModel MEMBER m_atcListModel NOTIFY viewModelReady())
    Q_PROPERTY(ActivePrivateChatsListModel* p_activePrivateChatsListModel MEMBER m_activePrivateChatsListModel NOTIFY viewModelReady())
    Q_PROPERTY(PrivateChatListModel* p_selectedPrivateChat MEMBER m_selectedPrivateChat NOTIFY selectedPrivateChatChanged())
    Q_PROPERTY(QString p_tickString MEMBER m_tickString NOTIFY ticked())
public:
    MainViewModel();
    ~MainViewModel();

    // Ticks the view model to refresh the QML base view. Chris's recipe to refresh without making a mess of the Qt thread.
    void                                Tic();

    // Displays a standard Alert box and returns without waiting for user confirmation.
    void                                ShowAlert(const std::string& text);

    // Append the specified message to the active frequency text box.
    Q_INVOKABLE
    void                                AppendActiveFrequencyMessage(QString message);

    // Returns a pointer to the ATC List Model.
    ATCListModel*                       GetATCListModel();

    // Sets the active radio to the specified index.
    Q_INVOKABLE
    void                                SetActiveRadio(int radioIndex);

    // Sets the option to use the IVAO weather to the specified value.
    Q_INVOKABLE
    void                                SetIVAOUseWeather(bool useWeather);

    // Sets the option to use aircraft labels within the simulator to the specified value.
    Q_INVOKABLE
    void                                SetTagging(bool useTagging);

    // Sends the specified message to the active frequency.
    Q_INVOKABLE
    void                                SendMessageToActiveFrequency(QString message);

    // Toggles the Transponder Mode betwen Standby and TX.
    Q_INVOKABLE
    void                                ToggleTransponderMode();
    // Transmits a XPDR IDENT instruction.
    Q_INVOKABLE
    void                                TransponderIdent();

    // Tunes the specified radio to the specified frequency.
    Q_INVOKABLE
    void                                TuneRadioToFrequency(int radioIndex, QString frequency);

    Q_INVOKABLE
    void                                SelectPrivateChatByEndpointId(QString endpointId);

    Q_INVOKABLE
    void                                SendMessageToSelectedPrivateChat(QString message);

    Q_INVOKABLE
    void                                SetMessagesPopupVisible(bool visible);

    void                                ProcessInboundTextMessage(QString source, QString dest, QString message);
signals:
    void                                ticked();
    void                                viewModelReady();
    void                                alertRequested(QString);
    void                                showMessagesPopupRequested();
    void                                newActiveFrequencyMessage(QString);
    void                                newTextMessage(QString,QString,QString);
    void                                selectedPrivateChatChanged();
private slots:
    void                                OnNewActiveFrequencyMessage(QString message);
    void                                OnNewTextMessage(QString source, QString dest, QString message);
private:
    ActiveFrequencyMessagesListModel*   m_activeFrequencyMessagesListModel;
    ATCListModel*                       m_atcListModel;
    ActivePrivateChatsListModel*        m_activePrivateChatsListModel;
    PrivateChatListModel*               m_selectedPrivateChat;
    QString                             m_tickString;
    bool                                m_messagesPopupVisible;
};

extern MainViewModel* sMainViewModel;

#endif
