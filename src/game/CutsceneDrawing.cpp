#include "CutsceneDrawing.h"
#include <cmath>
#include <algorithm>
static void drawCharacter(GLuint playerProgram, GLint damageLocation, GLint brightnessLocation,
                          GLuint playerVBO, GLuint playerVAO, GLuint texture, int frame, int frameCount,
                          float centerX, float centerY, float halfWidth, float halfHeight,
                          bool flipHorizontal, float charBrightness) {
    if (!texture || frameCount <= 0)
        return;
    float u0 = static_cast<float>(frame) / frameCount;
    float u1 = static_cast<float>(frame + 1) / frameCount;
    if (flipHorizontal) {
        std::swap(u0, u1);
    }
    const float quad[] = {centerX - halfWidth, centerY + halfHeight, u0, 1.0f,
                          centerX + halfWidth, centerY + halfHeight, u1, 1.0f,
                          centerX + halfWidth, centerY - halfHeight, u1, 0.0f,
                          centerX - halfWidth, centerY + halfHeight, u0, 1.0f,
                          centerX + halfWidth, centerY - halfHeight, u1, 0.0f,
                          centerX - halfWidth, centerY - halfHeight, u0, 0.0f};
    glUseProgram(playerProgram);
    glUniform1f(damageLocation, 0.0f);
    glUniform1f(brightnessLocation, charBrightness);
    glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
    glBindVertexArray(playerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawConversation(ConversationCutscene &conversation, double currentFrameTime, int gameLeft,
                      int gameBottom, int gameWidth, int gameHeight, GLuint backgroundTexture,
                      GLuint playerProgram, GLint damageLocation, GLint brightnessLocation, GLuint playerVBO,
                      GLuint playerVAO, GLuint shaderProgram, GLuint VBO, GLuint VAO,
                      const GLuint playerAnimationTextures[3][4], const int playerAnimationFrames[3][4],
                      const GLuint robotEnemyTextures[3]) {
    glViewport(gameLeft, gameBottom, gameWidth, gameHeight);
    glClearColor(0.015f, 0.02f, 0.035f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const float backgroundQuad[] = {-1, 1, 0, 1, 1, 1,  1, 1, 1,  -1, 1, 0,
                                    -1, 1, 0, 1, 1, -1, 1, 0, -1, -1, 0, 0};
    if (backgroundTexture) {
        glUseProgram(playerProgram);
        glUniform1f(damageLocation, 0.0f);
        glUniform1f(brightnessLocation, 0.25f);
        glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(backgroundQuad), backgroundQuad);
        glBindVertexArray(playerVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    std::vector<float> stageGeom;

    addRectangle(stageGeom, -1.0f, -0.78f, 1.0f, -1.0f, 0.04f, 0.05f, 0.07f);

    addRectangle(stageGeom, -1.0f, -0.772f, 1.0f, -0.78f, 0.08f, 0.55f, 0.75f);

    drawGeometry(stageGeom, shaderProgram, VAO, VBO);

    int speaker = conversation.getSpeaker();
    const DialogueLine &dialogue = conversation.getCurrentDialogue();

    float pBob = (speaker == 0) ? static_cast<float>(std::sin(currentFrameTime * 8.0) * 0.015)
                                : static_cast<float>(std::sin(currentFrameTime * 3.0) * 0.005);
    float eBob = (speaker == 1) ? static_cast<float>(std::sin(currentFrameTime * 9.0) * 0.012)
                                : static_cast<float>(std::sin(currentFrameTime * 2.5) * 0.004);

    int pFrames = playerAnimationFrames[0][0];
    int pFrame =
        (pFrames > 0) ? static_cast<int>(currentFrameTime * (speaker == 0 ? 7.0 : 3.5)) % pFrames : 0;
    float pBrightness = (speaker == 0) ? 1.0f : 0.52f;
    drawCharacter(playerProgram, damageLocation, brightnessLocation, playerVBO, playerVAO,
                  playerAnimationTextures[0][0], pFrame, pFrames, -0.52f, -0.32f + pBob, 0.38f, 0.58f, false,
                  pBrightness);

    int eFrames = 2;
    int eFrame = static_cast<int>(currentFrameTime * (speaker == 1 ? 5.0 : 2.0)) % eFrames;
    float eBrightness = (speaker == 1) ? 1.0f : 0.52f;
    drawCharacter(playerProgram, damageLocation, brightnessLocation, playerVBO, playerVAO,
                  robotEnemyTextures[0], eFrame, eFrames, 0.52f, -0.30f + eBob, 0.44f, 0.58f, true,
                  eBrightness);

    std::vector<float> bubbleGeom;
    float left = -0.82f;
    float right = 0.82f;
    addRectangle(bubbleGeom, left, 0.84f, right, 0.24f, 0.04f, 0.05f, 0.07f);
    drawText(bubbleGeom, dialogue.speakerTag, left + 0.05f, 0.78f, 0.008f, 0.9f, 0.8f, 0.4f);
    std::string text = std::string(dialogue.line1) + " " + dialogue.line2;
    drawWrappedText(bubbleGeom, text, left + 0.05f, 0.66f, right - left - 0.10f, 0.011f, 0.12f);
    drawText(bubbleGeom, "SKIP", 0.83f, 0.92f, 0.005f);

    drawGeometry(bubbleGeom, shaderProgram, VAO, VBO);
}
