#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>

using namespace std;

// ============================================================================
// LINUX / ANSI CONSOLE COLORS
// ============================================================================

const string RESET_COLOR = "\033[0m";

const string HUMAN_COLOR  = "\033[92m"; // Bright Green
const string BUG_COLOR    = "\033[91m"; // Bright Red
const string ROBOT_COLOR  = "\033[96m"; // Bright Cyan
const string MUTANT_COLOR = "\033[95m"; // Bright Magenta
const string REBEL_COLOR  = "\033[93m"; // Bright Yellow

void setColor(const string& color)
{
    cout << color;
}

void resetColor()
{
    cout << RESET_COLOR;
}

void setFactionColor(bool faction)
{
    if (faction == BUGS)
        setColor(BUG_COLOR);
    else
        setColor(HUMAN_COLOR);
}

void setScenarioColor(int scenario)
{
    if (scenario == SCENARIO_ROBOTS)
        setColor(ROBOT_COLOR);
    else if (scenario == SCENARIO_MUTANTS)
        setColor(MUTANT_COLOR);
    else if (scenario == SCENARIO_REBELS)
        setColor(REBEL_COLOR);
    else
        resetColor();
}

// ============================================================================
// CONSTANTS
// ============================================================================

const int MAX_UNITS = 5;

const int HUMAN = 0;
const int BUGS = 1;

// ============================================================================
// SCENARIOS
// ============================================================================

const int SCENARIO_STANDARD = 0;
const int SCENARIO_ROBOTS = 1;
const int SCENARIO_MUTANTS = 2;
const int SCENARIO_REBELS = 3;

const int MAX_SCENARIOS = 4;

// ============================================================================
// UPGRADE LIMITS
// ============================================================================

const int MAX_BANK = 5;
const int MAX_TRAINING = 5;
const int MAX_NUCLEAR = 3;
const int MAX_WEAPON = 4;

const int MAX_ATTACK_TRAINING = 5;
const int MAX_MOBILITY = 1;

// ============================================================================
// UPGRADE COSTS
// ============================================================================

const int BANK_COSTS[5] =
{
    250,
    500,
    750,
    1000,
    1500
};

const int TRAINING_COSTS[5] =
{
    200,
    400,
    600,
    800,
    1000
};

const int NUCLEAR_COSTS[3] =
{
    500,
    1000,
    2000
};

const int WEAPON_COSTS[4] =
{
    300,
    750,
    1500,
    2500
};

const int ATTACK_TRAINING_COSTS[5] =
{
    300,
    600,
    900,
    1200,
    1500
};

const int MOBILITY_COST = 700;

// ============================================================================
// THERMAL MISSILE
// ============================================================================

const int THERMAL_MISSILE_COST = 1800;
const int THERMAL_DAMAGE_PERCENT = 20;

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
// SCENARIO STRUCT
// ============================================================================

struct Scenario
{
    string name;
    string description;

    int startingCash;

    int bankTier;
    int trainingTier;
    int nuclearTier;
    int weaponTier;
    int attackTrainingTier;
    int mobilityTier;

    int startingArmy[5];
};

// ============================================================================
// TERMINAL FUNCTIONS
// ============================================================================

void clearScreen()
{
    // ANSI escape sequence:
    // 2J = clear screen
    // H  = move cursor to top-left
    cout << "\033[2J\033[H";
}

void sleepMs(int milliseconds)
{
    this_thread::sleep_for(
        chrono::milliseconds(milliseconds)
    );
}

void pauseScreen()
{
    cout << "\nPress ENTER to continue...";

    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );

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

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

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

    cout << u8R"( ░██       ░██                             ░██████████                                  ░██
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

void factionScreen(
    bool faction,
    int cash,
    int wave,
    int scenario,
    int attackTrainingTier,
    int mobilityTier)
{
    resetColor();

    string title = faction
        ? "BUG HIVE"
        : "HUMAN MILITARY BASE";

    cout << "\n";

    cout << "+============================================================================+\n";
    cout << "|                                                                            |\n";
    cout << "|                     ";

    setFactionColor(faction);

    cout << title;

    resetColor();

    int padding =
        72 - static_cast<int>(title.length());

    cout << string(max(0, padding / 2), ' ');

    cout << "|\n";
    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    setFactionColor(faction);

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
|           '.         .'       '-.   -'       '.         .'                 |
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

    resetColor();

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
|    +----------------+                                                      |
|    | [5] GIVE UP    |                                                      |
|    |                |                                                      |
|    | End campaign   |                                                      |
|    +----------------+                                                      |
|                                                                            |
+============================================================================+
|                              SELECT ACTION                                 |
+============================================================================+
)";

    cout << "|                         CASH: $" << left << setw(8)
         << cash
         << "                                  |\n";

    cout << "|                         WAVE: " << left << setw(9)
         << wave
         << "                                  |\n";

    cout << "|                         SCENARIO: ";

    setScenarioColor(scenario);

    cout << left
         << setw(49)
         << (
            scenario == SCENARIO_STANDARD ? "STANDARD WAR" :
            scenario == SCENARIO_ROBOTS ? "ROBOT UPRISING" :
            scenario == SCENARIO_MUTANTS ? "MUTANT OUTBREAK" :
            "REBEL WAR"
         );

    resetColor();

    cout << "|\n";

    cout << "|                         ATTACK BONUS: +"
         << left
         << setw(42)
         << attackTrainingTier * 10
         << "% |\n";

    cout << "|                         MOBILITY: "
         << left
         << setw(48)
         << (
            mobilityTier
            ? "+1 Speed for Tier 34 units"
            : "Not researched"
         )
         << "|\n";

    cout << "+============================================================================+\n";

    resetColor();
}

// ============================================================================
// GET BASE UNITS
// ============================================================================

void getBaseUnits(
    bool faction,
    Unit units[])
{
    if (!faction)
    {
        units[0] = {"Soldier", 100, 100, 25, 15, 10};
        units[1] = {"Heavy Soldier", 250, 220, 45, 35, 6};
        units[2] = {"Tank", 500, 500, 80, 55, 5};
        units[3] = {"Destroyer Tank", 900, 850, 130, 75, 3};
        units[4] = {"Commander", 1800, 1100, 170, 90, 8};
    }
    else
    {
        units[0] = {"Buggo", 60, 80, 20, 10, 12};
        units[1] = {"Buggo Elite", 160, 170, 38, 25, 9};
        units[2] = {"Hive Titan", 350, 400, 70, 45, 4};
        units[3] = {"Hive King", 650, 700, 110, 60, 3};
        units[4] = {"Hive Queen", 1800, 1200, 180, 100, 7};
    }
}

// ============================================================================
// APPLY PLAYER UNIT UPGRADES
// ============================================================================

void applyUnitUpgrades(
    Unit units[],
    int attackTrainingTier,
    int mobilityTier)
{
    double attackMultiplier =
        1.0 + attackTrainingTier * 0.10;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        units[i].attack =
            static_cast<int>(
                units[i].attack *
                attackMultiplier
            );

        if (mobilityTier > 0 && i >= 2)
            units[i].speed += 1;
    }
}

// ============================================================================
// SCENARIO DATA
// ============================================================================

void getScenario(
    int scenario,
    bool faction,
    Scenario& data)
{
    for (int i = 0; i < MAX_UNITS; i++)
        data.startingArmy[i] = 0;

    data.name = "STANDARD WAR";
    data.description =
        "A conventional war between Humans and Bugs.";

    data.startingCash = 1000;

    data.bankTier = 0;
    data.trainingTier = 0;
    data.nuclearTier = 0;
    data.weaponTier = 0;
    data.attackTrainingTier = 0;
    data.mobilityTier = 0;

    if (scenario == SCENARIO_STANDARD)
    {
        data.name = "STANDARD WAR";
        data.description =
            "A conventional war between Humans and Bugs.";
        data.startingCash = 1000;
    }
    else if (scenario == SCENARIO_ROBOTS)
    {
        data.name = "ROBOT UPRISING";
        data.description =
            "Autonomous war machines have declared war on everyone.";

        data.startingCash = 1800;

        data.bankTier = 1;
        data.trainingTier = 1;
        data.weaponTier = 1;

        data.startingArmy[0] = 3;
        data.startingArmy[1] = 2;
    }
    else if (scenario == SCENARIO_MUTANTS)
    {
        data.name = "MUTANT OUTBREAK";
        data.description =
            "A biological disaster has created a mutant army.";

        data.startingCash = 1500;

        data.trainingTier = 2;
        data.nuclearTier = 1;
        data.weaponTier = 1;

        data.startingArmy[0] = 5;
        data.startingArmy[1] = 1;
    }
    else if (scenario == SCENARIO_REBELS)
    {
        data.name =
            faction ? "BUG REBELLION" : "HUMAN REBELLION";

        data.description =
            faction
            ? "A rebel Bug faction has broken away from the Hive."
            : "Human rebels have turned against the military.";

        data.startingCash = 2200;

        data.bankTier = 1;
        data.weaponTier = 2;

        data.startingArmy[0] = 4;
        data.startingArmy[1] = 2;
        data.startingArmy[2] = 1;
    }
}

// ============================================================================
// SCENARIO SELECTION
// ============================================================================

int chooseScenario(bool faction)
{
    clearScreen();

    resetColor();

    cout << "\n";
    cout << "+============================================================================+\n";
    cout << "|                           CHOOSE SCENARIO                                 |\n";
    cout << "+============================================================================+\n\n";

    setScenarioColor(SCENARIO_STANDARD);

    cout << " [1] STANDARD WAR\n";

    resetColor();

    cout << "     Conventional Human vs Bug warfare.\n";
    cout << "     Starting cash: $1000\n";
    cout << "     No starting upgrades.\n\n";

    setScenarioColor(SCENARIO_ROBOTS);

    cout << " [2] ROBOT UPRISING\n";

    resetColor();

    cout << "     Autonomous robots attack everyone.\n";
    cout << "     Starting cash: $1800\n";
    cout << "     Bank Tier 1 + Training Tier 1\n";
    cout << "     3 basic units + 2 heavy units\n\n";

    setScenarioColor(SCENARIO_MUTANTS);

    cout << " [3] MUTANT OUTBREAK\n";

    resetColor();

    cout << "     Mutants have overrun the battlefield.\n";
    cout << "     Starting cash: $1500\n";
    cout << "     Training Tier 2 + Nuclear Tier 1\n";
    cout << "     5 basic units + 1 heavy unit\n\n";

    setScenarioColor(SCENARIO_REBELS);

    cout << " [4] ";

    if (faction == HUMAN)
        cout << "HUMAN REBELLION\n";
    else
        cout << "BUG REBELLION\n";

    resetColor();

    cout << "     A rebel faction has risen against you.\n";
    cout << "     Starting cash: $2200\n";
    cout << "     Bank Tier 1 + Weapon Tier 2\n";
    cout << "     4 basic + 2 heavy + 1 vehicle\n\n";

    cout << "+============================================================================+\n";

    cout << "\nSelect scenario: ";

    int choice = getIntInput();

    while (choice < 1 || choice > 4)
    {
        cout << "Please select a scenario from 1-4: ";
        choice = getIntInput();
    }

    return choice - 1;
}

// ============================================================================
// ARMY DISPLAY
// ============================================================================

void displayArmy(
    bool faction,
    int army[],
    int trainingTier,
    int weaponTier,
    int attackTrainingTier,
    int mobilityTier)
{
    Unit units[MAX_UNITS];

    getBaseUnits(faction, units);

    applyUnitUpgrades(
        units,
        attackTrainingTier,
        mobilityTier
    );

    cout << "\n";
    cout << "+============================================================================+\n";
    cout << "|                              YOUR ARMY                                     |\n";
    cout << "+============================================================================+\n";

    bool hasUnits = false;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        hasUnits = true;

        cout << "|  "
             << left
             << setw(20)
             << units[i].name;

        setFactionColor(faction);

        cout << " x"
             << setw(6)
             << army[i];

        resetColor();

        cout << " HP:"
             << setw(5)
             << units[i].health

             << " ATK:"
             << setw(5)
             << units[i].attack

             << " DEF:"
             << setw(5)
             << units[i].defense

             << " SPD:"
             << setw(4)
             << units[i].speed

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

    int maxUnits = weaponTier + 1;

    if (maxUnits > MAX_UNITS)
        maxUnits = MAX_UNITS;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        units[i].cost =
            units[i].cost *
            (100 - trainingTier * 10) /
            100;
    }

    string title =
        faction
        ? "BUG HIVE - RECRUITMENT"
        : "HUMAN MILITARY - RECRUITMENT";

    clearScreen();

    resetColor();

    cout << "\n";

    cout << "+============================================================================+\n";
    cout << "|                                                                            |\n";
    cout << "|                      ";

    setFactionColor(faction);

    cout << title;

    resetColor();

    int padding =
        72 - static_cast<int>(title.length());

    cout << string(
        max(0, padding / 2),
        ' '
    );

    cout << "|\n";
    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    setFactionColor(faction);

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

    resetColor();

    cout << "+----------------------------------------------------------------------------+\n";
    cout << "|                         AVAILABLE CASH: $"
         << setw(8)
         << left
         << cash
         << "                      |\n";

    cout << "|                         TRAINING DISCOUNT: "
         << trainingTier * 10
         << "%                           |\n";

    cout << "+============================================================================+\n";
    cout << "|                            AVAILABLE UNITS                                |\n";
    cout << "+============================================================================+\n";

    cout << "|  #   UNIT                OWNED      COST      HP    ATK   DEF   SPD       |\n";
    cout << "|  ------------------------------------------------------------------------  |\n";

    for (int i = 0; i < MAX_UNITS; i++)
    {
        cout << "|  ["
             << i + 1
             << "] ";

        if (i >= maxUnits)
        {
            cout << left
                 << setw(20) << "LOCKED"
                 << setw(10) << "-"
                 << setw(10) << "-"
                 << setw(6) << "-"
                 << setw(6) << "-"
                 << setw(6) << "-"
                 << setw(8) << "-"
                 << "|\n";

            continue;
        }

        cout << left
             << setw(20)
             << units[i].name;

        setFactionColor(faction);

        cout << setw(10)
             << army[i];

        resetColor();

        cout << "$"
             << right
             << setw(7)
             << units[i].cost

             << setw(7)
             << units[i].health

             << setw(6)
             << units[i].attack

             << setw(6)
             << units[i].defense

             << setw(6)
             << units[i].speed

             << "   |\n";
    }

    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    int totalUnits = 0;

    for (int i = 0; i < MAX_UNITS; i++)
        totalUnits += army[i];

    cout << "|  Total units in army: "
         << left
         << setw(51)
         << totalUnits
         << "|\n";

    cout << "+----------------------------------------------------------------------------+\n";
    cout << "|  [0] RETURN                                                               |\n";
    cout << "+============================================================================+\n";

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

        cout << "| Upgrade Advanced Weaponry to unlock it.     |\n";

        if (choice == 5)
        {
            if (!faction)
                cout << "| This unlocks your Commander.                 |\n";
            else
                cout << "| This unlocks your Hive Queen.                |\n";
        }

        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    Unit selected = units[choice - 1];

    if (choice == 5 && army[4] >= 1)
    {
        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|              UNIT LIMIT REACHED             |\n";
        cout << "+==============================================+\n";

        cout << "| Your army can only have ONE "
             << left
             << setw(17)
             << selected.name
             << "|\n";

        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    cout << "\nHow many "
         << selected.name
         << "(s) do you want to recruit? ";

    int quantity = getIntInput();

    if (quantity <= 0)
    {
        cout << "\nInvalid quantity!\n";
        pauseScreen();
        return;
    }

    if (choice == 5 && quantity > 1)
    {
        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|              ONE-PER-ARMY LIMIT             |\n";
        cout << "+==============================================+\n";

        cout << "| You can only recruit ONE "
             << left
             << setw(17)
             << selected.name
             << "|\n";

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
        cout << "|              INSUFFICIENT FUNDS             |\n";
        cout << "+==============================================+\n";

        cout << "| Unit:       "
             << left
             << setw(31)
             << selected.name
             << "|\n";

        cout << "| Quantity:   "
             << setw(31)
             << quantity
             << "|\n";

        cout << "| Unit cost:  $"
             << setw(30)
             << selected.cost
             << "|\n";

        cout << "| Total cost: $"
             << setw(30)
             << totalCost
             << "|\n";

        cout << "| Your cash:  $"
             << setw(30)
             << cash
             << "|\n";

        cout << "+==============================================+\n";

        pauseScreen();
        return;
    }

    cash -= static_cast<int>(totalCost);

    army[choice - 1] += quantity;

    cout << "\n";
    cout << "+==============================================+\n";
    cout << "|               UNITS RECRUITED!              |\n";
    cout << "+==============================================+\n";

    cout << "| Unit:       "
         << left
         << setw(31)
         << selected.name
         << "|\n";

    cout << "| Quantity:   "
         << setw(31)
         << quantity
         << "|\n";

    cout << "| Total cost: $"
         << setw(30)
         << totalCost
         << "|\n";

    cout << "| Remaining:  $"
         << setw(30)
         << cash
         << "|\n";

    cout << "| Army count: "
         << setw(31)
         << army[choice - 1]
         << "|\n";

    cout << "+==============================================+\n";

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
    int& weaponTier,
    int& attackTrainingTier,
    int& mobilityTier)
{
    while (true)
    {
        clearScreen();

        setFactionColor(faction);

        string title =
            faction
            ? "BUG HIVE - UPGRADES"
            : "HUMAN MILITARY - UPGRADES";

        cout << "\n";

        cout << "+============================================================================+\n";
        cout << "|                         " << title
             << " |\n";
        cout << "+============================================================================+\n";

        cout << "|                                                                            |\n";
        cout << "|                         AVAILABLE CASH: $"
             << setw(8)
             << left
             << cash
             << "                      |\n";

        cout << "+----------------------------------------------------------------------------+\n";

        cout << "|  [1] BANK                                                                 |\n";
        cout << "|      Tier: "
             << bankTier
             << "/5                                                             |\n";

        cout << "|      Effect: ";

        if (bankTier == 0)
            cout << "No bonus to wave income";
        else
            cout << "+"
                 << bankTier * 100
                 << " cash after successful waves";

        cout << "\n";

        if (bankTier < MAX_BANK)
            cout << "|      Next upgrade cost: $"
                 << BANK_COSTS[bankTier]
                 << "\n";
        else
            cout << "|      MAXIMUM TIER REACHED\n";

        cout << "|\n";

        cout << "|  [2] BETTER TRAINING EQUIPMENT                                            |\n";
        cout << "|      Tier: "
             << trainingTier
             << "/5                                                             |\n";

        cout << "|      Effect: "
             << trainingTier * 10
             << "% cheaper recruitment\n";

        if (trainingTier < MAX_TRAINING)
            cout << "|      Next upgrade cost: $"
                 << TRAINING_COSTS[trainingTier]
                 << "\n";
        else
            cout << "|      MAXIMUM TIER REACHED\n";

        cout << "|\n";

        cout << "|  [3] NUCLEAR OPTION RESEARCH                                              |\n";
        cout << "|      Tier: "
             << nuclearTier
             << "/3                                                             |\n";

        cout << "|      Status: ";

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
                 << NUCLEAR_COSTS[nuclearTier]
                 << "\n";
        else
            cout << "|      NUCLEAR RESEARCH COMPLETE\n";

        cout << "|\n";

        cout << "|  [4] ADVANCED WEAPONRY                                                    |\n";
        cout << "|      Tier: "
             << weaponTier
             << "/4                                                             |\n";

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
            cout << "ALL UNIT TYPES + ULTIMATE UNIT";

        cout << "\n";

        if (weaponTier < MAX_WEAPON)
            cout << "|      Next upgrade cost: $"
                 << WEAPON_COSTS[weaponTier]
                 << "\n";
        else
            cout << "|      ALL ADVANCED UNITS UNLOCKED\n";

        cout << "|\n";

        cout << "|  [5] ATTACK TRAINING                                                      |\n";
        cout << "|      Tier: "
             << attackTrainingTier
             << "/5                                                             |\n";

        cout << "|      Effect: +"
             << attackTrainingTier * 10
             << "% attack                                                        |\n";

        if (attackTrainingTier < MAX_ATTACK_TRAINING)
        {
            cout << "|      Next upgrade cost: $"
                 << ATTACK_TRAINING_COSTS[attackTrainingTier]
                 << "\n";
        }
        else
        {
            cout << "|      MAXIMUM +50% ATTACK REACHED\n";
        }

        cout << "|\n";

        cout << "|  [6] ADVANCED MOBILITY                                                    |\n";
        cout << "|      Tier: "
             << mobilityTier
             << "/1                                                             |\n";

        cout << "|      Effect: +1 speed to Tier 3 and Tier 4 units                         |\n";

        if (mobilityTier == 0)
            cout << "|      Upgrade cost: $"
                 << MOBILITY_COST
                 << "\n";
        else
            cout << "|      MAXIMUM TIER REACHED\n";

        cout << "|\n";
        cout << "+============================================================================+\n";
        cout << "|  [0] RETURN                                                               |\n";
        cout << "+============================================================================+\n";

        resetColor();

        cout << "\nSelect an upgrade: ";

        int choice = getIntInput();

        if (choice == 0)
            return;

        if (choice < 1 || choice > 6)
        {
            cout << "\nInvalid selection!\n";
            pauseScreen();
            continue;
        }

        if (choice == 1)
        {
            if (bankTier >= MAX_BANK)
            {
                cout << "\nBank is already fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost = BANK_COSTS[bankTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            bankTier++;

            cout << "\nBank upgraded to Tier "
                 << bankTier
                 << "!\n";

            pauseScreen();
        }

        else if (choice == 2)
        {
            if (trainingTier >= MAX_TRAINING)
            {
                cout << "\nTraining equipment is fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost = TRAINING_COSTS[trainingTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            trainingTier++;

            cout << "\nTraining equipment upgraded to Tier "
                 << trainingTier
                 << "!\n";

            cout << "Recruitment is now "
                 << trainingTier * 10
                 << "% cheaper.\n";

            pauseScreen();
        }

        else if (choice == 3)
        {
            if (nuclearTier >= MAX_NUCLEAR)
            {
                cout << "\nNuclear research is already complete!\n";
                pauseScreen();
                continue;
            }

            int cost = NUCLEAR_COSTS[nuclearTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            nuclearTier++;

            cout << "\nNuclear research advanced to Tier "
                 << nuclearTier
                 << "/3.\n";

            if (nuclearTier == 3)
                cout << "Visit SABOTAGE to purchase the final components.\n";

            pauseScreen();
        }

        else if (choice == 4)
        {
            if (weaponTier >= MAX_WEAPON)
            {
                cout << "\nAdvanced Weaponry is fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost = WEAPON_COSTS[weaponTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            weaponTier++;

            cout << "\nAdvanced Weaponry upgraded to Tier "
                 << weaponTier
                 << "/4!\n";

            if (weaponTier == 1)
                cout << "Heavy infantry unlocked!\n";
            else if (weaponTier == 2)
                cout << "Heavy vehicles unlocked!\n";
            else if (weaponTier == 3)
                cout << "Advanced vehicles unlocked!\n";
            else if (weaponTier == 4)
            {
                if (!faction)
                    cout << "COMMANDER UNLOCKED!\n";
                else
                    cout << "HIVE QUEEN UNLOCKED!\n";
            }

            pauseScreen();
        }

        else if (choice == 5)
        {
            if (attackTrainingTier >= MAX_ATTACK_TRAINING)
            {
                cout << "\nAttack Training is fully upgraded!\n";
                pauseScreen();
                continue;
            }

            int cost =
                ATTACK_TRAINING_COSTS[attackTrainingTier];

            if (cash < cost)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= cost;
            attackTrainingTier++;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|          ATTACK TRAINING UPGRADED            |\n";
            cout << "+==============================================+\n";

            cout << "| Tier: "
                 << attackTrainingTier
                 << "/5                                      |\n";

            cout << "| Attack bonus: +"
                 << attackTrainingTier * 10
                 << "%                                    |\n";

            cout << "+==============================================+\n";

            pauseScreen();
        }

        else if (choice == 6)
        {
            if (mobilityTier >= MAX_MOBILITY)
            {
                cout << "\nAdvanced Mobility is fully upgraded!\n";
                pauseScreen();
                continue;
            }

            if (cash < MOBILITY_COST)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= MOBILITY_COST;
            mobilityTier++;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|            ADVANCED MOBILITY                |\n";
            cout << "+==============================================+\n";
            cout << "| Tier 3 and Tier 4 units gain +1 speed.     |\n";
            cout << "+==============================================+\n";

            pauseScreen();
        }
    }
}

// ============================================================================
// ENEMY UNITS
// ============================================================================

void getEnemyUnits(
    bool faction,
    int scenario,
    Unit enemyUnits[])
{
    if (scenario == SCENARIO_ROBOTS)
    {
        enemyUnits[0] = {"Scout Robot", 80, 100, 28, 17, 14};
        enemyUnits[1] = {"Combat Robot", 200, 240, 45, 36, 9};
        enemyUnits[2] = {"Robot Tank", 450, 550, 70, 45, 6};
        enemyUnits[3] = {"Destroyer Robot", 850, 900, 130, 80, 5};
        enemyUnits[4] = {"ROBOT OVERLORD", 2200, 1200, 200, 100, 8};
        return;
    }

    if (scenario == SCENARIO_MUTANTS)
    {
        enemyUnits[0] = {"Mutant", 70, 120, 26, 8, 11};
        enemyUnits[1] = {"Mutant Brute", 180, 270, 40, 20, 6};
        enemyUnits[2] = {"Mutant Beast", 400, 550, 70, 30, 7};
        enemyUnits[3] = {"Mutant Abomination", 800, 950, 130, 50, 4};
        enemyUnits[4] = {"MUTANT ALPHA", 2000, 1500, 190, 70, 6};
        return;
    }

    if (scenario == SCENARIO_REBELS)
    {
        if (!faction)
        {
            enemyUnits[0] = {"Rebel Soldier", 90, 110, 27, 12, 11};
            enemyUnits[1] = {"Rebel Heavy", 230, 230, 45, 38, 7};
            enemyUnits[2] = {"Rebel Tank", 450, 520, 76, 45, 5};
            enemyUnits[3] = {"Rebel Destroyer", 850, 850, 110, 65, 4};
            enemyUnits[4] = {"REBEL COMMANDER", 1900, 1200, 160, 80, 8};
        }
        else
        {
            enemyUnits[0] = {"Rebel Buggo", 65, 85, 21, 7, 13};
            enemyUnits[1] = {"Rebel Elite Bug", 170, 160, 38, 21, 10};
            enemyUnits[2] = {"Rebel Hive Beast", 380, 420, 70, 35, 5};
            enemyUnits[3] = {"Rebel Hive King", 700, 730, 105, 52, 4};
            enemyUnits[4] = {"REBEL QUEEN", 1900, 1100, 175, 90, 8};
        }

        return;
    }

    if (!faction)
    {
        enemyUnits[0] = {"Buggo", 60, 80, 20, 10, 12};
        enemyUnits[1] = {"Buggo Elite", 160, 170, 38, 25, 9};
        enemyUnits[2] = {"Hive Titan", 350, 400, 70, 45, 4};
        enemyUnits[3] = {"Hive King", 650, 700, 110, 60, 3};
        enemyUnits[4] = {"Hive Queen", 1800, 1200, 180, 100, 7};
    }
    else
    {
        enemyUnits[0] = {"Soldier", 100, 100, 25, 15, 10};
        enemyUnits[1] = {"Heavy Soldier", 250, 220, 45, 35, 6};
        enemyUnits[2] = {"Tank", 500, 500, 80, 55, 5};
        enemyUnits[3] = {"Destroyer Tank", 900, 850, 130, 75, 3};
        enemyUnits[4] = {"Commander", 1800, 1100, 170, 90, 8};
    }
}

// ============================================================================
// CREATE ENEMY WAVE
// ============================================================================

void createEnemyWave(
    bool faction,
    int scenario,
    int wave,
    int enemyArmy[])
{
    for (int i = 0; i < MAX_UNITS; i++)
        enemyArmy[i] = 0;

    enemyArmy[0] = 4 + wave;

    if (wave >= 3)
        enemyArmy[1] = 1 + (wave - 3) / 2;

    if (wave >= 6)
        enemyArmy[2] = 1 + (wave - 6) / 3;

    if (wave >= 10)
        enemyArmy[3] = 1 + (wave - 10) / 4;

    if (wave >= 15)
        enemyArmy[4] = 1;

    if (scenario == SCENARIO_ROBOTS)
    {
        enemyArmy[0] += 2;

        if (wave >= 5)
            enemyArmy[1]++;

        if (wave >= 9)
            enemyArmy[2]++;

        if (wave >= 13)
            enemyArmy[3]++;

        if (wave >= 19)
            enemyArmy[4]++;
    }
    else if (scenario == SCENARIO_MUTANTS)
    {
        enemyArmy[0] = 3 + wave / 2;

        if (wave >= 3)
            enemyArmy[1] += 2;

        if (wave >= 6)
            enemyArmy[2]++;

        if (wave >= 11)
            enemyArmy[3]++;

        if (wave >= 15)
            enemyArmy[4] = 1;
    }
    else if (scenario == SCENARIO_REBELS)
    {
        enemyArmy[0] += 1;

        if (wave >= 3)
            enemyArmy[1]++;

        if (wave >= 5)
            enemyArmy[2]++;

        if (wave >= 10)
            enemyArmy[3]++;

        if (wave >= 16)
            enemyArmy[4] = 1;
    }
}

// ============================================================================
// CREATE ENEMY HEALTH
// ============================================================================

void createEnemyHealth(
    bool faction,
    int scenario,
    int enemyArmy[],
    int enemyHealth[])
{
    Unit enemyUnits[MAX_UNITS];

    getEnemyUnits(
        faction,
        scenario,
        enemyUnits
    );

    for (int i = 0; i < MAX_UNITS; i++)
    {
        enemyHealth[i] =
            enemyArmy[i] * enemyUnits[i].health;
    }
}

// ============================================================================
// DISPLAY ENEMY WAVE
// ============================================================================

void displayEnemyWave(
    bool faction,
    int scenario,
    int wave,
    int enemyArmy[],
    int enemyHealth[])
{
    Unit enemyUnits[MAX_UNITS];

    getEnemyUnits(
        faction,
        scenario,
        enemyUnits
    );

    cout << "\n";
    cout << "+============================================================================+\n";

    setScenarioColor(scenario);

    cout << "|                         ENEMY WAVE "
         << left
         << setw(4)
         << wave
         << "                                ";

    resetColor();

    cout << "|\n";

    cout << "+============================================================================+\n";

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (enemyArmy[i] <= 0)
            continue;

        cout << "|  "
             << left
             << setw(22)
             << enemyUnits[i].name;

        setScenarioColor(scenario);

        cout << " x"
             << setw(5)
             << enemyArmy[i];

        resetColor();

        cout << " HP:"
             << setw(7)
             << enemyHealth[i]

             << " ATK:"
             << setw(5)
             << enemyUnits[i].attack

             << " DEF:"
             << setw(5)
             << enemyUnits[i].defense

             << " SPD:"
             << setw(4)
             << enemyUnits[i].speed

             << " |\n";
    }

    cout << "+============================================================================+\n";
}

// ============================================================================
// CALCULATE PLAYER ARMY POWER
// ============================================================================

long long calculateArmyPower(
    bool faction,
    int army[],
    int attackTrainingTier,
    int mobilityTier)
{
    Unit units[MAX_UNITS];

    getBaseUnits(
        faction,
        units
    );

    applyUnitUpgrades(
        units,
        attackTrainingTier,
        mobilityTier
    );

    long long power = 0;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        long long unitPower =
            static_cast<long long>(units[i].attack) * 3
            + static_cast<long long>(units[i].defense) * 2
            + units[i].health
            + units[i].speed * 5;

        power += unitPower * army[i];
    }

    return power;
}

// ============================================================================
// CALCULATE ENEMY POWER
// ============================================================================

long long calculateEnemyPower(
    bool faction,
    int scenario,
    int enemyArmy[],
    int enemyHealth[])
{
    Unit units[MAX_UNITS];

    getEnemyUnits(
        faction,
        scenario,
        units
    );

    long long power = 0;

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (enemyArmy[i] <= 0)
            continue;

        long long fullHealth =
            static_cast<long long>(enemyArmy[i]) *
            units[i].health;

        double healthRatio = 1.0;

        if (fullHealth > 0)
        {
            healthRatio =
                static_cast<double>(enemyHealth[i]) /
                static_cast<double>(fullHealth);
        }

        if (healthRatio < 0)
            healthRatio = 0;

        long long unitPower =
            static_cast<long long>(units[i].attack) * 3
            + static_cast<long long>(units[i].defense) * 2
            + units[i].health
            + units[i].speed * 5;

        unitPower =
            static_cast<long long>(
                unitPower * healthRatio
            );

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
    int enemyArmy[],
    int attackTrainingTier,
    int mobilityTier,
    int scenario,
    int enemyHealth[])
{
    Unit playerUnits[MAX_UNITS];

    getBaseUnits(
        faction,
        playerUnits
    );

    long long playerPower =
        calculateArmyPower(
            faction,
            army,
            attackTrainingTier,
            mobilityTier
        );

    long long enemyPower =
        calculateEnemyPower(
            faction,
            scenario,
            enemyArmy,
            enemyHealth
        );

    if (playerPower <= 0)
        return;

    double lossRatio =
        static_cast<double>(enemyPower) /
        static_cast<double>(
            playerPower + enemyPower
        );

    lossRatio =
        max(
            0.10,
            min(0.75, lossRatio)
        );

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        int losses =
            static_cast<int>(
                army[i] * lossRatio
            );

        if (losses == 0 && army[i] > 2)
            losses = 1;

        if (i == 4 && army[i] == 1)
        {
            if (lossRatio >= 0.65)
                losses = 1;
            else
                losses = 0;
        }

        losses =
            min(
                losses,
                army[i]
            );

        army[i] -= losses;

        if (losses > 0)
        {
            cout << losses
                 << " "
                 << playerUnits[i].name
                 << "(s) were lost.\n";
        }
    }
}

// ============================================================================
// HAS ANY UNITS
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
// CAN AFFORD ANY UNIT
// ============================================================================

bool canAffordAnyUnit(
    bool faction,
    int cash,
    int trainingTier,
    int weaponTier,
    int army[])
{
    Unit units[MAX_UNITS];

    getBaseUnits(
        faction,
        units
    );

    int maxUnits = weaponTier + 1;

    if (maxUnits > MAX_UNITS)
        maxUnits = MAX_UNITS;

    for (int i = 0; i < maxUnits; i++)
    {
        int discountedCost =
            units[i].cost *
            (100 - trainingTier * 10) /
            100;

        if (i == 4 && army[4] >= 1)
            continue;

        if (cash >= discountedCost)
            return true;
    }

    return false;
}

// ============================================================================
// THERMAL MISSILE DAMAGE
// ============================================================================

void applyThermalMissileDamage(
    bool faction,
    int scenario,
    int enemyArmy[],
    int enemyHealth[])
{
    Unit enemyUnits[MAX_UNITS];

    getEnemyUnits(
        faction,
        scenario,
        enemyUnits
    );

    cout << "\n";
    cout << "============================================================\n";
    cout << "                 *** THERMAL IMPACT ***\n";
    cout << "============================================================\n\n";

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (enemyArmy[i] <= 0)
            continue;

        int oldHealth = enemyHealth[i];

        int damage =
            static_cast<int>(
                enemyHealth[i] *
                (THERMAL_DAMAGE_PERCENT / 100.0)
            );

        if (damage < 1)
            damage = 1;

        enemyHealth[i] =
            max(
                0,
                enemyHealth[i] - damage
            );

        cout << enemyUnits[i].name
             << " lost "
             << damage
             << " HP"
             << " ("
             << oldHealth
             << " -> "
             << enemyHealth[i]
             << ")\n";
    }

    cout << "\nThe thermal missile damaged EVERY enemy type in the wave.\n";
}

// ============================================================================
// ATTACK / FIGHT SCREEN
// ============================================================================

bool attackScreen(
    bool faction,
    int scenario,
    int& cash,
    int bankTier,
    int& wave,
    int army[],
    int attackTrainingTier,
    int mobilityTier,
    bool& thermalMissile)
{
    clearScreen();

    setFactionColor(faction);

    if (!hasAnyUnits(army))
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

    int enemyArmy[MAX_UNITS];
    int enemyHealth[MAX_UNITS];

    createEnemyWave(
        faction,
        scenario,
        wave,
        enemyArmy
    );

    createEnemyHealth(
        faction,
        scenario,
        enemyArmy,
        enemyHealth
    );

    // ========================================================================
    // ATTACK PREPARATION
    // ========================================================================

    while (true)
    {
        clearScreen();

        setFactionColor(faction);

        cout << "+============================================================================+\n";
        cout << "|                              ATTACK                                       |\n";
        cout << "+============================================================================+\n";

        cout << "| Scenario: ";

        setScenarioColor(scenario);

        cout << left
             << setw(65)
             << (
                scenario == SCENARIO_STANDARD ? "STANDARD WAR" :
                scenario == SCENARIO_ROBOTS ? "ROBOT UPRISING" :
                scenario == SCENARIO_MUTANTS ? "MUTANT OUTBREAK" :
                "REBEL WAR"
             );

        resetColor();

        cout << "|\n";

        cout << "| Wave: "
             << left
             << setw(70)
             << wave
             << "|\n";

        cout << "+============================================================================+\n";

        resetColor();

        displayArmy(
            faction,
            army,
            0,
            0,
            attackTrainingTier,
            mobilityTier
        );

        displayEnemyWave(
            faction,
            scenario,
            wave,
            enemyArmy,
            enemyHealth
        );

        long long playerPower =
            calculateArmyPower(
                faction,
                army,
                attackTrainingTier,
                mobilityTier
            );

        long long enemyPower =
            calculateEnemyPower(
                faction,
                scenario,
                enemyArmy,
                enemyHealth
            );

        cout << "\n";
        cout << "+============================================================================+\n";
        cout << "|                              BATTLE POWER                                 |\n";
        cout << "+============================================================================+\n";

        cout << "| Your army power:   "
             << left
             << setw(58)
             << playerPower
             << "|\n";

        cout << "| Enemy wave power:  "
             << left
             << setw(58)
             << enemyPower
             << "|\n";

        cout << "+============================================================================+\n";

        cout << "\n[1] BEGIN BATTLE\n";

        if (thermalMissile)
            cout << "[2] FIRE THERMAL MISSILE\n";

        cout << "[0] CANCEL ATTACK\n";

        cout << "\nSelect option: ";

        int choice = getIntInput();

        if (choice == 0)
            return false;

        if (choice == 2 && thermalMissile)
        {
            clearScreen();

            setFactionColor(faction);

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|             THERMAL MISSILE                 |\n";
            cout << "+==============================================+\n";

            cout << "| COST: $1800                                  |\n";
            cout << "| DAMAGE: 20% OF EACH ENEMY TYPE'S HP         |\n";
            cout << "+==============================================+\n\n";

            cout << "Launching thermal missile...\n\n";

            sleepMs(700);

            cout << "3...\n";
            sleepMs(500);

            cout << "2...\n";
            sleepMs(500);

            cout << "1...\n";
            sleepMs(500);

            cout << "\n*** THERMAL MISSILE LAUNCHED ***\n";

            sleepMs(700);

            applyThermalMissileDamage(
                faction,
                scenario,
                enemyArmy,
                enemyHealth
            );

            thermalMissile = false;

            resetColor();

            pauseScreen();

            continue;
        }

        if (choice != 1)
        {
            cout << "\nInvalid selection!\n";
            pauseScreen();
            continue;
        }

        break;
    }

    // ========================================================================
    // BATTLE
    // ========================================================================

    clearScreen();

    setFactionColor(faction);

    cout << "+============================================================================+\n";
    cout << "|                         *** BATTLE START ***                              |\n";
    cout << "+============================================================================+\n\n";

    cout << "Your forces advance...\n\n";

    long long playerPower =
        calculateArmyPower(
            faction,
            army,
            attackTrainingTier,
            mobilityTier
        );

    long long enemyPower =
        calculateEnemyPower(
            faction,
            scenario,
            enemyArmy,
            enemyHealth
        );

    if (playerPower >= enemyPower)
    {
        cout << "Your army overwhelms the enemy wave!\n\n";

        applyCombatLosses(
            faction,
            army,
            enemyArmy,
            attackTrainingTier,
            mobilityTier,
            scenario,
            enemyHealth
        );

        int baseReward =
            600 + wave * 250;

        int bankBonus =
            bankTier * 100;

        int totalReward =
            baseReward + bankBonus;

        cash += totalReward;

        cout << "\n";
        cout << "+==============================================+\n";
        cout << "|               WAVE DEFEATED!                |\n";
        cout << "+==============================================+\n";

        cout << "| Wave:          "
             << left
             << setw(29)
             << wave
             << "|\n";

        cout << "| Base reward:   $"
             << setw(27)
             << baseReward
             << "|\n";

        cout << "| Bank bonus:    $"
             << setw(27)
             << bankBonus
             << "|\n";

        cout << "| Total reward:  $"
             << setw(27)
             << totalReward
             << "|\n";

        cout << "| New cash:      $"
             << setw(27)
             << cash
             << "|\n";

        cout << "+==============================================+\n";

        wave++;

        cout << "\nThe next enemy wave will be stronger.\n";

        resetColor();

        pauseScreen();

        return true;
    }

    cout << "The enemy overwhelms your forces!\n\n";

    for (int i = 0; i < MAX_UNITS; i++)
    {
        if (army[i] <= 0)
            continue;

        int losses =
            max(
                1,
                army[i] / 2
            );

        losses =
            min(
                losses,
                army[i]
            );

        army[i] -= losses;

        cout << losses
             << " units lost from your ";

        setFactionColor(faction);

        cout << (
            faction
            ? "bug "
            : "human "
        );

        resetColor();

        cout << "forces.\n";
    }

    cout << "\nThe wave was NOT defeated.\n";
    cout << "Recruit more units and try again.\n";

    resetColor();

    pauseScreen();

    return false;
}

// ============================================================================
// SABOTAGE / NUCLEAR SYSTEM
// ============================================================================

void sabotageScreen(
    bool faction,
    int& cash,
    int nuclearTier,
    bool& warhead,
    bool& guidance,
    bool& core,
    bool& thermalMissile,
    bool& gameWon)
{
    const int WARHEAD_COST = 2500;
    const int GUIDANCE_COST = 3000;
    const int CORE_COST = 4500;

    while (true)
    {
        clearScreen();

        setFactionColor(faction);

        string factionName =
            faction
            ? "BUG HIVE"
            : "HUMAN MILITARY";

        cout << "+============================================================================+\n";
        cout << "|                            SABOTAGE                                       |\n";
        cout << "+============================================================================+\n";
        cout << "|                                                                            |\n";
        cout << "|                         "
             << factionName
             << "                                      |\n";
        cout << "|                                                                            |\n";
        cout << "|                       WEAPONS PROGRAM                                     |\n";
        cout << "|                                                                            |\n";
        cout << "+----------------------------------------------------------------------------+\n";

        cout << "| Available cash: $"
             << left
             << setw(56)
             << cash
             << "|\n";

        cout << "| Nuclear research: "
             << nuclearTier
             << "/3                                                       |\n";

        cout << "+============================================================================+\n";

        cout << "|  [1] NUCLEAR WARHEAD                                                      |\n";
        cout << "|                                                                            |\n";

        if (warhead)
            cout << "|      STATUS: PURCHASED                                                    |\n";
        else
            cout << "|      STATUS: NOT PURCHASED - COST $2500                                   |\n";

        cout << "|                                                                            |\n";

        cout << "|  [2] GUIDANCE SYSTEM                                                      |\n";
        cout << "|                                                                            |\n";

        if (guidance)
            cout << "|      STATUS: PURCHASED                                                    |\n";
        else
            cout << "|      STATUS: NOT PURCHASED - COST $3000                                   |\n";

        cout << "|                                                                            |\n";

        cout << "|  [3] NUCLEAR CORE                                                         |\n";
        cout << "|                                                                            |\n";

        if (core)
            cout << "|      STATUS: PURCHASED                                                    |\n";
        else
            cout << "|      STATUS: NOT PURCHASED - COST $4500                                   |\n";

        cout << "|                                                                            |\n";
        cout << "+----------------------------------------------------------------------------+\n";

        cout << "|  [4] THERMAL MISSILE                                                      |\n";
        cout << "|                                                                            |\n";

        if (thermalMissile)
        {
            cout << "|      STATUS: READY                                                        |\n";
            cout << "|      Effect: Damages all enemy types in a wave.                           |\n";
        }
        else
        {
            cout << "|      STATUS: NOT PURCHASED                                                |\n";
            cout << "|      COST: $1800                                                          |\n";
        }

        cout << "|                                                                            |\n";
        cout << "+----------------------------------------------------------------------------+\n";

        bool nukeComplete =
            warhead &&
            guidance &&
            core;

        if (nukeComplete)
        {
            cout << "|  [5] *** LAUNCH NUCLEAR WEAPON ***                                       |\n";
            cout << "|      ALL NUCLEAR COMPONENTS ARE READY.                                   |\n";
        }
        else
        {
            cout << "|  [5] LAUNCH NUCLEAR WEAPON - LOCKED                                      |\n";
            cout << "|      Purchase all 3 nuclear parts first.                                 |\n";
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

            cout << "\nNuclear Warhead acquired!\n";

            pauseScreen();
        }

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

            cout << "\nGuidance System acquired!\n";

            pauseScreen();
        }

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

            cout << "\nNuclear Core acquired!\n";
            cout << "ALL NUCLEAR COMPONENTS ARE READY.\n";

            pauseScreen();
        }

        else if (choice == 4)
        {
            if (thermalMissile)
            {
                cout << "\nYou already have a Thermal Missile ready.\n";
                pauseScreen();
                continue;
            }

            if (cash < THERMAL_MISSILE_COST)
            {
                cout << "\nNot enough cash!\n";
                pauseScreen();
                continue;
            }

            cash -= THERMAL_MISSILE_COST;

            thermalMissile = true;

            cout << "\n";
            cout << "+==============================================+\n";
            cout << "|          THERMAL MISSILE ACQUIRED           |\n";
            cout << "+==============================================+\n";
            cout << "| Cost: $1800                                  |\n";
            cout << "| Effect: Removes 20% HP from every enemy     |\n";
            cout << "| type in the next wave you attack.           |\n";
            cout << "| The missile is consumed when fired.         |\n";
            cout << "+==============================================+\n";

            pauseScreen();
        }

        else if (choice == 5)
        {
            if (!nukeComplete)
            {
                cout << "\n";
                cout << "+==============================================+\n";
                cout << "|             LAUNCH SYSTEM LOCKED            |\n";
                cout << "+==============================================+\n";
                cout << "| You must purchase all three nuclear parts. |\n";
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
            sleepMs(700);

            cout << "2...\n";
            sleepMs(700);

            cout << "1...\n";
            sleepMs(700);

            cout << "\n";
            cout << "                  *** LAUNCH ***\n\n";

            sleepMs(1000);

            cout << "                  *** IMPACT ***\n\n";

            sleepMs(1200);

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
        cout << "The Bug Hive has lost all of its forces.\n";
    else
        cout << "The Human Military has lost all of its forces.\n";

    cout << "There are no resources remaining to continue the war.\n\n";

    cout << "============================================================================\n";
    cout << "                              YOU LOSE!\n";
    cout << "============================================================================\n\n";

    resetColor();

    pauseScreen();
}

// ============================================================================
// GIVE UP CONFIRMATION
// ============================================================================

bool giveUpScreen(bool faction)
{
    clearScreen();

    setFactionColor(faction);

    cout << "\n";
    cout << "+============================================================================+\n";
    cout << "|                              GIVE UP                                      |\n";
    cout << "+============================================================================+\n";
    cout << "|                                                                            |\n";
    cout << "| Are you sure you want to abandon the campaign?                            |\n";
    cout << "|                                                                            |\n";
    cout << "| All progress will be lost and the game will end.                          |\n";
    cout << "|                                                                            |\n";
    cout << "+----------------------------------------------------------------------------+\n";

    resetColor();

    cout << "\nEnter 1 to GIVE UP or 0 to CANCEL: ";

    int choice = getIntInput();

    if (choice == 1)
    {
        clearScreen();

        setFactionColor(faction);

        cout << "\n";
        cout << "============================================================================\n";
        cout << "                         CAMPAIGN ABANDONED\n";
        cout << "============================================================================\n\n";

        cout << "You have chosen to give up the war.\n";
        cout << "Your faction has surrendered the battlefield.\n\n";

        cout << "============================================================================\n";
        cout << "                              GAME OVER\n";
        cout << "============================================================================\n\n";

        resetColor();

        pauseScreen();

        return true;
    }

    cout << "\nReturning to the command center...\n";

    resetColor();

    pauseScreen();

    return false;
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

    while (
        factionChoice != HUMAN &&
        factionChoice != BUGS
    )
    {
        cout << "Please enter 0 for Humans or 1 for Bugs: ";

        factionChoice = getIntInput();
    }

    bool faction =
        (factionChoice == BUGS);

    // ========================================================================
    // SCENARIO SELECTION
    // ========================================================================

    int scenario =
        chooseScenario(faction);

    Scenario scenarioData;

    getScenario(
        scenario,
        faction,
        scenarioData
    );

    // ========================================================================
    // PLAYER VARIABLES
    // ========================================================================

    int cash =
        scenarioData.startingCash;

    int bankTier =
        scenarioData.bankTier;

    int trainingTier =
        scenarioData.trainingTier;

    int nuclearTier =
        scenarioData.nuclearTier;

    int weaponTier =
        scenarioData.weaponTier;

    int attackTrainingTier =
        scenarioData.attackTrainingTier;

    int mobilityTier =
        scenarioData.mobilityTier;

    // ========================================================================
    // ARMY
    // ========================================================================

    int army[MAX_UNITS];

    for (int i = 0; i < MAX_UNITS; i++)
    {
        army[i] =
            scenarioData.startingArmy[i];
    }

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
    // THERMAL MISSILE
    // ========================================================================

    bool thermalMissile = false;

    // ========================================================================
    // GAME STATE
    // ========================================================================

    bool gameWon = false;
    bool gameLost = false;
    bool gaveUp = false;

    // ========================================================================
    // SCENARIO INTRO
    // ========================================================================

    clearScreen();

    setScenarioColor(scenario);

    cout << "\n";
    cout << "============================================================================\n";
    cout << "                           " << scenarioData.name << "\n";
    cout << "============================================================================\n\n";

    cout << scenarioData.description << "\n\n";

    cout << "Starting cash: $"
         << cash
         << "\n";

    cout << "Starting Bank Tier: "
         << bankTier
         << "\n";

    cout << "Starting Training Tier: "
         << trainingTier
         << "\n";

    cout << "Starting Nuclear Tier: "
         << nuclearTier
         << "\n";

    cout << "Starting Weapon Tier: "
         << weaponTier
         << "\n";

    cout << "\n";

    resetColor();

    pauseScreen();

    // ========================================================================
    // MAIN GAME LOOP
    // ========================================================================

    while (!gameWon &&
           !gameLost &&
           !gaveUp)
    {
        // ====================================================================
        // DEFEAT CHECK
        // ====================================================================

        if (
            !hasAnyUnits(army) &&
            !canAffordAnyUnit(
                faction,
                cash,
                trainingTier,
                weaponTier,
                army
            )
        )
        {
            gameLost = true;
            break;
        }

        clearScreen();

        factionScreen(
            faction,
            cash,
            wave,
            scenario,
            attackTrainingTier,
            mobilityTier
        );

        cout << "\nSelect action: ";

        int action = getIntInput();

        switch (action)
        {
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

            case 2:
            {
                upgradeScreen(
                    faction,
                    cash,
                    bankTier,
                    trainingTier,
                    nuclearTier,
                    weaponTier,
                    attackTrainingTier,
                    mobilityTier
                );

                break;
            }

            case 3:
            {
                attackScreen(
                    faction,
                    scenario,
                    cash,
                    bankTier,
                    wave,
                    army,
                    attackTrainingTier,
                    mobilityTier,
                    thermalMissile
                );

                break;
            }

            case 4:
            {
                sabotageScreen(
                    faction,
                    cash,
                    nuclearTier,
                    warhead,
                    guidance,
                    core,
                    thermalMissile,
                    gameWon
                );

                break;
            }

            case 5:
            {
                if (giveUpScreen(faction))
                    gaveUp = true;

                break;
            }

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

        cout << "Scenario completed: ";

        setScenarioColor(scenario);

        cout << scenarioData.name;

        cout << "\n\n";

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

    // ========================================================================
    // GIVE UP END
    // ========================================================================

    if (gaveUp)
    {
        clearScreen();

        setFactionColor(faction);

        cout << "\n";
        cout << "============================================================================\n";
        cout << "                         CAMPAIGN OVER\n";
        cout << "============================================================================\n\n";

        cout << "You abandoned the campaign.\n";
        cout << "The war has been lost by your faction.\n\n";

        cout << "Thanks for playing my game - K1X28\n";

        cout << "\n";

        resetColor();
    }

    resetColor();

    return 0;
}
