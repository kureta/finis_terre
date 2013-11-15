//
//  Boid.cpp
//  finis_terre
//
//  Created by Sahin Kureta on 05/11/13.
//
//

#include "Boid.h"

Boid::Boid(ofVec3f inPos, ofColor ic)
{
    pos.set(inPos);
    initPos.set(inPos);
    ofVec3f vel = ofVec3f(ofRandom(-10.0f,10.0f), ofRandom(-10.0f,10.0f), ofRandom(-10.0f,10.0f));
    acc = ofVec3f(0.0f, 0.0f, 0.0f);
    c = ic;
    cn = ic;
    maxSpeed = 8.0f; // maxSpeed = c.r *6.0/255 + 2;
    float sat = c.getSaturation() /255.0;
    float br = (c.getBrightness()/255.0);
    maxSteerForce = .1f; // maxSteerForce = .1 + (sat*br)*.1;
}

void Boid::colorSwap(ofColor nextColor)
{
    cn = nextColor;
}

// this function triggers boid behavior
void Boid::flocking()
{
    acc += steer(ofVec3f(ofRandom(-50, 50), ofRandom(-50, 50)), false);
    move();
    //checkBounds();
    //render();
}

// this function moves particles towards the picture
void Boid::picture()
{
    if (pos!=initPos)
        acc += steer(initPos, true) * 5.0f;
    //println(initPos);
    move();
}

void Boid::move()
{
    vel += acc; //add acceleration to velocity
    vel.limit(maxSpeed); //make sure the velocity vector magnitude does not exceed maxSpeed
    pos += vel; //add velocity to position
    acc *= 0; //reset acceleration
    
    // Move the color
    float h = c.getHue();
    float s = c.getSaturation();
    float b = c.getBrightness();
    float hn = cn.getHue();
    float sn = cn.getSaturation();
    float bn = cn.getBrightness();
    
    float delta = 4.0f;
    
    h -= (h-hn)/delta;
    s -= (s-sn)/delta;
    b -= (b-bn)/delta;
    
    c.setHsb(h, s, b);
}

//steering. If arrival==true, the boid slows to meet the target. Credit to Craig Reynolds
ofVec3f Boid::steer(ofVec3f target, Boolean arrival)
{
    ofVec3f steer; //creates vector for steering
    if (!arrival)
    {
        steer.set(target - pos); //steering vector points towards target (switch target and pos for avoiding)
        steer.limit(maxSteerForce); //limits the steering force to maxSteerForce
    }
    else
    {
        ofVec3f targetOffset = target - pos;
        float distance = targetOffset.length();
        if (distance > 5.0f)
        {
            float mymy = 1024.f;
            float rampedSpeed = maxSpeed*(distance/100);
            float clippedSpeed = min(rampedSpeed, maxSpeed);
            ofVec3f desiredVelocity = targetOffset * (clippedSpeed/distance);
            steer.set(desiredVelocity - vel);
            steer.limit(maxSteerForce);
        } else {
            //pos = target;
        }
    }
    return steer;
}

