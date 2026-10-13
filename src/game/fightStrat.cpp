#include "fightStrat.h"
#include <iostream>

using namespace std;

void meleeStrat::attack(int dmg) {
    // hit enemy close up
    cout << "ouch, punched: " << dmg << endl;
}

void rangedStrat::attack(int dmg) {
    // shoot from far
    cout << "shot laser: " << dmg << endl;
}

void defenseStrat::attack(int dmg) {
    // block or counter
    cout << "shield blocked" << endl;
}