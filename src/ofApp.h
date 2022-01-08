#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <string> 
#include "Particle.h"
#include "ParticleEmitter.h"

typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;

//  Shape base class
//
class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside() { return false; }

	glm::vec3 pos;
	float rotation = 0.0;
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);

	vector<glm::vec3> verts;
};

//  TriangleShape
//
class TriangleShape : public Shape {
public:
	
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	void draw();
	ofImage spriteImage;
	// Get heading vector for the ship 
	//
	glm::vec3 heading() {
		
		return glm::normalize(glm::vec3(cos(glm::radians(rotation + 270)) * 10, 
			sin(glm::radians(rotation + 270)) * 10,0));


	}

	// Physics data goes here  (for integrate() );
	//
	ofVec3f thrust;
	ofVec3f accel;
	float damping;
	float mass;
	ofVec3f force;

	//  Integrator Function 
	//
	void integrate() {
		
		
		ofVec3f velocity = thrust * heading();
		//cout << velocity << endl;
		//cout << heading() << endl;
		// (1) update position from velocity and time interval
		// (2) update velocity (based on acceleration
		// (3) multiply final result by the damping factor to sim drag
		//
	
		float fr = ofGetFrameRate();
		float dt = 1.0 / fr;
		//cout << velocity << endl;
		pos += velocity ;
		velocity += accel * dt;
		velocity = velocity * damping;
	}
};

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	ofVec2f trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	void update();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;	
	float width, height;  
	ofVec3f heading;
	glm::vec3 pos;
	float cycles;
	float scale;
	 
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	vector<Sprite> sprites;
	int removeNear(ofVec3f point, float dist);
	//glm::vec3 curveEval(float x, float scale, float cycles);
	ofSoundPlayer explosionSound;
	ParticleEmitter emitter;
};

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public BaseObject {
public:
	//TriangleShape triangleRef;
	Emitter(SpriteSystem*);
	virtual void move() {};
	glm::vec3 heading;
	void draw();
	void start();
	void stop();
	void setLifespan(float);    // in milliseconds
	void setVelocity(ofVec3f);  // pixel/sec
	void setChildImage(ofImage);
	void setChildSize(float w, float h) { childWidth = w; childHeight = h; }
	void setImage(ofImage);
	void setRate(float);
	float maxDistPerFrame();
	void update();
	SpriteSystem* sys;
	float rate;
	ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	float childWidth, childHeight;
	void launch();
	void shoot();
	ofSoundPlayer firingSound;
};


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void checkCollisions();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	Emitter* invader, * invader2, * invader3, * invader4, * turret;
	vector<Emitter*> invaders;
	int numEmitters;

	ofImage spriteImage;
	ofImage backgroundImage;
	ofImage laserImage;
	ofImage invaderImage;


	ofSoundPlayer firingSound;
	ofSoundPlayer explosionSound;
	ofSoundPlayer musicSound;
	
	ofVec3f mouse_last;
	bool imageLoaded;
	bool bIdle;
	bool bHide;
	bool gOver;

	ofxFloatSlider rate;
	ofxFloatSlider thrust;
	ofxFloatSlider life;
	ofxVec3Slider velocity;
	ofxToggle drawHeading;
	ofxLabel screenSize;
	ofxToggle drawPaths;
	ofxFloatSlider scale;
	ofxFloatSlider cycles;


	glm::vec3 curveEval(float x, float scale, float cycles);
	glm::vec3 heading;

	// application data

	glm::vec3 lastMouse;   // location of where mouse moved last (when dragging)
	TriangleShape tri = TriangleShape(glm::vec3(-20, 20, 0), glm::vec3(0, -40, 0), glm::vec3(20, 20, 0));

	bool draggable = false;		// indicates that the triangle can be dragged
	glm::vec3 mouseLast;		// the last position of the mouse 

	ofxPanel gui;
	ofxToggle useImage;

	int score = 0;
	string scoreString;

	//forces
	TurbulenceForce* turbForce;
	GravityForce* gravityForce;
	ImpulseRadialForce* radialForce;

	ofxVec3Slider turbMin;
	ofxVec3Slider turbMax;
	ofxFloatSlider mass;
	ofxFloatSlider radialForceVal;
	ofxFloatSlider height;

	ofxFloatSlider gravity;
	ofxFloatSlider damping;
	ofxFloatSlider radius;
	ofxVec3Slider particleVelocity;
	ofxFloatSlider particleLifespan;
	ofxFloatSlider particleRate;

	ParticleEmitter emitter;
	

	//font

	ofTrueTypeFont	timerFont;
	ofTrueTypeFont	gameShark30;
	int timer;
};
