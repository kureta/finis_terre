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
    maxSpeed = 16.0f; // maxSpeed = c.r *6.0/255 + 2;
    maxPicSpeed = 16.0f;
    float sat = c.getSaturation() /255.0;
    float br = (c.getBrightness()/255.0);
    maxSteerForce = .7f; // maxSteerForce = .1 + (sat*br)*.1;
    maxPicForce = .3f;
}

void Boid::colorSwap(ofColor nextColor)
{
    cn = nextColor;
}

// this function triggers boid behavior
void Boid::flocking(ofVec3f _atr)
{
    acc += steer(_atr + ofVec3f(ofRandom(-300, 300),ofRandom(-300, 300),ofRandom(-300, 300)), false);
    move(false);
    //checkBounds();
    //render();
}

// this function moves particles towards the picture
void Boid::picture()
{
    acc += steer(initPos, true) * 5.0f;
    //println(initPos);
    move(true);
}

void Boid::move(bool arrival)
{
    vel += acc; //add acceleration to velocity
    if (arrival)
    {
        vel.limit(maxPicSpeed); //make sure the velocity vector magnitude does not exceed maxSpeed
    } else {
        vel.limit(maxSpeed);
    }
    pos += vel; //add velocity to position
    acc *= 0; //reset acceleration
    
    // Move the color
    float h = c.getHue();
    float s = c.getSaturation();
    float b = c.getBrightness();
    float hn = cn.getHue();
    float sn = cn.getSaturation();
    float bn = cn.getBrightness();
    int a = c.a;
    int an = cn.a;
    
    float delta = 4.0f;
    
    h -= (h-hn)/delta;
    s -= (s-sn)/delta;
    b -= (b-bn)/delta;
    
    c.setHsb(h, s, b);
    
    if (an < a){
        a -= 5;
    } else if (an > a){
        a += 10;
    }
    if (a > 255)
        a = 255;
    if (a < 0)
        a = 0;
    c.a = a;
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
        if (distance > 50.0f)
        {
            float rampedSpeed = maxPicSpeed*(distance);
            float clippedSpeed = min(rampedSpeed, maxPicSpeed);
            ofVec3f desiredVelocity = targetOffset * (clippedSpeed/distance);
            steer.set(desiredVelocity - vel);
            steer.limit(maxPicForce);
        } else {
            pos.set(initPos);
            vel *= 0.0;
            acc *= 0.0;
            steer *= 0.0;
        }
    }
    return steer;
}

