/*
 * File:   main.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "configuration/ExternalUIConfiguration.h"

#include "network/ASIOCore.h"
#include "network/ExternalInterfaceNetworkClient.h"

#include "viewmodels/MainViewModel.h"
#include "viewmodels/ConnectViewModel.h"
#include "viewmodels/NetworkConnectViewModel.h"

#include "SimulatorData.h"

#include <thread>

std::thread tickThread;

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    // Init ASIO Core
    ASIOCore_Initialize();

    // Create the network client.
    sExternalInterfaceNetworkClient = std::make_shared<ExternalInterfaceNetworkClient>();

    QQmlApplicationEngine engine;

    // Load Configuration
    sExternalUIConfiguration = new ExternalUIConfiguration();
    sExternalUIConfiguration->LoadConfiguration();
    engine.rootContext()->setContextProperty("ExternalUIConfiguration", sExternalUIConfiguration);

    // Init Simulator Data
    sSimulatorData = new SimulatorData();
    engine.rootContext()->setContextProperty("SimulatorData", sSimulatorData);

    sMainViewModel = new MainViewModel();
    engine.rootContext()->setContextProperty("MainViewModel", sMainViewModel);

    // Create the view models and register them.
    sConnectViewModel = new ConnectViewModel();
    engine.rootContext()->setContextProperty("ConnectViewModel", sConnectViewModel);

    sNetworkConnectViewModel = new NetworkConnectViewModel();
    engine.rootContext()->setContextProperty("NetworkConnectViewModel", sNetworkConnectViewModel);

    QFontDatabase::addApplicationFont(":/fonts/B612-Regular.ttf");

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    bool tickThreadContinue = false;
    // Start tick thread
    tickThread = std::thread([&tickThreadContinue]()
    {
        tickThreadContinue = true;
        while(tickThreadContinue)
        {
            sMainViewModel->Tic();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    int ret = app.exec();

    tickThreadContinue = false;
    tickThread.join();

    sExternalInterfaceNetworkClient->Disconnect();

    // Shutdown ASIO Core
    ASIOCore_Shutdown();

    return ret;
}
