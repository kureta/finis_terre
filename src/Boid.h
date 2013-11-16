#ifndef __finis_terre__Boid__
#define __finis_terre__Boid__

#include "ofMain.h"

class Boid
{
public:
    //fields
    ofVec3f pos; ofVec3f vel; ofVec3f acc; //pos, velocity, and acceleration in a vector datatype
    ofVec3f initPos;
    float maxSpeed; //maximum magnitude ofr the velocity vector
    float maxSteerForce; //maximum magnitude of the steering vector
    float maxPicSpeed;
    float maxPicForce;
    ofColor c; //color
    ofColor cn; // previous color
    float width = 160;
    float height = 120;
    float depth = 200;
    
    //constructors
    Boid(ofVec3f inPos, ofColor ic);
    
    void flocking();
    void picture();
    void move(bool arrival);
    ofVec3f steer(ofVec3f target, Boolean arrival);
    
    void colorSwap(ofColor nextColor);
};

#endif /* defined(__finis_terre__Boid__) */
