/*
 * File:   PopupManager.js
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

.pragma library

.import "Creator.js" as Creator

// Shows the specified popup on top of the stack.
// Parameter name is the name of the popup (without "Popup")
function showPopup(name)
{
    var c = Creator.createComponent("qrc:/qml/popups/" + name + "Popup.qml");

    if(c === null)
        return;

    var win = Creator.getMainWindow();

    var obj = c.createObject(win, { "parent": win });

    if(obj === null)
        return;

    obj.x = win.width / 2.0 - obj.width / 2.0;
    obj.y = win.height / 2.0 - obj.height / 2.0;

    obj.open();

    return obj;
}

function alert(text)
{
    var dialog = Creator.createObjectFromUri("qrc:/qml/popups/AlertDialog.qml");

    if(dialog !== null)
    {
        dialog.text = text;
        dialog.open();
    }
}
