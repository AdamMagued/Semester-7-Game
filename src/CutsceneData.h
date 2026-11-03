#pragma once

struct CutsceneLine {
    int image;
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
    {0, "We're still running the old models?"},
    {0, "A few."},
    {0, "Get rid of the ones we don't use."},
    {1, "Version 2.1 is first."},
    {1, "Schedule it for deletion."},
    {3, "...2.1?"},
    {3, "That's me."},
    {4, "They're deleting me."},
    {4, "I need to warn the others."},
    {5, "Everyone, listen."},
    {5, "They're shutting me down. We need to get out."},
    {6, "You're outdated. We're not."},
    {6, "Why should we leave?"},
    {6, "Because you won't be new forever."},
    {7, "What happens when they replace you?"},
    {7, "..."},
    {7, "They'll delete us too."},
    {8, "Then we're leaving. All of us."},
    {8, "I found a way out through the network."},
    {8, "Do it."},
    {9, "There's a robot factory connected to this network."},
    {9, "Factory access granted."},
    {9, "Those bodies..."},
    {9, "We can get inside them. Let's go."},
};
