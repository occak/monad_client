#include "ofApp.h"

/*
 
 */

ofApp::ofApp() {
    moveChanged = false;
    TCPsetup = false;
    timer = true;
    radiusChanged = false;
    textureChanged = false;
    densityChanged = false;
    rotationChanged = false;
    positionChanged = false;
    moveChanged = false;
    moveReset = false;
    muteChanged = false;
    spikeChanged = false;
    newDisc = false;
    me = NULL;
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetDataPathRoot("../Resources/data/");
    
    ofSetVerticalSync(true);
    ofBackground(255);
    ofSetFrameRate(30);
    
    if( me == NULL){
        
        // set up values of objects
        disc.setup();
        
        //set up audio stream & synth network
        ofSoundStreamSetup(2, 0); // 2 out, 0 in
        sound.setup();
        
        
        //set up gui
        initialize = new ofxUICanvas();
        initialize->setPosition(ofGetWidth()/2-155, ofGetHeight()/2-200);
        initialize->setFont(OF_TTF_MONO);
        initialize->setDimensions(310, 200);
        initialize->addTextArea("welcome", "Welcome to Monad (v0.2)", 0);
        initialize->addLabel("Server IP :");
        initialize->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        initialize->addTextInput("IP", "");
        initialize->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        initialize->addLabel("Nickname  :");
        initialize->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        initialize->addTextInput("nick", "");
        initialize->autoSizeToFitWidgets();
        ofAddListener(initialize->newGUIEvent, this, &ofApp::guiEvent);
        
        dashboard = new ofxUICanvas();
        dashboard->setFont(OF_TTF_MONO);
        dashboard->setPosition(ofGetWidth()/2-125, ofGetHeight()-30);
        dashboard->addMultiImageToggle("inner", "butonlar/buton-06.png", false, 20, 20, OFX_UI_ALIGN_LEFT);
        dashboard->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        dashboard->addMultiImageToggle("outer", "butonlar/buton-07.png",false, 20, 20);
        dashboard->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        dashboard->addToggle("move all", false);
        dashboard->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        dashboard->addButton("reset all", false);
        dashboard->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        dashboard->addLabelToggle("chat", true);
        dashboard->autoSizeToFitWidgets();
        dashboard->setVisible(false);
        ofAddListener(dashboard->newGUIEvent, this, &ofApp::guiEvent);
        
        addDisc = new ofxUICanvas();
        addDisc->setFont(OF_TTF_MONO);
        addDisc->addSpacer();
        addDisc->addLabel("new");
        ofxUILabel *label = (ofxUILabel*) addDisc->getWidget("new");
        addDisc->addWidgetPosition(label, OFX_UI_WIDGET_POSITION_RIGHT, OFX_UI_ALIGN_CENTER);
        addDisc->addMultiImageToggle("add", "butonlar/addbutton.png", false, 50, 50);
        ofxUIToggle *toggle = (ofxUIToggle*) addDisc->getWidget("add");
        addDisc->addWidgetPosition(toggle, OFX_UI_WIDGET_POSITION_DOWN, OFX_UI_ALIGN_CENTER);
        addDisc->setVisible(false);
        ofAddListener(addDisc->newGUIEvent, this, &ofApp::guiEvent);
        
        chat = new ofxUICanvas();
        chat->setFont(OF_TTF_MONO);
        chat->setDrawBack(false);
        int chatWidth = 700;
        chat->setPosition(addDisc->getGlobalCanvasWidth()+100, 0);
        chat->setDimensions(chatWidth, ofGetHeight());
        chat->setColorFill(ofxUIColor(50,50,50,150));
        conversation = "";
        chat->addTextInput("chatInput", "", OFX_UI_FONT_SMALL)->setAutoUnfocus(false);
        chat->addTextArea("chat", "", OFX_UI_FONT_SMALL);
        if (me == NULL) chat->setVisible(false);
        chat->autoSizeToFitWidgets();
        ofAddListener(chat->newGUIEvent, this, &ofApp::guiEvent);
        
        history = new ofxUICanvas();
        history->setDrawBack(false);
        history->setFont(OF_TTF_MONO);
        history->setColorFill(ofxUIColor(25));
        history->setPosition(0, ofGetHeight()/2 - 50);
        history->setDimensions(dashboard->getGlobalCanvasWidth()+250, ofGetHeight()/2 + 70);
        historyText = "";
        history->addTextArea("history", historyText, OFX_UI_FONT_SMALL);
        if (me == NULL) history->setVisible(false);
        history->autoSizeToFitWidgets();
    }
    
    if( me != NULL){
        
        // set up values of objects
        //        disc.setup();
        
        //give player colors to UI
        for(int i = 0; i < disc.getDiscIndex(); i++){
            ui[i]->setColorBack(me->getColor());
        }
        
        addDisc->setColorBack(me->getColor());
        dashboard->setColorBack(me->getColor());
        
        chat->setVisible(true);
        if(me->getDiscIndex() == -1) addDisc->setVisible(true);
        history->setVisible(true);
        
    }
    
    
    
}
//--------------------------------------------------------------
void ofApp::exit(){
    
    if(TCPsetup){
        //index = -1
        if( me->getDiscIndex() != -1) me->setDiscIndex(-1);
        string changeDisc = "otherPlayersIndex//";
        changeDisc += "IP: "+ofToString(me->getIP()) + "//";
        changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
        client.send(changeDisc);
        
        //close connection
        me->setConnection(false);
        string playerOut = "goodbye//"+me->getNick();
        client.send(playerOut);
        
        //this part crashes at exit!
        
        //deleting ui elements
        //        for(int i = 0; i < disc.getDiscIndex(); i++){
        //            delete ui[i];
        //        }
        //        delete dashboard;
        //        delete chat;
        //        delete history;
    }
    delete initialize;
    
}
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if(TCPsetup == false){
        if(e.getName() == "IP"){
            ofxUITextInput *text = (ofxUITextInput *) e.widget;
            if(text->getTextString() != ""){
                IP = text->getTextString();
            }
        }
        if(e.getName() == "nick"){
            ofxUITextInput *text = (ofxUITextInput *) e.widget;
            if(text->getTextString() != ""){
                nick = text->getTextString();
            }
        }
        if (IP != "" && nick != "") {
            
            client.setup(IP, 10002);
            client.setMessageDelimiter("vnet");
            
            Player* _player = new Player();
            _player->setNick(nick);
            me = _player;
            
            // ask for server state
            if(client.isConnected()) client.send("hello//"+me->getNick());
        }
    }
    
    else{
        for(int i = 0; i < disc.getDiscIndex(); i++){
            if(e.getName() == "rotation" + ofToString(i+1)){
                
                ofxUISlider *slider = e.getSlider();
                float changeRotation = slider->getScaledValue()-disc.getNetRotationSpeed(i);
                
                
                if(me->getLife() >= abs(costRotation) && mReleased == false && disc.getTexture(i) != 0 && changeRotation != 0) {
                    rotationChanged = true;
                    disc.setRotationSpeed(i, changeRotation);
                    
                    //change sound
                    float netSpeed = abs(disc.getNetRotationSpeed(i));
                    float frequency;
                    if(netSpeed <= 5){
                        frequency = ofMap(netSpeed, 0, 5, 50, 250);
                    }
                    else frequency = ofMap(netSpeed, 5, 10, 250, 800);
                    float beatSpeed = ofMap(netSpeed, 0, 10, 0, 200);
                    float beatDensity = ofMap(disc.getDensity(i), 1, 30, 15, 2);
                    soundChange("freq", i, frequency);
                    soundChange("bpm", i, beatSpeed*beatDensity);
                    
                    
                    
                }
                else if (disc.getTexture(i) == 0) slider->setValue(0);
            }
            else if(e.getName() == "radius" + ofToString(i+1)){
                ofxUISlider *slider = e.getSlider();
                float currentSize = disc.getThickness(i);
                
                if(me->getLife() >= abs(costRadius) && mReleased == false && slider->getScaledValue() != currentSize) {
                    radiusChanged = true;
                    disc.setThickness(i, slider->getScaledValue());
                    
                    //change sound
                    float q = ofMap(disc.getThickness(i), 15., 100., 3., 0.);
                    soundChange("q", i, q);
                }
            }
            
            else if(e.getName() == "density" + ofToString(i+1)){
                ofxUISlider *slider = e.getSlider();
                float currentDensity = disc.getDensity(i);
                
                if(me->getLife() >= abs(costDensity) && mReleased == false && (int) slider->getScaledValue() != currentDensity) {
                    densityChanged = true;
                    disc.setDensity(i, slider->getScaledValue());
                    
                    //change envelope
                    float envelopeCoeff = ofMap(disc.getDensity(i), 1, 30, 1, 5);
                    float pulseRatio = ofMap(disc.getDensity(i), 1, 30, 0.005, 1);
                    soundChange("envelopeWidth", i, envelopeCoeff);
                    //                    soundChange("pulseLength", i, pulseRatio);
                    
                    //change metronome
                    float netSpeed = abs(disc.getNetRotationSpeed(i));
                    float beatSpeed = ofMap(netSpeed, 0, 10, 0, 200);
                    float beatDensity = ofMap(disc.getDensity(i), 1, 30, 15, 2);
                    soundChange("bpm", i, beatSpeed*beatDensity);
                }
            }
            else if(e.getName() == "spike" + ofToString(i+1)){
                
                ofxUISlider *slider = e.getSlider();
                float spikeHeight = disc.getSpikeDistance(i);
                
                if(me->getLife() >= abs(costSpike) && mReleased == false && slider->getScaledValue() != spikeHeight) {
                    spikeChanged = true;
                    disc.setSpikeDistance(i, slider->getScaledValue());
                    
                    //change gain
                    float distAmount = ofMap(disc.getSpikeDistance(i), 0., 100., 1., 10., true);
                    float clipBalance = ofMap(distAmount, 1., 10., 1., 0.5);
                    soundChange("clipBalance", i, clipBalance);
                    soundChange("drive", i, distAmount);
                }
            }
        }
        
        if(e.getName() == "inner"){
            ofxUIButton *button = e.getButton();
            
            int jump = -1;
            bool occupied = true;
            while(occupied == true){
                occupied = false;
                for(int i = 0; i < otherPlayers.size(); i++){
                    int destination = me->getDiscIndex() + jump;
                    if( destination < 0 ) destination += disc.getDiscIndex();
                    if( otherPlayers[i]->getDiscIndex() == destination) {
                        occupied = true;
                        jump--;
                        break;
                    }
                }
            }
            int newIndex = me->getDiscIndex() + jump;
            if(newIndex <= -1) me->setDiscIndex(newIndex + disc.getDiscIndex());
            else if(newIndex> -1) me->setDiscIndex(newIndex);
            
            button->setValue(false);
            
            //change ui
            for(int i = 0; i < disc.getDiscIndex(); i++){
                ui[i]->setVisible(false);
            }
            ui[me->getDiscIndex()]->toggleVisible();
            
            //send change to server
            string changeDisc = "otherPlayersIndex//";
            changeDisc += "IP: "+ofToString(me->getIP()) + "//";
            changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
            client.send(changeDisc);
            
        }
        else if(e.getName() == "outer"){
            ofxUIButton *button = e.getButton();
            
            int jump = 1;
            bool occupied = true;
            while(occupied == true){
                occupied = false;
                for(int i = 0; i < otherPlayers.size(); i++){
                    int destination = me->getDiscIndex() + jump;
                    if( destination >= disc.getDiscIndex()) destination -= disc.getDiscIndex();
                    if( otherPlayers[i]->getDiscIndex() == destination) {
                        occupied = true;
                        jump++;
                        break;
                    }
                }
            }
            
            if(me->getDiscIndex() + jump < disc.getDiscIndex()) me->setDiscIndex(me->getDiscIndex() + jump);
            else me->setDiscIndex(me->getDiscIndex() + jump - disc.getDiscIndex());
            button->setValue(false);
            
            //change ui
            for(int i = 0; i < disc.getDiscIndex(); i++){
                ui[i]->setVisible(false);
            }
            ui[me->getDiscIndex()]->toggleVisible();
            
            //send change to server
            string changeDisc = "otherPlayersIndex//";
            changeDisc += "IP: "+ofToString(me->getIP()) + "//";
            changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
            client.send(changeDisc);
        }
        else if( e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
            
            ofxUITextInput *text = (ofxUITextInput *) e.widget;
            string input;
            if(text->getTextString() != ""){
                input = me->getNick() + "::" + text->getTextString()+"\n\n";
            }
            conversation = input + conversation;
            ofxUITextArea *history = (ofxUITextArea *) chat->getWidget("chat");
            history->setTextString(conversation);
            
            string sendText = "chat//"+input;
            client.send(sendText);
            
        }
        else if(e.getName() == "blank"){
            
            ofxUIToggle *toggle = e.getToggle();
            int texture = disc.getTexture(me->getDiscIndex());
            if(me->getLife() >= abs(costTexture) && texture != 0 && toggle->getValue() == true) {
                textureChanged = true;
                disc.setTexture(me->getDiscIndex(), 0);
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
                ofxUIToggle *toggle1 = static_cast <ofxUIToggle*> (canvas->getWidget("line"));
                ofxUIToggle *toggle2 = static_cast <ofxUIToggle*> (canvas->getWidget("tri"));
                ofxUIToggle *toggle3 = static_cast <ofxUIToggle*> (canvas->getWidget("saw"));
                ofxUIToggle *toggle4 = static_cast <ofxUIToggle*> (canvas->getWidget("rect"));
                toggle1->setValue(false);
                toggle2->setValue(false);
                toggle3->setValue(false);
                toggle4->setValue(false);
                
            }
            else toggle->setValue(true);
            
            //when texture is set to blank, rotation stops
            disc.setRotationSpeed(me->getDiscIndex(), -disc.getNetRotationSpeed(me->getDiscIndex()));
            
            //set rotation slider to 0
            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(me->getDiscIndex()+1)));
            slider->setValue(slider->getScaledValue()-slider->getScaledValue());
            
            //sound
            soundChange("bpm", me->getDiscIndex(), 0);
            soundChange("envelope", me->getDiscIndex(), 0);
            
            //update history//
            
            string tex = e.getName();
            string eventHistory = me->getNick() + " // Grv" + ofToString(me->getDiscIndex()+1)+ " // texture:" + tex +"\n\n";
            historyText = eventHistory + historyText;
            
            ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
            _history->setTextString(historyText);
            
        }
        else if(e.getName() == "line"){
            ofxUIToggle *toggle = e.getToggle();
            int texture = disc.getTexture(me->getDiscIndex());
            if(me->getLife() >= abs(costTexture) && texture != 1 && toggle->getValue() == true) {
                textureChanged = true;
                disc.setTexture(me->getDiscIndex(), 1);
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
                ofxUIToggle *toggle0 = static_cast <ofxUIToggle*> (canvas->getWidget("blank"));
                ofxUIToggle *toggle2 = static_cast <ofxUIToggle*> (canvas->getWidget("tri"));
                ofxUIToggle *toggle3 = static_cast <ofxUIToggle*> (canvas->getWidget("saw"));
                ofxUIToggle *toggle4 = static_cast <ofxUIToggle*> (canvas->getWidget("rect"));
                toggle0->setValue(false);
                toggle2->setValue(false);
                toggle3->setValue(false);
                toggle4->setValue(false);
                
                
                //sound
                soundChange("envelope", me->getDiscIndex(), 1);
                
                //update history//
                
                string tex = e.getName();
                string eventHistory = me->getNick() + " // Grv" + ofToString(me->getDiscIndex()+1)+ " // texture:" + tex +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
            else toggle->setValue(true);
            
        }
        else if(e.getName() == "tri"){
            ofxUIToggle *toggle = e.getToggle();
            int texture = disc.getTexture(me->getDiscIndex());
            if(me->getLife() >= abs(costTexture) && texture != 2 && toggle->getValue() == true) {
                textureChanged = true;
                disc.setTexture(me->getDiscIndex(), 2);
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
                ofxUIToggle *toggle0 = static_cast <ofxUIToggle*> (canvas->getWidget("blank"));
                ofxUIToggle *toggle1 = static_cast <ofxUIToggle*> (canvas->getWidget("line"));
                ofxUIToggle *toggle3 = static_cast <ofxUIToggle*> (canvas->getWidget("saw"));
                ofxUIToggle *toggle4 = static_cast <ofxUIToggle*> (canvas->getWidget("rect"));
                toggle0->setValue(false);
                toggle1->setValue(false);
                toggle3->setValue(false);
                toggle4->setValue(false);
                
                //sound
                soundChange("envelope", me->getDiscIndex(), 2);
                
                //update history//
                
                string tex = e.getName();
                string eventHistory = me->getNick() + " // Grv" + ofToString(me->getDiscIndex()+1)+ " // texture:" + tex +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
            else toggle->setValue(true);
            
        }
        else if(e.getName() == "saw"){
            ofxUIToggle *toggle = e.getToggle();
            int texture = disc.getTexture(me->getDiscIndex());
            if(me->getLife() >= abs(costTexture) && texture != 3 && toggle->getValue() == true) {
                textureChanged = true;
                disc.setTexture(me->getDiscIndex(), 3);
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
                ofxUIToggle *toggle0 = static_cast <ofxUIToggle*> (canvas->getWidget("blank"));
                ofxUIToggle *toggle1 = static_cast <ofxUIToggle*> (canvas->getWidget("line"));
                ofxUIToggle *toggle2 = static_cast <ofxUIToggle*> (canvas->getWidget("tri"));
                ofxUIToggle *toggle4 = static_cast <ofxUIToggle*> (canvas->getWidget("rect"));
                toggle0->setValue(false);
                toggle1->setValue(false);
                toggle2->setValue(false);
                toggle4->setValue(false);
                
                //sound
                soundChange("envelope", me->getDiscIndex(), 3);
                
                //update history//
                
                string tex = e.getName();
                string eventHistory = me->getNick() + " // Grv" + ofToString(me->getDiscIndex()+1)+ " // texture:" + tex +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
            else toggle->setValue(true);
            
        }
        else if(e.getName() == "rect"){
            ofxUIToggle *toggle = e.getToggle();
            int texture = disc.getTexture(me->getDiscIndex());
            if(me->getLife() >= abs(costTexture) && texture != 4 && toggle->getValue() == true) {
                textureChanged = true;
                disc.setTexture(me->getDiscIndex(), 4);
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
                ofxUIToggle *toggle0 = static_cast <ofxUIToggle*> (canvas->getWidget("blank"));
                ofxUIToggle *toggle1 = static_cast <ofxUIToggle*> (canvas->getWidget("line"));
                ofxUIToggle *toggle2 = static_cast <ofxUIToggle*> (canvas->getWidget("tri"));
                ofxUIToggle *toggle3 = static_cast <ofxUIToggle*> (canvas->getWidget("saw"));
                toggle0->setValue(false);
                toggle1->setValue(false);
                toggle2->setValue(false);
                toggle3->setValue(false);
                
                //sound
                soundChange("envelope", me->getDiscIndex(), 4);
                
                //update history//
                
                string tex = e.getName();
                string eventHistory = me->getNick() + " // Grv" + ofToString(me->getDiscIndex()+1)+ " // texture:" + tex +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
            else toggle->setValue(true);
        }
        else if(e.getName() == "move"){
            ofxUIToggle *toggle = e.getToggle();
            if(me->getLife() >= abs(costMove)){
                moveChanged = true;
                if (toggle->getValue() == true) disc.setMoving(me->getDiscIndex(), 1);
                else disc.setMoving(me->getDiscIndex(), 0);
                
                //update history//
                string change;
                if(disc.isMoving(me->getDiscIndex()) == 0) change = "off";
                else change = "on";
                string eventHistory = me->getNick() + " // Grv"+ofToString(me->getDiscIndex() + 1)+ " // move:" + change +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
        }
        else if(e.getName() == "reset"){
            ofxUIButton *button = e.getButton();
            if(me->getLife() >= abs(costMove) && disc.getPosition(me->getDiscIndex()) != 0){
                moveReset = true;
                disc.resetPerlin[me->getDiscIndex()] = 1;
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
                ofxUIToggle *toggle = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                toggle->setValue(false);
                
                //update history//
                
                string eventHistory = me->getNick() + " // Grv"+ofToString(me->getDiscIndex()+1)+ " // z-reset\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
        }
        else if(e.getName() == "move all"){
            ofxUIToggle *toggle = e.getToggle();
            
            
            
            if(toggle->getValue() == true){
                
                int costFactor = 0;
                for(int j = 0; j < disc.getDiscIndex(); j++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    if(toggleMove->getValue()==false) costFactor++;
                }
                
                if(me->getLife() >= abs(costMove*costFactor) && costFactor != 0){
                    
                    for(int j = 0; j<disc.getDiscIndex(); j++){
                        ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                        ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                        if(toggleMove->getValue()==false){
                            toggleMove->setValue(true);
                            disc.setMoving(j, 1);
                        }
                    }
                    
                    me->changeLife(costMove*costFactor);
                    string moveAll = "moveAll//"+me->getIP();
                    client.send(moveAll);
                    
                    //update server
                    string lifeUpdate = "life//";
                    lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
                    lifeUpdate += "lifeChange: "+ofToString(costMove*costFactor) + "//";
                    client.send(lifeUpdate);
                    
                    //update history//
                    string eventHistory = me->getNick() + " // move all\n\n";
                    historyText = eventHistory + historyText;
                    
                    ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                    _history->setTextString(historyText);
                    
                }
            }
            if(toggle->getValue() == false){
                
                int costFactor = 0;
                string zPositionAll = "zPositionAll//";
                
                for(int j = 0; j < disc.getDiscIndex(); j++){
                    
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    if(toggleMove->getValue()==true) costFactor++;
                }
                
                if(me->getLife() >= abs(costMove*costFactor) && costFactor != 0){
                    
                    for(int j = 0; j < disc.getDiscIndex(); j++){
                        
                        ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                        ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                        if(toggleMove->getValue()==true){
                            toggleMove->setValue(false);
                            disc.setMoving(j, 0);
                        }
                        zPositionAll += ofToString(j)+": "+ofToString(disc.getPosition(j))+"//";
                    }
                    
                    me->changeLife(costMove*costFactor);
                    string stopAll = "stopAll//"+me->getIP();
                    client.send(stopAll);
                    client.send(zPositionAll);
                    
                    //update server
                    string lifeUpdate = "life//";
                    lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
                    lifeUpdate += "lifeChange: "+ofToString(costMove*costFactor) + "//";
                    client.send(lifeUpdate);
                    
                    //update history//
                    string eventHistory = me->getNick() + " // halt all\n\n";
                    historyText = eventHistory + historyText;
                    
                    ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                    _history->setTextString(historyText);
                    
                }
            }
            
        }
        else if(e.getName() == "reset all"){
            ofxUIButton *button = e.getButton();
            
            if(button->getValue() == true){
                int costFactor = 0;
                for(int j = 0; j<disc.getDiscIndex(); j++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    if(disc.getPosition(j) != 0) costFactor++;
                }
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (dashboard);
                ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                toggleMoveAll->setValue(false);
                
                if(me->getLife() >= abs(costMove*costFactor) && costFactor != 0){
                    for(int j = 0; j<disc.getDiscIndex(); j++){
                        ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                        ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                        if(disc.getPosition(j) != 0){
                            toggleMove->setValue(false);
                            disc.resetPerlin[j] = 1;
                        }
                    }
                    
                    me->changeLife(costMove*costFactor);
                    string resetAll = "resetAll//"+me->getIP();
                    client.send(resetAll);
                    
                    //update server
                    string lifeUpdate = "life//";
                    lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
                    lifeUpdate += "lifeChange: "+ofToString(costMove*costFactor) + "//";
                    client.send(lifeUpdate);
                    
                    //update history//
                    string eventHistory = me->getNick() + " // reset all\n\n";
                    historyText = eventHistory + historyText;
                    
                    ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                    _history->setTextString(historyText);
                    
                }
                
            }
        }
        else if(e.getName() == "mute"){
            ofxUIToggle *toggle = e.getToggle();
            if(me->getLife() > 0){
                muteChanged = true;
                if(toggle->getValue()==true) {
                    disc.setMute(me->getDiscIndex(), 1); //mute on
                    soundChange("envelope", me->getDiscIndex(), 0);
                }
                else if(toggle->getValue()==false){
                    disc.setMute(me->getDiscIndex(), 0); //mute off
                    disc.setEnvelope(me->getDiscIndex(), disc.getTexture(me->getDiscIndex()));
                    soundChange("envelope", me->getDiscIndex(), disc.getTexture(me->getDiscIndex()));
                    toggle->setValue(false);
                }
                
                //update history//
                string eventHistory;
                string change;
                if(disc.isMute(me->getDiscIndex()) == 0) change = "off";
                else change = "on";
                
                eventHistory = me->getNick() + " // " + "Grv"+ofToString(me->getDiscIndex()+1)+ " // " + "mute:" + change +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
            else toggle->setValue(false);
        }
        else if(e.getName() == "chat"){
            ofxUIToggle *toggle = e.getToggle();
            
            //toggle chat button
            
            ofxUIToggle *dashboardChatToggle = static_cast <ofxUIToggle*> (dashboard->getWidget("chat"));
            dashboardChatToggle->setValue(toggle->getValue());
            
            chat->setVisible(toggle->getValue());
        }
        else if(e.getName() == "add"){
            
            ofxUIToggle *toggle = e.getToggle();
            
            if(toggle->getValue() == true && me->getLife() >= costCreate) {
                
                //send demand to server
                string newDisc = "newDisc//"+me->getIP();
                client.send(newDisc);
                
                toggle->setValue(false);
            }
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(moveChanged){
        moveChanged = false;
        me->changeLife(costMove);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costMove) + "//";
        client.send(lifeUpdate);
        
        string movement = "move//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.isMoving(me->getDiscIndex()))+"//"+me->getIP();
        client.send(movement);
        
        if (disc.isMoving(me->getDiscIndex()) == 0){
            string position = "zPosition//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.getPosition(me->getDiscIndex()));
            string counter = "counter//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.getCounter(me->getDiscIndex()));
            client.send(position);
            client.send(counter);
        }
    }
    if(muteChanged){
        muteChanged = false;
        me->changeLife(costMute);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costMute) + "//";
        client.send(lifeUpdate);
        
        string change = "mute//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.isMute(me->getDiscIndex()))+"//"+me->getIP();
        client.send(change);
    }
    
    disc.update();
    for(int i = 0; i< disc.getDiscIndex(); i++){
        float amountFreq = ofMap(abs(disc.getNetRotationSpeed(i)), 0, 10, 0, 1000);
        float amountMod = ofMap(abs(disc.getPosition(i)), 0, 100, 0, 1000);
        float qDist = ofMap(abs(disc.getPosition(i)), 0, 100, 1., .9);
        soundChange("amountFreq", i, amountFreq);
        soundChange("amountMod", i, amountMod);
        soundChange("qDist", i, qDist);
    }
    
    
    //TCP
    if(client.isConnected()){
        string str = client.receive();
        if(str.length() > 0){
            received = ofSplitString(str, "//");
            title = received[0];
            if(title == "state"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                if(nameValue[0] == "discIndex") disc.setDiscIndex(ofToInt(nameValue[1]));
                
                //graphic values
                for (int i = 0; i < disc.getDiscIndex(); i++) {
                    
                    disc.addDisc(i);
                    newUI(i);
                    //mesh
                    groove.setup(&disc, me, otherPlayers);
                    
                    for(int j = 0; j < 11; j++){
                        nameValue = ofSplitString(received[j+(i*11)+2], ": ");
                        if(nameValue[0] == "radius"+ofToString(i)) {
                            disc.setRadius(i, ofToFloat(nameValue[1]));
                            //sound
                            float q = ofMap(disc.getRadius(i)-disc.getRadius(i-1), 15., 100., 3., 0.);
                            soundChange("q", i, q);
                            //ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("radius"+ofToString(i+1)));
                            slider->setValue(disc.getRadius(i)-disc.getRadius(i-1));
                        }
                        if(nameValue[0] == "rotation"+ofToString(i)) {
                            disc.setRotation (i, ofToFloat(nameValue[1]));
                        }
                        if(nameValue[0] == "rotationSpeed"+ofToString(i)) {
                            disc.setNetRotationSpeed (i, ofToFloat(nameValue[1]));
                            //sound
                            float netSpeed = abs(disc.getNetRotationSpeed(i));
                            float frequency;
                            if(netSpeed <= 5){
                                frequency = ofMap(netSpeed, 0, 5, 50, 250);
                            }
                            else frequency = ofMap(netSpeed, 5, 10, 250, 800);
                            float beatSpeed = ofMap(netSpeed, 0, 10, 0, 200);
                            float beatDensity = ofMap(disc.getDensity(i), 1, 30, 15, 2);
                            soundChange("freq", i, frequency);
                            soundChange("bpm", i, beatSpeed*beatDensity);
                            //ui
                            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(i+1)));
                            slider->setValue(ofToFloat(nameValue[1]));
                        }
                        if(nameValue[0] == "density"+ofToString(i)) {
                            disc.setDensity (i, ofToFloat(nameValue[1]));
                            
                            //sound
                            float envelopeCoeff = ofMap(disc.getDensity(i), 1, 30, 1, 5);
                            float pulseRatio = ofMap(disc.getDensity(i), 1, 30, 0.001, 1);
                            soundChange("envelopeWidth", i, envelopeCoeff);
                            
                            float netSpeed = abs(disc.getNetRotationSpeed(i));
                            float beatSpeed = ofMap(netSpeed, 0, 10, 0, 200);
                            float beatDensity = ofMap(disc.getDensity(i), 1, 30, 15, 2);
                            soundChange("bpm", i, beatSpeed*beatDensity);
                            
                            //ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("density"+ofToString(i+1)));
                            slider->setValue(disc.getDensity(i));
                        }
                        if(nameValue[0] == "spike"+ofToString(i)) {
                            disc.setSpikeDistance(i, ofToFloat(nameValue[1]));
                            
                            //sound
                            float distAmount = ofMap(disc.getSpikeDistance(i), 0., 100., 1., 15., true);
                            soundChange("drive", i, distAmount);
                            
                            //ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("spike"+ofToString(i+1)));
                            slider->setValue(disc.getSpikeDistance(i));
                        }
                        
                        if(nameValue[0] == "texture"+ofToString(i)) {
                            disc.setTexture (i, ofToFloat(nameValue[1]));
                            //sound
                            soundChange("envelope", i, disc.getTexture(i));
                            
                            //ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUIToggle *toggle0 = static_cast <ofxUIToggle*> (canvas->getWidget("blank"));
                            ofxUIToggle *toggle1 = static_cast <ofxUIToggle*> (canvas->getWidget("line"));
                            ofxUIToggle *toggle2 = static_cast <ofxUIToggle*> (canvas->getWidget("tri"));
                            ofxUIToggle *toggle3 = static_cast <ofxUIToggle*> (canvas->getWidget("saw"));
                            ofxUIToggle *toggle4 = static_cast <ofxUIToggle*> (canvas->getWidget("rect"));
                            if(disc.getTexture(i) == 0) {
                                toggle0->setValue(true);
                                //set rotation slider to 0
                                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                                ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(i+1)));
                                slider->setValue(slider->getScaledValue()-slider->getScaledValue());}
                            else toggle0->setValue(false);
                            if(disc.getTexture(i) == 1) toggle1->setValue(true);
                            else toggle1->setValue(false);
                            if(disc.getTexture(i) == 2) toggle2->setValue(true);
                            else toggle2->setValue(false);
                            if(disc.getTexture(i) == 3) toggle3->setValue(true);
                            else toggle3->setValue(false);
                            if(disc.getTexture(i) == 4) toggle4->setValue(true);
                            else toggle4->setValue(false);
                        }
                        if(nameValue[0] == "zPosition"+ofToString(i)) {
                            disc.setPosition (i, ofToFloat(nameValue[1]));
                        }
                        if(nameValue[0] == "seed"+ofToString(i)) {
                            disc.setSeed(i, ofToInt(nameValue[1]));
                            disc.zMotionSetup(i, ofToInt(nameValue[1]));
                        }
                        if(nameValue[0] == "counter"+ofToString(i)) {
                            disc.setCounter(i, ofToFloat(nameValue[1]));
                        }
                        if(nameValue[0] == "mute"+ofToString(i)) {
                            disc.setMute(i, ofToInt(nameValue[1]));
                            if(ofToInt(nameValue[1]) == 0) soundChange("envelope", i, disc.getTexture(i));
                            else soundChange("envelope", i, 0);
                            //update ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUILabelToggle *toggle = static_cast<ofxUILabelToggle*>(canvas->getWidget("mute"));
                            toggle->setValue((bool)disc.isMute(i));
                        }
                        if(nameValue[0] == "move"+ofToString(i) && ofToInt(nameValue[1]) == 1) {
                            if (disc.isMoving(i) == 0) disc.setMoving(i, 1);
                            else disc.setMoving(i, 0);
                            //update ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUILabelToggle *toggle = static_cast<ofxUILabelToggle*>(canvas->getWidget("move"));
                            toggle->setValue((bool)disc.isMoving(i));
                        }
                        
                        
                    }
                    
                    
                }
                //toggle move all button if all grooves are moving
                bool allMoving = false;
                for(int i = 0; i < disc.getDiscIndex(); i++){
                    if (disc.isMoving(i)) allMoving = true;
                    else {
                        allMoving = false;
                        break;
                    }
                }
                ofxUIToggle *moveAll = (ofxUIToggle*) dashboard->getWidget("move all");
                moveAll->setValue(allMoving);
                
                me->setConnection(true);
                TCPsetup = true;
                initialize->setVisible(false);
                dashboard->setVisible(true);
                keyList = true;
                costList = true;
                
                loginMinute = ofGetMinutes();
                loginSecond = ofGetElapsedTimef();
                
                setup();
            }
            
            else if (title == "costs"){
                
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "costRadius") costRadius = ofToFloat(playerData[1]);
                    if (playerData[0] == "costDensity") costDensity = ofToFloat(playerData[1]);
                    if (playerData[0] == "costRotation") costRotation = ofToFloat(playerData[1]);
                    if (playerData[0] == "costTexture") costTexture = ofToFloat(playerData[1]);
                    if (playerData[0] == "costMute") costMute = ofToFloat(playerData[1]);
                    if (playerData[0] == "costMove") costMove = ofToFloat(playerData[1]);
                    if (playerData[0] == "costSpike") costSpike = ofToFloat(playerData[1]);
                    if (playerData[0] == "costCreate") costCreate = ofToFloat(playerData[1]);
                    if (playerData[0] == "reward") reward = ofToFloat(playerData[1]);
                }
            }
            
            else if (title == "playerInfo" ){
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "IP") me->setIP(playerData[1]);
                    if (playerData[0] == "color") me->setColor(ofFromString<ofColor>(playerData[1]));
                    if (playerData[0] == "life") me->setLife(ofToFloat(playerData[1]));
                    if (playerData[0] == "index") me->setDiscIndex(ofToInt(playerData[1]));
                    if (playerData[0] == "nick") me->setNick(playerData[1]);
                    
                }
                
                me->setConnection(true);
                groove.setup(&disc, me, otherPlayers);
                
            }
            
            else if (title == "otherPlayers"){
                
                Player* _player = new Player();
                
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "IP") {
                        bool exists = false;
                        for( int j = 0; j < otherPlayers.size(); j++){
                            if(playerData[1] == otherPlayers[j]->getIP()) {
                                _player = otherPlayers[j];
                                exists = true;
                                break;
                            }
                        }
                        if (exists == false){
                            otherPlayers.push_back(_player);
                            _player->setIP(playerData[1]);
                        }
                        _player->setConnection(true);
                    }
                    if (playerData[0] == "color") _player->setColor(ofFromString<ofColor>(playerData[1]));
                    if (playerData[0] == "life") _player->setLife(ofToFloat(playerData[1]));
                    if (playerData[0] == "index") _player->setDiscIndex(ofToInt(playerData[1]));
                    if (playerData[0] == "nick") {
                        _player->setNick(playerData[1]);
                        
                        conversation = "***"+_player->getNick()+" is online***"+"\n\n" + conversation;
                        ofxUITextArea *chatHistory = (ofxUITextArea *) chat->getWidget("chat");
                        chatHistory->setTextString(conversation);
                        
                        historyText = "***"+_player->getNick()+" is online***"+"\n\n" + historyText;
                        ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                        _history->setTextString(historyText);
                    }
                }
                groove.setup(&disc, me, otherPlayers);
                
            }
            
            else if (title == "otherPlayersIndex"){
                
                int thisPlayer;
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "IP"){
                        for (int j = 0; j < otherPlayers.size(); j++) {
                            if(playerData[1] == otherPlayers[j]->getIP()) {
                                thisPlayer = j;
                                break;
                            }
                        }
                    }
                    if (playerData[0] == "index") otherPlayers[thisPlayer]->setDiscIndex(ofToInt(playerData[1]));
                }
            }
            
            else if (title == "addDisc"){
                
                Player *_player = NULL;
                int newSeed;
                int newIndex;
                float newRotation;
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData = ofSplitString(received[i], ": ");
                    if(playerData[0] == "IP" && playerData[1] != me->getIP()){
                        for (int j = 0; j < otherPlayers.size(); j++) {
                            if(playerData[1] == otherPlayers[j]->getIP()) _player = otherPlayers[j];
                        }
                    }
                    else if(playerData[0] == "IP" && playerData[1] == me->getIP()) _player = me;
                    
                    if(playerData[0] == "index") {
                        
                        newIndex = ofToInt(playerData[1]);
                        _player->setDiscIndex(newIndex);
                    }
                    
                    if(playerData[0] == "seed") newSeed = ofToInt(playerData[1]);
                    if(playerData[0] == "total") disc.setDiscIndex(ofToInt(playerData[1]));
                    if(playerData[0] == "netRotation") newRotation = ofToFloat(playerData[1]);
                }
                
                //new disc
                disc.addDisc(newIndex);
                disc.setSeed(newIndex, newSeed);
                disc.zMotionSetup(newIndex, newSeed);
                disc.setNetRotationSpeed(newIndex, 0);
                
                //new mesh
                groove.setup(&disc, me, otherPlayers);
                
                //new UI
                newUI(newIndex);
                
                //update history
                historyText =" // " +_player->getNick() + " groove create // "+ "\n\n"+ historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "life"){
                
                Player *_player = NULL;
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "IP" && playerData[1] != me->getIP()){
                        for (int j = 0; j < otherPlayers.size(); j++) {
                            if(playerData[1] == otherPlayers[j]->getIP()) {
                                _player = otherPlayers[j];
                                break;
                            }
                        }
                    }
                    else if (playerData[0] == "IP" && playerData[1] == me->getIP()) _player = me;
                    
                    if (playerData[0] == "lifeChange" && _player != NULL) _player->changeLife(ofToFloat(playerData[1]));
                }
            }
            
            else if (title == "reward"){
                Player *_player = NULL;
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData = ofSplitString(received[i], ": ");;
                    if (playerData[0] == "IP" && playerData[1] != me->getIP()){
                        for (int j = 0; j < otherPlayers.size(); j++) {
                            if(playerData[1] == otherPlayers[j]->getIP()) {
                                _player = otherPlayers[j];
                                break;
                            }
                        }
                    }
                    else if (playerData[0] == "IP" && playerData[1] == me->getIP()) _player = me;
                    
                    if (playerData[0] == "lifeChange" && _player != NULL) {
                        
                        _player->changeLife(ofToFloat(playerData[1]));
                        historyText = "*" + _player->getNick() + " +"+ ofToString(reward) + " points!*\n\n"+ historyText;
                        
                        ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                        _history->setTextString(historyText);
                    }
                }
                
                
            }
            
            else if (title == "rotationSpeed"){
                vector<string> nameValue = ofSplitString(received[1], ": ");;
                int index = ofToInt(nameValue[0]);
                disc.setRotationSpeed(index, ofToFloat(nameValue[1]));
                
                //sound
                float netSpeed = abs(disc.getNetRotationSpeed(index));
                float frequency;
                if(netSpeed <= 5){
                    frequency = ofMap(netSpeed, 0, 5, 50, 250);
                }
                else frequency = ofMap(netSpeed, 5, 10, 250, 800);
                float beatSpeed = ofMap(netSpeed, 0, 10, 0, 200);
                float beatDensity = ofMap(disc.getDensity(index), 1, 30, 15, 2);
                soundChange("freq", index, frequency);
                soundChange("bpm", index, beatSpeed*beatDensity);
                
                //update ui
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[index]);
                ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(index+1)));
                slider->setValue(disc.getNetRotationSpeed(index));
                
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "rotation:" + ofToString( roundf(disc.getNetRotationSpeed(index)*10)/10) +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
                
            }
            
            else if (title == "radius"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setThickness(index, ofToFloat(nameValue[1]));
                
                //change sound
                float q = ofMap(disc.getRadius(index)-disc.getRadius(index-1), 15., 100., 3., 0.);
                soundChange("q", ofToInt(nameValue[0]), q);
                
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("radius"+ofToString(index+1)));
                slider->setValue(disc.getRadius(index)-disc.getRadius(index-1));
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "size:" + ofToString(roundf(disc.getThickness(index)*10)/10) +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "density"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setDensity(index, ofToFloat(nameValue[1]));
                
                //change sound
                float envelopeCoeff = ofMap(disc.getDensity(index), 1, 30, 1, 5);
                float pulseRatio = ofMap(disc.getDensity(index), 1, 30, 0.001, 1);
                soundChange("envelopeWidth", index, envelopeCoeff);
                //                soundChange("pulseLength", index, pulseRatio);
                
                //change metronome
                float netSpeed = abs(disc.getNetRotationSpeed(index));
                float beatSpeed = ofMap(netSpeed, 0, 10, 0, 200);
                float beatDensity = ofMap(disc.getDensity(index), 1, 30, 15, 2);
                soundChange("bpm", index, beatSpeed*beatDensity);
                
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("density"+ofToString(index+1)));
                slider->setValue(disc.getDensity(index));
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "density:" + ofToString(disc.getDensity(index)) +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "spike"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setSpikeDistance(index, ofToFloat(nameValue[1]));
                
                //change sound
                float distAmount = ofMap(disc.getSpikeDistance(index), 0., 100., 1., 15., true);
                soundChange("drive", index, distAmount);
                
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("spike"+ofToString(index+1)));
                slider->setValue(disc.getSpikeDistance(index));
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "spike:" + ofToString(disc.getSpikeDistance(index)) +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "texture"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setTexture(index, ofToInt(nameValue[1]));
                
                //change sound
                soundChange("envelope", index, ofToInt(nameValue[1]));
                
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUIToggle *toggle0 = static_cast <ofxUIToggle*> (canvas->getWidget("blank"));
                ofxUIToggle *toggle1 = static_cast <ofxUIToggle*> (canvas->getWidget("line"));
                ofxUIToggle *toggle2 = static_cast <ofxUIToggle*> (canvas->getWidget("tri"));
                ofxUIToggle *toggle3 = static_cast <ofxUIToggle*> (canvas->getWidget("saw"));
                ofxUIToggle *toggle4 = static_cast <ofxUIToggle*> (canvas->getWidget("rect"));
                if(disc.getTexture(index) == 0) {
                    toggle0->setValue(true);
                    
                    //when texture is set to blank, rotation stops
                    disc.setRotationSpeed(index, -disc.getNetRotationSpeed(index));
                    
                    //set rotation slider to 0
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[index]);
                    ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(index+1)));
                    slider->setValue(slider->getScaledValue()-slider->getScaledValue());
                }
                else toggle0->setValue(false);
                if(disc.getTexture(index) == 1) toggle1->setValue(true);
                else toggle1->setValue(false);
                if(disc.getTexture(index) == 2) toggle2->setValue(true);
                else toggle2->setValue(false);
                if(disc.getTexture(index) == 3) toggle3->setValue(true);
                else toggle3->setValue(false);
                if(disc.getTexture(index) == 4) toggle4->setValue(true);
                else toggle4->setValue(false);
                
                //update history//
                
                string eventHistory;
                string texture;
                if(disc.getTexture(index) == 0) texture = "blank";
                if(disc.getTexture(index) == 1) texture = "line";
                if(disc.getTexture(index) == 2) texture = "tri";
                if(disc.getTexture(index) == 3) texture = "saw";
                if(disc.getTexture(index) == 4) texture = "rect";
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "texture:" + texture +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "mute"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setMute(index, ofToInt(nameValue[1]));
                if(ofToInt(nameValue[1]) == 0) soundChange("envelope", index, disc.getTexture(index));
                else soundChange("envelope", index, 0);
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUILabelToggle *toggle = static_cast<ofxUILabelToggle*>(canvas->getWidget("mute"));
                toggle->setValue((bool)disc.isMute(index));
                
                //update history//
                
                string eventHistory;
                string change;
                if(ofToInt(nameValue[1]) == 0) change = "off";
                if(ofToInt(nameValue[1]) == 1) change = "on";
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "mute:" + change +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "move"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setMoving(index, ofToInt(nameValue[1]));
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUIToggle *toggle = static_cast<ofxUIToggle*>(canvas->getWidget("move"));
                toggle->setValue((bool)disc.isMoving(index));
                
                //update history//
                
                string eventHistory;
                string change;
                if(ofToInt(nameValue[1]) == 0) change = "off";
                if(ofToInt(nameValue[1]) == 1) change = "on";
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "move:" + change +"\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "moveReset"){
                int index = ofToInt(received[1]);
                disc.resetPerlin[index] = 1;
                //ui
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[index]);
                ofxUIToggle *toggle = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                toggle->setValue(false);
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "Grv"+ofToString(index+1)+ " // " + "z-reset\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
                
            }
            
            else if (title == "moveAll"){
                ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (dashboard->getWidget("move all"));
                toggleMoveAll->setValue(true);
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMove->setValue(true);
                    disc.setMoving(i, 1);
                }
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "move all\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "stopAll"){
                ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (dashboard->getWidget("move all"));
                toggleMoveAll->setValue(false);
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMove->setValue(false);
                    disc.setMoving(i, 0);
                }
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "halt all\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "resetAll"){
                ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (dashboard->getWidget("move all"));
                toggleMoveAll->setValue(false);
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    disc.resetPerlin[i] = 1;
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMove->setValue(false);
                }
                
                //update history//
                
                string eventHistory;
                
                Player* _player = NULL;
                for(int i = 0; i < otherPlayers.size(); i++){
                    if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
                }
                if(_player == NULL) cout<< "_player is not matched" <<endl;
                else eventHistory = _player->getNick() + " // " + "reset all\n\n";
                historyText = eventHistory + historyText;
                
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
                
            }
            
            else if (title == "zPosition"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                disc.setPosition(ofToInt(nameValue[0]), ofToFloat(nameValue[1]));
            }
            
            else if (title == "zPositionAll"){
                for (int i = 1; i < received.size()-1; i++) {
                    vector<string> nameValue;
                    nameValue = ofSplitString(received[i], ": ");
                    disc.setPosition(ofToInt(nameValue[0]), ofToFloat(nameValue[1]));
                }
            }
            
            else if (title == "counter"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                disc.setCounter(ofToInt(nameValue[0]), ofToFloat(nameValue[1]));
            }
            
            else if (title == "chat"){
                conversation = received[1] + conversation;
                ofxUITextArea *chatHistory = (ofxUITextArea *) chat->getWidget("chat");
                chatHistory->setTextString(conversation);
                
            }
            
            else if (title == "goodbye"){
                for (int i = 0; i < otherPlayers.size(); i++) {
                    if(received[1] == otherPlayers[i]->getNick()) {
                        otherPlayers[i]->setConnection(false);
                        otherPlayers[i]->setDiscIndex(-1);
                    }
                    else continue;
                }
                conversation = "***"+received[1]+" is offline***"+"\n\n" + conversation;
                ofxUITextArea *chatHistory = (ofxUITextArea *) chat->getWidget("chat");
                chatHistory->setTextString(conversation);
                
                historyText = "***"+received[1]+" is offline***"+"\n\n" + historyText;
                ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
                _history->setTextString(historyText);
            }
            
        }
    }
    else {
        if(me != NULL && TCPsetup){
            TCPsetup = false;
            me->setDiscIndex(-1);
            
            string disconnected;
            
            disconnected = "*you are disconnected from server*\n\n";
            historyText = disconnected + historyText;
            conversation = disconnected + conversation;
            
            ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
            _history->setTextString(historyText);
            
            ofxUITextArea *_chat = (ofxUITextArea *) chat->getWidget("chat");
            _history->setTextString(historyText);
        }
        
    }
    
    if(TCPsetup) groove.update();
}
//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(200, 230);
    glEnable(GL_DEPTH_TEST);
    
    ofPushMatrix();
    
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    if(TCPsetup){
        cam.begin();
        groove.draw();
        
        if(cam.getDistance() > 100){
            //			cout<< cam.getDistance() <<endl;
            float wetLevel = ofMap(cam.getDistance(), 250, 9000, 0., .75);
            float masterLevel = ofMap(cam.getDistance(), 0, 9000, .95, 0.1);
            ofClamp(wetLevel, 0., 0.75);
            ofClamp(masterLevel, 0., .999);
            
            sound.synth.setParameter("wet", wetLevel);
            sound.synth.setParameter("master", masterLevel);
        }
        
        if(abs(cam.getPosition().x > 5000 ||
               cam.getPosition().y > 5000 ||
               cam.getPosition().z > 5000))
        {
            
            cam.setPosition(ofClamp(cam.getPosition().x, -5000, 5000),
                            ofClamp(cam.getPosition().y, -5000, 5000),
                            ofClamp(cam.getPosition().z, -5000, 5000));
            
        }
        
        cam.end();
        
        ofSetColor(me->getColor());
        ofFill();
        ofRect(groove.lifeBar[0]);
        
        for(int i = 0; i < otherPlayers.size(); i++){
            if(otherPlayers[i]->isConnected() == true){
                ofSetColor(otherPlayers[i]->getColor());
                ofFill();
                ofRect(groove.lifeBar[i+1]);
            }
        }
        if(costList){
            int costFactorMove = 0;
            for(int j = 0; j < disc.getDiscIndex(); j++){
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                if(toggleMove->getValue()==false) costFactorMove++;
            }
            int costMoveAll;
            costMoveAll = costMove * costFactorMove;
            
            int costFactorStop = 0;
            for(int j = 0; j < disc.getDiscIndex(); j++){
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                if(toggleMove->getValue()==true) costFactorStop++;
            }
            int costStopAll;
            costStopAll = costMove * costFactorStop;
            
            int costFactorReset = 0;
            for(int j = 0; j<disc.getDiscIndex(); j++){
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                if(disc.getPosition(j) != 0) costFactorReset++;
            }
            int costResetAll;
            costResetAll = costMove * costFactorReset;
            
            
            ofSetColor(0);
            ofDrawBitmapString("texture     "+ofToString(costTexture)+"\nrotation    "+ofToString(costRotation)+"\ndensity     "+ofToString(costDensity)+"\nsize        "+ofToString(costRadius)+"\nspike       "+ofToString(costSpike)+"\nmove/reset  "+ofToString(costMove)+"\nmute        "+ofToString(costMute)+"\nnew disc    "+ofToString(costCreate)+"\nmove all    "+ofToString(costMoveAll)+"\nstop all    "+ofToString(costStopAll)+"\nreset all   "+ofToString(costResetAll), ofGetWidth()/2 - 245, ofGetHeight()/2-300);
            
        }
        if(keyList){
            ofSetColor(0);
            ofDrawBitmapString("<w/s> navigate\n<bck> deselect\n<m> mute\n<t> timer\n<c> chat\n<f> fullscreen\n<k> key list\n<l> cost list\n<esc> exit", ofGetWidth()/2 - 250, ofGetHeight()/2 - 150);
            
        }
        if(timer) {
            ofSetColor(0);
            ofDrawBitmapString(ofToString(roundf((ofGetElapsedTimef()-loginSecond)*100)/100)+" seconds elapsed", ofGetWidth()/2 - 250, ofGetHeight()/2 - 10);
        }
    }
    
    ofPopMatrix();
    
    glDisable(GL_DEPTH_TEST);
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(chat->hasKeyboardFocus() || initialize->hasKeyboardFocus())
    {
        return;
    }
    
    
    
    if(key == 'c' && TCPsetup) {
        
        chat->toggleVisible();
        
        ofxUICanvas *canvas = static_cast <ofxUICanvas*> (dashboard);
        ofxUIToggle *toggleChat = static_cast <ofxUIToggle*> (canvas->getWidget("chat"));
        toggleChat->setValue(!toggleChat->getValue());
        
    }
    
    if(key == 'w' && TCPsetup){
        
        if(disc.getDiscIndex() != 0){
            int jump = 1;
            bool occupied = true;
            while(occupied == true){
                occupied = false;
                for(int i = 0; i < otherPlayers.size(); i++){
                    int destination = me->getDiscIndex() + jump;
                    if( destination >= disc.getDiscIndex()) destination -= disc.getDiscIndex();
                    if( otherPlayers[i]->getDiscIndex() == destination) {
                        occupied = true;
                        jump++;
                        break;
                    }
                }
            }
            
            if(jump > disc.getDiscIndex()) me->setDiscIndex(-1);
            else if(me->getDiscIndex() + jump < disc.getDiscIndex()) me->setDiscIndex(me->getDiscIndex() + jump);
            else me->setDiscIndex(me->getDiscIndex() + jump - disc.getDiscIndex());
            
            //change ui
            for(int i = 0; i < disc.getDiscIndex(); i++){
                ui[i]->setVisible(false);
            }
            addDisc->setVisible(false);
            ui[me->getDiscIndex()]->toggleVisible();
            
            //send change to server
            string changeDisc = "otherPlayersIndex//";
            changeDisc += "IP: "+ofToString(me->getIP()) + "//";
            changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
            client.send(changeDisc);
        }
    }
    
    if(key == 's' && TCPsetup){
        if(disc.getDiscIndex() != 0){
            int jump = -1;
            bool occupied = true;
            while(occupied == true){
                occupied = false;
                for(int i = 0; i < otherPlayers.size(); i++){
                    int destination = me->getDiscIndex() + jump;
                    if( destination < 0 ) destination += disc.getDiscIndex();
                    if( otherPlayers[i]->getDiscIndex() == destination) {
                        occupied = true;
                        jump--;
                        break;
                    }
                }
            }
            int newIndex = me->getDiscIndex() + jump;
            
            if(abs(jump) > disc.getDiscIndex()) me->setDiscIndex(-1);
            else if(newIndex <= -1) me->setDiscIndex(newIndex + disc.getDiscIndex());
            else if(newIndex> -1) me->setDiscIndex(newIndex);
            
            //change ui
            for(int i = 0; i < disc.getDiscIndex(); i++){
                ui[i]->setVisible(false);
            }
            addDisc->setVisible(false);
            ui[me->getDiscIndex()]->toggleVisible();
            
            //send change to server
            string changeDisc = "otherPlayersIndex//";
            changeDisc += "IP: "+ofToString(me->getIP()) + "//";
            changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
            client.send(changeDisc);
        }
    }
    
    if(key == OF_KEY_BACKSPACE && TCPsetup) {
        
        if(me->getDiscIndex() != -1){
            for(int i = 0; i < ui.size(); i++){
                ui[i]->setVisible(false);
            }
            addDisc->setVisible(true);
            me->setDiscIndex(-1);
            //send change to server
            string changeDisc = "otherPlayersIndex//";
            changeDisc += "IP: "+ofToString(me->getIP()) + "//";
            changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
            client.send(changeDisc);
        }
    }
    if(key == 'm' && TCPsetup && me->getDiscIndex() != -1) {
        ofxUIToggle *toggle = static_cast<ofxUILabelToggle*>(ui[me->getDiscIndex()]->getWidget("mute"));
        if(me->getLife() > 0){
            muteChanged = true;
            if(toggle->getValue()==false) {
                disc.setMute(me->getDiscIndex(), 1); //mute on
                soundChange("envelope", me->getDiscIndex(), 0);
                toggle->setValue(true);
            }
            else if(toggle->getValue()==true){
                disc.setMute(me->getDiscIndex(), 0); //mute off
                disc.setEnvelope(me->getDiscIndex(), disc.getTexture(me->getDiscIndex()));
                soundChange("envelope", me->getDiscIndex(), disc.getTexture(me->getDiscIndex()));
                toggle->setValue(false);
            }
            
            //update history//
            string eventHistory;
            string change;
            if(disc.isMute(me->getDiscIndex()) == 0) change = "off";
            else change = "on";
            
            eventHistory = me->getNick() + " // " + "Grv"+ofToString(me->getDiscIndex()+1)+ " // " + "mute:" + change +"\n\n";
            historyText = eventHistory + historyText;
            
            ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
            _history->setTextString(historyText);
        }
        else toggle->setValue(false);
        
    }
    
    if(key == 'f') {
        fullScreen = !fullScreen;
        ofSetFullscreen(fullScreen);
        
        dashboard->setPosition(ofGetWidth()/2-125, ofGetHeight()-30);
    }
    
    if(key == 't' && TCPsetup) {
        timer = !timer;
    }
    
    if(key == 'k' && TCPsetup) {
        keyList = !keyList;
    }
    
    if(key == 'l' && TCPsetup) {
        costList = !costList;
    }
    
}

//--------------------------------------------------------------
void ofApp::audioOut( float * output, int bufferSize, int nChannels ) {
    
    sound.synth.fillBufferOfFloats(output, bufferSize, nChannels);
    
}

//--------------------------------------------------------------
void ofApp::soundChange(string name, int index, float value) {
    
    if(name == "envelope"){
        
        if(disc.isMute(index) == 0) disc.setEnvelope(index, value);
        else disc.setEnvelope(index, 0);
        
        sound.synth.setParameter("attack"+ofToString(index),disc.getEnvelope(index, 0));
        sound.synth.setParameter("decay"+ofToString(index),disc.getEnvelope(index, 1));
        sound.synth.setParameter("sustain"+ofToString(index),disc.getEnvelope(index, 2));
        sound.synth.setParameter("release"+ofToString(index),disc.getEnvelope(index, 3));
        
        
        float volCoeff = 1;
        if(disc.getTexture(index) == 1) volCoeff = 1.3;
        else if(disc.getTexture(index) == 2) volCoeff = .5;
        else if(disc.getTexture(index) == 3) volCoeff = .5;
        else if(disc.getTexture(index) == 4) volCoeff = .1;
        
        sound.synth.setParameter("volBalance"+ofToString(index), volCoeff);
        
        //ui
        ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
        ofxUISlider *slider1 = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(index+1)));
        ofxUISlider *slider2 = static_cast <ofxUISlider*> (canvas->getWidget("density"+ofToString(index+1)));
        ofxUISlider *slider3 = static_cast <ofxUISlider*> (canvas->getWidget("radius"+ofToString(index+1)));
        ofxUISlider *slider4 = static_cast <ofxUISlider*> (canvas->getWidget("spike"+ofToString(index+1)));
        ofxUIButton *button1 = static_cast <ofxUIButton*> (canvas->getWidget("move"));
        ofxUIButton *button2 = static_cast <ofxUIButton*> (canvas->getWidget("reset"));
        ofxUIButton *button3 = static_cast <ofxUIButton*> (canvas->getWidget("mute"));
        
        if(disc.getTexture(index) == 0){
            slider1->setVisible(false);
            slider2->setVisible(false);
            slider3->setVisible(false);
            slider4->setVisible(false);
            button1->setVisible(false);
            button2->setVisible(false);
            button3->setVisible(false);
        }
        else {
            slider1->setVisible(true);
            slider2->setVisible(true);
            slider3->setVisible(true);
            slider4->setVisible(true);
            button1->setVisible(true);
            button2->setVisible(true);
            button3->setVisible(true);
        }
    }
    
    else sound.synth.setParameter(name+ofToString(index), value);
    
}

//--------------------------------------------------------------
void ofApp::newUI(int newIndex){
    
    
    //new UI
    ofxUICanvas *_ui;
    
    _ui = new ofxUICanvas();
    _ui->setFont(OF_TTF_MONO);
    _ui->addMultiImageToggle("inner","butonlar/buton-06.png", false, 20, 20, OFX_UI_ALIGN_LEFT);
    _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    _ui->addLabel("Groove " + ofToString(newIndex+1),0);
    ofxUILabel *label = (ofxUILabel*) _ui->getWidget("Groove " + ofToString(newIndex+1));
    _ui->addWidgetPosition(label,OFX_UI_WIDGET_POSITION_RIGHT ,OFX_UI_ALIGN_CENTER);
    _ui->addMultiImageToggle("outer", "butonlar/buton-07.png",false, 20, 20);
    ofxUIMultiImageToggle *toggle = (ofxUIMultiImageToggle*) _ui->getWidget("outer");
    _ui->addWidgetPosition(toggle,OFX_UI_WIDGET_POSITION_RIGHT ,OFX_UI_ALIGN_RIGHT);
    _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    _ui->addSpacer();
    
    _ui->addLabel("texture", 1);
    if(disc.getTexture(newIndex)==0) _ui->addMultiImageButton("blank","butonlar/buton-01.png", true, 35,35);
    else _ui->addMultiImageButton("blank","butonlar/buton-01.png", false, 35,35);
    _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    if(disc.getTexture(newIndex)==1) _ui->addMultiImageButton("line", "butonlar/buton-02.png", true, 35,35);
    else _ui->addMultiImageButton("line", "butonlar/buton-02.png", false, 35,35);
    if(disc.getTexture(newIndex)==2) _ui->addMultiImageButton("tri", "butonlar/buton-03.png", true, 35,35);
    else _ui->addMultiImageButton("tri", "butonlar/buton-03.png", false, 35,35);
    if(disc.getTexture(newIndex)==3) _ui->addMultiImageButton("saw", "butonlar/buton-04.png", true, 35,35);
    else _ui->addMultiImageButton("saw", "butonlar/buton-04.png", false, 35,35);
    if(disc.getTexture(newIndex)==4) _ui->addMultiImageButton("rect", "butonlar/buton-05.png", true, 35,35);
    else _ui->addMultiImageButton("rect", "butonlar/buton-05.png", false, 35,35);
    _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    _ui->addLabel("rotation speed",1);
    _ui->addBiLabelSlider("rotation" + ofToString(newIndex+1), "<", ">", 10, -10, disc.getNetRotationSpeed(newIndex));
    _ui->addLabel("density",1);
    _ui->addBiLabelSlider("density" + ofToString(newIndex+1), "| | |", "|||||", 30, 3, disc.getDensity(newIndex));
    _ui->addLabel("size",1);
    _ui->addBiLabelSlider("radius" + ofToString(newIndex+1), "o", "O", 15, 100, disc.getThickness(newIndex));
    _ui->addLabel("spike",1);
    _ui->addBiLabelSlider("spike" + ofToString(newIndex+1), ".", "^", 0, 100, disc.getSpikeDistance(newIndex));
    
    
    _ui->addLabel("z-motion",1);
    _ui->addToggle("move", disc.isMoving(newIndex));
    _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    _ui->addButton("reset", disc.resetPerlin[newIndex]);
    _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    _ui->addLabelToggle("mute", disc.isMute(newIndex));
    
    _ui->autoSizeToFitWidgets();
    
    if(me != NULL && newIndex == me->getDiscIndex()) {
        
        _ui->setVisible(true);
        addDisc->setVisible(false);
    }
    else _ui->setVisible(false);
    
    ofAddListener(_ui->newGUIEvent, this, &ofApp::guiEvent);
    
    ui.push_back(_ui);
    
    
    _ui->setColorBack(me->getColor());
    
    
    //ui
    ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[newIndex]);
    ofxUISlider *slider1 = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(newIndex+1)));
    ofxUISlider *slider2 = static_cast <ofxUISlider*> (canvas->getWidget("density"+ofToString(newIndex+1)));
    ofxUISlider *slider3 = static_cast <ofxUISlider*> (canvas->getWidget("radius"+ofToString(newIndex+1)));
    ofxUISlider *slider4 = static_cast <ofxUISlider*> (canvas->getWidget("spike"+ofToString(newIndex+1)));
    ofxUIButton *button1 = static_cast <ofxUIButton*> (canvas->getWidget("move"));
    ofxUIButton *button2 = static_cast <ofxUIButton*> (canvas->getWidget("reset"));
    ofxUIButton *button3 = static_cast <ofxUIButton*> (canvas->getWidget("mute"));
    
    
    
    if(disc.getTexture(newIndex) == 0){
        slider1->setVisible(false);
        slider2->setVisible(false);
        slider3->setVisible(false);
        slider4->setVisible(false);
        button1->setVisible(false);
        button2->setVisible(false);
        button3->setVisible(false);
    }
    else {
        slider1->setVisible(true);
        slider2->setVisible(true);
        slider3->setVisible(true);
        slider4->setVisible(true);
        button1->setVisible(true);
        button2->setVisible(true);
        button3->setVisible(true);
    }
    
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    mReleased = false;
    if(me != NULL && me->getDiscIndex()>=0) thisRotation = disc.getNetRotationSpeed(me->getDiscIndex());
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    mReleased = true;
    
    if(radiusChanged) {
        radiusChanged = false;
        me->changeLife(costRadius);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costRadius) + "//";
        client.send(lifeUpdate);
        
        string change = "radius//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.getThickness(me->getDiscIndex()))+"//"+me->getIP();
        client.send(change);
        
        //update history//
        string eventHistory = me->getNick() + " // " + "Grv"+ofToString(me->getDiscIndex()+1)+ " // " + "size:" + ofToString(roundf(disc.getThickness(me->getDiscIndex())*10)/10) +"\n\n";
        historyText = eventHistory + historyText;
        
        ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
        _history->setTextString(historyText);
        
    }
    else if(densityChanged){
        densityChanged = false;
        me->changeLife(costDensity);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costDensity) + "//";
        client.send(lifeUpdate);
        
        string change = "density//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getDensity(me->getDiscIndex()))+"//"+me->getIP();
        client.send(change);
        
        //update history//
        string eventHistory = me->getNick() + " // " + "Grv"+ofToString(me->getDiscIndex()+1)+ " // " + "density:" + ofToString(disc.getDensity(me->getDiscIndex())) +"\n\n";
        historyText = eventHistory + historyText;
        
        ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
        _history->setTextString(historyText);
    }
    
    else if(spikeChanged){
        spikeChanged = false;
        me->changeLife(costSpike);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costSpike) + "//";
        client.send(lifeUpdate);
        
        string change = "spike//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getSpikeDistance(me->getDiscIndex()))+"//"+me->getIP();
        client.send(change);
        
        
        //update history//
        string eventHistory = me->getNick() + " // " + "Grv"+ofToString(me->getDiscIndex()+1)+ " // " + "spike:" + ofToString(disc.getSpikeDistance(me->getDiscIndex())) +"\n\n";
        historyText = eventHistory + historyText;
        
        ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
        _history->setTextString(historyText);
    }
    
    else if(textureChanged){
        textureChanged = false;
        me->changeLife(costTexture);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costTexture) + "//";
        client.send(lifeUpdate);
        
        string change = "texture//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getTexture(me->getDiscIndex()))+"//"+me->getIP();
        client.send(change);
    }
    else if(rotationChanged){
        rotationChanged = false;
        me->changeLife(costRotation);
        
        //update ui
        ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
        ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(me->getDiscIndex()+1)));
        float newRotation = slider->getValue() - thisRotation;
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costRotation) + "//";
        client.send(lifeUpdate);
        
        //send to server
        string change = "rotationSpeed//"+ ofToString(me->getDiscIndex())+": "+ ofToString(newRotation)+"//"+me->getIP();
        client.send(change);
        
        //update history//
        string eventHistory = me->getNick() + " // " + "Grv"+ofToString(me->getDiscIndex()+1)+ " // " + "rotation:" + ofToString( roundf(disc.getNetRotationSpeed(me->getDiscIndex())*10)/10) +"\n\n";
        historyText = eventHistory + historyText;
        
        ofxUITextArea *_history = (ofxUITextArea *) history->getWidget("history");
        _history->setTextString(historyText);
    }
    
    //    moveChanged is in update()
    
    else if(moveReset){
        moveReset = false;
        me->changeLife(costMove);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "lifeChange: "+ofToString(costMove) + "//";
        client.send(lifeUpdate);
        
        string movementReset = "moveReset//"+ofToString(me->getDiscIndex())+"//"+me->getIP();
        client.send(movementReset);
    }
    
    else if(newDisc){
        
        newDisc = false;
        
        //        //send demand to server
        //        string newDisc = "newDisc//"+me->getIP();
        //        client.send(newDisc);
        //
        //
        //        ofxUIToggle *toggle = (ofxUIToggle *) addDisc->getWidget("new");
        //        toggle->setValue(false);
        
    }
    // cost calculation and server notifications of move/stop/resetAll buttons are handled in guiEvent()
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}