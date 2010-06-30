/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           voleffects.inc
 *  Type:           Module
 *  Description:    Volume effects.
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

new VolBeamSprite;
new VolBeamHalo;
new bool:VolSpritesValid = false;

VolEffectsInit()
{
    VolSpritesValid = false;
}

VolEffectsOnCfgsExecuted()
{
    decl String:beamSprite[PLATFORM_MAX_PATH];
    decl String:haloSprite[PLATFORM_MAX_PATH];
    beamSprite[0] = 0;
    haloSprite[0] = 0;
    
    GetConVarString(g_hCvarsList[CVAR_VOL_BEAM_SPRITE], beamSprite, sizeof(beamSprite));
    GetConVarString(g_hCvarsList[CVAR_VOL_HALO_SPRITE], haloSprite, sizeof(haloSprite));
    
    
}

VolSendWireframe(client, volumeIndex)
{
}
