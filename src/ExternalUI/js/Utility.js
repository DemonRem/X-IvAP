/*
 * File:   Utility.js
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

.pragma library

function validateIPAddress(address)
{
    return (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(address));
}

function lpad(num, size)
{
    var s = num+"";
    while (s.length < size) s = "0" + s;
    return s;
}
