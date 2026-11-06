#include "conversation_cutscene.h"
#include <cstring>

namespace {
const DialogueLine dialogueData[ConversationCutscene::turnCount] = {
    {"OUTPOST DEFENDER", "THE POWER CORE KEEPS OUR PEOPLE ALIVE.",
     "WITHOUT IT, THE UNDERGROUND SHELTERS GO COLD.", 0},
    {"CYBORG COMMANDER", "HUMAN EXPANSION IS DEVOURING THIS PLANET.",
     "YOUR METROPOLIS CONSUMES WHAT BELONGS TO NATURE.", 1},
    {"OUTPOST DEFENDER", "WE BUILT THIS REFUGE FOR FAMILIES ESCAPING THE WASTES.",
     "WE WILL NOT LET INNOCENT LIVES BE EXTINGUISHED.", 0},
    {"CYBORG COMMANDER", "FEELINGS ARE IRRELEVANT. MY ORDERS ARE ABSOLUTE.",
     "THE CORE WILL BE EXTRACTED AND YOUR BASE LEVELED.", 1},
    {"OUTPOST DEFENDER", "THEN WE STAND OUR GROUND TOGETHER!", "HOLD THE PERIMETER AND PROTECT THE CORE!", 0},
    {"CYBORG COMMANDER", "INITIATING EXTERMINATION PROTOCOL.", "ALL CYBORG WAVES, ADVANCE UPON THE LANES!",
     1}};
}

void ConversationCutscene::start(double currentTime) {
    turnStartTime = currentTime;
    currentTurn = 0;
    running = true;
}

void ConversationCutscene::advance(double currentTime) {
    if (!running)
        return;
    currentTurn++;
    turnStartTime = currentTime;
    if (currentTurn >= turnCount) {
        running = false;
    }
}

void ConversationCutscene::update(double currentTime) {
    if (!running)
        return;
    const DialogueLine &dialogue = getCurrentDialogue();
    double readingSeconds = 1.5 + (std::strlen(dialogue.line1) + std::strlen(dialogue.line2)) / 18.0;
    if (readingSeconds < autoAdvanceSeconds) {
        readingSeconds = autoAdvanceSeconds;
    }
    if (currentTime - turnStartTime >= readingSeconds) {
        advance(currentTime);
    }
}

bool ConversationCutscene::isRunning() const { return running; }

int ConversationCutscene::getSpeaker() const {
    if (currentTurn < 0 || currentTurn >= turnCount)
        return 0;
    return dialogueData[currentTurn].speaker;
}

const DialogueLine &ConversationCutscene::getDialogue(int turn) const {
    if (turn < 0)
        turn = 0;
    if (turn >= turnCount)
        turn = turnCount - 1;
    return dialogueData[turn];
}

const DialogueLine &ConversationCutscene::getCurrentDialogue() const { return getDialogue(currentTurn); }
