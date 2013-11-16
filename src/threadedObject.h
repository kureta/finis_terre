#ifndef _THREADED_OBJECT
#define _THREADED_OBJECT

#include "ofMain.h"

// this is not a very exciting example yet
// but ofThread provides the basis for ofNetwork and other
// operations that require threading.
//
// please be careful - threading problems are notoriously hard
// to debug and working with threads can be quite difficult

#include "ofMain.h"

class threadedObject : public ofThread, public ofImage {
public:
    string fileName;
    
    void threadedFunction() {
        if(lock()) {
            saveImage(fileName);
            unlock();
        } else {
            printf("ofxThreadedImageSaver - cannot save %s cos I'm locked", fileName.c_str());
        }
        stopThread();
    }
    
    void saveThreaded(string fileName) {
        this->fileName = fileName;
        startThread(false, false);   // blocking, verbose
    }
};

#endif
