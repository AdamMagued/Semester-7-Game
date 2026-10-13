#include "entity.h"

class playerClass: public EntityClass{
    public:
    enum playerRange{
        MELEE,
        RANGED,
        DEFENSE
    };

    enum playerState{
        vulnerable,
        invulnerable,
        dead
    };

    int playerCost;
    playerRange playerType;
    int lane;
    int health; 
    int damage;







}