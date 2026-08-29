# War-Front
***"A game made in C++ with entirely ASCII graphics. Make army and conquer the enemy faction."***

Welcome to warfront. The world is currently at war. The Humans on planet earth were destroyed by the newly mutated, battle ready bugs.

First compile the game based on your OS: Use Linux for Linux OS' (or android's, tho the ASCII art is glitched on most android compilers because of the small screen size) or use the regular version for windows.

***How to play:***
Start by picking a faction. The two factions are different in two ways:
1. Bugs are cheaper so you can buy more of them
2. Humans have much higher stats then bugs

After you pick your faction you enter the command screen. Here you can do 4 things:
1. Recruit-By new units with cash
2. Upgrade-Upgrade your economy, units or unlock nuke parts
3. Battle-Do combat, use your units for a simulation of a battle between your units and a incremental wave of enemy units
4. Sabotage-unlocks the ending, Gain all nuke parts from upgrade, buy them and send the nuke

***How battles work:***
Each battle is a simulation, every unit attack eachother, the unit with the higher speed goes first. Then they deal attack-(defense/2) damage to the enemy. The enemy is chosen via the unit type. With higher unit types attacking higher unit types. 

***How unit power works:***
Unit power is a simple calculation used to simplify how strong your army is. It's only visible during a battle, here are the calculations for it:
HP+ATK*3+DEF*2+SPD*5A

That should explain the game. Good luck and massacre those bugs/humans
