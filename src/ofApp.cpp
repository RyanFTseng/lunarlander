#include "ofApp.h"
glm::mat4 T;

void TriangleShape::draw() {
	glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(pos));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1.0), this->scale);

	T = translate * rot * scale;

	//ofSetColor(ofColor::darkBlue);

	ofPushMatrix();

	ofMultMatrix(T);
	ofDrawTriangle(verts[0], verts[1], verts[2]);
	spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
	ofPopMatrix();
}

BaseObject::BaseObject() {
	trans = ofVec3f(0, 0, 0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(ofVec3f pos) {
	trans = pos;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 20;
	height = 20;
}


// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {
	

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0 + trans.x, -height / 2.0 + trans.y);
	}

	else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + trans.x, -height / 2.0 + trans.y, width, height);
	}

	

}



//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
	
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		
		//sprites[i].heading = glm::normalize(curveEval(sprites[i].pos.x + sprites[i].speed, sprites[i].scale, sprites[i].cycles) - sprites[i]. pos);
		sprites[i].trans += sprites[i].velocity/ ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

// remove all sprites within a given dist of point, return number removed
//
int SpriteSystem::removeNear(ofVec3f point, float dist) {
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;
	int count = 0;

	while (s != sprites.end()) {
		ofVec3f v = s->trans - point;
		if (v.length() < dist) {
			emitter.setPosition(s->trans );
			tmp = sprites.erase(s);
			explosionSound.play();
			emitter.sys->reset();

			emitter.start();
			
			count++;
			s = tmp;

		}
		else s++;
	}
	return count;
}

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem* spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;

	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	childWidth = 10;
	childHeight = 10;
}

//  Draw the Emitter 
void Emitter::draw() {
	// draw sprite system
	sys->draw();
}

//shoot function for the turret
void Emitter::shoot() {
	float time = ofGetElapsedTimeMillis();
	
	if ((time - lastSpawned) > (1000.0 / rate)) {
		// spawn a new sprite
		Sprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		firingSound.play();
		sprite.setPosition(trans+ (0,-20,0));
		
		sprite.birthtime = time;
		sys->add(sprite);
		lastSpawned = time;
		sys->update();
	}

}

void Emitter::launch() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0 / rate)) {
		// spawn a new sprite
		Sprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = ofVec3f(ofRandom(-35,35),ofRandom(500,1000),velocity.z);
		float path = ofRandom(0, 10);
		ofVec3f v = sprite.velocity;
		sprite.lifespan = lifespan;
		sprite.setPosition(trans);
		sprite.birthtime = time;
		sys->add(sprite);
		lastSpawned = time;
	}
	sys->update();
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;


	sys->update();
}

// Start/Stop the emitter.
//
void Emitter::start() {
	if (!started) {
		started = true;
		lastSpawned = ofGetElapsedTimeMillis();
	}
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
	childWidth = img.getWidth();
	childHeight = img.getHeight();
}

void Emitter::setImage(ofImage img) {
	image = img;
}

float Emitter::maxDistPerFrame() {
	return  velocity.length() / ofGetFrameRate();
}

void Emitter::setRate(float r) {
	rate = r;
}



 
void ofApp::setup() {
	
	// load sprite image(s), exit with error if we can't
	//defaultImage
	string spriteFile = "images/ship.png";
	if (!spriteImage.load(spriteFile)) {
		cout << "ERROR: Can't load image file: " << spriteFile << endl;
		ofExit();
	}
	
	string backgroundFile = "images/background.png";
	if (!backgroundImage.load(backgroundFile)) {
		cout << "ERROR: Can't load image file: " << backgroundFile << endl;
		ofExit();
	}

	string laserFile = "images/laser.png";
	if (!laserImage.load(laserFile)) {
		cout << "ERROR: Can't load image file: " << laserFile << endl;
		ofExit();
	}

	string invaderFile = "images/invader.png";
	if (!invaderImage.load(invaderFile)) {
		cout << "ERROR: Can't load image file: " << invaderFile << endl;
		ofExit();
	}

	//load sound files
	string firingFile = "sounds/shoot.wav";
	if (!firingSound.load(firingFile)) {
		cout << "ERROR: Can't load audio file: " << firingFile << endl;
		ofExit();
	}
	firingSound.setVolume(0.2f);

	string explosionFile = "sounds/explosion.wav";
	if (!explosionSound.load(explosionFile)) {
		cout << "ERROR: Can't load audio file: " << explosionFile << endl;
		ofExit();
	}
	explosionSound.setVolume(0.2f);
	
	string musicFile = "sounds/bgm.mp3";
	if (!musicSound.load(musicFile)) {
		cout << "ERROR: Can't load audio file: " << musicFile << endl;
		ofExit();
	}
	musicSound.setLoop(true);
	musicSound.setVolume(0.2f);

	gameShark30.load("fonts/Game Shark.otf", 30, true, true);
	gameShark30.setLineHeight(34.0f);
	gameShark30.setLetterSpacing(1.035);

	timerFont.load("fonts/Game Shark.otf", 30, true, true);
	timerFont.setLineHeight(34.0f);
	timerFont.setLetterSpacing(1.035);

	tri.pos = ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() - 400 / 2.0, 0);
	
	turret = new Emitter(new SpriteSystem());
	tri.spriteImage = spriteImage;
	turret->setPosition(ofVec3f(tri.pos[0], tri.pos[1],0));

	turret->setChildImage(laserImage);
	turret->firingSound = firingSound;
	invader = new Emitter(new SpriteSystem());
	invader->setChildImage(invaderImage);
	invader->setPosition(ofVec3f(ofGetWindowWidth() / 2, 10, 0));
	invader->velocity.set(0,400,0);
	invader->setLifespan(3000);
	invader->setRate(0.5);
	invader->sys->explosionSound = explosionSound;

	invader2 = new Emitter(new SpriteSystem());
	invader2->setChildImage(invaderImage);
	//invader2->setPosition(ofVec3f(0, ofGetWindowHeight()/2, 0));
	invader2->setPosition(ofVec3f(ofGetWindowWidth() / 4, 10, 0));
	invader2->velocity.set(tri.pos*tri.heading());
	invader2->setLifespan(3000);
	invader2->setRate(0.5);
	invader2->sys->explosionSound = explosionSound;

	invader3 = new Emitter(new SpriteSystem());
	invader3->setChildImage(invaderImage);
	invader3->setPosition(ofVec3f(ofGetWindowWidth()*0.75, 10, 0));
	invader3->velocity.set(0, 400, 0);
	invader3->setLifespan(3000);
	invader3->setRate(0.5);
	invader3->sys->explosionSound = explosionSound;

	invader4 = new Emitter(new SpriteSystem());
	invader4->setChildImage(invaderImage);
	invader4->setPosition(ofVec3f(ofGetWindowWidth() *0.65, 10, 0));
	invader4->velocity.set(0, 400, 0);
	invader4->setLifespan(3000);
	invader4->setRate(0.5);
	invader4->sys->explosionSound = explosionSound;

	gui.setup();
	gui.add(rate.setup("rate", 10, 1, 10));
	gui.add(life.setup("life", 2.5, .1, 10));
	gui.add(drawHeading.setup("Draw heading", false));
	gui.add(velocity.setup("velocity", ofVec3f(0, -500, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(thrust.setup("Thrust", 1, 1, 10));
	gui.add(drawPaths.setup("Draw path", false));
	gui.add(scale.setup("Scale", 200, 1, 400));
	gui.add(cycles.setup("Cycles", 4, 1, 10));
	gui.add(particleVelocity.setup("Particle Velocity", ofVec3f(100, 100, 0), ofVec3f(0, 0, 0), ofVec3f(100, 100, 100)));
	gui.add(damping.setup("Damping", .99, .1, 1.0));
	gui.add(gravity.setup("Gravity", 10, 1, 20));
	gui.add(radius.setup("Radius", .05, .01, .3));
	gui.add(turbMin.setup("Turbulence Min", ofVec3f(0, 0, 0), ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20)));
	gui.add(turbMax.setup("Turbulence Max", ofVec3f(0, 0, 0), ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20)));
	gui.add(radialForceVal.setup("Radial Force", 1000, 100, 5000));
	gui.add(height.setup("Clamping", 10, 0, 100));
	gui.add(particleRate.setup("Rate", 1.0, .5, 60.0));
	bHide = true;
	bIdle = true;
	gOver = false;
	// set up the emitter forces
	//
	turbForce = new TurbulenceForce(ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20));
	gravityForce = new GravityForce(ofVec3f(0, -gravity, 0));
	radialForce = new ImpulseRadialForce(1000.0);


	emitter.sys->addForce(turbForce);
	emitter.sys->addForce(gravityForce);
	emitter.sys->addForce(radialForce);


	emitter.setVelocity(ofVec3f(particleVelocity->x, particleVelocity->y, particleVelocity->z ));
	emitter.setOneShot(true);
	emitter.setEmitterType(RadialEmitter);
	emitter.setGroupSize(50);

	invader->sys->emitter = emitter;
	invader2->sys->emitter = emitter;
	invader3->sys->emitter = emitter;
	invader4->sys->emitter = emitter;
}

//--------------------------------------------------------------
void ofApp::update() {
	ofSeedRandom();
	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds 
	turret->setVelocity(tri.heading()*-velocity->y);
	turret->update();
	//spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
	
	turret->setPosition(ofVec3f(tri.pos[0] , tri.pos[1]));
	invader->launch();
	invader2->launch();
	invader3->launch();
	invader4->launch();
	//for (int i = 0; i < invader2->sys->sprites.size(); i++) {
	//	invader2->sys->sprites[i].heading =
			//glm::normalize(curveEval(invader2->sys->sprites[i].pos.x 
				//+ invader2->sys->sprites[i].speed, invader2->sys->sprites[i].scale, invader2->sys->sprites[i].cycles)
			//	- invader2->sys->sprites[i].pos);
		//invader2->sys->sprites[i].trans += invader2->sys->sprites[i].heading * invader2->sys->sprites[i].speed;

		//cout << invader2->sys->sprites[i].speed << endl;
	//}
	
	//boundary check
	if (tri.pos.x < 20 ) {
		//cout <<"pos "<< tri.pos.x << endl;
		tri.pos.x = 25;
		tri.thrust = ofVec3f(0, 0, 0);
	}
	if (tri.pos.x >ofGetWidth()-20) {
		//cout << "pos " << tri.pos.x << endl;
		tri.pos.x = ofGetWidth() - 25;
		tri.thrust = ofVec3f(0, 0, 0);
	}
	if (tri.pos.y < 20) {
		//cout << "pos " << tri.pos.x << endl;
		tri.pos.y = 30;
		tri.thrust = ofVec3f(0, 0, 0);
	}
	if (tri.pos.y > ofGetHeight() - 20) {
		//cout << "pos " << tri.pos.x << endl;
		tri.pos.y = ofGetHeight() - 25;
		tri.thrust = ofVec3f(0, 0, 0);
	}
	
	tri.integrate(); 

	checkCollisions();
	
	
	invader->sys->emitter.update();
	invader2->sys->emitter.update();

	invader3->sys->emitter.update();
	invader4->sys->emitter.update();
}

//  This is a simple O(M x N) collision check
//  For each missle check to see which invaders you hit and remove them
//
void ofApp::checkCollisions() {

	// find the distance at which the two sprites (missles and invaders) will collide
	// detect a collision when we are within that distance.
	//
	float collisionDist = turret->childHeight / 2 + invader->childHeight / 2;
	float collisionDist2 = turret->childHeight / 2 + invader2->childHeight / 2;
	// Loop through all the missiles, then remove any invaders that are within
	// "collisionDist" of the missiles.  the removeNear() function returns the
	// number of missiles removed.
	//


	for (int i = 0; i < turret->sys->sprites.size(); i++) {
		glm::vec4 transPoint = glm::vec4(turret->sys->sprites[i].trans.x, turret->sys->sprites[i].trans.y,0,0) ;
		glm::vec2 point = glm::vec2(transPoint);
		score += invader->sys->removeNear(point, collisionDist);
		score += invader2->sys->removeNear(point, collisionDist2);
		score += invader3->sys->removeNear(point, collisionDist2);
		score += invader4->sys->removeNear(point, collisionDist2);
	}
}


//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(ofColor::white);

	backgroundImage.draw(0, 0);
	
	ofSetColor(ofColor::white);
	invader->draw();
	invader2->draw();
	invader3->draw();
	invader4->draw();
	
	if (drawPaths) {
		for (int i = 0; i < ofGetWidth(); i++) {
			glm::vec3 p = curveEval(i, scale, cycles);
			ofDrawCircle(p.x, p.y, 1);
		}
	}


	ofSetColor(ofColor::white);
	
	
	turret->draw();
	tri.draw();
	
	int t = (int)ofGetElapsedTimef();
	invader->sys->emitter.draw();
	invader2->sys->emitter.draw();
	invader3->sys->emitter.draw();
	invader4->sys->emitter.draw();
	emitter.draw();
	ofSetColor(ofColor::white);
	// draw heading vector
	//
	if (drawHeading) {
		ofSetColor(ofColor::red);
		ofDrawLine(tri.pos, tri.pos + tri.heading()*100);

	}

	if (bIdle) {
		ofSetColor(ofColor::black);
		gameShark30.drawString("Press Space to Play!", (ofGetWidth() / 2) - 200, (ofGetHeight() / 2) - 50);
		
	}
	else {
		
		ofSetColor(ofColor::black);
		gameShark30.drawString("Score: " + std::to_string(score), 10, 50);
		timerFont.drawString("Time: " + std::to_string(t), (ofGetWidth() -300), 50);
		
	}
	if (t == 30) {
		gOver = true;
		bIdle = true;
		timerFont.drawString("GAME OVER" + std::to_string(t), (ofGetWidth() / 2) - 200, (ofGetHeight() / 2) - 50);
		tri.thrust = ofVec3f(0, 0, 0);
		tri.rotation = 0;
		tri.pos = ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() - 400 / 2.0, 0);
		invader->stop();
		invader2->stop();
		invader3->stop();
		invader4->stop();
		for (int i = 0; i < invader->sys->sprites.size(); i++) {
			invader->sys->remove(i);
		}
		for (int i = 0; i < invader2->sys->sprites.size(); i++) {
			invader2->sys->remove(i);
		}
		for (int i = 0; i < invader3->sys->sprites.size(); i++) {
			invader3->sys->remove(i);
		}
		for (int i = 0; i < invader4->sys->sprites.size(); i++) {
			invader4->sys->remove(i);
		}
		ofResetElapsedTimeCounter();
	}
	cout << timer << endl;
	if (!bHide) {
		gui.draw();
	}
	
}

// Given x in pixel coordinates, return (x, y, z) on the sin wave
// Note that "z" is not used, so it is set to "0".
//
// Additional Parameters
//    scale - scales the curve in Y  (the amplitude)
//    cycles - number of cycles in sin wave.
//
glm::vec3 ofApp::curveEval(float x, float scale, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (cycles * x * PI) / ofGetWidth();
	return (glm::vec3(x, -scale * sin(u) + (ofGetHeight() / 2), 0));
}


// inside() test method 
bool TriangleShape::inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	glm::vec3 v1 = glm::normalize(p1 - p);
	glm::vec3 v2 = glm::normalize(p2 - p);
	glm::vec3 v3 = glm::normalize(p3 - p);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (draggable) {										// if the triangle can be dragged
		glm::vec3 mousePoint = glm::vec3(x, y, 0);			// get the coordinates of the mouse point
		glm::vec3 difference = mousePoint - mouseLast;		// calculate the difference between the current location and the previous location of the mouse
		tri.pos += difference;								// add the difference to the triangle's position
		mouseLast = mousePoint;								// update the last position of the mouse
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	glm::vec3 p1 = T * glm::vec4(tri.verts[0], 1);
	glm::vec3 p2 = T * glm::vec4(tri.verts[1], 1);
	glm::vec3 p3 = T * glm::vec4(tri.verts[2], 1);

	if (tri.inside(glm::vec3(x, y, 0), p1, p2, p3) == true) {
		draggable = true;					// indicate that the triangle can be dragged
		mouseLast = glm::vec3(x, y, 0);		// set the last position of the mouse

	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	draggable = false;		// disable dragging of the triangle once the mouse is released
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

void ofApp::keyPressed(int key) {
	
	if (!bIdle) {
		switch (key) {
		case 'C':
		case 'c':
			break;
		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
		case 'H':
		case 'h':
			bHide = !bHide;
			break;
		case 'r':
			break;
		case 's':
			break;
		case 'u':
			break;
		case OF_KEY_ALT:
			break;
		case OF_KEY_CONTROL:
			break;
		case OF_KEY_SHIFT:
			break;
		case OF_KEY_DEL:
			break;
		case ' ':
			turret->shoot();
			
			break;
		case 'Z':
		case 'z':
			
			break;
		case 'X':
		case 'x':
			
			break;

		case OF_KEY_UP:
			tri.thrust += .5;
			break;
		case OF_KEY_DOWN:
			tri.thrust -= .5;
			break;

		case OF_KEY_LEFT:
			tri.rotation -= 20;
			break;
		case OF_KEY_RIGHT:
			tri.rotation += 20;
			break;
		}
	}
	if (bIdle) {
		//start game if idle
		switch (key) {
		case ' ':
			
			invader->start();
			turret->start();
			invader2->start();
			invader3->start();
			invader4->start();
			bIdle = false;
			musicSound.play();

			ofResetElapsedTimeCounter();
			timer = ofGetElapsedTimef();
			break;
		}
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_UP:     // go forward
		if (tri.thrust.length() > 0) {
			tri.thrust -= 1;
		}
		break;
	case OF_KEY_DOWN:   // go backward
		if (tri.thrust.length() < 0) {
			tri.thrust += 1;
		}
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

