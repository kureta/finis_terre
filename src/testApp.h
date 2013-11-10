#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxGamepadHandler.h"
#include "Boid.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
    int step;
    int width;
    int height;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
    void axisChanged(ofxGamepadAxisEvent &e);
    void buttonPressed(ofxGamepadButtonEvent &e);
    void buttonReleased(ofxGamepadButtonEvent &e);
    
    float posSpeed; float rotSpeed;
    float dx; float dy; float dz;
    float drx; float dry;
    
    // shader
    ofShader shader;
    bool useShader;
    
    ofImage texture;
    
    void createBoids();
    vector<Boid> boids;
};
