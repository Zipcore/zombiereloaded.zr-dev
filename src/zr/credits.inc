/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          credits.inc
 *  Type:          Core
 *  Description:   API for controlling how many "credits" a client has.
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
 * Name of the cookie for credit storage.
 */
#define CREDITS_COOKIE "zr_credits"

/**
 * @section The approximate "exchange rates" from cash to credits.
 */
#define CREDITS_RATE_TO_CASH 1000
#define CREDITS_RATE_FROM_CASH 0.001
/**
 * @endsection
 */

/**
 * Cookie handle for credit storage.
 */
new Handle:g_hCreditsCookie = INVALID_HANDLE;

/**
 * Create credits-related cookies here.
 */
CreditsOnCookiesCreate()
{
    // Create credit storage cookie handle if doesn't exist.
    if (g_hCreditsCookie == INVALID_HANDLE)
    {
        g_hCreditsCookie = RegClientCookie(CREDITS_COOKIE, "The amount of credits a client has.", CookieAccess_Protected);
    }
}

// Credit storage API functions.

/**
 * Give credits to a client.
 * 
 * @client  The client index.
 * @amount  The amount of credits to give.
 * 
 * @return  The new amount of credits.
 */
stock CreditsGiveCredits(client, amount)
{
    // Get the current amount.
    new current = CreditsGetCredits(client);
    
    // Add to the current amount and set.
    CreditsSetCredits(client, current + amount);
    
    // Return the new amount of credits.
    return CreditsGetCredits(client);
}

/**
 * Negative of CreditsGive.
 * 
 * @client  The client index.
 * @amount  The amount of credits to take.
 * 
 * @return  The new amount of credits.
 */
stock CreditsTakeCredits(client, amount)
{
    // Negate the value and feed it to CreditsGive.
    return CreditsGiveCredits(client, -amount);
}

/**
 * Set the amount of credits a client has.
 * 
 * @param client    The client index.
 * @param amount    The amount of credits.
 */
stock CreditsSetCredits(client, amount)
{
    // Forward pre-event to modules.
    new Action:result = APIOnCreditsModifiedPre(client, amount);
    
    // If the forward returned Plugin_Handled or Plugin_Stop, then stop this function.
    if (result == Plugin_Handled || result == Plugin_Stop)
    {
        return;
    }
    
    // Get how many credits the client had before.
    new oldvalue = CreditsGetCredits(client);
    
    // Set the new value.
    CookiesSetInt(client, g_hCreditsCookie, amount);
    
    // Forward event to modules.
    APIOnCreditsModified(client, amount, oldvalue);
}

// This is the description for the OnCreditsModified forward.
/**
 * Called when credits on a client are modified.
 * 
 * @param client    The client index.
 * @param newvalue  The current amount of credits.
 * @param oldvalue  How many the client had before the change.
 */

/**
 * Get the amount of credits a client has.
 * 
 * @param client    The client index.
 * 
 * @param return    The amount of credits the client has.
 */
stock CreditsGetCredits(client)
{
    // Return the amount of credits the client has.
    return CookiesGetInt(client, g_hCreditsCookie);
}

/**
 * Converts currencies.
 * 
 * @param value         The value of the currency you are converting from. (rounded to nearest whole number)
 * @param exchangerate  The exchange rate for this currency to another. (See defines at the top of the file)
 * 
 * @return              The value of the new currency. (rounded to nearest whole number)
 */
stock CreditsConvertCurrency(value, Float:exchangerate)
{
    return RoundToNearest(Float:value *= exchangerate);
}
