#pragma once

struct DialogueLine {
    const char *speakerTag;
    const char *line1;
    const char *line2;
    int speaker;
};

class ConversationCutscene {
  public:
    static constexpr int turnCount = 6;
    static constexpr double autoAdvanceSeconds = 5.0;

    void start(double currentTime);
    void advance(double currentTime);
    void update(double currentTime);
    bool isRunning() const;

    int getSpeaker() const;
    const DialogueLine &getDialogue(int turn) const;
    const DialogueLine &getCurrentDialogue() const;

  private:
    double turnStartTime = 0.0;
    int currentTurn = 0;
    bool running = false;
};
