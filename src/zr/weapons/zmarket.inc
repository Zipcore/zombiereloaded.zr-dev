/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zmarket.inc
 *  Type:          Module
 *  Description:   ZMarket module, provides menu of weapons to buy from.
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
 * @section Name of the cookies for ZMarket rebuy/auto-rebuy data.
 */
#define ZMARKET_COOKIE_AUTOREBUY "zr_zmarket_autorebuy"
#define ZMARKET_COOKIE_REBUY "zr_zmarket_rebuy"
/**
 * @endsection
 */

/**
 * Token to separate each weapon in a client's weapon setup (define value basically obsolete)
 */
#define ZMARKET_WEAPON_COOKIE_TOKEN "|"

/**
 * The maximum number of weapons you can have per slot.
 */
#define ZMARKET_MAX_WEAPONS_PER_SLOT 20

/**
 * Variable to store buyzone offset value.
 */
new g_iToolsInBuyZone;

/**
 * Array to store the client's current weapon type within menu.
 */
new g_iZMarketCurType[MAXPLAYERS + 1];

/**
 * Array handle to store the amount of purchases left for a weapon.
 */
new Handle:g_hZMarketPurchaseCount[MAXPLAYERS + 1];

/**
 * Cookie handle for auti-rebuy.
 */
new Handle:g_hZMarketAutoRebuyCookie = INVALID_HANDLE;

/**
 * Cookie handle array for weapon setups.
 */
new Handle:g_hZMarketWeaponSetupCookie[WEAPONS_SLOTS_MAX + 1];

/**
 * Create commands specific to ZMarket.
 */
ZMarketOnCommandsCreate()
{
    // Register ZMarket command.
    RegConsoleCmd(SAYHOOKS_KEYWORD_ZMARKET, ZMarketCommand, "Opens custom buymenu.");
}

ZMarketOnCookiesCreate()
{
    // Create auto-rebuy cookie handle if doesn't exist.
    if (g_hZMarketAutoRebuyCookie == INVALID_HANDLE)
    {
        g_hZMarketAutoRebuyCookie = RegClientCookie(ZMARKET_COOKIE_AUTOREBUY, "The toggle state of auto-rebuy.", CookieAccess_Protected);
    }
    
    decl String:rebuycookiename[32];
    decl String:rebuycookiedesc[64];
    
    // x = Weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX + 1; x++)
    {
        // Format cookie name and description.
        Format(rebuycookiename, sizeof(rebuycookiename), "%s_%d", ZMARKET_COOKIE_REBUY, x);
        Format(rebuycookiedesc, sizeof(rebuycookiedesc), "Current weapon(s) in weapon setup for slot %d", x);
        
        // Create weapon setup cookie handle if it doesn't exist.
        if (g_hZMarketWeaponSetupCookie[x] == INVALID_HANDLE)
        {
            g_hZMarketWeaponSetupCookie[x] = RegClientCookie(rebuycookiename, rebuycookiedesc, CookieAccess_Protected);
        }
    }
}

/**
 * Find ZMarket-specific offsets here.
 */
ZMarketOnOffsetsFound()
{
    // If offset "m_bInBuyZone" can't be found, then stop the plugin.
    g_iToolsInBuyZone = FindSendPropInfo("CCSPlayer", "m_bInBuyZone");
    if (g_iToolsInBuyZone == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CCSPlayer::m_bInBuyZone\" was not found.");
    }
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
ZMarketClientInit(client)
{
    // If purchase count data exists, then destroy before creating again.
    if (g_hZMarketPurchaseCount[client] != INVALID_HANDLE)
    {
        CloseHandle(g_hZMarketPurchaseCount[client]);
    }
    
    // Create a new array handle to store purchase count data for client.
    g_hZMarketPurchaseCount[client] = CreateTrie();
    
    // Initialize auto-rebuy data.
    decl String:zmarketautorebuy[8];
    GetClientCookie(client, g_hZMarketAutoRebuyCookie, zmarketautorebuy, sizeof(zmarketautorebuy));
    
    // If the cookie is empty, then set the default value.
    if (!zmarketautorebuy[0])
    {
        // Set cookie to false.
        CookiesSetClientCookieBool(client, g_hZMarketAutoRebuyCookie, false);
    }
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
ZMarketOnClientDisconnect(client)
{
    // Destroy ZMarket array data for client.
    if (g_hZMarketPurchaseCount[client] != INVALID_HANDLE)
    {
        CloseHandle(g_hZMarketPurchaseCount[client]);
    }
    
    // Reset handle.
    g_hZMarketPurchaseCount[client] = INVALID_HANDLE;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ZMarketOnClientSpawnPost(client)
{
    // Reset purchase counts for client.
    ZMarketResetPurchaseCount(client);
    
    // If auto-rebuy is disabled, then ensure it is also disabled on the client as well.
    new bool:zmarketrebuyauto = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_REBUY_AUTO]);
    if (!zmarketrebuyauto)
    {
        CookiesSetClientCookieBool(client, g_hZMarketAutoRebuyCookie, false);
    }
    
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }
    
    // If client is a zombie, then stop.
    if (InfectIsClientInfected(client))
    {
        return;
    }
    
    // If auto-rebuy is enabled, then force client to rebuy weapons.
    if (CookiesGetClientCookieBool(client, g_hZMarketAutoRebuyCookie))
    {
        ZMarketRebuySavedSetup(client, true);
    }
}

/**
 * Reset the purchase count(s) for a client.
 * 
 * @param client    The client index.
 */
ZMarketResetPurchaseCount(client)
{
    // Clear out the trie of all data.
    ClearTrie(g_hZMarketPurchaseCount[client]);
}

/**
 * Set the purchases left for a client of a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon name.
 * @param value     The amount of purchases left for the weapon.
 */
ZMarketSetPurchaseCount(client, const String:weapon[], value, bool:add = false)
{
    // Initialize variable (value is 0)
    new purchasemax;
    
    // If we are adding, then update variable with current ammo value.
    if (add)
    {
        purchasemax = ZMarketGetPurchaseCount(client, weapon);
    }
    
    // Set value in client's trie.
    SetTrieValue(g_hZMarketPurchaseCount[client], weapon, purchasemax + value);
}

/**
 * Get the purchases left for a client of a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon name.
 * @param return    The amount of purchases left for the weapon.
 */
ZMarketGetPurchaseCount(client, const String:weapon[])
{
    // Get value in client's trie.
    new value;
    GetTrieValue(g_hZMarketPurchaseCount[client], weapon, value);
    return value;
}

/**
 * Sends main ZMarket menu to client.
 *  
 * @param client    The client index.
 * @return          True if the menu was sent successfully, false if not.
 */
bool:ZMarketMenuMain(client)
{
    // If the module isn't enabled, or the weapons file isn't loaded, then stop.
    new bool:weapons = GetConVarBool(g_hCvarsList[CVAR_WEAPONS]);
    new bool:weaponsloaded = ConfigIsConfigLoaded(File_Weapons);
    if (!weapons || !weaponsloaded)
    {
        // Telle client feature is disabled.
        TranslationPrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // If ZMarket module is disabled, then stop.
    new bool:zmarket = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET]);
    if (!zmarket)
    {
        // Telle client feature is disabled.
        TranslationPrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // Create menu handle.
    new Handle:menu_zmarket_main = CreateMenu(ZMarketMenuMainHandle);
    
    // Set translation target as client.
    SetGlobalTransTarget(client);
    
    // If rebuy is enabled, then add it to the menu.
    new bool:zmarketrebuy = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_REBUY]);
    new bool:zmarketautorebuy = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_REBUY_AUTO]);
    
    decl String:title[MENU_LINE_TITLE_LENGTH];
    decl String:rebuysavedsetup[MENU_LINE_REG_LENGTH];
    decl String:buyweapons[MENU_LINE_REG_LENGTH];
    decl String:savecurrentsetup[MENU_LINE_REG_LENGTH];
    decl String:viewsavedsetup[MENU_LINE_REG_LENGTH];
    decl String:autorebuy[MENU_LINE_REG_LENGTH];
    
    // Get auto-rebuy setting.
    decl String:autorebuysetting[MENU_LINE_SMALL_LENGTH];
    ConfigBoolToSetting(CookiesGetClientCookieBool(client, g_hZMarketAutoRebuyCookie), autorebuysetting, sizeof(autorebuysetting), true, client);
    
    // Format menu options.
    Format(title, sizeof(title), "%t\n ", "Weapons menu zmarket main title");
    Format(rebuysavedsetup, sizeof(rebuysavedsetup), "%t", "Weapons menu zmarket main rebuy saved setup");
    Format(buyweapons, sizeof(buyweapons), "%t", "Weapons menu zmarket main buy weapons");
    Format(savecurrentsetup, sizeof(savecurrentsetup), "%t", "Weapons menu zmarket main save current setup");
    Format(viewsavedsetup, sizeof(viewsavedsetup), "%t", "Weapons menu zmarket main view weapon setup");
    Format(autorebuy, sizeof(autorebuy), "%t", "Weapons menu zmarket main auto-rebuy", autorebuysetting);
    
    // Add formatted options to menu.
    SetMenuTitle(menu_zmarket_main, title);
    AddMenuItem(menu_zmarket_main, "Rebuy Saved Setup", rebuysavedsetup, MenuGetItemDraw(zmarketrebuy || zmarketautorebuy));
    AddMenuItem(menu_zmarket_main, "Buy Weapons", buyweapons);
    AddMenuItem(menu_zmarket_main, "Save Current Setup", savecurrentsetup, MenuGetItemDraw(zmarketrebuy || zmarketautorebuy));
    AddMenuItem(menu_zmarket_main, "View Weapon Setup", viewsavedsetup, MenuGetItemDraw(zmarketrebuy || zmarketautorebuy));
    AddMenuItem(menu_zmarket_main, "Auto-Rebuy", autorebuy, MenuGetItemDraw(zmarketautorebuy));
    
    // Set exit back button.
    SetMenuExitBackButton(menu_zmarket_main, true);
        
    DisplayMenu(menu_zmarket_main, client, MENU_TIME_FOREVER);
    
    // Menu sent successfully.
    return true;
}

/**
 * Called when client selects option in the main ZMarket menu, and handles it.
 *  
 * @param menu_zmarket_main    Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public ZMarketMenuMainHandle(Handle:menu_zmarket_main, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            // Buy Saved Setup
            case 0:
            {
                // Force client to rebuy weapons.
                ZMarketRebuySavedSetup(client);
                
                // Resend menu.
                ZMarketMenuMain(client);
            }
            // Buy Weapons
            case 1:
            {
                // Send the weapon types list.
                ZMarketMenuTypes(client);
            }
            // Save Current Setup
            case 2:
            {
                // Gets a client's current weapon setup.
                ZMarketSaveCurrentSetup(client);
                
                // Resend menu.
                ZMarketMenuMain(client);
            }
            // View Weapon Setup
            case 3:
            {
                // Show a client their current weapon setup.
                ZMarketMenuWeaponSetup(client);
            }
            // Auto-rebuy
            case 4:
            {
                // Toggle the auto-rebuy cookie.
                ZMarketToggleAutoRebuy(client);
                
                // Resend menu.
                ZMarketMenuMain(client);
            }
        }
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            ZMenuMain(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_main);
    }
}

/**
 * Sends list of weapons in a client's weapon setup to the client.
 *  
 * @param client    The client index.
 */
bool:ZMarketMenuWeaponSetup(client)
{
    // Create menu handle.
    new Handle:menu_zmarket_weaponsetup = CreateMenu(ZMarketMenuWeaponSetupHandle);
    
    // Transfer cookie values into an array.
    new String:rebuyweapons[WeaponsSlot][WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    ZMarketCookiesToArray(client, rebuyweapons, WEAPONS_SLOTS_MAX + 1, sizeof(rebuyweapons[]));
    
    SetGlobalTransTarget(client);
    
    // Get the empty translation.
    decl String:empty[MENU_LINE_SMALL_LENGTH];
    Format(empty, sizeof(empty), "%t", "Menu none");
    
    // Count and format.
    new weaponcount[WeaponsSlot];
    for (new x = 0; x < sizeof(rebuyweapons); x++)
    {
        // Ignore the NVG slot, we handle this separately.
        if (WeaponsSlot:x == Slot_NVGs)
        {
            continue;
        }
        
        // Count how many weapons are in the slot before inserting the "empty" phrase.
        weaponcount[x] = ShoppingListCount(rebuyweapons[x], "", ZMARKET_WEAPON_COOKIE_TOKEN);
        
        // If the client doesn't have any weapons in this slot, then set the weapon to the "empty" phrase.
        if (weaponcount[x] == 0)
        {
            strcopy(rebuyweapons[x], sizeof(rebuyweapons[]), empty);
        }
    }
    
    // Copy "Yes/No" to NVGs string.
    decl String:nvgsbool[MENU_LINE_SMALL_LENGTH];
    ConfigBoolToSetting(bool:rebuyweapons[Slot_NVGs][0], nvgsbool, sizeof(nvgsbool), true, client);
    
    decl String:title[MENU_LINE_HUGE_LENGTH];
    decl String:clearall[MENU_LINE_REG_LENGTH];
    decl String:primary[MENU_LINE_HUGE_LENGTH];
    decl String:secondary[MENU_LINE_HUGE_LENGTH];
    decl String:melee[MENU_LINE_HUGE_LENGTH];
    decl String:projectile[MENU_LINE_HUGE_LENGTH];
    decl String:explosive[MENU_LINE_HUGE_LENGTH];
    decl String:nvgs[MENU_LINE_HUGE_LENGTH];
    decl String:shoppinglist[MENU_LINE_HUGE_LENGTH];
    
    // Format all the lines of the menu.
    Format(title, sizeof(title), "%t\n ", "Weapons menu zmarket weapon setup title");
    Format(clearall, sizeof(clearall), "%t\n ", "Weapons menu zmarket weapon setup clear all");
    
    // Format the primary slot.
    if (weaponcount[Slot_Primary] <= 1)
    {
        Format(primary, sizeof(primary), "%t %s", "Weapons menu zmarket weapon setup primary", rebuyweapons[Slot_Primary]);
    }
    else
    {
        // TODO: MAKE FORMATTING A CVAR
        ShoppingListConstruct(rebuyweapons[Slot_Primary], shoppinglist, MENU_LINE_HUGE_LENGTH, ZMARKET_WEAPON_COOKIE_TOKEN, "\n \t", " (##)");
        Format(primary, sizeof(primary), "%t \n \t%s", "Weapons menu zmarket weapon setup primary", shoppinglist);
    }
    
    // Format the secondary slot.
    if (weaponcount[Slot_Secondary] <= 1)
    {
        Format(secondary, sizeof(secondary), "%t %s", "Weapons menu zmarket weapon setup secondary", rebuyweapons[Slot_Secondary]);
    }
    else
    {
        // TODO: MAKE FORMATTING A CVAR
        ShoppingListConstruct(rebuyweapons[Slot_Secondary], shoppinglist, MENU_LINE_HUGE_LENGTH, ZMARKET_WEAPON_COOKIE_TOKEN, "\n \t", " (##)");
        Format(secondary, sizeof(secondary), "%t \n \t%s", "Weapons menu zmarket weapon setup secondary", shoppinglist);
    }
    
    // Format the melee slot.
    if (weaponcount[Slot_Melee] <= 1)
    {
        Format(melee, sizeof(melee), "%t %s", "Weapons menu zmarket weapon setup melee", rebuyweapons[Slot_Melee]);
    }
    else
    {
        // TODO: MAKE FORMATTING A CVAR
        ShoppingListConstruct(rebuyweapons[Slot_Melee], shoppinglist, MENU_LINE_HUGE_LENGTH, ZMARKET_WEAPON_COOKIE_TOKEN, "\n \t", " (##)");
        Format(melee, sizeof(melee), "%t \n \t%s", "Weapons menu zmarket weapon setup melee", shoppinglist);
    }
    
    // Format the projectile slot.
    if (weaponcount[Slot_Projectile] <= 1)
    {
        Format(projectile, sizeof(projectile), "%t %s", "Weapons menu zmarket weapon setup projectile", rebuyweapons[Slot_Projectile]);
    }
    else
    {
        // TODO: MAKE FORMATTING A CVAR
        ShoppingListConstruct(rebuyweapons[Slot_Projectile], shoppinglist, MENU_LINE_HUGE_LENGTH, ZMARKET_WEAPON_COOKIE_TOKEN, "\n \t", " (##)");
        Format(projectile, sizeof(projectile), "%t \n \t%s", "Weapons menu zmarket weapon setup projectile", shoppinglist);
    }
    
    // Format the explosive slot.
    if (weaponcount[Slot_Explosive] <= 1)
    {
        Format(explosive, sizeof(explosive), "%t %s", "Weapons menu zmarket weapon setup explosive", rebuyweapons[Slot_Explosive]);
    }
    else
    {
        // TODO: MAKE FORMATTING A CVAR
        ShoppingListConstruct(rebuyweapons[Slot_Explosive], shoppinglist, MENU_LINE_HUGE_LENGTH, ZMARKET_WEAPON_COOKIE_TOKEN, "\n \t", " (##)");
        Format(explosive, sizeof(explosive), "%t \n \t%s", "Weapons menu zmarket weapon setup explosive", shoppinglist);
    }
    
    Format(nvgs, sizeof(nvgs), "%t %s", "Weapons menu zmarket weapon setup nvgs", nvgsbool);
    
    
    // Add formatted options to menu.
    SetMenuTitle(menu_zmarket_weaponsetup, title);
    AddMenuItem(menu_zmarket_weaponsetup, "-1", clearall, MenuGetItemDraw(!StrEqual(rebuyweapons[Slot_Primary], empty) || !StrEqual(rebuyweapons[Slot_Secondary], empty) || !StrEqual(rebuyweapons[Slot_Melee], empty) || !StrEqual(rebuyweapons[Slot_Projectile], empty) || !StrEqual(rebuyweapons[Slot_Explosive], empty)));
    AddMenuItem(menu_zmarket_weaponsetup, "0", primary, MenuGetItemDraw(!StrEqual(rebuyweapons[Slot_Primary], empty)));
    AddMenuItem(menu_zmarket_weaponsetup, "1", secondary, MenuGetItemDraw(!StrEqual(rebuyweapons[Slot_Secondary], empty)));
    AddMenuItem(menu_zmarket_weaponsetup, "2", melee, MenuGetItemDraw(!StrEqual(rebuyweapons[Slot_Melee], empty)));
    AddMenuItem(menu_zmarket_weaponsetup, "3", projectile, MenuGetItemDraw(!StrEqual(rebuyweapons[Slot_Projectile], empty)));
    AddMenuItem(menu_zmarket_weaponsetup, "4", explosive, MenuGetItemDraw(!StrEqual(rebuyweapons[Slot_Explosive], empty)));
    AddMenuItem(menu_zmarket_weaponsetup, "5", nvgs, MenuGetItemDraw(bool:rebuyweapons[Slot_NVGs][0]));
    
    // Set exit back button.
    SetMenuExitBackButton(menu_zmarket_weaponsetup, true);
    
    // Send menu to client.
    DisplayMenu(menu_zmarket_weaponsetup, client, MENU_TIME_FOREVER);
}

/**
 * Allow the user to press the "Back" button to go to main ZMarket menu.
 *  
 * @param menu_zmarket_weaponsetup    Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public ZMarketMenuWeaponSetupHandle(Handle:menu_zmarket_weaponsetup, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Convert the menu slot chosen by the client to a weapon slot datatype.
        decl String:infostring[4];
        GetMenuItem(menu_zmarket_weaponsetup, slot, infostring, sizeof(infostring));
        new WeaponsSlot:weaponslot = WeaponsSlot:StringToInt(infostring);
        
        // This means the "Clear All" button was pressed.
        if (weaponslot == Slot_Invalid)
        {
            // Include NVGs by adding one to the max.
            for (new x = 0; x < WEAPONS_SLOTS_MAX + 1; x++)
            {
                // Clear rebuy slot.
                ZMarketHandleRebuyCookie(client, WeaponsSlot:x, "");
            }
            
            // Re-send menu.
            ZMarketMenuWeaponSetup(client);
            
            return;
        }
        
        // Get the list of weapons stored under this weapon slot.
        decl String:weapons[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
        ZMarketGetRebuyCookie(client, weaponslot, weapons, sizeof(weapons));
        
        // Count how many weapons are in the slot.
        new weaponcount = ShoppingListCount(weapons, "", ZMARKET_WEAPON_COOKIE_TOKEN);
        
        // If there's only 1 weapon in the slot then just clear it.
        if (weaponcount <= 1)
        {
            // Clear rebuy slot.
            ZMarketHandleRebuyCookie(client, weaponslot, "");
            
            // Re-send menu.
            ZMarketMenuWeaponSetup(client);
        }
        else
        {
            // Open a submenu to handle multiple weapons in a slot.
            ZMarketMenuSubWeaponSetup(client, weaponslot);
        }
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            ZMarketMenuMain(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_weaponsetup);
    }
}

ZMarketMenuSubWeaponSetup(client, WeaponsSlot:weaponslot)
{
    // Create menu handle.
    new Handle:menu_zmarket_subweaponsetup = CreateMenu(ZMarketMenuSubWeaponSetupHandle);
    
    // Set translation target as client.
    SetGlobalTransTarget(client);
    
    decl String:title[MENU_LINE_TITLE_LENGTH];
    decl String:clearall[MENU_LINE_REG_LENGTH];
    
    // Format all the lines of the menu.
    Format(title, sizeof(title), "%t\n ", "Weapons menu zmarket sub weapon setup title");
    Format(clearall, sizeof(clearall), "%t\n ", "Weapons menu zmarket weapon setup clear all");
    
    decl String:infostring[WEAPONS_MAX_LENGTH + 5];  // +5 chars to allow for the serialization formatting.
    
    // Set menu title.
    SetMenuTitle(menu_zmarket_subweaponsetup, title);
    
    // Add "Clear All" menu item.
    Format(infostring, sizeof(infostring), "%d|__All", _:weaponslot);
    AddMenuItem(menu_zmarket_subweaponsetup, infostring, clearall);
    
    // Get the weapons to show in the menu.
    decl String:weaponlist[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    ZMarketGetRebuyCookie(client, weaponslot, weaponlist, sizeof(weaponlist));
    
    // Explode the shopping list to add as individual menu items.
    new String:weapons[ZMARKET_MAX_WEAPONS_PER_SLOT][WEAPONS_MAX_LENGTH];
    ExplodeString(weaponlist, ZMARKET_WEAPON_COOKIE_TOKEN, weapons, ZMARKET_MAX_WEAPONS_PER_SLOT, WEAPONS_MAX_LENGTH);
    
    // Add weapons to the menu.
    for (new x = 0; x < sizeof(weapons); x++)
    {
        // If there are no more weapons, then break the loop here.
        if (!weapons[x][0])
            break;
        
        Format(infostring, sizeof(infostring), "%d|%s", _:weaponslot, weapons[x]); // don't separate with "_" because a weapon name could contain it, "|" is less likely.
        AddMenuItem(menu_zmarket_subweaponsetup, infostring, weapons[x]);
    }
    
    // Set exit back button.
    SetMenuExitBackButton(menu_zmarket_subweaponsetup, true);
        
    DisplayMenu(menu_zmarket_subweaponsetup, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the view weapon setup menu, and handles it.
 *  
 * @param menu_zmarket_subweaponsetup   Handle of the menu being used.
 * @param action                        The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                        The client index.
 * @param slot                          The slot index selected (starting from 0).
 */ 
public ZMarketMenuSubWeaponSetupHandle(Handle:menu_zmarket_subweaponsetup, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get the slot and weapon selected.
        decl String:infostring[WEAPONS_MAX_LENGTH];
        GetMenuItem(menu_zmarket_subweaponsetup, slot, infostring, sizeof(infostring));
        
        // Separate slot and weapon in an array.
        new String:weaponinfo[2][WEAPONS_MAX_LENGTH];
        ExplodeString(infostring, "|", weaponinfo, 2, WEAPONS_MAX_LENGTH);
        new WeaponsSlot:weaponslot = WeaponsSlot:StringToInt(weaponinfo[0]);
        
        // weaponinfo[0] = weaponslot = slot
        // weaponinfo[1] = weapon
        
        if (StrEqual(weaponinfo[1], "__All"))
        {
            // Clear rebuy slot.
            ZMarketHandleRebuyCookie(client, weaponslot, "");
            
            // Go back to weapon setup
            ZMarketMenuWeaponSetup(client);
            
            return;
        }
        
        // Remove from cookie.
        new bool:success = ZMarketRemoveFromRebuyCookie(client, weaponslot, weaponinfo[1]);
        
        // If no weapon was removed then something went wrong in the menu.
        if (!success)
        {
            LogEvent(false, LogType_Error, LOG_GAME_EVENTS, LogModule_Weapons, "Weapon Setup", "Client %N used the ZMarket menu to try and remove invalid weapon \"%s\".  Contact plugin support.", client, weaponinfo[1]);
            return;
        }
        
        // Get list of weapons in the slot
        decl String:weapons[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
        ZMarketGetRebuyCookie(client, weaponslot, weapons, sizeof(weapons));
        
        // Resend this menu if there is more to remove, resend weapon setup menu if all is removed.
        if (ShoppingListCount(weapons, "", ZMARKET_WEAPON_COOKIE_TOKEN) > 0)
            ZMarketMenuSubWeaponSetup(client, weaponslot);
        else
            ZMarketMenuWeaponSetup(client);
    }
    
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            ZMarketMenuWeaponSetup(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_subweaponsetup);
    }
}

/**
 * Sends weapon type list to client.
 *  
 * @param client    The client index.
 */
ZMarketMenuTypes(client)
{
    // Create menu handle.
    new Handle:menu_zmarket_types = CreateMenu(ZMarketMenuTypesHandle);
    
    // Set translation target as client.
    SetGlobalTransTarget(client);
    
    decl String:title[MENU_LINE_TITLE_LENGTH];
    Format(title, sizeof(title), "%t\n ", "Weapons menu zmarket types title");
    SetMenuTitle(menu_zmarket_types, title);
    
    decl String:typename[WEAPONS_MAX_LENGTH];
    
    // x = Array index.
    new size = GetArraySize(arrayWeaponTypes);
    for (new x = 0; x < size; x++)
    {
        // Get name of type.
        RestrictWeaponTypeGetName(x, typename, sizeof(typename));
        
        // Add item to menu.
        AddMenuItem(menu_zmarket_types, typename, typename);
    }
    
    // If there are no weapons, add an "(Empty)" line.
    if (size == 0)
    {
        SetGlobalTransTarget(client);
        
        decl String:empty[MENU_LINE_SMALL_LENGTH];
        Format(empty, sizeof(empty), "%t", "Menu empty");
        
        AddMenuItem(menu_zmarket_types, "empty", empty, ITEMDRAW_DISABLED);
    }
    
    // Set exit back button.
    SetMenuExitBackButton(menu_zmarket_types, true);
        
    DisplayMenu(menu_zmarket_types, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapons list menu, and handles it.
 *  
 * @param menu_zmarket_types    Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public ZMarketMenuTypesHandle(Handle:menu_zmarket_types, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        decl String:weapontype[WEAPONS_MAX_LENGTH];
        GetMenuItem(menu_zmarket_types, slot, weapontype, sizeof(weapontype));
        
        // Get weapon index.
        new weapontypeindex = RestrictTypeToIndex(weapontype);
        
        // If weapon type index is -1, then something went very wrong.
        if (weapontypeindex == -1)
        {
            // Resend menu.
            ZMarketMenuTypes(client);
            
            return;
        }
        
        // Menu slot index is = weapon type index.
        g_iZMarketCurType[client] = weapontypeindex;
        
        // Send weapons of the selected type in a menu to client.
        ZMarketMenuTypeWeapons(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            ZMarketMenuMain(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_types);
    }
}

/**
 * Sends a list of weapons of a certain type in a menu to the client.
 *  
 * @param client    The client index.
 */
ZMarketMenuTypeWeapons(client)
{
    // Create menu handle.
    new Handle:menu_zmarket_typeweapons = CreateMenu(ZMarketMenuTypeWeaponsHandle);
    
    // Get name of current weapon type.
    decl String:typename[WEAPONS_MAX_LENGTH];
    RestrictWeaponTypeGetName(g_iZMarketCurType[client], typename, sizeof(typename));
    
    decl String:title[MENU_LINE_TITLE_LENGTH];
    decl String:typeweapon[WEAPONS_MAX_LENGTH];
    decl String:typeweapondisplay[MENU_LINE_REG_LENGTH];
    decl String:typeweaponentity[WEAPONS_MAX_LENGTH];
    decl String:display[MENU_LINE_BIG_LENGTH];
    
    // Get an array populated with all weapons of the given type.
    new Handle:arrayTypeWeapons;
    new count = RestrictGetTypeWeapons(g_iZMarketCurType[client], arrayTypeWeapons);
    
    SetGlobalTransTarget(client);
    
    // x = Array index.
    for (new x = 0; x < count; x++)
    {
        // Get weapon index to check restricted status of.
        new weaponindex = GetArrayCell(arrayTypeWeapons, x);
        
        // Get name of weapon.
        WeaponsGetName(weaponindex, typeweapon, sizeof(typeweapon));
        
        // Get entity name of weapon.
        WeaponsGetEntity(weaponindex, typeweaponentity, sizeof(typeweaponentity));
        
        // Check if client is buying the weapon or ammo for it, and get the price of the item.
        new bool:hasweapon = WeaponsClientHasWeapon(client, typeweaponentity);
        
        // DO ITEM PRICE STUFF HERE.
        
        new itemprice;
        
        // Do appropriate formatting for the type of item client is buying.
        new WeaponsSlot:slot = WeaponsGetSlot(weaponindex);
        if (!hasweapon || slot == Slot_Projectile)
        {
            // Get the weapon price.
            itemprice = WeaponsGetZMarketPrice(weaponindex);
        }
        else
        {
            // Get the weapon's ammo price.
            itemprice = WeaponsGetAmmoPrice(weaponindex);
            
            // Tell client they are buying ammo.
            Format(typeweapondisplay, sizeof(typeweapondisplay), "%s %t", typeweapon, "Weapons menu zmarket types weapon ammo");
        }
        
        // If the itemprice is invalid, then set to known integer to be later replaced.
        if (itemprice < 0)
        {
            itemprice = -1;
        }
        
        // DO RESTRICTED WEAPON STUFF HERE.
        
        // Get restricted status on the weapon.
        new bool:restricted = RestrictIsWeaponRestricted(weaponindex);
        
        // If weapon is restricted then format "[]" around it.
        strcopy(typeweapondisplay, sizeof(typeweapondisplay), typeweapon);
        if (restricted)
        {
            Format(typeweapondisplay, sizeof(typeweapondisplay), "[%s]", typeweapon);
        }
        
        // DO PURCHASE MAX STUFF HERE.
        
        // Get the purchase count information for this weapon.
        new purchasemax = WeaponsGetZMarketPurchaseMax(weaponindex);
        new purchasecount = ZMarketGetPurchaseCount(client, typeweapon);
        new purchasesleft = purchasemax - purchasecount;
        
        // If the purchases left for the weapon goes below 0, fix it.
        if (purchasesleft < 0)
        {
            purchasesleft = 0;
        }
        
        // If max purchases is disabled for the weapon, then set as known integer to be later replaced.
        if (purchasemax <= 0)
        {
            purchasesleft = -1;
        }
        
        // Format all the information together.
        
        // Format price onto the menu entry.
        Format(display, sizeof(display), "%t", "Weapons menu zmarket types weapon info", typeweapondisplay, itemprice, purchasesleft);
        
        // Remove the known invalid number from the string, and replace with N/A, and remove currency symbol.
        ReplaceString(display, sizeof(display), "$-1", "N/A");
        ReplaceString(display, sizeof(display), "-1", "N/A");
        
        // Disable option if it isn't toggleable.
        new bool:itemdraw = ((itemprice > -1) && !restricted && (purchasemax <= 0 || purchasesleft > 0));
        AddMenuItem(menu_zmarket_typeweapons, typeweapon, display, MenuGetItemDraw(itemdraw));
    }
    
    Format(title, sizeof(title), "%t\n ", "Weapons menu zmarket types weapon type title", typename);
    SetMenuTitle(menu_zmarket_typeweapons, title);
    
    // Destroy the array handle.
    CloseHandle(arrayTypeWeapons);
    
    // Set menu back button.
    SetMenuExitBackButton(menu_zmarket_typeweapons, true);
    
    // Display menu to client.
    DisplayMenu(menu_zmarket_typeweapons, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapon group menu, and handles it.
 *  
 * @param menu_zmarket_typeweapons  Handle of the menu being used.
 * @param action                    The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                    The client index.
 * @param slot                      The slot index selected (starting from 0).
 */ 
public ZMarketMenuTypeWeaponsHandle(Handle:menu_zmarket_typeweapons, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get name of current weapon type.
        decl String:typename[WEAPONS_MAX_LENGTH];
        RestrictWeaponTypeGetName(g_iZMarketCurType[client], typename, sizeof(typename));
        
        decl String:typeweapon[WEAPONS_MAX_LENGTH];
        GetMenuItem(menu_zmarket_typeweapons, slot, typeweapon, sizeof(typeweapon));
        
        // Equip weapon on client.
        ZMarketEquip(client, typeweapon);
        
        // Open types menu.
        ZMarketMenuTypes(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            ZMarketMenuTypes(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_typeweapons);
    }
}

/**
 * Equip a weapon on a client.
 * 
 * @param client    The client index.
 * @param weapon    The weapon to equip. (must be in weapons config file)
 * @param rebuy     (Optional) If client is rebuying, ammo will be ignored.
 */
stock bool:ZMarketEquip(client, const String:weapon[], bool:rebuy = false)
{
    // Get weapon index.
    new weaponindex = WeaponsNameToIndex(weapon);
    
    // If weapon index is -1, then either something went very wrong or the weapon has been removed from the config.
    if (weaponindex == -1)
    {
        // Something went very wrong.
        if (!rebuy)
            LogEvent(false, LogType_Error, LOG_GAME_EVENTS, LogModule_Weapons, "ZMarket Equip", "Client %N selected a non-existent weapon (%s) from ZMarket weapon list.", client, weapon);
        // Weapon in setup has been removed from the config.
        else
        {
            // Search for the weapon in every slot and remove it.
            
            // Copy all cookie values to an array
            new String:rebuyweapons[WeaponsSlot][WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
            ZMarketCookiesToArray(client, rebuyweapons, WEAPONS_SLOTS_MAX + 1, sizeof(rebuyweapons[]));
            
            // x = Weapon slot
            for (new x = 0; x < WEAPONS_SLOTS_MAX + 1; x++)
            {
                // Remove all occurences of the weapon.
                while (StrContains(rebuyweapons[x], weapon, false) > -1)
                {
                    ShoppingListRemove(rebuyweapons[x], sizeof(rebuyweapons[]), weapon, ZMARKET_WEAPON_COOKIE_TOKEN);
                }
            }
            
            // Save tbe modified array back into the cookies.
            ZMarketArrayToCookies(client, rebuyweapons, WEAPONS_SLOTS_MAX + 1, sizeof(rebuyweapons[]));
            
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Weapons, "ZMarket Equip", "Client %N has an invalid weapon saved in his weapon setup (%s), removing from setup.", client, weapon);
        }
            
        return false;
    }
    
    // Get slot index of the weapon being purchased.
    new WeaponsSlot:slot = WeaponsGetSlot(weaponindex);
    
    // If client is a zombie, then update weapon weaponsetup and stop.
    if (InfectIsClientInfected(client))
    {
        // Update cookie with new weapon.
        ZMarketHandleRebuyCookie(client, slot, weapon);
        
        TranslationPrintToChat(client, "Weapons zmarket updated weapon setup");
        return false;
    }
    
    new bool:zmarketbuyzone = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]);
    if (!rebuy && zmarketbuyzone && !ZMarketIsClientInBuyZone(client))
    {
        // Update cookie with new weapon.
        ZMarketHandleRebuyCookie(client, slot, weapon);
        
        TranslationPrintToChat(client, "Weapons zmarket updated weapon setup");
        return false;
    }
    
    // Get entity name of the weapon.
    decl String:weaponentity[WEAPONS_MAX_LENGTH];
    WeaponsGetEntity(weaponindex, weaponentity, sizeof(weaponentity));
    
    // Get the appropriate price of the item being purchased.
    new bool:hasweapon = WeaponsClientHasWeapon(client, weaponentity);
    new itemprice = (hasweapon && slot != Slot_Projectile) ? WeaponsGetAmmoPrice(weaponindex) : WeaponsGetZMarketPrice(weaponindex);
    
    // If the weapon price is below 0, then something went wrong.
    if (itemprice < 0)
    {
        return false;
    }
    
    // Get the display name for the weapon.
    decl String:weapondisplay[WEAPONS_MAX_LENGTH];
    WeaponsGetName(weaponindex, weapondisplay, sizeof(weapondisplay));
    
    // Check to make sure the weapon isn't restricted.
    new bool:restricted = RestrictIsWeaponRestricted(weaponindex);
    if (restricted)
    {
        // If rebuy is attempting to rebuy a restricted weapon, then remove it from the weapon setup.
        if (rebuy)
        {
            // Remove from cookie.
            new bool:success = ZMarketRemoveFromRebuyCookie(client, slot, weapondisplay, true);
            
            // If no weapon was removed then something went wrong in the menu.
            if (!success)
            {
                LogEvent(false, LogType_Error, LOG_GAME_EVENTS, LogModule_Weapons, "Weapon Setup Rebuy", "Client %N has an invalid weapon in his weapon setup: \"%s\".  Contact plugin support.", client, weapondisplay);
                return false;
            }
            
            // Tell client a restricted weapon has been automatically removed from their weapon setup.
            TranslationPrintToChat(client, "Weapons zmarket weapon setup restricted", weapondisplay);
            
            return false;
        }
        
        TranslationPrintToChat(client, "Weapon is restricted", weapondisplay);
        return false;
    }
    
    // Get the purchase count information for this weapon.
    new purchasemax = WeaponsGetZMarketPurchaseMax(weaponindex);
    new purchasecount = ZMarketGetPurchaseCount(client, weapon);
    new purchasesleft = purchasemax - purchasecount;
    if (purchasemax > 0 && purchasesleft <= 0)
    {
        TranslationPrintToChat(client, "Weapons zmarket purchase max", weapondisplay, purchasemax);
        return false;
    }
    
    // If the slot is a projectile, then get information we need to compare later.
    if (slot == Slot_Projectile)
    {
        // Get the grenade type the client is trying to buy.
        new WeaponAmmoGrenadeType:grenadetype = WeaponAmmoEntityToGrenadeType(weaponentity);
        
        if (grenadetype == GrenadeType_Invalid)
        {
            LogEvent(false, LogType_Error, LOG_GAME_EVENTS, LogModule_Weapons, "Grenades", "Client \"%L\" attempted to buy weapon entity \"%s\" marked as a projectile. Check your weapon config.", client, weaponentity);
            return false;
        }
        
        // How many grenades does the client currently have?
        new grenadecount = WeaponAmmoGetGrenadeCount(client, grenadetype);
        
        // How many grenades can the client hold?
        new grenadelimit = WeaponAmmoGetGrenadeLimit(grenadetype);
        
        // If client is at, or exceeds the grenade limit, then stop.
        if (grenadecount >= grenadelimit)
        {
            // Cient can't carry any more of this type of grenade.
            if (!rebuy)
                TranslationPrintToChat(client, "Weapons zmarket grenade max", grenadelimit);
            
            return false;
        }
    }
    
    // Get client's current money.
    new cash = AccountGetClientCash(client);
    
    // If client doesn't have enough money, tell them, resend menu, and then stop.
    if (cash < itemprice)
    {
        // Update cookie with new weapon.
        ZMarketHandleRebuyCookie(client, slot, weapon);
        
        TranslationPrintToChat(client, "Weapons zmarket updated weapon setup");
        return false;
    }
    
    // Set client's new cash after purchase.
    AccountSetClientCash(client, cash - itemprice);
    
    // Get a list of the client's current weapons.
    new weapons[WeaponsSlot];
    WeaponsGetClientWeapons(client, weapons);
    
    // Check if client is buying the weapon or ammo for it.
    if (!hasweapon || slot == Slot_Projectile || slot == Slot_NVGs)
    {
        // If the item is a projectile or NVGs, then skip.
        if (slot != Slot_Projectile && slot != Slot_NVGs)
        {
            // If there is already a weapon in the slot, then force client to drop it.
            if (weapons[slot] > -1)
            {
                // Force client to drop the weapon.
                WeaponsForceClientDrop(client, weapons[slot]);
            }
        }
        
        // Give client the weapon.
        GivePlayerItem(client, weaponentity);
        
        // Update cookie with new weapon.
        ZMarketHandleRebuyCookie(client, slot, weapon);
        
        // Add 1 to the client's purchase count.
        ZMarketSetPurchaseCount(client, weapon, 1, true);
        
        // If client isn't rebuying the weapon, then tell them the weapon has been purchased.
        if (!rebuy)
        {
            // Tell client they bought a weapon.
            if (slot == Slot_Primary || slot == Slot_Secondary)
                TranslationPrintToChat(client, "Weapons zmarket purchase with ammo", weapondisplay);
            else
                TranslationPrintToChat(client, "Weapons zmarket purchase no ammo", weapondisplay);
        }
    }
    else if (!rebuy)
    {
        // Get ammo type and stop if it's invalid.
        decl String:weaponammo[WEAPONS_MAX_LENGTH];
        WeaponsGetAmmoType(weaponindex, weaponammo, sizeof(weaponammo));
        if (!weaponammo[0])
        {
            return false;
        }
        
        // Give ammo and preserve client's clip ammo value on both primary/secondary weapons.
        new prim_clip = WeaponAmmoGetAmmo(weapons[Slot_Primary], true);
        new sec_clip = WeaponAmmoGetAmmo(weapons[Slot_Secondary], true);
        GivePlayerItem(client, weaponammo);
        WeaponAmmoSetAmmo(weapons[Slot_Primary], true, prim_clip);
        WeaponAmmoSetAmmo(weapons[Slot_Secondary], true, sec_clip);
    }
    
    return true;
}

/**
 * Saves a client's current weapon setup in the rebuy cache.
 * 
 * @param client    The client index.
 * @return          True if the menu sent successfully, false if not.
 */
bool:ZMarketSaveCurrentSetup(client)
{
    // If client is dead, then tell them they must be alive and stop.
    if (!IsPlayerAlive(client))
    {
        TranslationPrintToChat(client, "Must be alive");
        return false;
    }
    
    // If client is a zombie, then tell them they must be human and stop.
    if (!IsPlayerAlive(client))
    {
        TranslationPrintToChat(client, "Must be human");
        return false;
    }
    
    // Get all of client's current weapons.
    new weapons[WeaponsSlot];
    WeaponsGetClientWeapons(client, weapons);
    
    decl String:weaponname[WEAPONS_MAX_LENGTH];
    decl String:weaponentity[WEAPONS_MAX_LENGTH];
    
    // x = Weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        // Cast int type into our custom enum type.
        new WeaponsSlot:weaponslot = WeaponsSlot:x;
        
        // If slot is empty, then clear this slot in rebuy cache and stop.
        if (weapons[x] == -1 || weaponslot == Slot_Projectile)
        {
            // Empty rebuy slot.
            ZMarketHandleRebuyCookie(client, weaponslot, "");
            continue;
        }
        
        // Get the name of the weapon.
        GetEdictClassname(weapons[x], weaponentity, sizeof(weaponentity));
        
        // Get the entity name of the weapon.
        WeaponsEntityToDisplay(weaponentity, weaponname, sizeof(weaponname));
        
        // Copy the name to the rebuy cache.
        ZMarketHandleRebuyCookie(client, weaponslot, weaponname);
    }
    
    // Now save the grenades.
    
    // Get a count of each grenade.
    new grenadecount[WeaponAmmoGrenadeType];
    WeaponAmmoGetGrenadeTally(client, grenadecount);
    
    decl String:grenades[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    
    // x = Grenade type
    for (new x = WEAPONAMMO_GRENADETYPE_LOW; x <= WEAPONAMMO_GRENADETYPE_HIGH; x++)
    {
        // Cast int type into our custom enum type.
        new WeaponAmmoGrenadeType:grenadetype = WeaponAmmoGrenadeType:x;
        
        // Convert into grenade entity name.
        WeaponAmmoGrenadeTypeToEntity(grenadetype, weaponentity, sizeof(weaponentity));
        
        // Convert entity name into the display name from the weapons config file.
        new weaponindex = WeaponsEntityToDisplay(weaponentity, weaponname, sizeof(weaponname));
        
        // Server is missing a grenade config in their weapons config file, so skip.
        if (weaponindex == -1)
            continue;
        
        for (new y = 0; y < grenadecount[x]; y++)
            ShoppingListAdd(grenades, sizeof(grenades), weaponname, ZMARKET_WEAPON_COOKIE_TOKEN);
    }
    
    // Copy the grenade list to the rebuy cache.
    ZMarketHandleRebuyCookie(client, Slot_Projectile, grenades, true);
    
    // Update nightvision ownership.
    new bool:nvgs = ToolsGetClientNVG(client, NVGProp_Owner);
    if (nvgs)
    {
        ZMarketHandleRebuyCookie(client, Slot_NVGs, "nvgs");
    }
    else
    {
        // Empty rebuy slot.
        ZMarketHandleRebuyCookie(client, Slot_NVGs, "");
    }
    
    // Tell client their weapon setup has been updated.
    TranslationPrintToChat(client, "Weapons zmarket save current weapon setup");
    
    return true;
}

/**
 * Transfer array values to rebuy cookies.
 * 
 * @param client        The client index.
 * @param rebuyweapons  The string array to copy results from.
 * @param maxweapons    The max amount of weapons in the array.
 * @param maxlen        The max length of each cookie result.
 */
stock ZMarketArrayToCookies(client, String:rebuyweapons[WeaponsSlot][], maxweapons, maxlen)
{
    decl String:rebuycookiename[32];
    new Handle:rebuycookie;
    
    // x = Weapon slot.
    for (new x = 0; x < maxweapons; x++)
    {
        // Format cookie name.
        Format(rebuycookiename, sizeof(rebuycookiename), "%s_%d", ZMARKET_COOKIE_REBUY, x);
        
        // Find cookie handle, and retrieve its value.
        rebuycookie = FindClientCookie(rebuycookiename);
        SetClientCookie(client, rebuycookie, rebuyweapons[x]);
        CloseHandle(rebuycookie);
    }
}

/**
 * Transfer rebuy cookies to an array for easier access.
 * 
 * @param client        The client index.
 * @param rebuyweapons  The string array to copy results to
 * @param maxweapons    The max amount of weapons in the array.
 * @param maxlen        The max length of each cookie result.
 */
stock ZMarketCookiesToArray(client, String:rebuyweapons[WeaponsSlot][], maxweapons, maxlen)
{
    decl String:rebuycookiename[32];
    new Handle:rebuycookie;
    
    // x = Weapon slot.
    for (new x = 0; x < maxweapons; x++)
    {
        // Format cookie name.
        Format(rebuycookiename, sizeof(rebuycookiename), "%s_%d", ZMARKET_COOKIE_REBUY, x);
        
        // Find cookie handle, and retrieve its value.
        rebuycookie = FindClientCookie(rebuycookiename);
        GetClientCookie(client, rebuycookie, rebuyweapons[x], maxlen);
        CloseHandle(rebuycookie);
    }
}

/**
 * Handles the rebuy cookie by controlling what is getting added to the list and what's being overwritten.
 *
 * @param client    The client index.
 * @param slot      The weapons slot to handle.
 * @param value     The value (weaponname) of the slot to handle.
 * @param overwrite If true, the function will overwrite the old rebuy cookie value with 'value.' 
 */
stock ZMarketHandleRebuyCookie(client, WeaponsSlot:slot, const String:value[], bool:overwrite = false)
{
    // Format cookie name.
    decl String:rebuycookiename[32];
    Format(rebuycookiename, sizeof(rebuycookiename), "%s_%d", ZMARKET_COOKIE_REBUY, _:slot);

    // Find cookie handle, and retrieve its value.
    new Handle:rebuycookie = FindClientCookie(rebuycookiename);
    if (rebuycookie == INVALID_HANDLE)
    {
        return;
    }
    
    // If the value is blank, then reset the cookie.
    if (!value[0])
    {
        SetClientCookie(client, rebuycookie, "");
        CloseHandle(rebuycookie);
        return;
    }
    
    // If were not setting the projectile weapon type, then set the value to only allow 1 value in the cookie.
    // This prevents the plugin from having multiple types of each weapon in a setup.
    if (slot != Slot_Projectile || overwrite)
    {
        SetClientCookie(client, rebuycookie, value);
        CloseHandle(rebuycookie);
        return;
    }
    
    // GRENADE STUFF
    
    // Get weapon index.
    new weaponindex = WeaponsNameToIndex(value);
    
    // Get entity name of the weapon.
    decl String:weaponentity[WEAPONS_MAX_LENGTH];
    WeaponsGetEntity(weaponindex, weaponentity, sizeof(weaponentity));
    
    // Get the grenade type.
    new WeaponAmmoGrenadeType:grenadetype = WeaponAmmoEntityToGrenadeType(weaponentity);
    
    // Get current weapon setup.
    decl String:weaponsetup[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    GetClientCookie(client, rebuycookie, weaponsetup, sizeof(weaponsetup));
    
    // Get how many grenades the client currently has in their setup.
    new grenadecount = ShoppingListCount(weaponsetup, value, ZMARKET_WEAPON_COOKIE_TOKEN);
    
    // Get grenade limit.
    new grenadelimit = WeaponAmmoGetGrenadeLimit(grenadetype);
    
    // If the client hasn't reached the limit, then add it to the weapon setup.
    if (grenadecount < grenadelimit)
    {
        ZMarketAddToRebuyCookie(client, slot, value);
    }
}

/**
 * Add a weapon to the weapon setup cookie given the slot and value.
 *
 * @param client    The client index.
 * @param slot      The weapon slot to set value to.
 * @param value     The value (weaponname) of the slot.
 */
stock ZMarketAddToRebuyCookie(client, WeaponsSlot:slot, const String:value[])
{
    // Format cookie name.
    decl String:rebuycookiename[32];
    Format(rebuycookiename, sizeof(rebuycookiename), "%s_%d", ZMARKET_COOKIE_REBUY, _:slot);

    // Find cookie handle, and retrieve its value.
    new Handle:rebuycookie = FindClientCookie(rebuycookiename);
    if (rebuycookie == INVALID_HANDLE)
    {
        return;
    }
    
    decl String:curvalue[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    decl String:newvalue[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    
    // Get current value.
    GetClientCookie(client, rebuycookie, curvalue, sizeof(curvalue));
    
    // Count how many weapons are in the slot.
    new weaponcount = ShoppingListCount(curvalue, "", ZMARKET_WEAPON_COOKIE_TOKEN);
    if (weaponcount >= ZMARKET_MAX_WEAPONS_PER_SLOT)
    {
        LogEvent(false, LogType_Error, LOG_GAME_EVENTS, LogModule_Weapons, "ZMarket", "Weapon setup for client %N has reached the maximum limit of %d weapons for slot %d.", client, ZMARKET_MAX_WEAPONS_PER_SLOT, _:slot);
        CloseHandle(rebuycookie);
        return;
    }
    
    // If the value isn't blank, then format new value onto the end separated by the defined token.
    if (curvalue[0])
    {
        Format(newvalue, sizeof(newvalue), "%s%s%s", curvalue, ZMARKET_WEAPON_COOKIE_TOKEN, value);
    }
    else
    {
        strcopy(newvalue, sizeof(newvalue), value);
    }
    
    // Set weapon to cookie.
    SetClientCookie(client, rebuycookie, newvalue);
    CloseHandle(rebuycookie);
}

/**
 * Remove a weapon from the weapon setup cookie given the slot and weapon.
 *
 * @param client    The client index.
 * @param slot      The weapon slot to remove weapon from.
 * @param value     The value (weaponname) of the slot.
 * @param all       True to remove all occurences, false to remove one from the list. 
 * 
 * @return          True if the weapon was found and removed, false if no weapon was found.  
 */
stock bool:ZMarketRemoveFromRebuyCookie(client, WeaponsSlot:slot, const String:value[], bool:all = false)
{
    decl String:weapons[WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    ZMarketGetRebuyCookie(client, slot, weapons, sizeof(weapons));
    
    new bool:success;
    
    if (all)
    {
        // Removes all occurences of a single weapon in the rebuy cookie
        while (StrContains(weapons, value, false) > -1)
        {
            ShoppingListRemove(weapons, sizeof(weapons), value, ZMARKET_WEAPON_COOKIE_TOKEN);
            success = true;
        }
    }
    else
    {
        // Remove weapon from list and overwrite old value.
        success = ShoppingListRemove(weapons, sizeof(weapons), value, ZMARKET_WEAPON_COOKIE_TOKEN);
    }
    
    // Overwrite old value.
    ZMarketHandleRebuyCookie(client, slot, weapons, true);
    
    return success;
}

/**
 * Gets a list of weapons in a given slot. 
 *
 * @param client    The client index.
 * @param slot      The weapons slot to handle.
 * @param value     The output string of the weapon list.
 * @param maxlen    The maximum length of the output string. 
 */
stock ZMarketGetRebuyCookie(client, WeaponsSlot:slot, String:value[], maxlen)
{
    // Format cookie name.
    decl String:rebuycookiename[32];
    Format(rebuycookiename, sizeof(rebuycookiename), "%s_%d", ZMARKET_COOKIE_REBUY, _:slot);

    // Find cookie handle, and retrieve its value.
    new Handle:rebuycookie = FindClientCookie(rebuycookiename);
    if (rebuycookie == INVALID_HANDLE)
    {
        return;
    }
    
    GetClientCookie(client, rebuycookie, value, maxlen);
    CloseHandle(rebuycookie);
}

/**
 * Force a client to rebuy their weapons.
 * 
 * @param client    The client index.
 */
ZMarketRebuySavedSetup(client, bool:autorebuy = false)
{
    // If client is a zombie, then stop.
    if (InfectIsClientInfected(client))
    {
        TranslationPrintToChat(client, "Must be human");
        return;
    }
    
    new bool:zmarketbuyzone = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]);
    if (!autorebuy && zmarketbuyzone && !ZMarketIsClientInBuyZone(client))
    {
        TranslationPrintToChat(client, "Weapons zmarket buyzone");
        return;
    }
    
    // Transfer cookie values into an array.
    new String:rebuyweapons[WeaponsSlot][WEAPONS_MAX_LENGTH * ZMARKET_MAX_WEAPONS_PER_SLOT];
    ZMarketCookiesToArray(client, rebuyweapons, WEAPONS_SLOTS_MAX + 1, sizeof(rebuyweapons[]));
    
    new String:weapons[ZMARKET_MAX_WEAPONS_PER_SLOT][WEAPONS_MAX_LENGTH];
    
    // x = Weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX + 1; x++)
    {
        // If slot is empty, then stop.
        if (!rebuyweapons[x][0])
        {
            continue;
        }
        
        ExplodeString(rebuyweapons[x], ZMARKET_WEAPON_COOKIE_TOKEN, weapons, ZMARKET_MAX_WEAPONS_PER_SLOT, WEAPONS_MAX_LENGTH);
        for (new y = 0; y < ZMARKET_MAX_WEAPONS_PER_SLOT; y++)
        {
            // If no more weapons are in the slot, then stop.
            if (!weapons[y][0])
                break;

            ZMarketEquip(client, weapons[y], true);
        }
    }
}

/**
 * Toggle auto-rebuy on a client.
 * 
 * @param client    The client index.
 */
ZMarketToggleAutoRebuy(client)
{
    // If auto-rebuy is disabled, then stop.
    new bool:zmarketrebuyauto = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_REBUY_AUTO]);
    if (!zmarketrebuyauto)
    {
        return;
    }
    
    // Get the cookie value.
    new bool:autorebuystate = CookiesGetClientCookieBool(client, g_hZMarketAutoRebuyCookie);
    
    // Toggle the value.
    CookiesSetClientCookieBool(client, g_hZMarketAutoRebuyCookie, !autorebuystate);
    
    // If auto-rebuy was enabled, then tell client it has been disabled.
    if (autorebuystate)
    {
        TranslationPrintToChat(client, "Weapons zmarket auto-rebuy toggle off");
    }
    // If auto-rebuy was disabled, then tell client it has been enabled.
    else
    {
        TranslationPrintToChat(client, "Weapons zmarket auto-rebuy toggle on");
    }
}

/**
 * Command callback (zmarket)
 * Opens custom buymenu.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ZMarketCommand(client, argc)
{
    // If client is console, then stop and tell them this feature is for players only.
    if (ZRIsConsole(client))
    {
        TranslationPrintToServer("Must be player");
        return Plugin_Handled;
    }
    
    // Send ZMarket menu.
    ZMarketMenuMain(client);
    
    // This stops the "Unknown command" message in client's console.
    return Plugin_Handled;
}

/**
 * Checks if a client is in a buyzone.
 * 
 * @param client    The client index.
 */
stock bool:ZMarketIsClientInBuyZone(client)
{
    // Return if client is in buyzone.
    return bool:GetEntData(client, g_iToolsInBuyZone);
}
