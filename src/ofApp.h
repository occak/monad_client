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
		ofApp();
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
    
    
    //audio
    void audioOut( float * output, int bufferSize, int nChannels );
    
    void soundChange(string name, int index, float value);
    
    //ui
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    
    //game
    float   costRadius = 0,
            costTexture  = 0,
            costDensity = 0,
            costRotation = 0,
            costMute = 0,
            costMove = 0,
            costSpike = 0,
            costCreate = 0,
            reward = 0;
    
    bool radiusChanged,
    textureChanged,
    densityChanged,
    rotationChanged,
    positionChanged,
    moveChanged,
    moveReset,
    moveAllChanged,
    muteChanged,
    spikeChanged,
    newDisc;
    
    void newUI(int index);
    
    
    ofEasyCam cam;
    
    //TCP
    ofxTCPClient client;
    vector<string> received;
    string title;
    int TCPport = 10005;
    
    //UDP
    ofxUDPManager udpSend;
    ofxUDPManager udpManage;
    vector<string> udpReceived;
    string udpTitle;
    int UDPport = 10003;
    
private:
    
    bool fullScreen;
    bool keyList;
    bool eventList;
    bool costList;
    bool mReleased;
    bool TCPsetup;
    bool timer;
    string IP /*= "127.0.0.1"*/;
    string nick;
    int port;
    int loginMinute;
    float loginSecond;
    float thisRotation;
    
    Groove groove;
    Disc disc;
    Sound sound;
    
    Player* me;
    vector<Player*> otherPlayers;
    
    vector<ofxUICanvas *> ui;
    ofxUICanvas *dashboard;
    ofxUICanvas *addDisc;
    
    
    ofxUICanvas *initialize;
    
    ofxUICanvas *chat;
    string conversation;
    
    ofxUICanvas *history;
    string historyText;
    
};

