#include "testApp.h"

string path = "yakin-hareketli";
int numFrames = 0;
int currentFrame = 1;
bool playing = false;
ofDirectory vert(path);
ofDirectory col(path);
ofSoundPlayer finis;

struct myPoint {
    float x;
    float y;
    float z;
};

struct myColor {
    float r;
    float g;
    float b;
    float a;
};

ofImage myImage;
int snapCounter = 0;

//--------------------------------------------------------------
void testApp::setup() {
    //some path, may be absolute or relative to bin/data
    finis.loadSound("finis-terre.wav");
    finis.play();
    finis.setPaused(!playing);
    
    //only show png files
    vert.allowExt("vert");
    col.allowExt("col");
    //populate the directory object
    vert.listDir();
    col.listDir();
    numFrames = vert.size();
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	nearThreshold = 250;
	farThreshold = 2000;
	
	ofSetFrameRate(30);
	
    shader.load("shaders/noise.vert", "shaders/noise.frag");
    ofDisableArbTex();
    texture.loadImage("particle.png");
    
    posSpeed = 8.0; rotSpeed = 2.0;
    dx = 0.0; dy = 0.0; dz = 0.0;
    drx = 0.0; dry = 0.0;
    ofxGamepadHandler::get()->enableHotplug();
    easyCam.disableMouseInput();
    easyCam.dolly(500.0);
    
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
    
    easyCam.dolly(dz);
    easyCam.truck(dx);
    easyCam.pan(dry);
    easyCam.tilt(drx);
}

//--------------------------------------------------------------
void testApp::draw() {
    if (playing)
        currentFrame++;
    if (currentFrame > numFrames) {
        currentFrame = 1;
        finis.setPositionMS(0);
    }
    
    easyCam.begin();
    drawPointCloud();
    easyCam.end();
	
	// draw instructions
    /*
	stringstream reportStream;
	reportStream << "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >)"
	<< ", fps: " << ofGetFrameRate() << endl;
	ofDrawBitmapString(reportStream.str(),20,652);
     */
}

void testApp::drawPointCloud() {
    glPointSize(40);
    
	ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    const char *fname = (path + "/vertice" + ofToString(currentFrame) + ".vert").c_str();
    ofBuffer shit = ofBufferFromFile(fname, true);
    myPoint *lolo = (myPoint*)shit.getBinaryBuffer();
    
    const char *fname2 = (path + "/color" + ofToString(currentFrame) + ".col").c_str();
    ofBuffer shit2 = ofBufferFromFile(fname2, true);
    myColor *lele = (myColor*)shit2.getBinaryBuffer();
    
    ofVec3f position;
    ofFloatColor color;
    
    for (int i = 0; i < 160*120; i++) {
        position.x = lolo[i].x;
        position.y = lolo[i].y;
        position.z = lolo[i].z;
        
        color.r = lele[i].r;
        color.g = lele[i].g;
        color.b = lele[i].b;
        
        if (position.distance(easyCam.getPosition()) < 1000.0f)
        {
            boids[i].initPos = position;
            boids[i].cn = color;
            boids[i].picture();
        } else {
            boids[i].cn = ofColor::white;
            boids[i].flocking();
        }
        mesh.addColor(boids[i].c);
        mesh.addVertex(boids[i].pos);
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

        case 'u':
			useShader = !useShader;
			break;
			
        case ' ':
            playing = !playing;
            finis.setPaused(!playing);
            break;
        case 'm':
            myImage.grabScreen(0, 0, 1280, 720);
            myImage.saveImage("partOfTheScreen-"+ofToString(snapCounter)+".png");
            snapCounter++;
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
