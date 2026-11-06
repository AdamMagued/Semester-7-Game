#pragma once
#include "conversation_cutscene.h"
#include "../renderer/Drawing.h"
void drawConversation(ConversationCutscene &conversation, double currentFrameTime, int gameLeft,
                      int gameBottom, int gameWidth, int gameHeight, GLuint backgroundTexture,
                      GLuint playerProgram, GLint damageLocation, GLint brightnessLocation, GLuint playerVBO,
                      GLuint playerVAO, GLuint shaderProgram, GLuint VBO, GLuint VAO,
                      const GLuint playerAnimationTextures[3][4], const int playerAnimationFrames[3][4],
                      const GLuint robotEnemyTextures[3]);
