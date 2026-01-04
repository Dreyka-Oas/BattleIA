#include "ResourceManager.h"

Font ResourceManager::systemFont = { 0 };
bool ResourceManager::fontLoaded = false;

Font ResourceManager::GetSystemFont() {
    if (!fontLoaded) {
        const char* fontPath = "";
        if (FileExists("Core/Inter-SemiBold.ttf")) fontPath = "Core/Inter-SemiBold.ttf";
        else if (FileExists("Inter-SemiBold.ttf")) fontPath = "Inter-SemiBold.ttf";
        else if (FileExists("C:/Windows/Fonts/segoeui.ttf")) fontPath = "C:/Windows/Fonts/segoeui.ttf";

        if (TextLength(fontPath) > 0) {
            systemFont = LoadFontEx(fontPath, 48, 0, 250);
            GenTextureMipmaps(&systemFont.texture);
            SetTextureFilter(systemFont.texture, TEXTURE_FILTER_TRILINEAR);
        } else {
            systemFont = GetFontDefault();
        }
        fontLoaded = true;
    }
    return systemFont;
}

void ResourceManager::UnloadResources() {
    if (fontLoaded) {
        if (systemFont.texture.id != GetFontDefault().texture.id) UnloadFont(systemFont);
        fontLoaded = false;
    }
}