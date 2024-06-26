/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "g_local.h"



/*
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
	if (deathmatch->value || coop->value)
		ent->client->showscores = true;
	VectorCopy (level.intermission_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2]*8;
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	// add the layout

	if (deathmatch->value || coop->value)
	{
		DeathmatchScoreboardMessage (ent, NULL);
		gi.unicast (ent, true);
	}

}

void BeginIntermission (edict_t *targ)
{
	int		i, n;
	edict_t	*ent, *client;

	if (level.intermissiontime)
		return;		// already activated

	game.autosaved = false;

	// respawn any dead clients
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (coop->value)
		{
			for (i=0 ; i<maxclients->value ; i++)
			{
				client = g_edicts + 1 + i;
				if (!client->inuse)
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_ITEMS; n++)
				{
					if (itemlist[n].flags & IT_KEY)
						client->client->pers.inventory[n] = 0;
				}
			}
		}
	}
	else
	{
		if (!deathmatch->value)
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		i = rand() & 3;
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	VectorCopy (ent->s.origin, level.intermission_origin);
	VectorCopy (ent->s.angles, level.intermission_angle);

	// move all clients to the intermission point
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		MoveClientToIntermission (client);
	}
}


/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage (edict_t *ent, edict_t *killer)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, k;
	int		sorted[MAX_CLIENTS];
	int		sortedscores[MAX_CLIENTS];
	int		score, total;
	int		picnum;
	int		x, y;
	gclient_t	*cl;
	edict_t		*cl_ent;
	char	*tag;

	// sort the clients by score
	total = 0;
	for (i=0 ; i<game.maxclients ; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse || game.clients[i].resp.spectator)
			continue;
		score = game.clients[i].resp.score;
		for (j=0 ; j<total ; j++)
		{
			if (score > sortedscores[j])
				break;
		}
		for (k=total ; k>j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	// print level name and exit rules
	string[0] = 0;

	stringlength = strlen(string);

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (i=0 ; i<total ; i++)
	{
		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		picnum = gi.imageindex ("i_fixme");
		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == ent)
			tag = "tag1";
		else if (cl_ent == killer)
			tag = "tag2";
		else
			tag = NULL;
		if (tag)
		{
			Com_sprintf (entry, sizeof(entry),
				"xv %i yv %i picn %s ",x+32, y, tag);
			j = strlen(entry);
			if (stringlength + j > 1024)
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
		}

		// send the layout
		Com_sprintf (entry, sizeof(entry),
			"client %i %i %i %i %i %i ",
			x, y, sorted[i], cl->resp.score, cl->ping, (level.framenum - cl->resp.enterframe)/600);
		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}


/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
	DeathmatchScoreboardMessage (ent, ent->enemy);
	gi.unicast (ent, true);
}


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showhelp = false;

	if (!deathmatch->value && !coop->value)
		return;

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboard (ent);
}

/*
==================
HelpTextComputer

Draw mod instructions
==================
*/
void HelpTextComputer(edict_t* ent)
{
	char	string[4096];

	Com_sprintf(string, sizeof(string),
		"xv -112 yv 8 picn inventory "			// stats
		"xv -96 yv 24 string2 \"Welcome to Quake Starve!\" "
		"xv -96 yv 32 string2 \"This mod adds features from\" "
		"xv -96 yv 40 string2 \"Don't Starve into Quake 2.\" "
		"xv -96 yv 48 string2 \"Your goal is to Survive\" "
		"xv -96 yv 56 string2 \"You will start with an axe\" "
		"xv -96 yv 64 string2 \"and a pickaxe, you can scroll\" "
		"xv -96 yv 72 string2 \"through your tools with the 1\" "
		"xv -96 yv 80 string2 \"key. Each tool can be used\" "
		"xv -96 yv 88 string2 \"for a specific gatherables.\" "
		"xv -96 yv 96 string2 \"They have durability so you can\" "
		"xv -96 yv 104 string2 \"craft more using the craft\" "
		"xv -96 yv 112 string2 \"command. The help screen will\" "
		"xv -96 yv 120 string2 \"show you all the things you can\" "
		"xv -96 yv 128 string2 \"craft by name. To spawn\" "
		"xv -96 yv 136 string2 \"resources, use spawnresource\" "
		"xv -96 yv 144 string2 \"command with one of the\" "
		"xv -96 yv 152 string2 \"following arguments: tree,\" "
		"xv -96 yv 160 string2 \"stone, marble, gold, grass.\" "
		"xv -96 yv 168 string2 \"grave.\" ");

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
	gi.unicast(ent, true);
}

/*
==================
HelpTextComputer

Draw mod instructions
==================
*/
void HelpTextComputer2(edict_t* ent)
{
	char	string[4096];

	Com_sprintf(string, sizeof(string),
		"xv 176 yv 8 picn inventory "			// crafts
		"xv 196 yv 24 string2 \"You have five stats to \" "
		"xv 196 yv 32 string2 \"maintain on top of health:\" "
		"xv 196 yv 48 string2 \"If your hunger gets too \" "
		"xv 196 yv 56 string2 \"low you take damage over \" "
		"xv 196 yv 64 string2 \"time.\" "
		"xv 196 yv 80 string2 \"If your sanity gets too\" "
		"xv 196 yv 88 string2 \"low nightmares will spawn.\" "
		"xv 196 yv 104 string2 \"You consume stamina when you\" "
		"xv 196 yv 112 string2 \"work.\" "
		"xv 196 yv 128 string2 \"If your temperature gets\" "
		"xv 196 yv 136 string2 \"too high or low you take\" "
		"xv 196 yv 144 string2 \"damage\" "
		"xv 196 yv 160 string2 \"Keep your mightiness up to\" "
		"xv 196 yv 168 string2 \"do tasks more efficiently.\" ");

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
	gi.unicast(ent, true);
}



/*
==================
HelpComputer

Draw help computer.
==================
*/
void HelpComputer (edict_t *ent)
{
	if (ent->client->modhelp == 0) {
		HelpTextComputer(ent);
		return;
	}
	else if (ent->client->modhelp == 1) {
		HelpTextComputer2(ent);
		return;
	}

	char	string[4096];
	char	*sk;

	if (skill->value == 0)
		sk = "easy";
	else if (skill->value == 1)
		sk = "medium";
	else if (skill->value == 2)
		sk = "hard";
	else
		sk = "hard+";

	// send the layout
	/*
	Com_sprintf (string, sizeof(string),
		"xv 32 yv 8 picn help "			// background
		"xv 202 yv 12 string2 \"%s\" "		// skill
		"xv 0 yv 24 cstring2 \"%s\" "		// level name
		"xv 0 yv 54 cstring2 \"%s\" "		// help 1
		"xv 0 yv 110 cstring2 \"%s\" "		// help 2
		"xv 50 yv 164 string2 \" kills     goals    secrets\" "
		"xv 50 yv 172 string2 \"%3i/%3i     %i/%i       %i/%i\" ", 
		sk,
		level.level_name,
		game.helpmessage1,
		game.helpmessage2,
		level.killed_monsters, level.total_monsters, 
		level.found_goals, level.total_goals,
		level.found_secrets, level.total_secrets);
	*/
	
	int* stone = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Stone"))];
	int* wood = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Wood"))];
	int* marble = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Marble"))];
	int* grass = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Grass"))];
	int* gold = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Gold"))];
	int* ruby = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Ruby"))];
	int* hmeat = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Hearty Meat"))];
	int* gmeat = &ent->client->pers.inventory[ITEM_INDEX(FindItem("Gourmet Meat"))];


	/* MOD ADDITION
	*  - Displays stats
	*  - Displays craftables
	*/
	Com_sprintf(string, sizeof(string),
		"xv -96 yv -80 picn inventory "			// stats
		"xv -64 yv -48 string2 \"Hunger: %i\" "		// hunger
		"xv -64 yv -16 string2 \"San: %i\" "		// sanity
		"xv -64 yv 16 string2 \"Stamina: %i\" "		// stamina
		"xv -64 yv 48 string2 \"Temp: %i\" "		// temperature
		"xv -64 yv 80 string2 \"Might: %i\" "		// mightiness
		"xv 160 yv -80 picn inventory "			// crafts
		"xv 180 yv -64 string2 \"|   item   | mats | craft? |\" " // table
		"xv 180 yv -48 string2 \"    pick    2s,1w     %s   \" " // pickaxe crafting
		"xv 180 yv -32 string2 \"    axe      2s,1w    %s   \" " // axe crafting
		"xv 180 yv -16 string2   \"   torch     2g,1w    %s   \" " // torch crafting
		"xv 180 yv 0 string2  \"  sickle     2s,1w    %s   \" " // sickle crafting
		"xv 180 yv 16 string2  \"  shovel     2s,1w    %s   \" " // shovel crafting
		"xv 180 yv 32 string2  \"  luxpick    2au,1r   %s   \" " // luxury pickaxe crafting
		"xv 180 yv 48 string2  \"   bell       2m      %s    \" " // luxury pickaxe crafting
		"xv 180 yv 64 string2  \"   drip      2hm      %s   \" " // iv drip crafting
		"xv 180 yv 80 string2  \"   stew       2gm     %s    \" " // vial of stew crafting
		"xv 32 yv 100 picn help "			// background
		"xv 202 yv 104 string2 \"%s\" "		// skill
		"xv 0 yv 116 cstring2 \"%s\" "		// level name
		"xv 0 yv 146 cstring2 \"%s\" "		// help 1
		"xv 0 yv 202 cstring2 \"%s\" "		// help 2
		"xv 50 yv 256 string2 \" kills     score    hi-score\" "
		"xv 50 yv 264 string2 \" %3i       %3i         %3i  \" ",
		ent->hunger,
		ent->sanity,
		ent->stamina,
		ent->temperature,
		ent->mightiness,
		(*stone >= 2 && *wood >= 1) ? "Y" : "N",
		(*stone >= 2 && *wood >= 1) ? "Y" : "N",
		(*grass >= 2 && *wood >= 1) ? "Y" : "N",
		(*stone >= 2 && *wood >= 1) ? "Y" : "N",
		(*stone >= 2 && *wood >= 1) ? "Y" : "N",
		(*gold >= 2 && *ruby >= 1) ? "Y" : "N",
		(*marble >= 1) ? "Y" : "N",
		(*hmeat >= 1) ? "Y" : "N",
		(*gmeat >= 1) ? "Y" : "N",
		sk,
		"The Constant",
		"Survive",
		"Beat your high score",
		level.killed_monsters,
		ent->lifetime,
		ent->highscore);

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, true);
}




/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent)
{
	// this is for backwards compatability
	if (deathmatch->value)
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if (ent->client->showhelp && (ent->client->pers.game_helpchanged == game.helpchanged))
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;
	HelpComputer (ent);
}

/*
==================
Cmd_HelpMod_f

Display the current help message
==================
*/
void Cmd_HelpMod_f(edict_t* ent)
{
	if (ent->client->modhelp == 0) {
		ent->client->modhelp = 1;
	}
	else if (ent->client->modhelp == 1) {
		ent->client->modhelp = 2;
	}
	else {
		ent->client->modhelp = 0;
	}
	gi.cprintf(ent, PRINT_HIGH, "Show mod help screen: %i\n", ent->client->modhelp);
}


//=======================================================================

/*
===============
G_SetStats
===============
*/
void G_SetStats (edict_t *ent)
{
	gitem_t		*item;
	int			index, cells;
	int			power_armor_type;

	//
	// health
	//
	ent->client->ps.stats[STAT_HEALTH_ICON] = level.pic_health;
	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	//
	// ammo
	//
	if (!ent->client->ammo_index /* || !ent->client->pers.inventory[ent->client->ammo_index] */)
	{
		ent->client->ps.stats[STAT_AMMO_ICON] = 0;
		ent->client->ps.stats[STAT_AMMO] = 0;
	}
	else
	{
		item = &itemlist[ent->client->ammo_index];
		ent->client->ps.stats[STAT_AMMO_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_AMMO] = ent->client->pers.inventory[ent->client->ammo_index];
	}
	
	//
	// armor
	//
	power_armor_type = PowerArmorType (ent);
	if (power_armor_type)
	{
		cells = ent->client->pers.inventory[ITEM_INDEX(FindItem ("cells"))];
		if (cells == 0)
		{	// ran out of cells for power armor
			ent->flags &= ~FL_POWER_ARMOR;
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/power2.wav"), 1, ATTN_NORM, 0);
			power_armor_type = 0;;
		}
	}

	index = ArmorIndex (ent);
	if (power_armor_type && (!index || (level.framenum & 8) ) )
	{	// flash between power armor and other armor icon
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex ("i_powershield");
		ent->client->ps.stats[STAT_ARMOR] = cells;
	}
	else if (index)
	{
		item = GetItemByIndex (index);
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_ARMOR] = ent->client->pers.inventory[index];
	}
	else
	{
		ent->client->ps.stats[STAT_ARMOR_ICON] = 0;
		ent->client->ps.stats[STAT_ARMOR] = 0;
	}

	//
	// pickup message
	//
	if (level.time > ent->client->pickup_msg_time)
	{
		ent->client->ps.stats[STAT_PICKUP_ICON] = 0;
		ent->client->ps.stats[STAT_PICKUP_STRING] = 0;
	}

	//
	// timers
	//
	if (ent->client->quad_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_quad");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->quad_framenum - level.framenum)/10;
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_invulnerability");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->invincible_framenum - level.framenum)/10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_envirosuit");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->enviro_framenum - level.framenum)/10;
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_rebreather");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->breather_framenum - level.framenum)/10;
	}
	else
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}

	//
	// selected item
	//
	if (ent->client->pers.selected_item == -1)
		ent->client->ps.stats[STAT_SELECTED_ICON] = 0;
	else
		ent->client->ps.stats[STAT_SELECTED_ICON] = gi.imageindex (itemlist[ent->client->pers.selected_item].icon);

	ent->client->ps.stats[STAT_SELECTED_ITEM] = ent->client->pers.selected_item;

	//
	// layouts
	//
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if (deathmatch->value)
	{
		if (ent->client->pers.health <= 0 || level.intermissiontime
			|| ent->client->showscores)
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}
	else
	{
		if (ent->client->showscores || ent->client->showhelp)
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}

	//
	// frags
	//
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;

	//
	// help icon / current weapon if not shown
	//
	if (ent->client->pers.helpchanged && (level.framenum&8) )
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex ("i_help");
	else if ( (ent->client->pers.hand == CENTER_HANDED || ent->client->ps.fov > 91)
		&& ent->client->pers.weapon)
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex (ent->client->pers.weapon->icon);
	else
		ent->client->ps.stats[STAT_HELPICON] = 0;

	ent->client->ps.stats[STAT_SPECTATOR] = 0;
}

/*
===============
G_CheckChaseStats
===============
*/
void G_CheckChaseStats (edict_t *ent)
{
	int i;
	gclient_t *cl;

	for (i = 1; i <= maxclients->value; i++) {
		cl = g_edicts[i].client;
		if (!g_edicts[i].inuse || cl->chase_target != ent)
			continue;
		memcpy(cl->ps.stats, ent->client->ps.stats, sizeof(cl->ps.stats));
		G_SetSpectatorStats(g_edicts + i);
	}
}

/*
===============
G_SetSpectatorStats
===============
*/
void G_SetSpectatorStats (edict_t *ent)
{
	gclient_t *cl = ent->client;

	if (!cl->chase_target)
		G_SetStats (ent);

	cl->ps.stats[STAT_SPECTATOR] = 1;

	// layouts are independant in spectator
	cl->ps.stats[STAT_LAYOUTS] = 0;
	if (cl->pers.health <= 0 || level.intermissiontime || cl->showscores)
		cl->ps.stats[STAT_LAYOUTS] |= 1;
	if (cl->showinventory && cl->pers.health > 0)
		cl->ps.stats[STAT_LAYOUTS] |= 2;

	if (cl->chase_target && cl->chase_target->inuse)
		cl->ps.stats[STAT_CHASE] = CS_PLAYERSKINS + 
			(cl->chase_target - g_edicts) - 1;
	else
		cl->ps.stats[STAT_CHASE] = 0;
}

