/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
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
 * Application Programming Interface (API)
 * 
 * To allow other plugins or extensions to interact directly with Zombie:Reloaded we need to implement
 * an API.  SourceMod allows us to do this by creating global "natives" or "forwards."
 * 
 * Natives are basically functions that can be called from any plugin that includes its definition.
 * Forwards are functions that are called on a given event. (Such as OnClientPutInServer)
 * ZR's API files are located in sourcemod/scripting/include/zr/category.zr.inc.  We chose to create multiple
 * files simply for organization.  Including zr.inc will automatically include the rest of the files as well.
 * 
 * To better understand how natives and forwards are created, go here:
 * http://wiki.alliedmods.net/Creating_Natives_(SourceMod_Scripting)
 * http://wiki.alliedmods.net/Function_Calling_API_(SourceMod_Scripting) 
 */

APIInit()
{
    // Here we register all natives and forwards that we need.
}
