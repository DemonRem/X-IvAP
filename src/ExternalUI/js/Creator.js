/*
 * File:   Creator.js
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

.pragma library

.import QtQuick 2.2 as QtQuick

var mainWindow = null;

function getMainWindow()
{
    return mainWindow;
}

function setMainWindow(window)
{
    mainWindow = window;
}

function createComponent(uri)
{
    var c = Qt.createComponent(uri);

    if(c === null)
    {
        console.error("Failed to create component (null) for " + uri);
        return null;
    }

    if(c.status !== QtQuick.Component.Ready)
    {
        console.error("Failed to create component (status=" + c.status + ") for " + uri);
        console.error(c.errorString());
        return null;
    }

    return c;
}

function createObjectFromUri(uri)
{
    var c = createComponent(uri);

    if(c === null)
        return null;

    return c.createObject();
}
