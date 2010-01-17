/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zombiereloaded.inc
 *  Type:          Include
 *  Description:   Main API include file.
 *  Notes:         Include this file to include the whole ZR API. 
 *
 *  Copyright (C) 2009-10  Greyscale, Richard Helgeby
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

#if defined _zr_included
 #endinput
#endif
#define _zr_included

#include <zr/log.zr>
#include <zr/infect.zr>
// #include <zr/[module].zr>

/**
 * Sample native description.
 *
 * @param param1    Parameter 1 description.
 * @param param2    Parameter 2 description. 
 *  
 * @return			Return description.
 * @error           Reasons for erroring. 
 */
// native Tag:ExampleNative(param1, param2);

/**
 * Returns plugin version info.
 * 
 * @param version   The version of the build.
 * @param maxlen    The max length of the version output string. 
 * @param revision  The revision number of this build.  Will be set to "" if it's not available.
 * @param maxlen2   The max length of the revision output string. 
 *  
 * @return			True if revision info is available, false if not.
 */
native bool:ZR_GetVersionInfo(String:version[], maxlen, String:revision[], maxlen2);