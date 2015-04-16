#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "Groove.h"
#include "Disc.h"
#include "Sound.h"
#include "ofxNetwork.h"
#include "Player.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    float triangleWave(float frequency);
    float squareWave(float frequency);
    float sawWave(float frequency);
    
    //audio
    void audioOut( float * output, int bufferSize, int nChannels );
    double phase;
    float volume;
    
    void soundChange(string name, int index, float value);
    
    //ui
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    
    //game
    float costRadius, costTexture, costDensity, costRotation, costMute;
    bool radiusChanged, textureChanged, densityChanged, rotationChanged, positionChanged = false;
		
    
    ofEasyCam cam;
    
    //TCP & udp
    ofxUDPManager receiver;
    ofxTCPClient client;
    vector<string> received;
    string title;
    
    
private:
    
    bool fullScreen;
    bool mReleased;
    
    Groove groove;
    Disc disc;
    Sound sound;
    
    Player* me;
    vector<Player*> otherPlayers;
    
    vector<ofxUICanvas *> ui;
//    vector<ofxUILabelToggle *> updateButtons;
    ofxUICanvas *updateButtons;
    
    ofxUICanvas *chat;
    string conversation;


};

