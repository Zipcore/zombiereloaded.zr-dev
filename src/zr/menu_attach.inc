/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          menu_attach.inc
 *  Type:          Core
 *  Description:   Attach to main SM admin menu from here.
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
 * @section @section Global variable to keep track of the admin menu and attached sub-menu.
 */
new Handle:g_hAdminMenu = INVALID_HANDLE;
new TopMenuObject:g_objZAdmin;
/**
 * @endsection
 */

/**
 * Attach to the SM admin menu.
 */
MenuAttachInit()
{
    // Check if the admin menu is ready to be attached to.
	new Handle:topmenu;
	if (LibraryExists("adminmenu") && ((topmenu = GetAdminTopMenu()) != INVALID_HANDLE))
	{
		/* If so, manually fire the callback */
		OnAdminMenuReady(topmenu);
	}
}

/**
 * Called when the a library is removed from the server.
 */
public OnLibraryRemoved(const String:name[])
{
    if (StrEqual(name, "adminmenu"))
    {
        g_hAdminMenu = INVALID_HANDLE;
    }
}

/**
 * Called when the admin menu is ready to be attached to.
 */
public OnAdminMenuReady(Handle:topmenu)
{
    // Make sure this doesn't get called twice.
	if (topmenu == g_hAdminMenu)
	{
		return;
	}
	
	// Copy topmenu to hAdminMenu to work with the above check.
	g_hAdminMenu = topmenu;
	
	// Attach to the menu.
	g_objZAdmin = AddToTopMenu(g_hAdminMenu, "ZR Submenu", TopMenuObject_Category, MenuAttachZAdminSubmenu, INVALID_TOPMENUOBJECT, _, ADMFLAG_GENERIC);
	
	// Create an item in our sub-menu to link to ZMarket.
	AddToTopMenu(g_hAdminMenu, "ZAdmin", TopMenuObject_Item, MenuAttachZAdmin, g_objZAdmin, "zadmin", ADMFLAG_GENERIC);
}

/**
 * Callback for the ZR submenu in the SM admin menu
 * Formats the item name in the main admin menu, and the title in the submenu.
 * 
 * @param topmenu			Handle to the TopMenu.
 * @param action			TopMenuAction being performed.
 * @param object_id			The object ID (if used).
 * @param param				The client index.
 * @param buffer			Output buffer (if used).
 * @param maxlength			Output buffer (if used).
 */
public MenuAttachZAdminSubmenu(Handle:topmenu, TopMenuAction:action, TopMenuObject:object_id, param, String:buffer[], maxlength)
{
    switch(action)
    {
        // The submenu item in the main admin menu is being rendered.
        case TopMenuAction_DisplayOption:
        {
            // Format the buffer from the translations file.
            SetGlobalTransTarget(param);
            Format(buffer, maxlength, "%t", "Menu attach submenu zr");
        }
        // The submenu title is being rendered in the submenu.
        case TopMenuAction_DisplayTitle:
        {
            // Format the buffer from the translations file.
            Format(buffer, maxlength, "%t", "Menu attach submenu zr title");
        }
    }
}

/**
 * Callback for the ZR submenu in the SM admin menu
 * Formats the item name in the ZR submenu, and sends the ZAdmin menu when selected.
 * 
 * @param topmenu			Handle to the TopMenu.
 * @param action			TopMenuAction being performed.
 * @param object_id			The object ID (if used).
 * @param param				The client index.
 * @param buffer			Output buffer (if used).
 * @param maxlength			Output buffer (if used).
 */
public MenuAttachZAdmin(Handle:topmenu, TopMenuAction:action, TopMenuObject:object_id, param, String:buffer[], maxlength)
{
    switch(action)
    {
        // The submenu is rendering and the ZAdmin option needs to be formatted.
        case TopMenuAction_DisplayOption:
        {
            // Format the buffer from the translations file.
            SetGlobalTransTarget(param);
            Format(buffer, maxlength, "%t", "Menu attach submenu zr zadmin");
        }
        // The ZAdmin option is being selected by the client.
        case TopMenuAction_SelectOption:
        {
            // Send ZAdmin menu.
            ZAdminMenu(param);
        }
    }
}
