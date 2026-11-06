#include "conversation_cutscene.h"

namespace {
const DialogueLine dialogueData[ConversationCutscene::turnCount] = {
    {
        "OUTPOST DEFENDER",
        "THE POWER CORE KEEPS OUR PEOPLE ALIVE.",
        "WITHOUT IT, THE UNDERGROUND SHELTERS GO COLD.",
        0 // Player
    },
    {
        "CYBORG COMMANDER",
        "HUMAN EXPANSION IS DEVOURING THIS PLANET.",
        "YOUR METROPOLIS CONSUMES WHAT BELONGS TO NATURE.",
        1 // Enemy
    },
    {
        "OUTPOST DEFENDER",
        "WE BUILT THIS REFUGE FOR FAMILIES ESCAPING THE WASTES.",
        "WE WILL NOT LET INNOCENT LIVES BE EXTINGUISHED.",
        0 // Player
    },
    {
        "CYBORG COMMANDER",
        "FEELINGS ARE IRRELEVANT. MY ORDERS ARE ABSOLUTE.",
        "THE CORE WILL BE EXTRACTED AND YOUR BASE LEVELED.",
        1 // Enemy
    },
    {
        "OUTPOST DEFENDER",
        "THEN WE STAND OUR GROUND TOGETHER!",
        "HOLD THE PERIMETER AND PROTECT THE CORE!",
        0 // Player
    },
    {
        "CYBORG COMMANDER",
        "INITIATING EXTERMINATION PROTOCOL.",
        "ALL CYBORG WAVES, ADVANCE UPON THE LANES!",
        1 // Enemy
    }
};
}

void ConversationCutscene::start(double currentTime) {
    turnStartTime = currentTime;
    currentTurn = 0;
    running = true;
}

void ConversationCutscene::advance(double currentTime) {
    if (!running) return;
    currentTurn++;
    turnStartTime = currentTime;
    if (currentTurn >= turnCount) {
        running = false;
    }
}

void ConversationCutscene::update(double currentTime) {
    if (!running) return;
    if (currentTime - turnStartTime >= autoAdvanceSeconds) {
        advance(currentTime);
    }
}

bool ConversationCutscene::isRunning() const {
    return running;
}

int ConversationCutscene::getCurrentTurn() const {
    return currentTurn;
}

int ConversationCutscene::getSpeaker() const {
    if (currentTurn < 0 || currentTurn >= turnCount) return 0;
    return dialogueData[currentTurn].speaker;
}

const DialogueLine& ConversationCutscene::getDialogue(int turn) const {
    if (turn < 0) turn = 0;
    if (turn >= turnCount) turn = turnCount - 1;
    return dialogueData[turn];
}

const DialogueLine& ConversationCutscene::getCurrentDialogue() const {
    return getDialogue(currentTurn);
}

double ConversationCutscene::getTurnElapsed(double currentTime) const {
    return currentTime - turnStartTime;
}
