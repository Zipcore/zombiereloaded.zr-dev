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
 * All ammo managers.
 */
new AmmoManagers[AMMO_MANAGER_MAX][AmmoManager];

/**
 * Ammo manager index for assigning ammo managers to clients.
 */
new AmmoManagerIndex[MAXPLAYERS + 1][WeaponsSlot];


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
 * Assigns new ammo managers to the specified client if they aren't assigned.
 *
 * @param client    Client index to assign ammo managers to.
 */
AmmoAssignManagers(client)
{
    // Get free ammo managers if they don't exist.
    for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
    {
        if (AmmoManagerIndex[client][slot] < 0)
        {
            new manager = AmmoGetFreeManager();
            
            // Verify index.
            if (manager < 0)
            {
                // This should never happen. It's just here to help catching bugs.
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo manager error", "Unable to get a free ammo manager for client %d. No free managers.", client);
                return;
            }
            
            // Assign manager to client.
            AmmoManagerIndex[client][slot] = manager;
        }
    }
}

/**
 * Attaches and enables ammo managers for the specified client.
 *
 * @param client    Client index.
 */
AmmoManagerAttach(client)
{
    // Get free ammo managers if they don't exist.
    AmmoAssignManagers(client);
    
    // Get indexes.
    new weaponProfile = ClassGetWeaponProfile(client);
    new primaryProfile = WeaponProfilesGetPriProfile(weaponProfile);
    new secondaryProfile = WeaponProfilesGetSecProfile(weaponProfile);
    
    // Load profiles.
    AmmoManagerLoad(AmmoManagerIndex[client][Slot_Primary], primaryProfile);
    AmmoManagerLoad(AmmoManagerIndex[client][Slot_Secondary], secondaryProfile);
    
    // Enable managers for each slot.
    for (new manager = 0; manager < sizeof(g_WeaponsSlotDummy); manager++)
    {
        AmmoManagerEnable(manager);
    }
}

/**
 * Detaches and disables ammo managers for the specified client.
 *
 * @param client    Client index.
 */
AmmoManagerDetach(client)
{
    // Disable managers for each slot.
    for (new manager = 0; manager < sizeof(g_WeaponsSlotDummy); manager++)
    {
        AmmoManagerDisable(manager);
    }
    
    // Clear managers.
    AmmoManagerClear(AmmoManagerIndex[client][Slot_Primary]);
    AmmoManagerClear(AmmoManagerIndex[client][Slot_Secondary]);
    
    // Remove indexes.
    for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
    {
        AmmoManagerIndex[client][slot] = -1;
    }
}

/**
 * Initializes the ammo manager.
 */
AmmoManagerInit()
{
    // Clear all ammo managers.
    for (new manager = 0; manager < AMMO_MANAGER_MAX; manager++)
    {
        AmmoManagerClear(manager);
    }
    
    // Reset indexes.
    for (new client = 0; client < MAXPLAYERS + 1; client++)
    {
        for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
        {
            AmmoManagerIndex[client][slot] = -1;
        }
    }
}

/**
 * Load the specified ammo manager with the specified profile.
 *
 * @param index         Ammo manager index.
 * @param ammoProfile   Ammo profile index.
 */
stock AmmoManagerLoad(index, ammoProfile)
{
    
}

/**
 * Resets data in the specified ammo manager and marks it as free.
 *
 * @param index     Ammo manager index.
 * @param inUse     Optional. Specifies whether the ammo manager should still
 *                  be marked as in use.
 */
AmmoManagerClear(index, bool:inUse = false)
{
    AmmoManagers[index][AmmoManager_InUse] = inUse;
    AmmoManagers[index][AmmoManager_Slot] = Slot_Invalid;
    AmmoManagers[index][AmmoManager_Clip] = 0;
    AmmoManagers[index][AmmoManager_Reserve] = 0;
    AmmoManagers[index][AmmoManager_ProfileIndex] = -1;
    AmmoManagers[index][AmmoManager_Timer] = INVALID_HANDLE;
}

/**
 * Applies current state of a ammo manager on a client.
 *
 * @param client    Client to apply to.
 * @param index     Ammo manager to apply.
 * @return          True on success, false otherwise.
 */
stock bool:AmmoManagerApply(client, index)
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
    new ammoProfile = AmmoManagers[index][AmmoManager_ProfileIndex];
    new AmmoMode:mode = AmmoGetMode(ammoProfile);
    if (mode == AmmoMode_FillClip ||
        mode == AmmoMode_FillReserve)
    {
        new Float:interval = AmmoGetInterval(ammoProfile);
        AmmoManagers[index][AmmoManager_Timer] = CreateTimer(interval, AmmoManagerTimer, index, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
        return true;
    }
    
    return false;
}

/**
 * Timer callback for ammo manager.
 */
public Action:AmmoManagerTimer(Handle:timer, any:index)
{
    new ammoProfile = AmmoManagers[index][AmmoManager_ProfileIndex];
    new AmmoMode:mode = AmmoGetMode(ammoProfile);
    
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
stock bool:AmmoIsValidManager(index)
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
            // Clear data.
            AmmoManagerClear(manager);
            
            // Mark as in use and return the index.
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
stock AmmoManagerSetProfile(index, profile)
{
    AmmoManagers[index][AmmoManager_ProfileIndex] = profile;
}

/**
 * Gets the ammo profile index in a ammo manager.
 *
 * @param index     Ammo manager.
 * @return          Ammo profile.
 */
stock AmmoManagerGetProfile(index)
{
    return AmmoManagers[index][AmmoManager_ProfileIndex];
}

/**
 * Sets the slot ID in a ammo manager.
 *
 * @param index     Ammo manager.
 * @param slot      What weapon slot the manager is managing.
 */
stock AmmoManagerSetSlot(index, WeaponsSlot:slot)
{
    AmmoManagers[index][AmmoManager_Slot] = slot;
}

/**
 * Gets the slot ID in a ammo manager.
 *
 * @param index     Ammo manager.
 * @return          What weapon slot the manager is managing.
 */
stock WeaponsSlot:AmmoManagerGetSlot(index)
{
    return AmmoManagers[index][AmmoManager_Slot];
}
