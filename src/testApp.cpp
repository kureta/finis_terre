#include "testApp.h"

string path = "yakin-on";
ofDirectory vert(path);
ofDirectory col(path);

//--------------------------------------------------------------
void testApp::setup() {
    //some path, may be absolute or relative to bin/data
    
    //only show png files
    vert.allowExt("vert");
    col.allowExt("col");
    //populate the directory object
    vert.listDir();
    col.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < vert.size(); i++){
        if (i % 1000 == 0)
        {
            ofLogNotice(path + "/vertice" + ofToString(i) + ".vert");
            ofLogNotice(path + "/color" + ofToString(i) + ".col");
        }
    }
    
    ofLogNotice(ofToString(vert.size()));
    
    ofExit();
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	nearThreshold = 250;
	farThreshold = 2000;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	// angle = 0;
	// kinect.setCameraTiltAngle(angle);
    angle = kinect.getCurrentCameraTiltAngle();
    shader.load("shaders/noise.vert", "shaders/noise.frag");
    ofDisableArbTex();
    texture.loadImage("particle.png");
    
    posSpeed = 8.0; rotSpeed = 2.0;
    dx = 0.0; dy = 0.0; dz = 0.0;
    drx = 0.0; dry = 0.0;
    ofxGamepadHandler::get()->enableHotplug();
    easyCam.disableMouseInput();
    easyCam.dolly(500.0);
    
    step = 4;
	width = 640/step;
    height = 480/step;
    
	//CHECK IF THERE EVEN IS A GAMEPAD CONNECTED
	if(ofxGamepadHandler::get()->getNumPads()>0){
        ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
        ofAddListener(pad->onAxisChanged, this, &testApp::axisChanged);
        ofAddListener(pad->onButtonPressed, this, &testApp::buttonPressed);
        ofAddListener(pad->onButtonReleased, this, &testApp::buttonReleased);
	}
    
    createBoids();
}

//--------------------------------------------------------------
void testApp::createBoids()
{
    for (int j=0;j<height;j++)
    {
        for (int i=0;i<width;i++)
        {
            boids.push_back(Boid(ofVec3f(ofRandom(1500.0f),ofRandom(1500.0f),ofRandom(1500.0f)), ofColor::white));
        }
    }
}

//--------------------------------------------------------------
void testApp::axisChanged(ofxGamepadAxisEvent& e)
{
	// cout << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
    switch (e.axis) {
		case 0:
            dx = e.value * posSpeed;
            break;
        case 1:
            dz = e.value * posSpeed;
            break;
        case 2:
            dry = -e.value * rotSpeed;
            break;
        case 3:
            drx = -e.value * rotSpeed;
            break;
    }
    
}

void testApp::buttonPressed(ofxGamepadButtonEvent& e)
{
	cout << "BUTTON " << e.button << " PRESSED" << endl;
}

void testApp::buttonReleased(ofxGamepadButtonEvent& e)
{
	cout << "BUTTON " << e.button << " RELEASED" << endl;
}

//--------------------------------------------------------------
void testApp::update() {
	ofBackground(50, 50, 50);
	kinect.update();
    
    easyCam.dolly(dz);
    easyCam.truck(dx);
    easyCam.pan(dry);
    easyCam.tilt(drx);
}

//--------------------------------------------------------------
void testApp::draw() {
    easyCam.begin();
    drawPointCloud();
    easyCam.end();
	
	// draw instructions
	stringstream reportStream;
	reportStream << "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >)"
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
	<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
	ofDrawBitmapString(reportStream.str(),20,652);
}

void testApp::drawPointCloud() {
	int w = 640;
	int h = 480;
    glPointSize(40);
    
	ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
            Boid *tempBoid = &boids[x/step + (y/step)*width];
			if(kinect.getDistanceAt(x, y) > nearThreshold & kinect.getDistanceAt(x, y) < farThreshold) {
                tempBoid->cn = kinect.getColorAt(x,y);
                tempBoid->initPos = kinect.getWorldCoordinateAt(x, y);
                tempBoid->picture();
			} else {
                // tempBoid->initPos = ofVec3f(ofRandom(300.0f), ofRandom(300.0f), ofRandom(300.0f));
                tempBoid->cn = ofColor::white;
                tempBoid->flocking();
            }
            mesh.addColor(boids[x/step + (y/step) * width].c);
            mesh.addVertex(boids[x/step + y/step * width].pos);
		}
	}
    
    ofColor myFace = ofColor::fromHsb(0.2, 1.0, 1.0);
    ofPushMatrix();
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    if (useShader) {
        shader.begin();
        shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.1 );
        shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.18 );
        shader.setUniformTexture("imageMask", texture.getTextureReference(), 1);
        
        shader.setUniformMatrix4f("modelview", easyCam.getModelViewMatrix());
        shader.setUniformMatrix4f("projection", easyCam.getProjectionMatrix());
        shader.setUniform2f("screenSize", 1280.0, 720.0);
        shader.setUniform1f("voxelSize", 40.0);
        shader.setUniform3f("myColor", myFace.r, myFace.g, myFace.b);
    }
    ofEnableAlphaBlending();
    ofEnablePointSprites();
    texture.getTextureReference().bind();
    float att[3] = { 1.0, 0.0, 0.0001f };
	glPointSize(100);
	glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
	glPointParameterf(GL_POINT_SIZE_MAX, 256.0f);
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
    mesh.drawVertices();
    ofDisablePointSprites();
    if (useShader) shader.end();
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit() {
	//kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
		case 'a':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case 'z':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case 's':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case 'x':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
        case 'u':
			useShader = !useShader;
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
