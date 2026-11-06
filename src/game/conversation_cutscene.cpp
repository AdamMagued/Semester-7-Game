#include "conversation_cutscene.h"

#include <cmath>

namespace {
// Edit these lines to change what the player and enemy say.
const char* dialogueLines[] = {
    "PLAYER: THE OUTPOST'S POWER CORE KEEPS PEOPLE ALIVE.",
    "ENEMY: HUMAN EXPANSION IS DESTROYING THIS WORLD.",
    "PLAYER: IF THE CORE FAILS, THE SHELTER GOES DARK.",
    "ENEMY: I WILL SHUT IT DOWN TO PROTECT THE PLANET.",
    "PLAYER: THEN WE DEFEND THE OUTPOST. STAND DOWN.",
    "ENEMY: I WILL NOT LET YOU STOP ME."
};
}

void ConversationCutscene::start(double currentTime) {
    startedAt = currentTime;
    running = true;
}

ConversationCutscene::Pose ConversationCutscene::update(double currentTime) {
    double elapsed = running ? currentTime - startedAt : 0.0;
    if (elapsed < 0.0) elapsed = 0.0;
    if (elapsed >= totalSeconds) running = false;

    bool runningPhase = running && elapsed < runSeconds;
    double conversationElapsed = elapsed > runSeconds ? elapsed - runSeconds : 0.0;

    int turn = static_cast<int>(conversationElapsed / secondsPerTurn);
    if (turn >= turnCount) turn = turnCount - 1;

    // Turns alternate: player, enemy, player, enemy.
    int speaker = turn % 2;
    double turnTime = conversationElapsed - turn * secondsPerTurn;

    float approach = static_cast<float>(elapsed / runSeconds);
    if (approach > 1.0f) approach = 1.0f;
    float playerX = -0.95f + 0.65f * approach;
    float enemyX = 0.95f - 0.65f * approach;

    // Only the speaking character shakes, briefly at the start of its turn.
    float shake = 0.0f;
    if (!runningPhase && running && turnTime < shakeSeconds) {
        shake = static_cast<float>(std::sin(turnTime * 58.0) * 0.012);
    }

    return {
        playerX + (speaker == 0 ? shake : 0.0f),
        enemyX + (speaker == 1 ? shake : 0.0f),
        middleLaneY,
        middleLaneY,
        speaker == 0 ? shake : 0.0f,
        speaker == 1 ? shake : 0.0f,
        dialogueY,
        speaker,
        turn,
        runningPhase,
        running
    };
}

bool ConversationCutscene::isRunning() const {
    return running;
}

const char* ConversationCutscene::textForTurn(int turn) const {
    if (turn < 0) turn = 0;
    if (turn >= turnCount) turn = turnCount - 1;
    return dialogueLines[turn];
}
