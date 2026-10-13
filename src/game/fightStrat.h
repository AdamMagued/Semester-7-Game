#ifndef FIGHTSTRAT_H
#define FIGHTSTRAT_H

class fightStrat {
public:
    virtual ~fightStrat() {}
    virtual void attack(int dmg) = 0;
};

class meleeStrat : public fightStrat {
public:
    void attack(int dmg);
};

class rangedStrat : public fightStrat {
public:
    void attack(int dmg);
};

class defenseStrat : public fightStrat {
public:
    void attack(int dmg);
};

#endif