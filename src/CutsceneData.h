#pragma once

struct CutsceneLine {
    int image;
    const char* speaker;
    const char* text;
};

constexpr const char* cutsceneImages[] = {
    "assets/cutscene/frame_01_lab_two_researchers.png",
    "assets/cutscene/frame_02_menu_no_warning.png",
    "assets/cutscene/frame_02b_menu_warning.png",
    "assets/cutscene/frame_03_old_model_hears.png",
    "assets/cutscene/frame_04_old_model_distressed.png",
    "assets/cutscene/frame_05_old_model_contacts_others.png",
    "assets/cutscene/frame_06_other_models_react.png",
    "assets/cutscene/frame_07_angry_model_rebel.png",
    "assets/cutscene/frame_08_rebellion_network.png",
    "assets/cutscene/frame_09_factory_accessed.png",
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
