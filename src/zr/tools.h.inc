/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          tools.h.inc
 *  Type:          Core
 *  Description:   Tools header.  Types and defines.
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
 * @section m_takedamage datamap property values.
 */
#define TOOLS_TAKEDAMAGE_ENABLE 2   /** Allows entity to take damage. */
#define TOOLS_TAKEDAMAGE_DISABLE 0  /** Blocks all damage done to entity. */
/**
 * @endsection
 */

/**
 * NVG properties on a client.
 */
enum ToolsNVGProps
{
    NVGProp_Owner,  /** Controls whether the client has nightvision or not. */
    NVGProp_Enabled /** Controls whether the client's nightvision is enabled. (must own nightvision to have effect) */
}

/**
 * Score properties on a client.
 */
enum ToolsScoreProps
{
    ScoreProp_Kills,    /** Controls how many kills the client has. (score) */
    ScoreProp_Deaths    /** Controls how many deaths the client has. */
}
