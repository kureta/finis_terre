#include "Scene.h"

Scene::Scene(string _path)
{
    numFrames = 0;
    currentFrame = 1;
    initialTime = 0;
    path = _path;
    
    //some path, may be absolute or relative to bin/data
    ofDirectory dir(path);
    //only show png files
    dir.allowExt("col");
    //populate the directory object
    dir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < dir.numFiles(); i++){
        string temp = dir.getPath(i);
        temp.erase(0,13);
        temp.erase(temp.end()-4, temp.end());
        colors.push_back(ofToInt(temp));
    }
    std::sort(colors.begin(), colors.end());
    ofDirectory dir2(path);
    dir2.allowExt("vert");
    //populate the directory object
    dir2.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < dir2.numFiles(); i++){
        string temp = dir2.getPath(i);
        temp.erase(0,15);
        temp.erase(temp.end()-5, temp.end());
        vertices.push_back(ofToInt(temp));
    }
    std::sort(vertices.begin(), vertices.end());
    //only show png files
    //populate the directory object
    numFrames = dir.size();
}