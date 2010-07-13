/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          menulib.inc
 *  Type:          Library
 *  Description:   
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
 * Menu filters.
 */
#define MENULIB_FILTER_TEAM         (1 << 0)    /** Only allow clients on a team. */
#define MENULIB_FILTER_UNASSIGNED   (1 << 1)    /** Only allow clients not on a team. */
#define MENULIB_FILTER_ALIVE        (1 << 2)    /** Only allow live clients. */
#define MENULIB_FILTER_DEAD         (1 << 3)    /** Only allow dead clients. */
#define MENULIB_FILTER_CANTARGET    (1 << 4)    /** Only list clients that the menu recipient can target. */

/**
 * Shows a list of all clients to a client, different handlers can be used for this, as well as title.
 * 
 * @param client    The client index.
 * @param handler   The menu handler.
 * @param title     The title of the menu.
 * @param filters   A bit field made up of MENULIB_FILTER_* defines.  See top of file.
 *                  Note: This is only checked at time of menu creation.  It is
 *                  recommended to re-check these filters in the callback.
 */
stock MenuLib_ClientMenu(client, MenuHandler:handler, const String:title[], filters)
{
    new Handle:menu_clients = CreateMenu(handler);
    SetMenuTitle(menu_clients, title);
    
    decl String:clientoption[64];
    decl String:clientuserid[8];
    new count;
    
    for (new menuclient = 1; menuclient <= MaxClients; menuclient++)
    {
        if (!IsClientInGame(menuclient))
            continue;
        
        // Check filters.
        if (Util_IsClientOnTeam(menuclient))
        {
            // The client is on a team, so check if the filter is asking only for unassigned clients.
            if (filters & MENULIB_FILTER_UNASSIGNED)
                continue;
        }
        else
        {
            // The client is unassigned, so check if the filter is asking only for clients on a team.
            if (filters & MENULIB_FILTER_TEAM)
                continue;
        }
        
        if (IsPlayerAlive(menuclient))
        {
            // The client is alive, so check if the filter is asking only for dead clients.
            if (filters & MENULIB_FILTER_DEAD)
                continue;
        }
        else
        {
            // The client is dead, so check if the filter is asking only for alive clients.
            if (filters & MENULIB_FILTER_TEAM)
                continue;
        }
        
        // Check if the menu recipient can target this client, if not then don't add to the menu.
        if (filters & MENULIB_FILTER_CANTARGET && !CanUserTarget(client, menuclient))
            continue;
        
        // Get client info.
        GetClientName(menuclient, clientoption, sizeof(clientoption));
        IntToString(GetClientUserId(menuclient), clientuserid, sizeof(clientuserid));
        
        // Add option to menu.
        AddMenuItem(menu_clients, clientuserid, clientoption);
        count++;
    }
    
    // If there are no clients, add an "(Empty)" line.
    if (count == 0)
    {
        decl String:empty[64];
        Format(empty, sizeof(empty), "%T", client, "Menu empty");
        AddMenuItem(menu_clients, "empty", empty, ITEMDRAW_DISABLED);
    }
    
    SetMenuExitBackButton(menu_clients, true);
    DisplayMenu(menu_clients, client, MENU_TIME_FOREVER);
}

/**
 * Retrieves the client index given a menu slot from a MenuLib_ClientMenu-constructed menu.
 * Must be used from inside the menu's callback.
 * 
 * @param menu  The menu handle.
 * @param slot  The menu slot that was selected.
 * 
 * @return      The client index, 0 if the selected client is no longer in the server.
 */
stock MenuLib_GetClientIndex(Handle:menu_clients, slot)
{
    // Get string stored in the menu slot.
    decl String:clientuserid[8];
    GetMenuItem(menu_clients, slot, clientuserid, sizeof(clientuserid));
    
    // Return the targeted client through their userid stored as a string in the menu slot.
    return GetClientOfUserId(StringToInt(clientuserid));
}

/**
 * Converts a boolean value into a menu draw define.
 * true =   ITEMDRAW_DEFAULT
 * false =  ITEMDRAW_DISABLED
 * 
 * @param condition The boolean to convert. 
 */
stock MenuLib_GetMenuItemDraw(bool:condition)
{
    return condition ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
}
