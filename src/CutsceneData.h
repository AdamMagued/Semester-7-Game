#pragma once

struct CutsceneLine {
    int image;
    const char* speaker;
    const char* text;
    // Safe area inside the speech bubble, measured on a 420 by 236 preview.
    int left, top, right, bottom;
    bool lightBox = false;
};

constexpr const char* cutsceneImages[] = {
    "assets/cutscene/frame_03_old_model_hears.png",
    "assets/cutscene/frame_01_lab_two_researchers.png",
    "assets/cutscene/frame_09_factory_accessed.png",
    "assets/cutscene/frame_05_old_model_contacts_others.png",
    "assets/cutscene/frame_07_angry_model_rebel.png",
    "assets/cutscene/frame_04_old_model_distressed.png",
    "assets/cutscene/frame_06_other_models_react.png",
    "assets/cutscene/frame_02_menu_no_warning.png",
    "assets/cutscene/frame_08_rebellion_network.png",
    "assets/cutscene/frame_02b_menu_warning.png",
};

constexpr CutsceneLine cutsceneLines[] = {
    {0, "Scientist 1", "We're still running the old models?", 86, 70, 168, 102},
    {0, "Scientist 2", "A few.", 243, 92, 312, 120},
    {0, "Scientist 1", "Get rid of the ones we don't use.", 86, 70, 168, 102},
    {1, "Scientist 2", "Version 2.1 is first.", 303, 21, 398, 56, true},
    {2, "Scientist 1", "Schedule it for deletion.", 19, 23, 127, 57},
    {3, "Old Model", "...2.1?", 91, 22, 325, 71},
    {3, "Old Model", "That's me.", 91, 22, 325, 71},
    {4, "Old Model", "They're deleting me.", 106, 23, 311, 74},
    {4, "Old Model", "I need to warn the others.", 106, 23, 311, 74},
    {5, "Old Model", "Everyone, listen.", 158, 32, 257, 70},
    {5, "Old Model", "They're shutting me down. We need to get out.", 158, 32, 257, 70},
    {6, "Skeptical Model", "You're outdated. We're not.", 78, 63, 147, 94},
    {6, "Skeptical Model", "Why should we leave?", 78, 63, 147, 94},
    {6, "Old Model", "Because you won't be new forever.", 276, 60, 361, 91},
    {7, "Old Model", "What happens when they replace you?", 40, 22, 370, 78},
    {7, "Aggressive Model", "...", 40, 22, 370, 78},
    {7, "Aggressive Model", "They'll delete us too.", 40, 22, 370, 78},
    {8, "Aggressive Model", "Then we're leaving. All of us.", 139, 29, 270, 74},
    {8, "Old Model", "I found a way out through the network.", 139, 29, 270, 74},
    {8, "Other Model", "Do it.", 335, 65, 370, 79},
    {9, "Old Model", "There's a robot factory connected to this network.", 22, 25, 184, 77},
    {9, "System", "Factory access granted.", 229, 24, 398, 77},
    {9, "Other Model", "Those bodies...", 229, 24, 398, 77},
    {9, "Old Model", "We can get inside them. Let's go.", 22, 25, 184, 77},
};
