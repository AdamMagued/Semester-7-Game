#pragma once

class ConversationCutscene {
public:
    struct Pose {
        float playerX;
        float enemyX;
        float playerY;
        float enemyY;
        float playerShakeX;
        float enemyShakeX;
        float dialogueY;
        int speakingCharacter; // 0 = player, 1 = enemy
        int turn;
        bool running;
        bool active;
    };

    static constexpr float middleLaneY = -0.04f;
    static constexpr float dialogueY = -0.88f;
    static constexpr double secondsPerTurn = 2.0;
    static constexpr double shakeSeconds = 0.55;
    static constexpr double runSeconds = 1.2;
    static constexpr int turnCount = 6;
    static constexpr double totalSeconds = runSeconds + turnCount * secondsPerTurn;

    void start(double currentTime);
    Pose update(double currentTime);
    bool isRunning() const;
    const char* textForTurn(int turn) const;

private:
    double startedAt = 0.0;
    bool running = false;
};
