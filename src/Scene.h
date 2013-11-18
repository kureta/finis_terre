#pragma once

#include "ofMain.h"

class Scene
{
public:
    Scene(string _path);
    string path;
    int numFrames;
    int currentFrame;
    
    unsigned long long initialTime;
    
    vector<int> colors;
    vector<int> vertices;
};
