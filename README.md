# Quake Starve
---
Quake Starve is a mod to Quake 2 implementing mechanics from Klei Entertainment's Don't Starve games. It adds new survival mechanics that change the dynamic of the game as a whole.

This mod adds five new stats for the player character to balance in addition to their health value:
 - **Hunger** - It's in the name of the game! If your hunger gets too low you will start taking damage
 - **Sanity** - You don't want to know what happens when you go insane. Creatures stalk the night in search of those that can see them.
 - **Temperature** - Try not to get too hot or too cold, wouldn't want you melting or freezing over.
 - **Mightiness** - Never. Skip. Leg Day. Keep your mightiness up to perform tasks more efficiently, don't let it get too low or you will become wimpy.
 - **Stamina** - You need to space out your workload, it's never good to overwork yourself.

The player is given access to an assortment of equipment to gather resources and craft the materials and items needed to maintain your stats. Survive as long as you can see if you can nab yourself a hefty high score.

Here are the instructions to install and start playing Quake Starve:
## Installation
---
This repository provides you with a zip file with all the necessary items to play this mod. It will have all the assets needed for the mod as well as a `gamex86.dll` file containing the logic of the mod.

If you have Quake 2 on Steam you would move the `quakestarve` folder into your Quake 2 directory:
`C:\Program Files (x86)\Steam\steamapps\common\Quake 2`

**IMPORTANT NOTE: You must not change the name of the mod folder, the mod requires your mod folder to be called `quakestarve` in order to modify the high score save file**

Now you are ready to start playing this mod!
## Playing the Mod
---
In order to play the mod you must make some modifications to your executable file to ensure it will run classic Quake, as this mod will only work on class Quake 2. These instructions are for Steam users, but some of these steps should be applicable to owners of the Quake 2 game standalone.

Go to your Quake 2 directory (under your Steam directory), right click the executable and click on `Properties` to modify the executable's properties.

Under the `Compatibility` tab of the executable properties, check "Run this program on compatibility mode for:" and select "Windows XP: Service Pack 2" (This is to ensure classic Quake 2 will run, if you have another way of getting it to run without crashing feel free to roll with that. This is simply the method I got working myself).

Once this is done, in Steam you will need to modify Quake 2's launch options. Select the gear icon on the game's page in your library and select `Properties`
 - Under the drop down in "Launch Options" select "Play Quake II (Original)"
 - Under "Advanced users may choose to enter modifications in their launch options," type `+set game quakestarve`

When you launch Quake 2 from Steam you should now be able to play Quake Starve.

If you want to make a custom shortcut to automatically launch your game on startup:
 - Create a shortcut to the Quake 2 executable on windows, open the properties of the shortcut, then at the end of the "Target" field append `+set game quakestarve`. This will create a shortcut for you to launch the executable with the mod. Note that this custom shortcut may be a little finnicky. From my experience I've had to launch the game through Steam first to get it to run Quake 2 classic by default.
## Commands
---
To interface with most of the game you will need to know some commands:

1. `modhelp` - This will cycle the help screen between values 0, 1, and 2. By default `F1` will open page 1 of a message explaining how to play the mod in game. Run `modhelp` a second time to open page 2, and run it a third time to open the main help screen of the mod (featuring crafting recipes and stats).
   
2. `craft` - This is how you craft things in game. The help screen will list out the names of craftable items and whether you can craft it or not with your given materials. If you want to craft a specific item, provide its name from that list as a parameter.
   
3. `spawnresource` - Since no custom maps have been made for this mod you will have to spawn custom resource gatherables yourself. To do this, choose any of the following as a parameter for this command:
	1. `tree` - Spawns a wood gatherable
	2. `stone` - Spawns a stone gatherable
	3. `marble` - Spawns a marble gatherable
	4. `grass` - Spawns a grass gatherable
	5. `gold` - Spawns a gold gatherable
	6. `grave` - Spawns a ruby gatherable

This is all you will need to check out the functionality of this mod. Good luck surviving!
## Licensing Information
---
Since this repository is a fork of the open source Quake 2 repository, I've included the README content from the original repository in this README:

```
This is the complete source code for Quake 2, version 3.19, buildable with
visual C++ 6.0.  The linux version should be buildable, but we haven't
tested it for the release.

The code is all licensed under the terms of the GPL (gnu public license).  
You should read the entire license, but the gist of it is that you can do 
anything you want with the code, including sell your new version.  The catch 
is that if you distribute new binary versions, you are required to make the 
entire source code available for free to everyone.

The primary intent of this release is for entertainment and educational 
purposes, but the GPL does allow commercial exploitation if you obey the 
full license.  If you want to do something commercial and you just can't bear 
to have your source changes released, we could still negotiate a separate 
license agreement (for $$$), but I would encourage you to just live with the 
GPL.

All of the Q2 data files remain copyrighted and licensed under the 
original terms, so you cannot redistribute data from the original game, but if 
you do a true total conversion, you can create a standalone game based on 
this code.

Thanks to Robert Duffy for doing the grunt work of building this release.

John Carmack
Id Software
```

Thank you John Carmack!