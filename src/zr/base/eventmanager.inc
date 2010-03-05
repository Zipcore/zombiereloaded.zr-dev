/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          eventmanager.inc
 *  Type:          Base
 *  Description:   Manages project events.
 *
 *  Copyright (C) 2009-2010  Greyscale
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

// All the event information we need is in this.
#include "zr/project_events"

/**
 * Provides the plugin a way to know if the event manager is included in the project.
 */
#define EVENT_MANAGER

// ---------------
//     Public
// ---------------

// See project.inc

/**
 * Uncomment this to profile every event in your project and log the times to file. (sourcemod/logs/)
 * The profiler is used in EventMgr_Forward.
 * WINDOWS ONLY! 
 */
//#define PROFILE_EVENTS

// ---------------
//     Private
// ---------------

#if defined PROFILE_EVENTS
    #include <profiler>
#endif

/**
 * Dummy array used as a way to count the cells required to store module event data.
 */
stock g_DummyEventData[ProjectEvents];

/**
 * Defines the block of data in the module data arrays that contains event function data.
 */
#define EVENT_DATA_FUNCTION g_iEMAllocatedIndexes[0]

/**
 * Array to store the index of the allocated space in the module data arrays for the event manager.
 */
new g_iEMAllocatedIndexes[1];

/**
 * This array stores adt array handles for every event.
 * These arrays will be populated with modules that register the event.
 * This will minimize the work the event forward has to do.
 */
new Handle:g_hEventModuleCache[ProjectEvents];

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin is loading.
 * TODO: Support multiple games.
 */
EventMgr_OnPluginStart()
{
    EventMgr_HookEvents();
    
    // Allocate 1 index for the data we want to store for each module.
    ModuleMgr_Allocate(1, g_iEMAllocatedIndexes);
    
    // Initialize the arrays that will be stored for later iteration.
    for (new eindex = 0; eindex < sizeof(g_hEventModuleCache); eindex++)
        g_hEventModuleCache[ProjectEvents:eindex] = CreateArray();
}

/**
 * Plugin is ending.
 */
EventMgr_OnPluginEnd()
{
    // Cleanup the event module cache.
    for (new eindex = 0; eindex < sizeof(g_hEventModuleCache); eindex++)
        CloseHandle(g_hEventModuleCache[ProjectEvents:eindex]);
}

/**
 * A module was just registered.  This is being called before the module has been assigned a module identifier.
 * 
 * @param adtModule The adt array of the module being registered.
 */
stock EventMgr_OnModuleRegister(Handle:adtModule)
{
    // Push the ProjectEvents enum as the "Function" type to the module's data array.
    // This is being pushed into our allocated space for event function data.
    new Function:funcProjectEvents[ProjectEvents];
    PushArrayArray(adtModule, funcProjectEvents[0], sizeof(funcProjectEvents));
}

/**
 * Base command is printing a module's info.
 * Print the module data allocated by the event manager.
 * Note: |stock| tag will stop this function from being compiled if the base command is disabled.
 * 
 * @param client    The client index the text is being printed to.
 * @param module    The module to print info for.
 */
stock EventMgr_OnPrintModuleInfo(client, Module:module)
{
    decl String:registeredevents[512];
    registeredevents[0] = 0;
    
    new ProjectEvents:event;
    for (new eindex = 0; eindex < sizeof(g_hEventModuleCache); eindex++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:eindex;
        
        if (EventMgr_GetModuleCacheIndex(module, event) == -1)
            continue;
        
        // Format each event onto a string.
        if (registeredevents[0] == 0)
            strcopy(registeredevents, sizeof(registeredevents), g_ProjectEventNames[event]);
        else
            Format(registeredevents, sizeof(registeredevents), "%s, %s", registeredevents, g_ProjectEventNames[event]);
    }
    
    // If the module has no registered events, then format in the "_None" phrase.
    if (registeredevents[0] == 0)
        Format(registeredevents, sizeof(registeredevents), "%t", "_None");
    
    // Print the module event info.
    PrintToServer("%T", "EventMgr modules info", LANG_SERVER, registeredevents);
}

// **********************************************
//                Public API
// **********************************************

/**
 * Registers an event to be forwarded to the module.
 * 
 * @param module        The module to forward event to.
 * @param event         The event to forward to the module.
 * @param functionname  The name of the function to forward the event to. (The function needs to be public)
 * 
 * return               The function ID to the event forward.  INVALID_FUNCTION is returned upon failure.
 */
stock Function:EventMgr_RegisterEvent(Module:module, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // Enable the event to be forwarded to the module, and set the function ID to the event forward.
    EventMgr_WriteFuncValue(module, event, func);
    
    // Add the module to the forward cache.
    PushArrayCell(g_hEventModuleCache[event], module);
    
    // Return the function ID.
    return func;
}

/**
 * Checks if an event is active for a module.
 * 
 * @param module    The module identifier.
 * @param event     The event to check.
 * 
 * @return          True if it's currently active, false if not.
 */
stock bool:EventMgr_IsEventActive(Module:module, ProjectEvents:event)
{
    return bool:(EventMgr_GetModuleCacheIndex(module, event) > -1);
}

/**
 * Disables an event in a module.
 * If the event is already disabled, nothing will change.
 * 
 * @param module    The module whose event is being disabled.
 * @param event     The event to disable in the module.
 */
stock EventMgr_Disable(Module:module, ProjectEvents:event)
{
    // Remove the module from the forward cache.
    new index = EventMgr_GetModuleCacheIndex(module, event);
    
    // Just stop if the module is already disabled, or non-existant.
    if (index == -1)
        return;
    
    RemoveFromArray(g_hEventModuleCache[event], index);
}

/**
 * Enables an event in a module.
 * If the event is already enabled, nothing will change.
 * 
 * @param module    The module whose event is being enabled.
 * @param event     The event to enable in the module.
 */
stock EventMgr_Enable(Module:module, ProjectEvents:event)
{
    new index = EventMgr_GetModuleCacheIndex(module, event);
    
    // Just stop if the module already exists in this event cache.
    if (index != -1)
        return;
    
    // Add the module to the forward cache.
    PushArrayCell(g_hEventModuleCache[event], module);
}

/**
 * Sets a new event forward function for the given event.
 * 
 * @param module        The module whose event forward is being set.
 * @param event         The event to set the event forward on.
 * @param functionname  The name of the function to forward the event to. (The function needs to be public)
 * 
 * return               The function ID to the event forward.  INVALID_FUNCTION is returned upon failure.
 */
stock Function:EventMgr_SetEventForward(Module:module, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // Enable the event in the module.
    EventMgr_WriteFuncValue(module, event, func);
    
    // Return the function ID.
    return func;
}

/**
 * Forwards an event and a list of data values to all modules who registered it.
 * 
 * @param event             The event to forward.  See enum ProjectEvents.
 * @param eventdata         An array with all data to forward to each module.
 * @param numValues         The number of elements in the first dimension of the eventdata array.
 * @param size              The max size of the arrays/strings if passing them through eventdata.  Otherwise set this to 1.
 * @param eventdatatypes    An array of datatypes for each event value.  See enum EventDataTypes.  This should line up with eventdata.
 * @param modules           An extra filter to narrow down which modules will see the event.  This MUST end with INVALID_MODULE.
 * 
 * @return                  An hook action defined in enum Action.
 */
new Module:g_ForwardToModule;  // The global variable to store the next module to receive the event being forwarded.
stock Action:EventMgr_Forward(ProjectEvents:event, any:eventdata[][], numValues, size, EventDataTypes:eventdatatypes[], Module:modules[] = {INVALID_MODULE})
{
    #if defined PROFILE_EVENTS
        new Handle:profiler = CreateProfiler();
        StartProfiling(profiler);
    #endif
    
    new Function:func;
    new Module:module;
    new Action:result;
    new Action:finalresult = Plugin_Continue;
    
    // Loop through all the modules.
    new count = GetArraySize(g_hEventModuleCache[event]);
    for (new moduleindex = 0; moduleindex < count; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:GetArrayCell(g_hEventModuleCache[event], moduleindex);
        
        // If the module is disabled, then stop this iteration.
        if (ModuleMgr_IsDisabled(module))
            continue;
        
        // Check if the module should see this event.
        new bool:pass_filter = bool:(modules[0] == INVALID_MODULE);
        for (new filter_module = 0; modules[filter_module] != INVALID_MODULE; filter_module++)
        {
            if (module == modules[filter_module])
                pass_filter = true;
        }
        
        if (!pass_filter)
            continue;
        
        // Read the function ID.
        func = EventMgr_ReadFuncValue(module, event);
        
        // Set the global variable to the next module who will be receiving this event.
        g_ForwardToModule = module;
        
        // Call the event forward.
        Call_StartFunction(GetMyHandle(), func);
        
        // Push each data value into the function parameter list.
        // Loop through each event value.
        for (new x = 0; x < numValues; x++)
        {
            switch (eventdatatypes[x])
            {
                case DataType_Cell:     Call_PushCell(_:eventdata[x][0]);
                case DataType_Float:    Call_PushFloat(Float:eventdata[x][0]);
                case DataType_Array:    Call_PushArray(eventdata[x], size);
                case DataType_String:   Call_PushString(eventdata[x]);
            }
        }
        
        // Call the function.
        Call_Finish(result);
        
        if (result == Plugin_Stop)
            return Plugin_Stop;
        else if (result == Plugin_Handled)
            finalresult = Plugin_Handled;
    }
    
    #if defined PROFILE_EVENTS
        StopProfiling(profiler);
        LogMessage("PROFILER: Event %s: Forwarded to %d modules, taking %f seconds to do.", g_ProjectEventNames[event], count, GetProfilerTime(profiler));
        CloseHandle(profiler);
    #endif
    
    return finalresult;
}

/**
 * This function is only valid if being called within a forwarded event in a module!
 * This is needed when more than one module is using the same event. 
 * Returns the module the the current event is being forwarded to.
 * 
 * @return  The module identifier.  
 */
stock Module:EventMgr_GetEventOwner()
{
    return g_ForwardToModule;
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Returns the index in the event module cache that holds the given module identifier.
 * 
 * @param module    The module identifier that is in the sought index.
 * @param event     The event that holds the module cache.
 * 
 * @return          The adt array index that holds the module specified.  -1 if the module isn't in the cache.
 */
stock EventMgr_GetModuleCacheIndex(Module:module, ProjectEvents:event)
{
    new count = GetArraySize(g_hEventModuleCache[event]);
    for (new moduleindex = 0; moduleindex < count; moduleindex++)
    {
        if (module == Module:GetArrayCell(g_hEventModuleCache[event], moduleindex))
            return moduleindex;
    }
    
    return -1;
}

/**
 * Event data reader that returns all event function data.
 * 
 * @param module    The module whose event function data to read.
 */
stock EventMgr_ReadAllFuncData(Module:module, Function:eventfuncdata[ProjectEvents])
{
    GetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Reset's all event function data for all events.
 * 
 * @param module    The module whose event function data to reset.
 */
stock EventMgr_ResetFuncData(Module:module)
{
    // Read all the event function data.
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(module, eventfuncdata);
    
    new ProjectEvents:event;
    
    // Loop through all events.
    // x = Index of the current event (see enum ProjectEvents)
    for (new eindex = 0; eindex < sizeof(eventfuncdata); eindex++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:eindex;
        
        // Reset the function ID to invalid.
        eventfuncdata[event] = INVALID_FUNCTION;
    }
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Event data reader that returns a single event function value.
 * 
 * @param module    The module whose event function data to read.
 * @param event     An event from enum ProjectEvents.
 * 
 * @return          The function ID of the event forward in the module.
 */
stock Function:EventMgr_ReadFuncValue(Module:module, ProjectEvents:event)
{
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(module, eventfuncdata);
    
    // Return event's enable value.
    return eventfuncdata[event];
}

/**
 * Module data writer that writes a new function ID for an event.
 * 
 * @param module    The module whose event function data to write.
 * @param event     The event to write new function ID to.
 * @param func      The new function ID to write as the event's forward function.
 */
stock EventMgr_WriteFuncValue(Module:module, ProjectEvents:event, Function:func)
{
    // Read all the module data.
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(module, eventfuncdata);
    
    // Write the new function ID.
    eventfuncdata[event] = func;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}
