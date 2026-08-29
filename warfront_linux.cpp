#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <vector>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

// ============================================================================
// UNIT STRUCT
// ============================================================================

struct Unit
{
    string name;
    int cost;
    int health;
    int attack;
    int defense;
    int speed;
};

// ============================================================================
// CONSTANTS
// ============================================================================

const int MAX_UNITS = 5;

const int HUMAN = 0;
const int BUGS = 1;

// ANSI console colours
const string HUMAN_COLOR = "\033[1;32m";   // Bright green
const string BUG_COLOR = "\033[1;35m";     // Bright magenta
const string DEFAULT_COLOR = "\033[0m";

// ============================================================================
// COLOUR FUNCTIONS
// ============================================================================

void setColor(const string& color)
{
    cout << color;
}

void setFactionColor(bool faction)
{
    if (faction)
        setColor(BUG_COLOR);
    else
        setColor(HUMAN_COLOR);
}

void resetColor()
{
    setColor(DEFAULT_COLOR);
}

// ============================================================================
// CLEAR / PAUSE
// ============================================================================

void clearScreen()
{
    cout << "\033[2J\033[1;1H";
}

void pauseScreen()
{
    cout << "\nPress ENTER to continue...";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ============================================================================
// SAFE INTEGER INPUT
// ============================================================================

int getIntInput()
{
    int value;

    while (!(cin >> value))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Invalid input. Enter a number: ";
    }

    return value;
}

// ============================================================================
// TITLE
// ============================================================================

void printTitle()
{
    resetColor();

    cout << R"( ░██       ░██                             ░██████████                                  ░██
 ░██       ░██                             ░██                                          ░██
 ░██  ░██  ░██  ░██████   ░██░████         ░██        ░██░████  ░███████  ░████████  ░████████
 ░██ ░████ ░██       ░██  ░███     ░██████ ░█████████ ░███     ░██    ░██ ░██    ░██    ░██
 ░██░██ ░██░██  ░███████  ░██              ░██        ░██      ░██    ░██ ░██    ░██    ░██
 ░████   ░████ ░██   ░██  ░██              ░██        ░██      ░██    ░██ ░██    ░██    ░██
 ░███     ░███  ░█████░██ ░██              ░██        ░██       ░███████  ░██    ░██     ░████
)" << endl;
}

// ============================================================================
// FACTION ASCII ART
// ============================================================================

void printFacs()
{
    cout << R"(
+--------------------------------+
|        .---.                   |
|   ___ /_____\                  |
|  /\.-`( '.' )                  |
| / /    \_-_/_                  |
| \ `-.-"`'V'//-.                |
|  `.__,   |// , \               |
|      |Ll //Ll|\ \              |
|      |__//   | \_\             |
|     /---|[]==| / /             |
|     \__/ |   \/\/              |
|     /_   | Ll_\|               |
|      |`^"""^`|                 |
|      |   |   |                 |
|      |   |   |                 |
|      |   |   |                 |
|      |   |   |                 |
|      L___l___J                 |
|       |_ | _|                  |
|      (___|___)                 |
+--------------------------------+

+--------------------------------+
|                                |
|                 .___     .___  |
|     ___              `~\     `~|
|   o~   `.               |      |
| .----._ `|             ,'      |
| `o     `\|___       __,|~~~~~~~|
|       /~~o   `~>-/|~ '         |
|      |_      <~   |            |
|        `-...-'~\./'            |
|                  `-, __'       |
|                    /'   `~~~~/'|
|                  /'        /'  |
|             ~~`---'   ~~`---'  |
+--------------------------------+
)" << '\n';
}

// ============================================================================
// FACTION SCREEN
// ============================================================================

void factionScreen(bool faction, int cash, int wave)
{
    setFactionColor(faction);

    const string title = faction
        ? "BUG HIVE"
        : "HUMAN MILITARY BASE";

    cout << "\n";

    cout << "+============================================================================+\n";
    cout << "|                                                                            |\n";
    cout << "|                     " << title;

    int padding = 72 - static_cast<int>(title.length());

    cout << string(max(0, padding / 2), ' ');

    cout << "|\n";
    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    if (!faction)
    {
        cout << R"(
|                                                                           |
|                         /\\                    /\\                        |
|                        /  \\       ___        /  \\                       |
|                       /____\\    /____\\     /____\\                      |
|                       | [] |     | | | |     | [] |                       |
|                       |____|     |_|_|_|     |____|                       |
|                          \\        | |         /                          |
|                    _______\\_______|_|________/______                     |
|                   /        |       |       |        \\                    |
|                  /_________|_______|_______|_________\\                   |
|                  |   []    |  ____ | ____  |    []   |                    |
|                  |---------| |    ||    |  |---------|                    |
|                  |         | | HQ || HQ |  |         |                    |
|                  |   []    | |____||____|  |     []  |                    |
|                  |_________|_______________|_________|                    |
|                    ||  ||                       ||  ||                    |
|                    ||  ||       ________        ||  ||                    |
|                    ||  ||      /        \\      ||  ||                    |
|                    ||__||     /  COMMAND \\     ||__||                    |
|                   /______\\  /____________\\  /______\\                   |
|                                                                           |
|              ___           ___              ___           ___             |
|             |   |         |   |            |   |         |   |            |
|             |___|=========|___|============|___|=========|___|            |
|                \\             |      HQ      |             /              |
|                 \\____________|______________|____________/               |
|                                                                           |
|                    [ HUMAN MILITARY COMMAND ]                             |
+---------------------------------------------------------------------------+
)";
    }
    else
    {
        cout << R"(
|                                                                            |
|                              ___                                           |
|                         _.-'`   `'-._                                      |
|                     _.-'             '-._                                  |
|                  .-'       .---.         '-.                               |
|                .'        .'     '.          '.                             |
|               /         /  .---.  \\           \\                          |
|              ;         ;  /     \\  ;           ;                          |
|              |         | |  (O)  | |           |                           |
|              ;         ;  \\     /  ;           ;                          |
|               \\         \\  '---'  /           /                          |
|                '.        '.____.'           .'                             |
|                  '-._                     _.-'                             |
|                      `'--.._______..--'`                                   |
|                                                                            |
|                ___             _.-^-._             ___                     |
|             .-'   '-.       .-'       '-.       .-'   '-.                  |
|           .'  .---.  '.    /    .---.    \\    .'  .---.  '.               |
|          /   /     \\   \\  ;    /     \\    ;  /   /     \\   \\          |
|         ;   |  (O)  |   | ;   |  (O)  |   ; |   |  (O)   |   ;             |
|         |   |  (O)   |   | ;   |  (O)  |   ; |   |  (O)   |   |            |
|         ;    \\     /    ;  \\   \\     /   /  ;    \\     /    ;          |
|          \\    '---'    /    '.  '---'  .'    \\    '---'    /             |
|           '.         .'       '-.   .-'       '.         .'                |
|             '-.___.-'            \\ /             '-.___.-'                |
|                                  / \\                                      |
|                         ________/   \\________                             |
|                      .-'                    '-.                            |
|                    .'     H I V E   C O R E    '.                          |
|                   /______________________________\\                        |
|                   |   ||   ||   ||   ||   ||   |                           |
|                   |   ||   ||   ||   ||   ||   |                           |
|                   |   ||   ||   ||   ||   ||   |                           |
|                   |___||___||___||___||___||___|                           |
|                                                                            |
|                     [ PRIMARY BUG HIVE ]                                   |
+----------------------------------------------------------------------------+
)";
    }

    cout << R"(
|                                                                            |
|    +----------------+        +----------------+                            |
|    | [1]  RECRUIT   |        | [2]  UPGRADE   |                            |
|    |                |        |                |                            |
|    | Add new units  |        | Improve forces |                            |
|    +----------------+        +----------------+                            |
|                                                                            |
|    +----------------+        +----------------+                            |
|    | [3]   ATTACK   |        | [4]  SABOTAGE  |                            |
|    |                |        |                |                            |
|    | Fight waves    |        | Build the Nuke |                            |
|    +----------------+        +----------------+                            |
|                                                                            |
+============================================================================+
|                              SELECT ACTION                                |
+============================================================================+
)";

    cout << "|                         CASH: $" << left << setw(8)
         << cash << "                                  |\n";

    cout << "|                         WAVE: " << left << setw(9)
         << wave << "                                  |\n";

    cout << "+============================================================================+\n";

    resetColor();
}

// ============================================================================
// BASE UNITS
// ============================================================================

void getBaseUnits(bool faction, Unit units[])
{
    if (!faction)
    {
        // HUMANS HAVE EXACTLY 5 UNITS
        units[0] = {"Soldier",        100,  100,  25, 15, 10};
        units[1] = {"Heavy Soldier",  250,  220,  45, 35,  6};
        units[2] = {"Tank",           500,  500,  80, 55,  5};
        units[3] = {"Destroyer Tank", 900,  850, 130, 75,  3};
        units[4] = {"Commander",     1800, 1100, 170, 90,  8};
    }
    else
    {
        // BUGS HAVE EXACTLY 5 UNITS
        units[0] = {"Buggo",       60,  80,  20, 10, 12};
        units[1] = {"Buggo Elite", 160, 170, 38, 25,  9};
        units[2] = {"Hive Titan",  350, 400, 70, 45,  4};
        units[3] = {"Hive King",   650, 700, 110, 60, 3};
        units[4] = {"Hive Queen", 1800,1200, 180,100, 7};
    }
}

// ============================================================================
// ARMY DISPLAY
// ============================================================================

void displayArmy(
    bool faction,
    int army[],
    int trainingTier,
    int weaponTier)
{
    Unit units[MAX_UNITS];

    getBaseUnits(faction, units);

    cout << "\n";
    cout << "+============================================================================+\n";
    cout << "|                              YOUR ARMY                                    |\n";
    cout << "+============================================================================+\n";

    bool hasUnits = false;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        hasUnits = true;

        cout << "|  " << left << setw(20) << units[i].name;

        setFactionColor(faction);

        cout << " x" << setw(6) << army[i];

        resetColor();

        cout << " HP:" << setw(5) << units[i].health
             << " ATK:" << setw(5) << units[i].attack
             << " DEF:" << setw(5) << units[i].defense
             << " SPD:" << setw(4) << units[i].speed
             << " |\n";
    }

    if (!hasUnits)
    {
        cout << "|                                                                            |\n";
        cout << "|                         NO UNITS RECRUITED                                |\n";
        cout << "|                                                                            |\n";
    }

    cout << "+============================================================================+\n";
}

// ============================================================================
// RECRUIT SCREEN
// ============================================================================

void recruitScreen(
    bool faction,
    int& cash,
    int trainingTier,
    int weaponTier,
    int army[])
{
    Unit units[MAX_UNITS];

    getBaseUnits(faction, units);

    // Weapon tiers unlock:
    // Tier 0 = Soldier/Buggo
    // Tier 1 = Heavy
    // Tier 2 = Tank/Titan
    // Tier 3 = Destroyer/Hive King
    // Tier 4 = Commander/Hive Queen

    int maxUnits = weaponTier + 1;

    if (maxUnits > MAX_UNITS)
        maxUnits = MAX_UNITS;

    // Training discount
    for (int i = 0; i < MAX_UNITS; i++)
    {
        units[i].cost =
            units[i].cost * (100 - trainingTier * 5) / 100;
    }

    string title = faction
        ? "BUG HIVE - RECRUITMENT"
        : "HUMAN MILITARY - RECRUITMENT";

    clearScreen();

    setFactionColor(faction);

    cout << "\n";

    cout << "+============================================================================+\n";
    cout << "|                                                                            |\n";
    cout << "|                      " << title;

    int padding = 72 - static_cast<int>(title.length());

    cout << string(max(0, padding / 2), ' ');

    cout << "|\n";
    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    if (!faction)
    {
        cout << R"(
|                         /\\                    /\\                        |
|                        /  \\       ___        /  \\                       |
|                       /____\\     /___\\     /____\\                      |
|                       | [] |     | | | |     | [] |                       |
|                       |____|     |_|_|_|     |____|                       |
|                          \\        | |         /                          |
|                    _______\\_______|_|________/______                     |
|                   /        |       |       |        \\                    |
|                  /_________|_______|_______|_________\\                   |
|                  |   []    |  ____ | ____  |    []   |                    |
|                  |---------| |    ||    |  |---------|                    |
|                  |         | | HQ || HQ |  |         |                    |
|                  |   []    | |____||____|  |     []  |                    |
|                  |_________|_______________|_________|                    |
|                                                                           |
|                       HUMAN RECRUITMENT CENTER                            |
)";
    }
    else
    {
        cout << R"(
|                              ___                                           |
|                         _.-'`   `'-._                                      |
|                     _.-'             '-._                                  |
|                  .-'       .---.         '-.                               |
|                .'        .'     '.          '.                             |
|               /         /  .---.  \\           \\                          |
|              ;         ;  /     \\  ;           ;                          |
|              |         | |  (O)  | |           |                           |
|              ;         ;  \\     /  ;           ;                          |
|               \\         \\  '---'  /           /                          |
|                '.        '.____.'           .'                             |
|                  '-._                     _.-'                             |
|                      `'--.._______..--'`                                   |
|                                                                            |
|                         BUG SPAWN CHAMBER                                  |
)";
    }

    cout << "+----------------------------------------------------------------------------+\n";
    cout << "|                                                                            |\n";
    cout << "|                         AVAILABLE CASH: $" << setw(8)
         << left << cash
         << "                      |\n";

    cout << "|                         TRAINING DISCOUNT: "
         << trainingTier * 5 << "%"
         << "                           |\n";

    cout << "|                                                                            |\n";
    cout << "+============================================================================+\n";
    cout << "|                            AVAILABLE UNITS                                |\n";
    cout << "|                                                                            |\n";

    for (int i = 0; i < MAX_UNITS; i++)
    {
        cout << "|  [" << i + 1 << "] ";

        if (i >= maxUnits)
        {
            cout << left << setw(18) << "LOCKED"
                 << "                                                     |\n";

            continue;
        }

        cout << left << setw(18) << units[i].name
             << " $" << right << setw(5) << units[i].cost
             << "   HP:" << setw(4) << units[i].health
             << "   ATK:" << setw(4) << units[i].attack
             << "   DEF:" << setw(4) << units[i].defense
             << "   SPD:" << setw(3) << units[i].speed
             << " |\n";
    }

    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";
    cout << "|  [0] RETURN                                                               |\n";
    cout << "+============================================================================+\n";

    resetColor();

    cout << "\nSelect a unit to recruit: ";

    int choice = getIntInput();

    if (choice == 0)
        return;

    if (choice < 1 || choice > MAX_UNITS)
    {
        cout << "\nInvalid selection!\n";
        pauseScreen();
        return;
    }

    if (choice > maxUnits)
    {
        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|                 UNIT LOCKED                  |\n";
        cout << "+==============================================+\n";

        if (choice == 5)
        {
            if (!faction)
            {
                cout << "| Upgrade Advanced Weaponry to Tier 4.         |\n";
                cout << "| This unlocks your Commander.                 |\n";
            }
            else
            {
                cout << "| Upgrade Advanced Weaponry to Tier 4.         |\n";
                cout << "| This unlocks your Hive Queen.                |\n";
            }
        }
        else
        {
            cout << "| Upgrade Advanced Weaponry to unlock it.      |\n";
        }

        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    Unit selected = units[choice - 1];

    // ========================================================================
    // ONE-PER-ARMY LIMIT FOR COMMANDER / HIVE QUEEN
    // ========================================================================

    if (choice == 5 && army[4] >= 1)
    {
        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|              UNIT LIMIT REACHED              |\n";
        cout << "+==============================================+\n";
        cout << "| Your army can only have ONE "
             << left << setw(17)
             << selected.name << "|\n";
        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    cout << "\nHow many " << selected.name
         << "(s) do you want to recruit? ";

    int quantity = getIntInput();

    if (quantity <= 0)
    {
        cout << "\nInvalid quantity!\n";
        pauseScreen();
        return;
    }

    // Commander / Hive Queen limited to one
    if (choice == 5 && quantity > 1)
    {
        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|              ONE-PER-ARMY LIMIT              |\n";
        cout << "+==============================================+\n";
        cout << "| You can only recruit ONE "
             << left << setw(17)
             << selected.name << "|\n";
        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    long long totalCost =
        static_cast<long long>(selected.cost) * quantity;

    if (totalCost > cash)
    {
        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|              INSUFFICIENT FUNDS              |\n";
        cout << "+==============================================+\n";
        cout << "| Unit:       " << left << setw(31)
             << selected.name << "|\n";
        cout << "| Quantity:   " << setw(31)
             << quantity << "|\n";
        cout << "| Unit cost:  $" << setw(30)
             << selected.cost << "|\n";
        cout << "| Total cost: $" << setw(30)
             << totalCost << "|\n";
        cout << "| Your cash:  $" << setw(30)
             << cash << "|\n";
        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    cash -= static_cast<int>(totalCost);

    army[choice - 1] += quantity;

    cout << "\n";
    cout << "+==============================================+\n";
    cout << "|               UNITS RECRUITED!               |\n";
    cout << "+==============================================+\n";
    cout << "| Unit:       " << left << setw(31)
         << selected.name << "|\n";
    cout << "| Quantity:   " << setw(31)
         << quantity << "|\n";
    cout << "| Unit cost:  $" << setw(30)
         << selected.cost << "|\n";
    cout << "| Total cost: $" << setw(30)
         << totalCost << "|\n";
    cout << "| Remaining:  $" << setw(30)
         << cash << "|\n";
    cout << "+==============================================+\n";

    cout << "\nYou now have "
         << army[choice - 1]
         << " " << selected.name << "(s).\n";

    pauseScreen();
}

// ============================================================================
// UPGRADE SCREEN
// ============================================================================

void upgradeScreen(
    bool faction,
    int& cash,
    int& bankTier,
    int& trainingTier,
    int& nuclearTier,
    int& weaponTier)
{
    const int MAX_BANK = 5;
    const int MAX_TRAINING = 5;
    const int MAX_NUCLEAR = 3;
    const int MAX_WEAPON = 4;

    int bankCosts[5] =
    {
        250,
        500,
        750,
        1000,
        1500
    };

    int trainingCosts[5] =
    {
        200,
        400,
        600,
        850,
        1200
    };

    int nuclearCosts[3] =
    {
        500,
        1000,
        2000
    };

    int weaponCosts[4] =
    {
        300,
        750,
        1500,
        2500
    };

    while (true)
    {
        clearScreen();

        setFactionColor(faction);

        string title = faction
            ? "BUG HIVE - UPGRADES"
            : "HUMAN MILITARY - UPGRADES";

        cout << "\n";

        cout << "+============================================================================+\n";
        cout << "|                                                                            |\n";
        cout << "|                      " << title;

        int padding = 72 - static_cast<int>(title.length());

        cout << string(max(0, padding / 2), ' ');

        cout << "|\n";
        cout << "|                                                                            |\n";
        cout << "+============================================================================+\n";

        cout << "|                                                                            |\n";
        cout << "|                         AVAILABLE CASH: $" << setw(8)
             << left << cash
             << "                      |\n";
        cout << "|                                                                            |\n";
        cout << "+----------------------------------------------------------------------------+\n";

        // BANK
        cout << "|                                                                            |\n";
        cout << "|  [1] BANK                                                                 |\n";
        cout << "|                                                                            |\n";
        cout << "|      Tier: " << bankTier << "/" << MAX_BANK
             << "                                                             |\n";

        cout << "|      Effect: ";

        if (bankTier == 0)
            cout << "No bonus to attack income";
        else
            cout << "+" << bankTier * 100
                 << " cash after each successful wave";

        cout << "\n";

        if (bankTier < MAX_BANK)
            cout << "|      Next upgrade cost: $" << bankCosts[bankTier];
        else
            cout << "|      MAXIMUM TIER REACHED";

        cout << "\n";

        // TRAINING
        cout << "|                                                                            |\n";
        cout << "|  [2] BETTER TRAINING EQUIPMENT                                            |\n";
        cout << "|                                                                            |\n";
        cout << "|      Tier: " << trainingTier << "/" << MAX_TRAINING
             << "                                                             |\n";

        cout << "|      Effect: ";

        if (trainingTier == 0)
            cout << "Normal recruitment prices";
        else
            cout << trainingTier * 5
                 << "% cheaper recruitment";

        cout << "\n";

        if (trainingTier < MAX_TRAINING)
            cout << "|      Next upgrade cost: $"
                 << trainingCosts[trainingTier];
        else
            cout << "|      MAXIMUM TIER REACHED";

        cout << "\n";

        // NUCLEAR
        cout << "|                                                                            |\n";
        cout << "|  [3] NUCLEAR OPTION RESEARCH                                               |\n";
        cout << "|                                                                            |\n";
        cout << "|      Tier: " << nuclearTier << "/" << MAX_NUCLEAR
             << "                                                             |\n";

        cout << "|      Research status: ";

        if (nuclearTier == 0)
            cout << "Not started";
        else if (nuclearTier == 1)
            cout << "Warhead research";
        else if (nuclearTier == 2)
            cout << "Delivery system";
        else
            cout << "COMPLETE";

        cout << "\n";

        if (nuclearTier < MAX_NUCLEAR)
            cout << "|      Next upgrade cost: $"
                 << nuclearCosts[nuclearTier];
        else
            cout << "|      NUCLEAR RESEARCH COMPLETE";

        cout << "\n";

        // WEAPONRY
        cout << "|                                                                            |\n";
        cout << "|  [4] ADVANCED WEAPONRY                                                    |\n";
        cout << "|                                                                            |\n";
        cout << "|      Tier: " << weaponTier << "/" << MAX_WEAPON
             << "                                                             |\n";

        cout << "|      Unlocked units: ";

        if (weaponTier == 0)
            cout << "Basic unit only";
        else if (weaponTier == 1)
            cout << "Basic + Heavy/Elite";
        else if (weaponTier == 2)
            cout << "Basic + Heavy/Elite + Tank/Titan";
        else if (weaponTier == 3)
            cout << "All standard vehicles";
        else
            cout << "ALL UNIT TYPES + COMMANDER/HIVE QUEEN";

        cout << "\n";

        if (weaponTier < MAX_WEAPON)
            cout << "|      Next upgrade cost: $"
                 << weaponCosts[weaponTier];
        else
            cout << "|      ALL ADVANCED UNITS UNLOCKED";

        cout << "\n";

        cout << "|                                                                            |\n";
        cout << "+============================================================================+\n";
        cout << "|  [0] RETURN                                                               |\n";
        cout << "+============================================================================+\n";

        resetColor();

        cout << "\nSelect an upgrade: ";

        int choice = getIntInput();

        if (choice == 0)
            return;

        if (choice < 1 || choice > 4)
        {
            cout << "\nInvalid selection!\n";
            pauseScreen();
            continue;
        }

        // BANK
        if (choice == 1)
        {
            if (bankTier >= MAX_BANK)
            {
                cout << "\nBank is already fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost = bankCosts[bankTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            bankTier++;

            cout << "\nBank upgraded to Tier "
                 << bankTier << "!\n";

            cout << "Successful wave reward bonus: +"
                 << bankTier * 100 << ".\n";

            pauseScreen();
        }

        // TRAINING
        else if (choice == 2)
        {
            if (trainingTier >= MAX_TRAINING)
            {
                cout << "\nTraining equipment is fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost = trainingCosts[trainingTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            trainingTier++;

            cout << "\nTraining equipment upgraded to Tier "
                 << trainingTier << "!\n";

            cout << "Recruitment is now "
                 << trainingTier * 5
                 << "% cheaper.\n";

            pauseScreen();
        }

        // NUCLEAR RESEARCH
        else if (choice == 3)
        {
            if (nuclearTier >= MAX_NUCLEAR)
            {
                cout << "\nNuclear research is already complete!\n";
                cout << "Go to SABOTAGE to purchase the final weapon parts.\n";

                pauseScreen();
                continue;
            }

            int cost = nuclearCosts[nuclearTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            nuclearTier++;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|          NUCLEAR RESEARCH ADVANCED           |\n";
            cout << "+==============================================+\n";
            cout << "| Nuclear tier: "
                 << nuclearTier << "/3                         |\n";
            cout << "+==============================================+\n";

            if (nuclearTier == 1)
            {
                cout << "| Warhead research completed.                  |\n";
            }
            else if (nuclearTier == 2)
            {
                cout << "| Nuclear delivery system completed.           |\n";
            }
            else
            {
                cout << "| ALL NUCLEAR RESEARCH COMPLETE!               |\n";
                cout << "| Visit SABOTAGE to buy the final parts.       |\n";
            }

            cout << "+==============================================+\n";

            pauseScreen();
        }

        // ADVANCED WEAPONRY
        else if (choice == 4)
        {
            if (weaponTier >= MAX_WEAPON)
            {
                cout << "\nAdvanced Weaponry is fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost = weaponCosts[weaponTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            weaponTier++;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|          ADVANCED WEAPONRY UPGRADED          |\n";
            cout << "+==============================================+\n";
            cout << "| Tier: " << weaponTier << "/4                                      |\n";
            cout << "+==============================================+\n";

            if (weaponTier == 1)
            {
                cout << "| Heavy infantry unlocked!                     |\n";
            }
            else if (weaponTier == 2)
            {
                cout << "| Heavy vehicles unlocked!                     |\n";
            }
            else if (weaponTier == 3)
            {
                cout << "| Ultimate vehicles unlocked!                  |\n";
            }
            else
            {
                if (!faction)
                {
                    cout << "| COMMANDER UNLOCKED!                          |\n";
                    cout << "| ONLY ONE COMMANDER MAY EXIST.                |\n";
                }
                else
                {
                    cout << "| HIVE QUEEN UNLOCKED!                         |\n";
                    cout << "| ONLY ONE HIVE QUEEN MAY EXIST.               |\n";
                }
            }

            cout << "+==============================================+\n";

            pauseScreen();
        }
    }
}

// ============================================================================
// ENEMY UNIT GENERATION
// ============================================================================

void getEnemyUnits(bool faction, Unit enemyUnits[])
{
    if (!faction)
    {
        // HUMAN PLAYER FIGHTS BUGS
        enemyUnits[0] = {"Buggo",       60,  80,  20, 10, 12};
        enemyUnits[1] = {"Buggo Elite", 160, 170, 38, 25,  9};
        enemyUnits[2] = {"Hive Titan",  350, 400, 70, 45,  4};
        enemyUnits[3] = {"Hive King",   650, 700, 110,60,  3};
        enemyUnits[4] = {"Hive Queen", 1800,1200, 180,100, 7};
    }
    else
    {
        // BUG PLAYER FIGHTS HUMANS
        enemyUnits[0] = {"Soldier",        100, 100, 25, 15,10};
        enemyUnits[1] = {"Heavy Soldier",  250, 220, 45, 35, 6};
        enemyUnits[2] = {"Tank",           500, 500, 80, 55, 5};
        enemyUnits[3] = {"Destroyer Tank", 900, 850,130, 75, 3};
        enemyUnits[4] = {"Commander",     1800,1100,170, 90, 8};
    }
}

// ============================================================================
// CREATE ENEMY WAVE
// ============================================================================

void createEnemyWave(
    bool faction,
    int wave,
    int enemyArmy[])
{
    Unit enemyUnits[MAX_UNITS];

    getEnemyUnits(faction, enemyUnits);

    for (int i = 0; i < MAX_UNITS; i++)
        enemyArmy[i] = 0;

    // BASIC ENEMIES
    enemyArmy[0] = 4 + wave;

    // HEAVY ENEMIES
    if (wave >= 3)
        enemyArmy[1] = 1 + (wave - 3) / 2;

    // TANK / TITAN
    if (wave >= 6)
        enemyArmy[2] = 1 + (wave - 6) / 3;

    // ULTIMATE STANDARD UNIT
    if (wave >= 10)
        enemyArmy[3] = 1 + (wave - 10) / 4;

    // COMMANDER / HIVE QUEEN
    if (wave >= 15)
        enemyArmy[4] = 1;
}

// ============================================================================
// DISPLAY ENEMY WAVE
// ============================================================================

void displayEnemyWave(
    bool faction,
    int wave,
    int enemyArmy[])
{
    Unit enemyUnits[MAX_UNITS];

    getEnemyUnits(faction, enemyUnits);

    cout << "\n";
    cout << "+============================================================================+\n";
    cout << "|                         ENEMY WAVE " << left << setw(4)
         << wave << "                                |\n";
    cout << "+============================================================================+\n";

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (enemyArmy[i] <= 0)
            continue;

        cout << "|  " << left << setw(20) << enemyUnits[i].name
             << " x" << setw(6) << enemyArmy[i]
             << " HP:" << setw(5) << enemyUnits[i].health
             << " ATK:" << setw(5) << enemyUnits[i].attack
             << " DEF:" << setw(5) << enemyUnits[i].defense
             << " SPD:" << setw(4) << enemyUnits[i].speed
             << " |\n";
    }

    cout << "+============================================================================+\n";
}

// ============================================================================
// CALCULATE TOTAL ARMY POWER
// ============================================================================

long long calculateArmyPower(
    bool faction,
    int army[])
{
    Unit units[MAX_UNITS];

    getBaseUnits(faction, units);

    long long power = 0;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        long long unitPower =
            static_cast<long long>(units[i].attack) * 3 +
            static_cast<long long>(units[i].defense) * 2 +
            units[i].health +
            units[i].speed * 5;

        power += unitPower * army[i];
    }

    return power;
}

// ============================================================================
// CALCULATE ENEMY POWER
// ============================================================================

long long calculateEnemyPower(
    bool faction,
    int enemyArmy[])
{
    Unit units[MAX_UNITS];

    getEnemyUnits(faction, units);

    long long power = 0;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (enemyArmy[i] <= 0)
            continue;

        long long unitPower =
            static_cast<long long>(units[i].attack) * 3 +
            static_cast<long long>(units[i].defense) * 2 +
            units[i].health +
            units[i].speed * 5;

        power += unitPower * enemyArmy[i];
    }

    return power;
}

// ============================================================================
// APPLY COMBAT LOSSES
// ============================================================================

void applyCombatLosses(
    bool faction,
    int army[],
    int enemyArmy[])
{
    Unit playerUnits[MAX_UNITS];

    getBaseUnits(faction, playerUnits);

    long long playerPower =
        calculateArmyPower(faction, army);

    long long enemyPower =
        calculateEnemyPower(faction, enemyArmy);

    if (playerPower <= 0)
        return;

    double lossRatio =
        static_cast<double>(enemyPower) /
        static_cast<double>(playerPower + enemyPower);

    // Keep losses between 10% and 75%.
    lossRatio =
        max(0.10, min(0.75, lossRatio));

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        int losses =
            static_cast<int>(army[i] * lossRatio);

        if (losses == 0 && army[i] > 2)
            losses = 1;

        // Commander / Hive Queen can either survive or die.
        if (i == 4 && army[i] == 1)
        {
            if (lossRatio >= 0.65)
                losses = 1;
            else
                losses = 0;
        }

        losses = min(losses, army[i]);

        army[i] -= losses;

        if (losses > 0)
        {
            cout << losses << " "
                 << playerUnits[i].name
                 << "(s) were lost.\n";
        }
    }
}

// ============================================================================
// CHECK IF PLAYER HAS ANY UNITS
// ============================================================================

bool hasAnyUnits(int army[])
{
    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] > 0)
            return true;
    }

    return false;
}

// ============================================================================
// CHECK IF PLAYER CAN AFFORD ANY CURRENTLY UNLOCKED UNIT
// ============================================================================

bool canAffordAnyUnit(
    bool faction,
    int cash,
    int trainingTier,
    int weaponTier,
    int army[])
{
    Unit units[MAX_UNITS];

    getBaseUnits(faction, units);

    int maxUnits = weaponTier + 1;

    if (maxUnits > MAX_UNITS)
        maxUnits = MAX_UNITS;

    for (int i = 0; i < maxUnits; i++)
    {
        int discountedCost =
            units[i].cost *
            (100 - trainingTier * 5) / 100;

        // Commander / Hive Queen
        if (i == 4)
        {
            if (army[4] >= 1)
                continue;

            if (cash >= discountedCost)
                return true;
        }
        else
        {
            if (cash >= discountedCost)
                return true;
        }
    }

    return false;
}

// ============================================================================
// LOSE SCREEN
// ============================================================================

void loseScreen(bool faction)
{
    clearScreen();

    setFactionColor(faction);

    cout << "\n";
    cout << "============================================================================\n";
    cout << "                         DEFEAT, GAME OVER!\n";
    cout << "============================================================================\n\n";

    cout << "Your faction has been completely defeated.\n\n";

    if (faction)
    {
        cout << "The Bug Hive has lost all of its forces.\n";
        cout << "There are no resources remaining to spawn another army.\n";
    }
    else
    {
        cout << "The Human Military has lost all of its forces.\n";
        cout << "There are no resources remaining to recruit another army.\n";
    }

    cout << "\n";
    cout << "The enemy controls the battlefield.\n";
    cout << "Your campaign has come to an end.\n\n";

    cout << "============================================================================\n";
    cout << "                              YOU LOSE!\n";
    cout << "============================================================================\n\n";

    resetColor();

    pauseScreen();
}

// ============================================================================
// ATTACK / FIGHT SCREEN
// ============================================================================

bool attackScreen(
    bool faction,
    int& cash,
    int bankTier,
    int& wave,
    int army[])
{
    clearScreen();

    setFactionColor(faction);

    bool hasArmy = hasAnyUnits(army);

    if (!hasArmy)
    {
        cout << "+============================================================================+\n";
        cout << "|                              ATTACK                                       |\n";
        cout << "+============================================================================+\n";
        cout << "|                                                                            |\n";
        cout << "| You have no units!                                                        |\n";
        cout << "| Recruit an army before attacking.                                         |\n";
        cout << "|                                                                            |\n";
        cout << "+============================================================================+\n";

        resetColor();

        pauseScreen();
        return false;
    }

    // CREATE WAVE
    int enemyArmy[MAX_UNITS];

    createEnemyWave(
        faction,
        wave,
        enemyArmy
    );

    // DISPLAY
    cout << "+============================================================================+\n";
    cout << "|                              ATTACK                                       |\n";
    cout << "+============================================================================+\n";
    cout << "|                                                                            |\n";
    cout << "|                         ENEMY TERRITORY                                   |\n";
    cout << "|                                                                            |\n";
    cout << "|                         WAVE " << left << setw(5)
         << wave << "                                     |\n";
    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    displayArmy(
        faction,
        army,
        0,
        0
    );

    displayEnemyWave(
        faction,
        wave,
        enemyArmy
    );

    long long playerPower =
        calculateArmyPower(faction, army);

    long long enemyPower =
        calculateEnemyPower(faction, enemyArmy);

    cout << "\n";
    cout << "+============================================================================+\n";
    cout << "|                              BATTLE POWER                                 |\n";
    cout << "+============================================================================+\n";

    cout << "| Your army power:   ";

    setFactionColor(faction);
    cout << left << setw(58) << playerPower;
    resetColor();

    cout << "|\n";

    cout << "| Enemy wave power:  "
         << left << setw(58)
         << enemyPower << "|\n";

    cout << "+============================================================================+\n";

    cout << "\nPress ENTER to begin the battle...";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    clearScreen();

    setFactionColor(faction);

    cout << "+============================================================================+\n";
    cout << "|                         *** BATTLE START ***                              |\n";
    cout << "+============================================================================+\n\n";

    cout << "Your forces advance...\n\n";

    if (playerPower >= enemyPower)
    {
        cout << "Your army overwhelms the enemy wave!\n\n";

        applyCombatLosses(
            faction,
            army,
            enemyArmy
        );

        // WAVE REWARD
        int baseReward =
            600 + (wave * 250);

        int bankBonus =
            bankTier * 100;

        int totalReward =
            baseReward + bankBonus;

        cash += totalReward;

        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|               WAVE DEFEATED!                |\n";
        cout << "+==============================================+\n";
        cout << "| Wave:          " << left << setw(29)
             << wave << "|\n";
        cout << "| Base reward:   $" << setw(27)
             << baseReward << "|\n";
        cout << "| Bank bonus:    $" << setw(27)
             << bankBonus << "|\n";
        cout << "| Total reward:  $" << setw(27)
             << totalReward << "|\n";
        cout << "| New cash:      $" << setw(27)
             << cash << "|\n";
        cout << "+==============================================+\n";

        wave++;

        cout << "\nThe next enemy wave will be stronger.\n";

        resetColor();

        pauseScreen();

        return true;
    }
    else
    {
        cout << "The enemy overwhelms your forces!\n\n";

        // Heavy losses
        for (int i = 0; i < MAX_UNITS; i++)
        {
            if (army[i] <= 0)
                continue;

            int losses =
                max(1, army[i] / 2);

            losses =
                min(losses, army[i]);

            army[i] -= losses;

            cout << losses << " units lost from your ";

            setFactionColor(faction);

            cout << (faction
                     ? "bug "
                     : "human ");

            resetColor();

            cout << "forces.\n";
        }

        cout << "\nThe wave was NOT defeated.\n";
        cout << "Recruit more units and try again.\n";

        resetColor();

        pauseScreen();

        return false;
    }
}

// ============================================================================
// SABOTAGE / NUKE SYSTEM
// ============================================================================

void sabotageScreen(
    bool faction,
    int& cash,
    int nuclearTier,
    bool& warhead,
    bool& guidance,
    bool& core,
    bool& gameWon)
{
    // TOTAL NUKE COST = $10,000
    const int WARHEAD_COST = 2500;
    const int GUIDANCE_COST = 3000;
    const int CORE_COST = 4500;

    while (true)
    {
        clearScreen();

        setFactionColor(faction);

        string factionName =
            faction ? "BUG HIVE" : "HUMAN MILITARY";

        cout << "+============================================================================+\n";
        cout << "|                            SABOTAGE                                       |\n";
        cout << "+============================================================================+\n";
        cout << "|                                                                            |\n";
        cout << "|                         " << factionName
             << "                                      |\n";
        cout << "|                                                                            |\n";
        cout << "|                       NUCLEAR PROGRAM                                     |\n";
        cout << "|                                                                            |\n";
        cout << "+----------------------------------------------------------------------------+\n";

        cout << "|                                                                            |\n";
        cout << "| Available cash: $" << left << setw(56)
             << cash << "|\n";

        cout << "| Nuclear research: " << nuclearTier << "/3"
             << "                                                       |\n";

        cout << "| Total nuclear weapon cost: $10,000                                        |\n";

        cout << "|                                                                            |\n";
        cout << "+============================================================================+\n";

        // WARHEAD
        cout << "|  [1] NUCLEAR WARHEAD                                                      |\n";
        cout << "|                                                                            |\n";

        if (warhead)
        {
            cout << "|      STATUS: PURCHASED                                                    |\n";
        }
        else
        {
            cout << "|      STATUS: NOT PURCHASED                                                |\n";
            cout << "|      COST:   $2500                                                        |\n";
        }

        cout << "|                                                                            |\n";

        // GUIDANCE
        cout << "|  [2] GUIDANCE SYSTEM                                                      |\n";
        cout << "|                                                                            |\n";

        if (guidance)
        {
            cout << "|      STATUS: PURCHASED                                                    |\n";
        }
        else
        {
            cout << "|      STATUS: NOT PURCHASED                                                |\n";
            cout << "|      COST:   $3000                                                        |\n";
        }

        cout << "|                                                                            |\n";

        // CORE
        cout << "|  [3] NUCLEAR CORE                                                         |\n";
        cout << "|                                                                            |\n";

        if (core)
        {
            cout << "|      STATUS: PURCHASED                                                    |\n";
        }
        else
        {
            cout << "|      STATUS: NOT PURCHASED                                                |\n";
            cout << "|      COST:   $4500                                                        |\n";
        }

        cout << "|                                                                            |\n";
        cout << "+----------------------------------------------------------------------------+\n";

        bool nukeComplete =
            warhead && guidance && core;

        if (nukeComplete)
        {
            cout << "|  [4] *** LAUNCH NUCLEAR WEAPON ***                                       |\n";
            cout << "|                                                                            |\n";
            cout << "|      ALL NUCLEAR COMPONENTS ARE READY.                                    |\n";
        }
        else
        {
            cout << "|  [4] LAUNCH NUCLEAR WEAPON - LOCKED                                      |\n";
            cout << "|                                                                            |\n";
            cout << "|      Purchase all 3 parts first.                                          |\n";
        }

        cout << "|                                                                            |\n";
        cout << "+============================================================================+\n";
        cout << "|  [0] RETURN                                                               |\n";
        cout << "+============================================================================+\n";

        resetColor();

        cout << "\nSelect sabotage option: ";

        int choice = getIntInput();

        if (choice == 0)
            return;

        // WARHEAD
        if (choice == 1)
        {
            if (warhead)
            {
                cout << "\nThe Nuclear Warhead has already been purchased.\n";
                pauseScreen();
                continue;
            }

            if (nuclearTier < 1)
            {
                cout << "\nYou need Nuclear Research Tier 1 first!\n";
                pauseScreen();
                continue;
            }

            if (cash < WARHEAD_COST)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= WARHEAD_COST;
            warhead = true;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|          NUCLEAR WARHEAD ACQUIRED            |\n";
            cout << "+==============================================+\n";
            cout << "| Cost: $2500                                  |\n";
            cout << "+==============================================+\n";

            pauseScreen();
        }

        // GUIDANCE
        else if (choice == 2)
        {
            if (guidance)
            {
                cout << "\nThe Guidance System has already been purchased.\n";
                pauseScreen();
                continue;
            }

            if (nuclearTier < 2)
            {
                cout << "\nYou need Nuclear Research Tier 2 first!\n";
                pauseScreen();
                continue;
            }

            if (!warhead)
            {
                cout << "\nPurchase the Nuclear Warhead first!\n";
                pauseScreen();
                continue;
            }

            if (cash < GUIDANCE_COST)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= GUIDANCE_COST;
            guidance = true;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|          GUIDANCE SYSTEM ACQUIRED            |\n";
            cout << "+==============================================+\n";
            cout << "| Cost: $3000                                  |\n";
            cout << "+==============================================+\n";

            pauseScreen();
        }

        // CORE
        else if (choice == 3)
        {
            if (core)
            {
                cout << "\nThe Nuclear Core has already been purchased.\n";
                pauseScreen();
                continue;
            }

            if (nuclearTier < 3)
            {
                cout << "\nYou need Nuclear Research Tier 3 first!\n";
                pauseScreen();
                continue;
            }

            if (!warhead || !guidance)
            {
                cout << "\nPurchase the Warhead and Guidance System first!\n";
                pauseScreen();
                continue;
            }

            if (cash < CORE_COST)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= CORE_COST;
            core = true;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|             NUCLEAR CORE ACQUIRED            |\n";
            cout << "+==============================================+\n";
            cout << "| Cost: $4500                                  |\n";
            cout << "|                                              |\n";
            cout << "| ALL NUCLEAR COMPONENTS ARE READY.            |\n";
            cout << "| THE WEAPON CAN NOW BE LAUNCHED.              |\n";
            cout << "+==============================================+\n";

            pauseScreen();
        }

        // LAUNCH
        else if (choice == 4)
        {
            if (!warhead || !guidance || !core)
            {
                cout << "\n";
                cout << "+==============================================+\n";
                cout << "|             LAUNCH SYSTEM LOCKED             |\n";
                cout << "+==============================================+\n";
                cout << "| You must purchase all three nuclear parts.  |\n";
                cout << "+==============================================+\n";

                pauseScreen();
                continue;
            }

            clearScreen();

            setFactionColor(faction);

            cout << "\n";
            cout << "==============================================================\n";
            cout << "                     *** WARNING ***\n";
            cout << "==============================================================\n\n";

            cout << "NUCLEAR WEAPON ARMED.\n\n";

            cout << "WARHEAD........................... ONLINE\n";
            cout << "GUIDANCE SYSTEM................... ONLINE\n";
            cout << "NUCLEAR CORE...................... ONLINE\n\n";

            cout << "TARGET LOCKED.\n";
            cout << "LAUNCH SEQUENCE INITIATED.\n\n";

            cout << "3...\n";
            this_thread::sleep_for(chrono::milliseconds(700));

            cout << "2...\n";
            this_thread::sleep_for(chrono::milliseconds(700));

            cout << "1...\n";
            this_thread::sleep_for(chrono::milliseconds(700));

            cout << "\n";
            cout << "                  *** LAUNCH ***\n\n";

            this_thread::sleep_for(chrono::milliseconds(1000));

            cout << "                  *** IMPACT ***\n\n";

            this_thread::sleep_for(chrono::milliseconds(1200));

            cout << "==============================================================\n";
            cout << "                         YOU WIN!\n";
            cout << "==============================================================\n\n";

            cout << "The enemy has been completely destroyed.\n";
            cout << "Your faction controls the battlefield.\n\n";

            gameWon = true;

            resetColor();

            pauseScreen();

            return;
        }

        else
        {
            cout << "\nInvalid selection!\n";
            pauseScreen();
        }
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main()
{
    resetColor();

    cout << "================================================================================================\n";

    printTitle();

    cout << "                                       -_A Game by:K1X28_-\n";

    cout << "================================================================================================\n";

    pauseScreen();
    clearScreen();

    // ========================================================================
    // FACTION SELECTION
    // ========================================================================

    printFacs();

    cout << "Choose a faction: Humans(0) or Bugs(1): ";

    int factionChoice = getIntInput();

    while (factionChoice != HUMAN &&
           factionChoice != BUGS)
    {
        cout << "Please enter 0 for Humans or 1 for Bugs: ";
        factionChoice = getIntInput();
    }

    bool faction =
        (factionChoice == BUGS);

    // ========================================================================
    // SET FACTION COLOUR
    // ========================================================================

    setFactionColor(faction);

    // ========================================================================
    // PLAYER VARIABLES
    // ========================================================================

    int cash = 1000;

    int bankTier = 0;
    int trainingTier = 0;
    int nuclearTier = 0;
    int weaponTier = 0;

    // ========================================================================
    // ARMY
    // ========================================================================

    // EXACTLY 5 UNIT SLOTS
    int army[MAX_UNITS] =
    {
        0,
        0,
        0,
        0,
        0
    };

    // ========================================================================
    // WAVE
    // ========================================================================

    int wave = 1;

    // ========================================================================
    // NUKE PARTS
    // ========================================================================

    bool warhead = false;
    bool guidance = false;
    bool core = false;

    // ========================================================================
    // GAME STATE
    // ========================================================================

    bool gameWon = false;
    bool gameLost = false;

    // ========================================================================
    // MAIN GAME LOOP
    // ========================================================================

    while (!gameWon && !gameLost)
    {
        // DEFEAT CHECK

        if (!hasAnyUnits(army) &&
            !canAffordAnyUnit(
                faction,
                cash,
                trainingTier,
                weaponTier,
                army))
        {
            gameLost = true;
            break;
        }

        clearScreen();

        factionScreen(
            faction,
            cash,
            wave
        );

        cout << "\nSelect action: ";

        int action =
            getIntInput();

        switch (action)
        {
            // RECRUIT
            case 1:
            {
                recruitScreen(
                    faction,
                    cash,
                    trainingTier,
                    weaponTier,
                    army
                );

                break;
            }

            // UPGRADE
            case 2:
            {
                upgradeScreen(
                    faction,
                    cash,
                    bankTier,
                    trainingTier,
                    nuclearTier,
                    weaponTier
                );

                break;
            }

            // ATTACK
            case 3:
            {
                attackScreen(
                    faction,
                    cash,
                    bankTier,
                    wave,
                    army
                );

                break;
            }

            // SABOTAGE
            case 4:
            {
                sabotageScreen(
                    faction,
                    cash,
                    nuclearTier,
                    warhead,
                    guidance,
                    core,
                    gameWon
                );

                break;
            }

            // INVALID
            default:
            {
                cout << "\nInvalid action!\n";
                pauseScreen();

                break;
            }
        }
    }

    // ========================================================================
    // GAME OVER / DEFEAT
    // ========================================================================

    if (gameLost)
    {
        loseScreen(faction);
    }

    // ========================================================================
    // VICTORY
    // ========================================================================

    if (gameWon)
    {
        clearScreen();

        setFactionColor(faction);

        cout << "\n";
        cout << "============================================================================\n";
        cout << "                        VICTORY, GAME COMPLETE!\n";
        cout << "============================================================================\n\n";

        cout << "Congratulations, Commander!\n";
        cout << "Your faction has won the war.\n\n";

        cout << "============================================================================\n";
        cout << "                              YOU WIN!\n";
        cout << "============================================================================\n\n";

        resetColor();

        pauseScreen();

        clearScreen();

        resetColor();

        cout << "Thanks for playing my game - K1X28";

        cout << "\n";
    }

    resetColor();

    return 0;
}
