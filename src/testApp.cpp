#include "testApp.h"

int currentScene = 0;

bool playing = false;

unsigned long long initialTime = 0;
int frameTime = 0;


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

int snapCounter = 0;

float minx = -600.0f;
float maxx = 900.0f;
float miny = -1000.0f;
float maxy = 815.0f;
float minz = 1700.0f;
float maxz = 2800.0f;

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_WARNING);
	
	nearThreshold = 250;
	farThreshold = 2000;
	
	ofSetFrameRate(30);
	
    shader.load("shaders/noise.vert", "shaders/noise.frag");
    ofDisableArbTex();
    texture.loadImage("particle.png");
    
    posSpeed = 16.0; rotSpeed = 4.0;
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
    ofSetBackgroundAuto(true);
    
    scenes.push_back(Scene("take-01"));
    scenes.push_back(Scene("take-02"));
    scenes.push_back(Scene("take-03"));
    scenes.push_back(Scene("take-08"));
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
    ofSetBackgroundAuto(true);
    if (playing)
    {
        frameTime = (int) (ofGetSystemTimeMicros() - initialTime)/1000;
        // currentFrame++;
        // myImage[currentFrame%8].grabScreen(0, 0, 1280, 720);
        // myImage[currentFrame%8].saveThreaded("exported-frames/" + ofToString(currentFrame) + ".png");
    }
    if (frameTime > scenes[currentScene].vertices[scenes[currentScene].currentFrame])
    {
        scenes[currentScene].currentFrame++;
    }
    if (scenes[currentScene].currentFrame > scenes[currentScene].numFrames) {
        scenes[currentScene].currentFrame = 1;
        initialTime = ofGetSystemTimeMicros();
    }
    
    easyCam.begin();
    drawPointCloud();
    easyCam.end();
	
	// draw instructions
    /*
	stringstream reportStream;
	reportStream << "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >)"
	<< "fps: " << ofGetFrameRate() << endl
    << "time: " << frameTime << endl
    << "frame: " << currentFrame << endl
    << "vertik: " << vertices[currentFrame] << endl
    << "kamera: " << easyCam.getPosition() << endl
    << minx << " - " << maxx << " - " << miny << " - " << maxy << " - " << minz << " - " << maxz << endl;
    
	ofDrawBitmapString(reportStream.str(),20,632);
    */
}

void testApp::drawPointCloud() {
	ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    ofMesh lines;
    lines.setMode(OF_PRIMITIVE_POINTS);
    
    const char *fname = (scenes[currentScene].path + "/vertice" + ofToString(scenes[currentScene].vertices[scenes[currentScene].currentFrame % scenes[currentScene].numFrames]) + ".vert").c_str();
    ofBuffer temp = ofBufferFromFile(fname, true);
    myPoint *fposition = (myPoint*)temp.getBinaryBuffer();
    
    const char *fname2 = (scenes[currentScene].path + "/color" + ofToString(scenes[currentScene].vertices[scenes[currentScene].currentFrame % scenes[currentScene].numFrames]) + ".col").c_str();
    ofBuffer temp2 = ofBufferFromFile(fname2, true);
    myColor *fcolor = (myColor*)temp2.getBinaryBuffer();
    
    ofVec3f position;
    ofFloatColor color;
    
    for (int i = 0; i < 160*120; i++) {
        position.x = fposition[i].x;
        position.y = fposition[i].y;
        position.z = fposition[i].z;
        
        color.r = fcolor[i].r;
        color.g = fcolor[i].g;
        color.b = fcolor[i].b;
        
        bool condition = currentScene == 2 ? position.x < maxx &&
        position.x > minx &&
        position.y < maxy &&
        position.y > miny &&
        position.z < maxz &&
        position.z > minz : position.z < 1500.0f;
        
        if (condition && position.length() > 10.0f)
        {
            color.a = 255;
            boids[i].initPos = position;
            boids[i].cn = color;
            boids[i].picture();
            lines.addVertex(position);
            lines.addColor(ofColor(ofColor::blueSteel, 20));
            
        } else {
            boids[i].cn = ofColor(255,255,255,0);
            boids[i].flocking(attractor());
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
    //lines.drawVertices();
    if (useShader) shader.end();
    ofPopMatrix();
}

ofVec3f testApp::attractor()
{
    float t = frameTime * 0.0001;
    
    float x = (sin(t*3)+sin(t*11)) * 1000.0f;
    float y = (sin(t*5)+sin(t*13)) * 1000.0f;
    float z = (sin(t*7)+sin(t*17)) * 1000.0f + 1000.0f;
    
    return ofVec3f(x,y,z);
}

//--------------------------------------------------------------
void testApp::exit() {

}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
        case '1':
            currentScene = 0;
            break;
        case '2':
            currentScene = 1;
            break;
        case '3':
            currentScene = 2;
            break;
        case '4':
            currentScene = 3;
            break;
		case 'a':
			minx +=10.0f;
			break;
			
		case 'z':
			minx -=10.0f;;
			break;
			
		case 's':
			maxx +=10.0f;;
			break;
			
		case 'x':
			maxx-=10.0f;;
			break;
            
        case 'd':
			miny+=10.0f;;
			break;
			
		case 'c':
			miny-=10.0f;;
			break;
            
        case 'f':
			maxy+=10.0f;;
			break;
			
		case 'v':
			maxy-=10.0f;;
			break;
            
        case 'g':
			minz+=10.0f;;
			break;
			
		case 'b':
			minz-=10.0f;;
			break;
            
        case 'h':
			maxz+=10.0f;;
			break;
			
		case 'n':
			maxz-=10.0f;;
			break;

        case 'u':
			useShader = !useShader;
			break;
			
        case ' ':
            playing = !playing;
            initialTime = ofGetSystemTimeMicros();
            break;
        
        case 'p':
            //myImage[currentFrame%8].grabScreen(0, 0, 1280, 720);
            //myImage[currentFrame%8].saveThreaded("exported-frames/" + ofToString(currentFrame) + ".png");
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
