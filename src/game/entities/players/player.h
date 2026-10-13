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
        defeated
    };

    int playerCost;
    playerRange playerType;
    int lane;
    int health; 
    int damage;
    int playerDurability; //how many hits the player can take before defeat








}