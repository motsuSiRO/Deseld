#pragma once
#include "model_data.h"
#include "FaceLoader.h"
bool BinarySkinmeshInput(const char* fbx_filename, ModelData* data, int mode);
bool BinarySkinmeshOutput(const char* fbx_filename, ModelData* data);

bool BinaryFaceInput(const char* fbx_filename, Face_List* data);
bool BinaryFaceOutput(const char* fbx_filename, Face_List* data);

