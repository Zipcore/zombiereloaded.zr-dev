/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          functionregister.inc
 *  Type:          Module
 *  Description:   Utility module stores a list of function IDs mapped to a name.
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
new Module:g_moduleFuncRegister;

/**
 * Function register (adt trie).
 */
new Handle:FunctionRegisterData = INVALID_HANDLE;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:FuncRegister_GetIdentifier() { return g_moduleFuncRegister; }

/**
 * Register this module.
 */
FuncRegister_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "Function Register");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "funcregister");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Utility module that stores a list of function IDs mapped to a name.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleFuncRegister = ModuleMgr_Register(moduledata);
}

/**
 * Plugin is loading.
 */
FuncRegister_OnPluginStart()
{
    // Register the module.
    FuncRegister_Register();
    
    // Prepare the register.
    FuncRegister_CreateRegister();
}

/**
 * Creates the function register.
 */
FuncRegister_CreateRegister()
{
    FunctionRegisterData = CreateTrie();
}


/***************************
 *    PUBLIC MODULE API    *
 ***************************/

/**
 * Adds a function to the register.
 *
 * @param function  Function to add.
 * @param name      Name to map the function to.
 *
 * @return          0 on success.
 *                  -1 if invalid function.
 *                  -2 if invalid name.
 *                  -3 if name already exist.
 */
FuncRegister_AddFunction(Function:function, const String:name[])
{
    // Validate function.
    if (function == INVALID_FUNCTION)
    {
        // Invalid function.
        return -1;
    }
    
    // Validate name.
    if (strlen(name) == 0)
    {
        // Invalid name.
        return -2;
    }
    
    new bool:result = SetTrieValue(FunctionRegisterData, name, function, false);
    
    // Check result.
    if (!result)
    {
        // Function already exist.
        return -3;
    }
    
    return 0;
}

/**
 * Removes the function mapped to the specified name.
 *
 * @param name      Name of function to remove.
 *
 * @return          True if removed, false if it doesn't exist.
 */
bool:FuncRegister_RemoveFunction(const String:name[])
{
    return RemoveFromTrie(FunctionRegisterData, name);
}

/**
 * Gets the function mapped to the specified name.
 *
 * @param name      Name of function to get.
 *
 * @return          Function on success, otherwise INVALID_FUNCTION.
 */
Function:FuncRegister_GetFunction(const String:name[])
{
    new Function:function;
    if (GetTrieValue(FunctionRegisterData, name, function))
    {
        return function;
    }
    return INVALID_FUNCTION;
}
