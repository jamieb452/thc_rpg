/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          hooks.inc
 *  Type:          Core
 *  Description:   hooks any player commands needed for the project
 *
 *  Copyright (C) 2009-2011  ArsiRC
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

#pragma semicolon 1

HookCmds()
{
    RegConsoleCmd("say",SayCommand);
    RegConsoleCmd("say_team",SayCommand);
}

public Action:SayCommand(client,argc)
{
    if(IsValidPlayer(client))
    {
        decl String:player[64],String:command[32];
        GetCmdArg(1,command,sizeof(command));
        GetCmdArg(2,player,sizeof(player));
        
        if(StrEqual(command,"rpgmenu",false) || StrEqual(command,"rpg",false))
            RPGMenu(client);
        else if(StrEqual(command,"rpgrank",false))
            GetPlayerRank(client,player);
        else if(StrEqual(command,"rpgtop10",false) || StrEqual(command,"rpgtop",false))
            GetPlayerTop10(client);
        else if(StrEqual(command,"rpgstats",false))
            StatsMenu(client);
    }
    else
    {
        decl String:command[1280];
        GetCmdArgString(command,sizeof(command));
        PrintToChatAll("console: %s", command);
    }
    
    return Plugin_Continue;
}
