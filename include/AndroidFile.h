#pragma once

#include <cstdio>
#include <android/asset_manager.h>

FILE* android_fopen(AAssetManager* assman, const char* fname, const char* mode);

