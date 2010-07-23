/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          mapconfig.inc
 *  Type:          Module
 *  Description:   Executes map configuration files.
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
 * This module's identifier.
 */
new Module:g_moduleMapConfig;

/**
 * Events created by this module.
 */
new ProjectEvent:g_EvOnPostConfigsExecuted;

/**
 * Register this module.
 */
MapConfig_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "Map configs");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "mapconfig");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Executes map configuration files.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleMapConfig = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_moduleBaseAdapter, "Event_OnEventsRegister",       "MapConfig_OnEventsRegister");
    
    // Create events.
    g_EvOnPostConfigsExecuted = EventMgr_CreateEvent("OnPostConfigsExecuted");
}

/**
 * Register all events here.
 */
public MapConfig_OnEventsRegister()
{
    EventMgr_RegisterEvent(g_moduleMapConfig, "Event_OnAutoConfigsBuffered",    "MapConfig_OnAutoConfigsBuffered");
    EventMgr_RegisterEvent(g_moduleMapConfig, "Event_OnConfigsExecuted",        "MapConfig_OnConfigsExecuted");
}

/**
 * Plugin is loading.
 */
MapConfig_OnPluginStart()
{
    // Register the module.
    MapConfig_Register();
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Executes map configs.
 */
public MapConfig_OnAutoConfigsBuffered()
{
    decl String:mapconfig[PLATFORM_MAX_PATH];
    
    // Get map name and format into config path.
    GetCurrentMap(mapconfig, sizeof(mapconfig));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.cfg", mapconfig);
    
    // Prepend cfg to path.
    decl String:path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    // Check if the file exist.
    if (!FileExists(path))
    {
        // There's no map config, stop.
        return;
    }
    
    // Execute config file.
    ServerCommand("exec %s", mapconfig);
    
    // Log action.
    LogMgr_Print(g_moduleMapConfig, LogType_Normal, "Map Configs", "Executed map config file: %s", path);
}

/**
 * All convars are set, executes post map configs.
 */
public MapConfig_OnConfigsExecuted()
{
    decl String:mapname[256];
    decl String:mapconfig[PLATFORM_MAX_PATH];
    decl String:path[PLATFORM_MAX_PATH];
    
    // Get map name and format into config path.
    GetCurrentMap(mapname, sizeof(mapname));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.post.cfg", mapname);
    
    // Prepend cfg to path.
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    // Check if the file exist.
    if (!FileExists(path))
    {
        // There's no post map config, stop.
        return;
    }
    
    // Execute config file.
    ServerCommand("exec %s", mapconfig);
    
    // Log action.
    LogMgr_Print(g_moduleMapConfig, LogType_Normal, "Map Configs", "Executed post map config file: %s", path);
    
    // Trigger event.
    EventMgr_Forward(g_EvOnPostConfigsExecuted, g_CommonEventData1, 0, 0, g_CommonDataType1);
}
