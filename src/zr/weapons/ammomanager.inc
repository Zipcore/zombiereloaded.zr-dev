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
#include "zr/weapons/ammoevents.inc"

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
    if (AmmoIsValidManager(index))
    {
        AmmoManagerStartTimer(index);
    }
}

AmmoManagerDisable(index)
{
    // Make sure the timer is stopped.
    if (AmmoIsValidManager(index))
    {
        ZREndTimer(AmmoManagers[index][AmmoManager_Timer]);
    }
}

/**
 * Assigns new ammo managers to the specified client if they aren't assigned.
 *
 * @param client    Client index to assign ammo managers to.
 */
AmmoAssignManagers(client)
{
    // Get free ammo managers for each slot if they don't exist.
    for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
    {
        // Check if the slot is supported and that there's no ammo manager
        // assigned to that slot.
        if (AmmoManagedSlots[slot] && AmmoManagerIndex[client][slot] < 0)
        {
            new manager = AmmoGetFreeManager();
            
            // Verify index.
            if (!AmmoIsValidManager(manager))
            {
                // This should never happen. It's just here to help catching bugs.
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo manager error", "Unable to get a free ammo manager for client %d. No free managers.", client);
                return;
            }
            
            // Assign manager to client.
            AmmoManagerIndex[client][slot] = manager;
            
            // Set slot type in manager.
            AmmoManagers[manager][AmmoManager_Slot] = WeaponsSlot:slot;
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
    // Assign ammo managers if they don't exist.
    AmmoAssignManagers(client);
    
    // Get profile indexes.
    new weaponProfile = ClassGetWeaponProfile(client);
    
    // Verify weapon profile. It might not be available.
    if (WeaponProfilesIsValidIndex(weaponProfile))
    {
        new profiles[WeaponsSlot];
        WeaponProfilesGetAmmoProfiles(weaponProfile, profiles);
        
        // Load ammo profiles.
        for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
        {
            // Try to attach the manager. If it fails the slot is not supported or
            // the ammo profile index is invalid.
            AmmoManagerAttachSlot(client, WeaponsSlot:slot, profiles[slot]);
        }
        
        LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Weapons, "Ammo manager", "Attatched ammo managers on %N.", client);
    }
}

/**
 * Attaches and enables a single ammo manager for a certain slot.
 *
 * @param client        Client index.
 * @param slot          What slot to attach.
 * @param ammoProfile   Ammo profile to use.
 * @return              True if successful, false otherwise.
 */
bool:AmmoManagerAttachSlot(client, WeaponsSlot:slot, ammoProfile)
{
    // Verify that the slot is managed.
    if (!AmmoManagedSlots[slot])
    {
        // The specified slot is not supported.
        return false;
    }
    
    // Verify ammo profile index.
    if (!AmmoIsValidIndex(ammoProfile))
    {
        // Index out of bounds.
        return false;
    }
    
    // Get the client's manager index for the specified slot.
    new manager = AmmoManagerIndex[client][slot];
    
    // Verify manager index.
    if (!AmmoIsValidManager(manager))
    {
        return false;
    }
    
    // Set ammo profile index.
    AmmoManagers[manager][AmmoManager_ProfileIndex] = ammoProfile;
    
    // Enable the manager.
    AmmoManagerEnable(manager);
    
    return true;
}

/**
 * Detaches and disables ammo managers for the specified client.
 *
 * @param client    Client index.
 */
AmmoManagerDetach(client)
{
    // Loop through all slots.
    for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
    {
        // Try to detach ammo manager. If it fails it's already detached.
        AmmoManagerDetachSlot(client, WeaponsSlot:slot);
    }
    
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Weapons, "Ammo manager", "Detached ammo managers on %N.", client);
}

/**
 * Detaches and disables a single ammo manager for a certain slot.
 *
 * @param client        Client index.
 * @param slot          What slot to detach.
 * @return              True if successful, false otherwise.
 */
bool:AmmoManagerDetachSlot(client, WeaponsSlot:slot)
{
    // Verify that the slot is managed.
    if (!AmmoManagedSlots[slot])
    {
        // The specified slot is not supported.
        return false;
    }
    
    // Get the client's manager index for the specified slot.
    new manager = AmmoManagerIndex[client][slot];
    
    // Verify manager index.
    if (!AmmoIsValidManager(manager))
    {
        return false;
    }
    
    // Disable the manager.
    AmmoManagerDisable(manager);
    
    // Clear manager data.
    AmmoManagerClear(manager);
    
    // Unassign manager from the client.
    AmmoManagerIndex[client][slot] = -1;
    
    return true;
}

/**
 * Detaches all ammo managers for all clients.
 */
AmmoManagerDetachAll()
{
    for (new client = 1; client < MaxClients; client++)
    {
        if (IsClientConnected(client))
        {
            AmmoManagerDetach(client);
        }
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
    
    // Reset manager indexes per client per slot.
    for (new client = 0; client < MAXPLAYERS + 1; client++)
    {
        for (new slot = 0; slot < sizeof(g_WeaponsSlotDummy); slot++)
        {
            AmmoManagerIndex[client][slot] = -1;
        }
    }
}

/**
 * Resets data in the specified ammo manager and marks it as free.
 *
 * @param index     Ammo manager index.
 * @param inUse     Optional. Specifies whether the ammo manager should still
 *                  be marked as in use. Default is false.
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
    // TODO: Here it should set player clip and clip reserve based on the
    //       current values in its ammo manager.
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
