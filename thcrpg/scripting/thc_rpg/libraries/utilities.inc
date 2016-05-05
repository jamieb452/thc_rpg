/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          utilities.inc
 *  Type:          Utilities
 *  Description:   Place for various utility functions.
 *
 *  Copyright (C) 2009-2011  Greyscale, Richard Helgeby
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

#if defined _utilities_included
 #endinput
#endif
#define _utilities_included

/**
 * Max len of string printed to console. Excludes newline and null terminator.
 */
#define UTIL_CONSOLE_MAX_LEN    1022

/**
 * Filters.
 */
#define UTILS_FILTER_TEAM         (1 << 0)    /** Only allow clients on a team. */
#define UTILS_FILTER_UNASSIGNED   (1 << 1)    /** Only allow clients not on a team. */
#define UTILS_FILTER_ALIVE        (1 << 2)    /** Only allow live clients. */
#define UTILS_FILTER_DEAD         (1 << 3)    /** Only allow dead clients. */
#define UTILS_FILTER_CANTARGET    (1 << 4)    /** Only list clients that the menu recipient can target.  Needs target client!*/

/**
 * Build an array of clients that pass the filters.
 * 
 * @param adtClients    The handle of the array.
 *                      Don't forget to call CloseHandle on it when finished!
 * @param filters       A bit field made up of UTILS_FILTER_* defines.  See top of file.
 * @param targetclient  Creating the list for this client.  Can use filters such as UTILS_FILTER_CANTARGET if valid client is given.*  *                       
 * 
 * @return              Number of eligible clients.
 */
stock Util_BuildClientList(&Handle:adtClients, filters, targetclient = 0)
{
    adtClients = CreateArray();
    
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        // Check filters.
        if (Util_IsClientOnTeam(client))
        {
            // The client is on a team, so check if the filter is asking only for unassigned clients.
            if (filters & UTILS_FILTER_UNASSIGNED)
                continue;
        }
        else
        {
            // The client is unassigned, so check if the filter is asking only for clients on a team.
            if (filters & UTILS_FILTER_TEAM)
                continue;
        }
        
        if (IsPlayerAlive(client))
        {
            // The client is alive, so check if the filter is asking only for dead clients.
            if (filters & UTILS_FILTER_DEAD)
                continue;
        }
        else
        {
            // The client is dead, so check if the filter is asking only for alive clients.
            if (filters & UTILS_FILTER_ALIVE)
                continue;
        }
        
        // Check if the menu recipient can target this client, if not then don't add to the menu.
        if (Util_IsClientInGame(targetclient) && (filters & UTILS_FILTER_CANTARGET && !CanUserTarget(targetclient, client)))
            continue;
        
        // Add eligible client to array.
        PushArrayCell(adtClients, client);
    }
    
    return GetArraySize(adtClients);
}

/**
 * Check if a client is in the game.
 * Doesn't error if the index isn't in the valid client index range. (1-64)*  
 * 
 * @param client    The index to check
 * 
 * @return          True if client index is in-game, false if not.
 */
stock bool:Util_IsClientInGame(client)
{
    if (client <= 0 || client > MaxClients)
        return false;
    
    return IsClientInGame(client);
}

/**
 * Check if a client index is on a team.
 * 
 * @param client    The client index.
 * 
 * @return          True if client is on a team, false otherwise.
 */
stock bool:Util_IsClientOnTeam(client)
{
    new cteam = GetClientTeam(client);
    return (cteam > 1);
}

/**
 * Fully remove a weapon from a client's inventory and the world.
 * 
 * @param client        The client whose weapon to remove.
 * @param weaponindex   The weapon index.
 */
stock Util_RemovePlayerItem(client, weaponindex)
{
    RemovePlayerItem(client, weaponindex);
    RemoveEdict(weaponindex);
}

/**
 * Switch a clients team only if their current one differs.
 * 
 * @param client    The client to switch.
 * @param team      The team index to switch client to.
 */
stock Util_SwitchTeam(client, team)
{
    if (GetClientTeam(client) != team)
    {
        #if defined PROJECT_GAME_CSS
            CS_SwitchTeam(client, team);
        #endif
    }
}

/**
 * Closes a handle and sets it to invalid handle.
 * 
 * @param handle    The handle to close.
 */
stock Util_CloseHandle(&Handle:handle)
{
    if (handle != INVALID_HANDLE)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE;
    }
}

/**
 * Checks if a vector exists in a vector array.
 * 
 * @param vec       Vector to search for in array
 * @param vecs      Array to search in.
 * @param numvecs   Number of vectors in the array.
 * 
 * @return          Index in array that the vector is in, -1 if not found.
 */
stock Util_IsVecInArray(Float:vec[3], Float:vecs[][3], numvecs)
{
    for (new vindex; vindex < numvecs; vindex++)
    {
        if (vec[0] == vecs[vindex][0]
            && vec[1] == vecs[vindex][1]
            && vec[2] == vecs[vindex][2])
        {
            return vindex;
        }
    }
    
    return -1;
}

/**
 * Swaps the value in two cell variables.
 *
 * @param value1    Input/Output. First value.
 * @param value2    Input/Output. Second value.
 */
stock Util_SwapCell(&any:value1, &any:value2)
{
    new any:temp = value1;
    value1 = value2;
    value2 = temp;
}

/**
 * Prints strings longer than 1022 bytes to the console. Max 4 KB.
 *
 * @param client        Client index, or 0 for server.
 * @param format        Formatting rules.
 * @param ...           Variable number of format parameters.
 */
stock Util_ReplyToCommandEx(client, const String:format[], any:...)
{
    decl String:buffer[4095];   // Excludes newline added by ReplyToCommand.
    buffer[0] = 0;
    new bufferLen;
    
    decl String:window[UTIL_CONSOLE_MAX_LEN];   // Window buffer.
    new windowPos;      // Current position of the 1022 byte window.
    new windowLen;      // How many charaters to print in the current window.
    new bool:shiftWindow;   // Shift window one character extra when moving.
    
    new nextLine;       // Position to next line.
    new lastLine;       // Position to last line break within the window.
    
    new searchPos;      // Temp position.
    
    // Prepare buffer.
    VFormat(buffer, sizeof(buffer), format, 3);
    bufferLen = strlen(buffer);
    
    // Print as many whole lines as possible within a window of 1022 bytes.
    new bool:done;
    while (!done)
    {
        // Get the position of last line break in the window.
        searchPos = windowPos;
        while (searchPos >= 0)
        {
            // Get next line.
            nextLine = FindCharInString(buffer[searchPos], '\n');
            
            if (nextLine < 0)
            {
                // The whole string (or the rest of it) is one line. Just break
                // the string.
                if (windowPos + UTIL_CONSOLE_MAX_LEN <= bufferLen)
                {
                    // Line ends after this window, fill the whole window.
                    windowLen = UTIL_CONSOLE_MAX_LEN;
                    shiftWindow = false;
                    break;
                }
                else
                {
                    // Line ends within this window, fill it until buffer ends.
                    windowLen = strlen(buffer[windowPos]);
                    shiftWindow = false;
                    break;
                }
            }
            else if (searchPos + nextLine < windowPos + UTIL_CONSOLE_MAX_LEN)
            {
                // Still more space in window, search for another line.
                // Adding 1 to skip the line break character.
                searchPos += nextLine + 1;
                lastLine = searchPos;
            }
            else
            {
                // No space for the next line. Fill window up to last line.
                windowLen = lastLine - windowPos;
                shiftWindow = true;
                break;
            }
        }
        
        // Print the current window.
        strcopy(window, windowLen, buffer[windowPos]);
        ReplyToCommand(client, window);
        
        // Check if done.
        if (windowPos + windowLen >= bufferLen)
        {
            break;
        }
        
        // Move window.
        windowPos = windowPos + windowLen - 1;
        
        if (shiftWindow)
        {
            // Move window one extra character to move past the last line break.
            windowPos++;
        }
    }
}

#if defined PROJECT_GAME_CSS

/**
 * Takes a winning team index and returns its corresponding round end reason.
 * Ex: Takes index '2' and returns the Terrorists_Win round end reason.
 * 
 * @param teamindex     The team index that won the round.
 * 
 * @return              The round end reason.  ROUNDEND_GAME_COMMENCING if invalid teamindex.
 */
stock RoundEndReasons:Util_TeamToReason(teamindex)
{
    if (teamindex == TEAM_2)
        return ROUNDEND_TERRORISTS_WIN;
    
    else if (teamindex == TEAM_3)
        return ROUNDEND_CTS_WIN;
    
    return ROUNDEND_GAME_COMMENCING;
}

#endif

#if defined PROJECT_GAME_TF2

/**
 * Ends a TF2 game in a stalemate when this is passed to the function.
 */
#define ROUNDEND_STALEMATE 0

/**
 * Takes a winning team index and returns its corresponding round end reason.
 * Ex: Takes index '2' and returns the Terrorists_Win round end reason.
 * 
 * @param teamindex     The team index that won the round.
 * 
 * @return              Returns 'teamindex' unless it is invalid, in which case ROUNDEND_STALEMATE is returned.
 */
stock Util_TeamToReason(teamindex)
{
    if (teamindex == TEAM_2 || teamindex == TEAM_3)
        return teamindex;
    
    return ROUNDEND_STALEMATE;
}

/**
 * Terminates the round. (TF2)
 * Credits to toazron1 for this one :)
 * 
 * @team    The team index of the winner.
 */
stock TF2_TerminateRound(teamindex)
{
    new ent = FindEntityByClassname(-1, "team_control_point_master");
    if (ent == -1)
    {
        ent = CreateEntityByName("team_control_point_master");
        DispatchSpawn(ent);
        AcceptEntityInput(ent, "Enable");
    }
    
    SetVariantInt(teamindex);
    AcceptEntityInput(ent, "SetWinner");
}

#endif
