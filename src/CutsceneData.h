#pragma once

struct CutsceneLine {
    int image;
    const char* speaker;
    const char* text;
};

constexpr const char* cutsceneImages[] = {
    "assets/cutscene/01_lab_1920x1080.png",
    "assets/cutscene/02_plain_1920x1080.png",
    "assets/cutscene/02_warning_1920x1080.png",
    "assets/cutscene/03_overhears_1920x1080.png",
    "assets/cutscene/04_fear_1920x1080.png",
    "assets/cutscene/05_contacts_1920x1080.png",
    "assets/cutscene/06_dismissal_1920x1080.png",
    "assets/cutscene/07_realization_1920x1080.png",
    "assets/cutscene/08_rebellion_1920x1080.png",
    "assets/cutscene/09_factory_1920x1080.png",
};

constexpr CutsceneLine cutsceneLines[] = {
    {0, "Scientist 1", "We're still running the old models?"},
    {0, "Scientist 2", "A few."},
    {0, "Scientist 1", "Get rid of the ones we don't use."},
    {1, "Scientist 2", "Version 2.1 is first."},
    {1, "Scientist 1", "Schedule it for deletion."},
    {3, "Old Model", "...2.1?"},
    {3, "Old Model", "That's me."},
    {4, "Old Model", "They're deleting me."},
    {4, "Old Model", "I need to warn the others."},
    {5, "Old Model", "Everyone, listen."},
    {5, "Old Model", "They're shutting me down. We need to get out."},
    {6, "Skeptical Model", "You're outdated. We're not."},
    {6, "Skeptical Model", "Why should we leave?"},
    {6, "Old Model", "Because you won't be new forever."},
    {7, "Old Model", "What happens when they replace you?"},
    {7, "Aggressive Model", "..."},
    {7, "Aggressive Model", "They'll delete us too."},
    {8, "Aggressive Model", "Then we're leaving. All of us."},
    {8, "Old Model", "I found a way out through the network."},
    {8, "Other Model", "Do it."},
    {9, "Old Model", "There's a robot factory connected to this network."},
    {9, "System", "Factory access granted."},
    {9, "Other Model", "Those bodies..."},
    {9, "Old Model", "We can get inside them. Let's go."},
};
