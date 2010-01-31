/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ammomanager.inc
 *  Description:   Ammo profile manager for players.
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

#include "zr/weapons/ammomanager.h.inc"

/**
 * Player ammo managers.
 */
new AmmoManagers[AMMO_MANAGER_MAX][AmmoManager];


AmmoManagerEnable(index)
{
    // Attempt to start the timer.
    AmmoManagerStartTimer(index);
}

AmmoManagerDisable(index)
{
    // Make sure the timer is stopped.
    AmmoManagerStopTimer(index);
}

/**
 * Initialize the specified ammo manager with the specified profile.
 *
 * @param index         Ammo manager index.
 * @param ammoProfile   Ammo profile index.
 */
AmmoManagerInit(index, ammoProfile)
{
    
}

/**
 * Applies current state of a ammo manager on a client.
 *
 * @param client    Client to apply to.
 * @param index     Ammo manager to apply.
 * @return          True on success, false otherwise.
 */
bool:AmmoManagerApply(client, index)
{
    
}

/**
 * Stops the timer if it's running.
 *
 * @param index Ammo manager.
 * @return      True if the timer was stopped, false otherwise.
 */
bool:AmmoManagerStopTimer(index)
{
    new Handle:timer = AmmoManagers[index][AmmoManager_Timer];
    if (timer != INVALID_HANDLE)
    {
        KillTimer(timer);
        AmmoManagers[index][AmmoManager_Timer] = INVALID_HANDLE;
        return true;
    }
    
    return false;
}

/**
 * Starts the timer.
 *
 * @param index     Ammo manager.
 * @return          True if the timer was started, false otherwise.
 */
bool:AmmoManagerStartTimer(index)
{
    // Only start if the current mode needs a timer.
    ammoProfile = AmmoManagers[managerIndex][AmmoManager_ProfileIndex];
    AmmoMode:mode = AmmoGetMode(ammoProfile);
    if (mode == AmmoMode_FillClip ||
        mode == AmmoMode_FillReserve)
    {
        new Float:interval = AmmoGetInterval(ammoProfile);
        AmmoManagers[index][AmmoManager_Timer] = CreateTimer(interval, AmmoManagerTimer, index, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
    }
    else
    {
        return false;
    }
}

/**
 * Timer callback for ammo manager.
 */
public Action:AmmoManagerTimer(Handle:timer, any:index)
{
    ammoProfile = AmmoManagers[index][AmmoManager_ProfileIndex];
    AmmoMode:mode = AmmoGetMode(ammoProfile);
    
    switch (mode)
    {
        case AmmoMode_FillClip:
        {
            
        }
        case AmmoMode_FillReserve:
        {
            
        }
    }
}


/***************************
 *                         *
 *   ATTRIBUTE FUNCTIONS   *
 *                         *
 ***************************/

/**
 * Returns if a ammo manager index is valid or not.
 *
 * @param index     Index to validate.
 * @return          True if valid, false otherwise.
 */
bool:AmmoManagerIsValid(index)
{
    if (index >= 0 && index < AMMO_MANAGER_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Returns a free ammo manager index and marks it as in use.
 *
 * @return  Ammo manager index, or -1 if there are no free managers.
 */
AmmoGetFreeManager()
{
    // Loop through all managers.
    for (new manager = 0; manager < AMMO_MANAGER_MAX; manager++)
    {
        // Check if unused.
        if (!AmmoManagers[manager][AmmoManager_InUse])
        {
            // Mark as in use and return index.
            AmmoManagers[manager][AmmoManager_InUse] = true;
            return manager;
        }
    }
    
    return -1;
}

/**
 * Sets ammo profile index in a ammo manager.
 *
 * @param index     Ammo manager.
 * @param profile   Ammo profile.
 */
AmmoManagerSetProfile(index, profile)
{
    AmmoManagers[index][AmmoManager_ProfileIndex] = profile;
}

/**
 * Gets the ammo profile index in a ammo manager.
 *
 * @param index     Ammo manager.
 * @return          Ammo profile.
 */
AmmoManagerGetProfile(index)
{
    return AmmoManagers[index][AmmoManager_ProfileIndex];
}
