/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           anticampmanager.inc
 *  Type:           Module
 *  Description:    Anti-camp managing API.
 *
 *  Copyright (C) 2009-2010  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * Creates a new anti-camp feature.
 *
 * @param interval      Optional. How often to trigger action.
 * @param action        Optional. What to do with players.
 * @param amount        Optional. Amount for some action types.
 * @param warningType   Optional. Where the warning should appear.
 * @param message       Optional. Override warning message.
 * @param name          Optional. Name to give this anticamp feature.
 * @return              Anticamp index, or negative on error.
 *                      -1 if no free anticamps
 *                      -2 if invalid interval
 *                      -3 if invalid amount
 */
VolAnticampAdd(Float:interval = ZR_ANTICAMP_DEF_INTERVAL,
               VolAnticampAction:action = ZR_ANTICAMP_DEF_ACTION,
               Float:amount = ZR_ANTICAMP_DEF_AMOUNT,
               VolAnticampWarningType:warningType = Anticamp_Chat,
               const String:message[] = ZR_ANTICAMP_DEF_MESSAGE,
               const String:name[] = ZR_ANTICAMP_DEF_NAME)
{
    // Validate values.
    if (interval <= 0)
    {
        return -2;
    }
    else if (amount < 0)
    {
        return -3;
    }
    
    // Get free anticamp index.
    new anticamp = VolAnticampGetFreeIndex();
    
    // Validate index.
    if (anticamp < 0)
    {
        return -1;
    }
    
    // Apply attributes.
    AnticampData[anticamp][Anticamp_Interval] = interval;
    AnticampData[anticamp][Anticamp_Action] = action;
    AnticampData[anticamp][Anticamp_Amount] = amount;
    AnticampData[anticamp][Anticamp_Warning] = warningType;
    
    TrimString(name);
    strcopy(AnticampData[anticamp][Anticamp_Name], ZR_ANTICAMP_NAME_LEN, name);
    
    if (strlen(message) > 0)
    {
        strcopy(AnticampData[anticamp][Anticamp_Message], ZR_ANTICAMP_MESSAGE_LEN, message);
    }
    
    return anticamp;
}

/**
 * Removes the specified anticamp.
 *
 * @param anticamp      Anticamp to remove.
 * @param recursive     Optional. Also remove the volume and its features.
 *                      Default is false.
 * @return              True if successful, false otherwise.
 */
bool:VolAnticampRemove(anticamp, bool:recursive = false)
{
    // Validate index.
    if (!VolAnticampIsValid(anticamp))
    {
        return false;
    }
    
    // Get the volume that use this anticamp.
    new volumeIndex = AnticampData[anticamp][Anticamp_VolumeIndex];
    
    // Check if recursive removal is enabled.
    if (recursive)
    {
        // Remove volume if valid.
        VolRemove(volumeIndex, recursive);
    }
    else
    {
        // Detach the anticamp if attached.
        VolDetachFeature(volumeIndex, VolFeature_Anticamp, anticamp);
    }
    
    // Remove the anticamp.
    VolAnticampReset(anticamp);
    
    return true;
}

/**
 * Removes all anticamps.
 *
 * @param recursive     Optional. Also remove the volume and its features.
 *                      Default is false.
 * @return              True if successful, false otherwise.
 */
VolAnticampRemoveAll(bool:recursive = false)
{
    for (new anticamp = 0; anticamp < ZR_ANTICAMP_MAX; anticamp++)
    {
        VolAnticampRemove(anticamp, recursive);
    }
}

/**
 * Search for a anticamp with the specified name.
 *
 * @param name              Name to search for.
 * @param caseSensitive     Optional. Use case sensitive search. Default is
 *                          false.
 * @return                  Anticamp index if found, -1 otherwise.
 */
VolAnticampFind(const String:name[], bool:caseSensitive = false)
{
    for (new anticamp = 0; anticamp < ZR_ANTICAMP_MAX; anticamp++)
    {
        if (VolAnticampInUse(anticamp))
        {
            if (StrEqual(AnticampData[anticamp][Anticamp_Name], ZR_ANTICAMP_NAME_LEN, caseSensitive))
            {
                return anticamp;
            }
        }
    }
    
    return -1;
}
