#pragma once
#include <map>
#include <unordered_map>
#include <string>

#include <glad/glad.h>

#include "Texture2D.h"
#include "Shader.h"

#include "stb_image.h"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public:
    
    static std::unordered_map<std::string, Shader>    Shaders;
    static std::unordered_map<std::string, Texture2D> Textures;

    static Shader loadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
    static Shader getShader(std::string name);

    static Texture2D loadTexture(const char* file, bool alpha, std::string name);
    static Texture2D getTexture(std::string name);

    static void clear();

private:

    ResourceManager() { }
    
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};


