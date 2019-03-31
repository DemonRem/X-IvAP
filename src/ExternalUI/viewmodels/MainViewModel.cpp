/*
 * File:   MainViewModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "MainViewModel.h"

#include "../network/ExternalInterfaceNetworkClient.h"

#include "../SimulatorData.h"
#include "../Utility.h"

#include <QDebug>

MainViewModel* sMainViewModel = 0;

MainViewModel::MainViewModel()
{
    m_activeFrequencyMessagesListModel = new ActiveFrequencyMessagesListModel();
    m_atcListModel = new ATCListModel();
    m_activePrivateChatsListModel = new ActivePrivateChatsListModel();

    m_selectedPrivateChat = 0;
    m_messagesPopupVisible = false;

    connect(this, SIGNAL(newActiveFrequencyMessage(QString)), this, SLOT(OnNewActiveFrequencyMessage(QString)));
    connect(this, SIGNAL(newTextMessage(QString,QString,QString)), this, SLOT(OnNewTextMessage(QString,QString,QString)));

    emit viewModelReady();
}

MainViewModel::~MainViewModel()
{

}

ATCListModel* MainViewModel::GetATCListModel()
{
    return m_atcListModel;
}

void MainViewModel::Tic()
{
    if(m_tickString == "")
        m_tickString = "*";
    else
        m_tickString = "";

    emit ticked();
}

void MainViewModel::ShowAlert(const std::string &text)
{
    emit alertRequested(QString::fromStdString(text));
}

void MainViewModel::AppendActiveFrequencyMessage(QString message)
{
    emit newActiveFrequencyMessage(message);
}

void MainViewModel::OnNewActiveFrequencyMessage(QString message)
{
    m_activeFrequencyMessagesListModel->AppendMessage(QString("%1\r\n").arg(message));
}

void MainViewModel::OnNewTextMessage(QString source, QString dest, QString message)
{
    if(dest == sSimulatorData->GetNetworkCallsign() && source != "SERVER")
    {
        // This is a private message, to me.
        if(!m_activePrivateChatsListModel->HasActiveChatWithEndpoint(source))
        {
            m_activePrivateChatsListModel->CreateActiveChatWithEndpoint(source);
        }

        PrivateChatListModel* pPrivateChat = m_activePrivateChatsListModel->GetPrivateChatModelByEndpoint(source);

        pPrivateChat->AppendMessage(false, message);

        if(pPrivateChat != m_selectedPrivateChat || !m_messagesPopupVisible)
        {
            pPrivateChat->SetHasUnreadMessages(true);
        }
    }
}

void MainViewModel::ToggleTransponderMode()
{
    sExternalInterfaceNetworkClient->SendMessage("XIVAPEI_CL_XPDR_MODE_TOGGLE");
}

void MainViewModel::TransponderIdent()
{
    sExternalInterfaceNetworkClient->SendMessage("XIVAPEI_CL_XPDR_IDENT");
}

void MainViewModel::TuneRadioToFrequency(int radioIndex, QString frequency)
{
    frequency = frequency.replace(".", "");

    QString message = QString("XIVAPEI_CL_RADIOTUNE|%1|%2").arg(radioIndex).arg(frequency);

    sExternalInterfaceNetworkClient->SendMessage(message.toStdString());
}

void MainViewModel::SetActiveRadio(int radioIndex)
{
    QString message = QString("XIVAPEI_CL_RADIOACTIVE|%1").arg(radioIndex);

    sExternalInterfaceNetworkClient->SendMessage(message.toStdString());
}

void MainViewModel::SendMessageToActiveFrequency(QString message)
{
    if(message.startsWith(".x "))
    {
        std::vector<std::string> exploded = Utility::str_explode(message.toStdString(), ' ');

        if(exploded.size() >= 2)
        {
            std::string xpdrCodeStr = exploded[1];

            if(xpdrCodeStr.length() != 4)
                return;

            for(int i = 0; i < 4; i++)
            {
                if(xpdrCodeStr[i] < '0' || xpdrCodeStr[i] > '7')
                    return;
            }

            QString message = QString("XIVAPEI_CL_XPDR_CODE_A|%1").arg(QString::fromStdString(xpdrCodeStr));
            sExternalInterfaceNetworkClient->SendMessage(message.toStdString());
        }
    }
    else if(message.startsWith(".c1 ") || message.startsWith(".c2 "))
    {
        std::vector<std::string> exploded = Utility::str_explode(message.toStdString(), ' ');

        if(exploded.size() >= 2)
        {
            QString freqStr = QString::fromStdString(exploded[1]);
            freqStr.replace(".", "");

            int freqValue = atoi(freqStr.toStdString().c_str());

            if(freqValue >= 118000 && freqValue <= 136975)
            {
                // Send this to server.
                QString destination = "";
                if(message.startsWith(".c1"))
                    destination = "1";
                else if(message.startsWith(".c2"))
                    destination = "2";
                else
                    return;

                QString message = QString("XIVAPEI_CL_RADIOTUNE|%1|%2").arg(destination).arg(freqStr);
                sExternalInterfaceNetworkClient->SendMessage(message.toStdString());
            }
        }
    }
    else if(message.startsWith(".chat"))
    {
        std::vector<std::string> exploded = Utility::str_explode(message.toStdString(), ' ');

        if(exploded.size() >= 2)
        {
            QString endpointId = QString::fromStdString(exploded[1]);

            if(!endpointId.isEmpty())
            {
                if(!m_activePrivateChatsListModel->HasActiveChatWithEndpoint(endpointId))
                {
                    m_activePrivateChatsListModel->CreateActiveChatWithEndpoint(endpointId);
                }

                // Switch to it visually - trigger popup if needed, and switch to that chat in display.
                SelectPrivateChatByEndpointId(endpointId);

                if(m_selectedPrivateChat)
                {
                    QString textMessage = QString("chat initialized with %1").arg(endpointId);
                    m_selectedPrivateChat->AppendMessage(false, textMessage);
                }

                emit showMessagesPopupRequested();
            }
        }
    }
    else
    {
        QString networkMessage = QString("XIVAPEI_CL_TEXTMESSAGE|%1|%2").arg(sSimulatorData->GetActiveFreq()).arg(message);
        sExternalInterfaceNetworkClient->SendMessage(networkMessage.toStdString());
    }
}

void MainViewModel::SetIVAOUseWeather(bool useWeather)
{
    QString networkMessage = QString("XIVAPEI_CL_IVAO_WX|%1").arg(useWeather ? 1 : 0);
    sExternalInterfaceNetworkClient->SendMessage(networkMessage.toStdString());
}

void MainViewModel::SetTagging(bool useTagging)
{
    QString networkMessage = QString("XIVAPEI_CL_TAGGING|%1").arg(useTagging ? 1 : 0);
    sExternalInterfaceNetworkClient->SendMessage(networkMessage.toStdString());
}

void MainViewModel::SelectPrivateChatByEndpointId(QString endpointId)
{
    if(m_activePrivateChatsListModel->HasActiveChatWithEndpoint(endpointId))
    {
        m_selectedPrivateChat = m_activePrivateChatsListModel->GetPrivateChatModelByEndpoint(endpointId);
        m_selectedPrivateChat->SetHasUnreadMessages(false);

        emit selectedPrivateChatChanged();
    }
}

void MainViewModel::SendMessageToSelectedPrivateChat(QString message)
{
    if(m_selectedPrivateChat)
    {
        QString networkMessage = QString("XIVAPEI_CL_TEXTMESSAGE|%1|%2").arg(m_selectedPrivateChat->GetEndpointId()).arg(message);
        sExternalInterfaceNetworkClient->SendMessage(networkMessage.toStdString());

        m_selectedPrivateChat->AppendMessage(true, message);
    }
}

void MainViewModel::SetMessagesPopupVisible(bool visible)
{
    m_messagesPopupVisible = visible;

    if(visible && m_selectedPrivateChat)
    {
        m_selectedPrivateChat->SetHasUnreadMessages(false);
    }
}

void MainViewModel::ProcessInboundTextMessage(QString source, QString dest, QString message)
{
    emit newTextMessage(source, dest, message);
}
