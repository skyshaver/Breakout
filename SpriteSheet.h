#pragma once

#include <glad/glad.h>

class SpriteSheet
{
    int spriteWidth, spriteHeight;
    int texWidth, texHeight;

    int tex;

public:
    SpriteSheet(int t, int tW, int tH, int sW, int sH)
        : tex(t), texWidth(tW), texHeight(tH), spriteWidth(sW), spriteHeight(sH)
    {}

    void drawSpriteFromSheet(float posX, float posY, int frameIndex);
};

