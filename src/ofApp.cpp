#include "ofApp.h"

/*
 
 perlin cpp - changed to sin Ã
 groove move factor ~(rotationSpeed*thickness/density) Ã
 
 event button create
 chat button
 
 move-stop-reset TCP Ã
 move-stop-reset costs Ã
 position update TCP Ã
 
 mute button Ã
 update mute button in initialize and change Ã
 change color maybe?
 
 */


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofBackground(255);
    
    //set up network
    client.setup("127.0.0.1", 10002);
    client.setMessageDelimiter("varianet");
    
    // ask for server state
    client.send("hello//");
    
    // set up values of objects
    disc.setup();
    
    //set up gui
    for(int i = 0; i < disc.getDiscIndex(); i++){
        
        ofxUICanvas *_ui;
        
        _ui = new ofxUICanvas();
        _ui->addButton("inner", false);
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        _ui->addLabel("Groove " + ofToString(i+1),0);
        _ui->addButton("outer", false);
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        _ui->addLabelToggle("mute", disc.isMute(i));
        _ui->addSpacer();
        _ui->addLabel("rotation",2);
        _ui->addBiLabelSlider("rotation" + ofToString(i+1), "CCW", "CW", 10, -10, disc.getNetRotationSpeed(i));
        _ui->addLabel("size",2);
        _ui->addBiLabelSlider("radius" + ofToString(i+1), "small", "large", 15, 100, disc.getRadius(i)-disc.getRadius(i-1));
        _ui->addLabel("density",2);
        _ui->addBiLabelSlider("density" + ofToString(i+1), "sparse", "dense", 30, 1, disc.getDensity(i));
        
        _ui->addLabel("texture", 1);
        if(disc.getTexture(i)==0) _ui->addToggle("blank", true);
        else _ui->addToggle("blank", false);
        
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        if(disc.getTexture(i)==1) _ui->addToggle("line", true);
        else _ui->addToggle("line", false);
        if(disc.getTexture(i)==2) _ui->addToggle("tri", true);
        else _ui->addToggle("tri", false);
        if(disc.getTexture(i)==3) _ui->addToggle("saw", true);
        else _ui->addToggle("saw", false);
        if(disc.getTexture(i)==4) _ui->addToggle("rect", true);
        else _ui->addToggle("rect", false);
        
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        _ui->addLabel("depth",1);
        _ui->addToggle("move", disc.isMoving(i));
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        _ui->addButton("reset", disc.resetPerlin[i]);
        
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        _ui->addSpacer();
        _ui->addToggle("move all", disc.isMoving(i));
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        _ui->addButton("reset all", disc.resetPerlin[i]);
        _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        _ui->addSpacer();
        _ui->addLabelToggle("chat", false);
        
        _ui->autoSizeToFitWidgets();
        _ui->setVisible(false);
        ofAddListener(_ui->newGUIEvent, this, &ofApp::guiEvent);
        
        ui.push_back(_ui);
        
    }
    
//    updateButtons = new ofxUICanvas();
//    updateButtons->setPosition(0, ofGetHeight()/2);
//    updateButtons->setDrawBack(false);
//    updateButtons->setFontSize(OFX_UI_FONT_SMALL, 5.8);
//    ofAddListener(updateButtons->newGUIEvent, this, &ofApp::guiEvent);
    
    chat = new ofxUICanvas();
    chat->setDrawBack(false);
    int chatWidth = 600;
    chat->setPosition(ofGetWidth()/2-chatWidth/2, 0);
    chat->setDimensions(chatWidth, ofGetHeight());
    chat->setColorFill(ofxUIColor(0,200,0));
    conversation = "";
    chat->addTextInput("chatInput", "", OFX_UI_FONT_LARGE)->setAutoUnfocus(false);
    chat->addTextArea("chat", "", OFX_UI_FONT_LARGE);
    chat->setVisible(false);
    
    ofAddListener(chat->newGUIEvent, this, &ofApp::guiEvent);
    
    //set up audio stream & synth network
    phase = 0;
    volume = 0;
    ofSoundStreamSetup(2, 0); // 2 out, 0 in
    
    //    sound.setup(&disc);
    
    // set up game costs
    costRadius = 1;
    costDensity = 1;
    costTexture = 1;
    costRotation = 1;
    costMute = 5;
    costMove = 2;
    
}
//--------------------------------------------------------------
void ofApp::exit(){
    
    for(int i = 0; i < disc.getDiscIndex(); i++){
        delete ui[i];
    }
    
    me->setConnection(false);
    string playerOut;
    playerOut = "goodbye//"+me->getIP();
    client.send(playerOut);
    
}
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    
    for(int i = 0; i < disc.getDiscIndex(); i++){
        if(e.getName() == "rotation" + ofToString(i+1)){
            ofxUISlider *slider = e.getSlider();
            if(me->getLife()>0 && mReleased == false){
                rotationChanged = true;
                float newRotation = slider->getScaledValue()-disc.getNetRotationSpeed(i);
                disc.setRotationSpeed(i, newRotation);
                
                //change sound
                float netSpeed = abs(disc.getNetRotationSpeed(i));
                float beat = ofMap(netSpeed, 0, 10, 0, 1000);
                soundChange("bpm", i, beat);
                
                //send to server
                string change = "rotationSpeed//"+ ofToString(i)+": "+ ofToString(newRotation);
                client.send(change);
                
            }
        }
        else if(e.getName() == "radius" + ofToString(i+1)){
            ofxUISlider *slider = e.getSlider();
            if(me->getLife()> 0 && mReleased == false) {
                radiusChanged = true;
                disc.setThickness(i, slider->getScaledValue());
                
                //change sound
                float q = ofMap(disc.getRadius(i)-disc.getRadius(i-1), 15, 100, 10, 0);
                soundChange("q", i, q);
                
                //send to server
                string change = "radius//"+ofToString(i)+": "+ofToString(slider->getScaledValue());
                client.send(change);
            }
        }
        else if(e.getName() == "density" + ofToString(i+1)){
            ofxUISlider *slider = e.getSlider();
            if(me->getLife()> 0 && mReleased == false) {
                densityChanged = true;
                disc.setDensity(i, slider->getScaledValue());
                
                //change sound
                float envelopeCoeff = ofMap(disc.getDensity(i), 1, 30, 1, 5);
                float pulseRatio = ofMap(disc.getDensity(i), 1, 30, 0.001, 1);
                soundChange("envelopeWidth", i, envelopeCoeff);
                soundChange("pulseLength", i, pulseRatio);
                
                //send to server
                string change = "density//"+ ofToString(i)+": "+ ofToString(slider->getScaledValue());
                client.send(change);
                
            }
        }
    }
    if(e.getKind() == OFX_UI_WIDGET_LABELTOGGLE && e.getName() != "mute" && e.getName() != "chat"){
        
        ofxUILabelToggle *updateButton = static_cast <ofxUILabelToggle*> (e.getToggle());
        if(updateButton->getValue() == true){
            me->setLife(me->getLife()+5);
        }
        else updateButton->setValue(true);
        
    }
    else if(e.getName() == "inner"){
        ofxUIButton *button = e.getButton();
        
        int jump = 1;
        bool occupied = true;
        while(occupied == true){
            occupied = false;
            for(int i = 0; i < otherPlayers.size(); i++){
                int destination = me->getDiscIndex() - jump;
                if( destination > disc.getDiscIndex()) destination += disc.getDiscIndex();
                if(otherPlayers[i]->getDiscIndex() == destination) {
                    occupied = true;
                    jump++;
                    break;
                }
                else continue;
            }
        }
        if(me->getDiscIndex() - jump > -1) me->setDiscIndex(me->getDiscIndex() - jump);
        else me->setDiscIndex(me->getDiscIndex() - jump + disc.getDiscIndex());
        
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
                if( destination > disc.getDiscIndex()) destination -= disc.getDiscIndex();
                if(otherPlayers[i]->getDiscIndex() == destination) {
                    occupied = true;
                    jump++;
                    break;
                }
                else continue;
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
            input = me->getIP() + "::" + text->getTextString()+"\n\n";
            
        }
        conversation = input + conversation;
        ofxUITextArea *history = (ofxUITextArea *) chat->getWidget("chat");
        history->setTextString(conversation);
        
    }
    else if(e.getName() == "blank"){
        ofxUIToggle *toggle = e.getToggle();
        int texture = disc.getTexture(me->getDiscIndex());
        if(me->getLife()> 0 && texture != 0) {
            
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
        //sound
        soundChange("bpm", me->getDiscIndex(), 0);
        soundChange("envelope", me->getDiscIndex(), 0);
        
        //send to server
        string change = "texture//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getTexture(me->getDiscIndex()));
        client.send(change);
    }
    else if(e.getName() == "line"){
        ofxUIToggle *toggle = e.getToggle();
        int texture = disc.getTexture(me->getDiscIndex());
        if(me->getLife()> 0 && texture != 1) {
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
        }
        else toggle->setValue(true);
        
        //sound
        soundChange("envelope", me->getDiscIndex(), 1);
        
        //send to server
        string change = "texture//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getTexture(me->getDiscIndex()));
        client.send(change);
        
    }
    else if(e.getName() == "tri"){
        ofxUIToggle *toggle = e.getToggle();
        int texture = disc.getTexture(me->getDiscIndex());
        if(me->getLife() > 0 && texture != 2) {
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
        }
        else toggle->setValue(true);
        
        //sound
        soundChange("envelope", me->getDiscIndex(), 2);
        
        //send to server
        string change = "texture//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getTexture(me->getDiscIndex()));
        client.send(change);
    }
    else if(e.getName() == "saw"){
        ofxUIToggle *toggle = e.getToggle();
        int texture = disc.getTexture(me->getDiscIndex());
        if(me->getLife() > 0 && texture != 3) {
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
        }
        else toggle->setValue(true);
        
        //sound
        soundChange("envelope", me->getDiscIndex(), 3);
        
        //send to server
        string change = "texture//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getTexture(me->getDiscIndex()));
        client.send(change);
    }
    else if(e.getName() == "rect"){
        ofxUIToggle *toggle = e.getToggle();
        int texture = disc.getTexture(me->getDiscIndex());
        if(me->getLife() > 0 && texture != 4) {
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
        }
        else toggle->setValue(true);
        
        //sound
        soundChange("envelope", me->getDiscIndex(), 4);
        
        //send to server
        string change = "texture//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getTexture(me->getDiscIndex()));
        client.send(change);
    }
    else if(e.getName() == "move"){
        ofxUIToggle *toggle = e.getToggle();
        if(me->getLife()>0){
            if (toggle->getValue() == true) {
                me->setLife(me->getLife()-costMove);
                disc.setMoving(me->getDiscIndex(), 1);
            }
            else {
                disc.setMoving(me->getDiscIndex(), 0);
            }
            
            string movement = "move//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.isMoving(me->getDiscIndex()));
            client.send(movement);
            
            if (toggle->getValue() == false){
                string position = "zPosition//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.getPosition(me->getDiscIndex()));
                string counter = "counter//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.getCounter(me->getDiscIndex()));
                client.send(position);
                client.send(counter);
                
            }
            
            //update server
            string lifeUpdate = "life//";
            lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
            lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
            client.send(lifeUpdate);
        }
    }
    else if(e.getName() == "reset"){
        ofxUIButton *button = e.getButton();
        if(me->getLife()>0){
            disc.resetPerlin[me->getDiscIndex()] = 1;
            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
            ofxUIToggle *toggle = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
            toggle->setValue(false);
            
            string movementReset = "moveReset//"+ofToString(me->getDiscIndex());
            client.send(movementReset);
        }
    }
    else if(e.getName() == "move all"){
        ofxUIToggle *toggle = e.getToggle();
        if(toggle->getValue() == true){
            if(me->getLife()>0){
                int costFactor = 0;
                for(int j = 0; j<disc.getDiscIndex(); j++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                    ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMoveAll->setValue(true);
                    if(toggleMove->getValue()==false){
                        toggleMove->setValue(true);
                        disc.setMoving(j, 1);
                        costFactor++;
                    }
                }
                me->setLife(me->getLife()-(costMove*costFactor));
                string moveAll = "moveAll//";
                client.send(moveAll);
                
                //update server
                string lifeUpdate = "life//";
                lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
                lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
                client.send(lifeUpdate);
            }
        }
        if(toggle->getValue() == false){
            if(me->getLife()>0){
                int costFactor = 0;
                string zPositionAll = "zPositionAll//";
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMoveAll->setValue(false);
                    if(toggleMove->getValue()==true){
                        toggleMove->setValue(false);
                        disc.setMoving(i, 0);
                        zPositionAll += ofToString(i)+": "+ofToString(disc.getPosition(i))+"//";
                        costFactor++;
                    }
                }
                me->setLife(me->getLife()-(costMove*costFactor));
                string stopAll = "stopAll//";
                client.send(stopAll);
                client.send(zPositionAll);
                
                //update server
                string lifeUpdate = "life//";
                lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
                lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
                client.send(lifeUpdate);
            }
        }
    }
    else if(e.getName() == "reset all"){
        ofxUIButton *button = e.getButton();
        if(button->getValue() == true){
            if(me->getLife()>0){
                int costFactor = 0;
                for(int j = 0; j<disc.getDiscIndex(); j++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
                    ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMoveAll->setValue(false);
                    if(disc.getPosition(j) != 0){
                        toggleMove->setValue(false);
                        disc.resetPerlin[j] = 1;
                        costFactor++;
                    }
                }
                me->setLife(me->getLife()-(costMove*costFactor));
                string resetAll = "resetAll//";
                client.send(resetAll);
                
                //update server
                string lifeUpdate = "life//";
                lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
                lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
                client.send(lifeUpdate);
            }
        }
    }
    else if(e.getName() == "mute"){
        ofxUIToggle *toggle = e.getToggle();
        if(me->getLife() > 0){
            me->setLife(me->getLife()-costMute);
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
            string change = "mute//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.isMute(me->getDiscIndex()));
            client.send(change);
            
            cout<< change <<endl;
        }
    }
    else if(e.getName() == "chat"){
        ofxUIToggle *toggle = e.getToggle();
        for(int j = 0; j<disc.getDiscIndex(); j++){
            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
            ofxUIToggle *toggleChat = static_cast <ofxUIToggle*> (canvas->getWidget("chat"));
            toggleChat->setValue(toggle->getValue());
        }
        chat->setVisible(toggle->getValue());
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
        disc.update();
    float size = 0;
    for(int i = 0; i< disc.getDiscIndex(); i++){
        float amountFreq = ofMap(abs(disc.getNetRotationSpeed(i)), 0, 10, 0, 10000);
        float amountMod = ofMap(abs(disc.getPosition(i)), 0, 50, 0, 10000);
        soundChange("amountFreq", i, amountFreq);
        soundChange("amountMod", i, amountMod);
        size += abs(disc.getPosition(i));
    }
    float avgSize = ofMap(size/disc.getDiscIndex(), 0, 200, 0.01, 1);
    sound.synth.setParameter("size", avgSize);
    sound.synth.setParameter("decay", .001*size);
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
                    for(int j = 0; j < 10; j++){
                        nameValue = ofSplitString(received[j+(i*10)+2], ": ");
                        if(nameValue[0] == "radius"+ofToString(i)) {
                            disc.setRadius(i, ofToFloat(nameValue[1]));
                            //sound
                            float q = ofMap(disc.getRadius(i)-disc.getRadius(i-1), 15, 100, 10, 0);
                            soundChange("q", i, q);
                            //ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("radius"+ofToString(i+1)));
                            slider->setValue(disc.getRadius(i)-disc.getRadius(i-1));
                        }
                        if(nameValue[0] == "density"+ofToString(i)) {
                            disc.setDensity (i, ofToFloat(nameValue[1]));
                            //sound
                            float envelopeCoeff = ofMap(disc.getDensity(i), 1, 30, 1, 5);
                            float pulseRatio = ofMap(disc.getDensity(i), 1, 30, 0.001, 1);
                            soundChange("envelopeWidth", i, envelopeCoeff);
                            soundChange("pulseLength", i, pulseRatio);
                            //ui
                            ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
                            ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("density"+ofToString(i+1)));
                            slider->setValue(disc.getDensity(i));
                        }
                        if(nameValue[0] == "rotation"+ofToString(i)) {
                            disc.setRotation (i, ofToFloat(nameValue[1]));
                        }
                        if(nameValue[0] == "rotationSpeed"+ofToString(i)) {
                            disc.setNetRotationSpeed (i, ofToFloat(nameValue[1]));
                            //sound
                            float netSpeed = abs(disc.getNetRotationSpeed(i));
                            float beat = ofMap(netSpeed, 0, 10, 0, 1000);
                            soundChange("bpm", i, beat);
                            //ui
                            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(i+1)));
                            slider->setValue(ofToFloat(nameValue[1]));
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
                            if(disc.getTexture(i) == 0) toggle0->setValue(true);
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
                        if(nameValue[0] == "posOffset"+ofToString(i)) {
                            disc.setPosOffset (i, ofToFloat(nameValue[1]));
                            //sound
//                            float amountFreq = ofMap(abs(disc.getNetRotationSpeed(i)), 0, 10, 0, 5000);
//                            float amountMod = ofMap(abs(disc.getPosition(i)), 0, 50, 0, 5000);
//                            soundChange("amountFreq", i, amountFreq);
//                            soundChange("amountMod", i, amountMod);
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
                        if(nameValue[0] == "perlin"+ofToString(i) && ofToInt(nameValue[1]) == 1 ) {
                            if (disc.isMoving(i) == 0) disc.setMoving(i, 1);
                            else disc.setMoving(i, 0);
                        }
                    }
                }
                //set up synths
                sound.setup(&disc);
            }
            
            else if (title == "scale"){
                //sound values
                vector<string> scaleValue;
                scaleValue = ofSplitString(received[1], ": ");
                for(int i = 0; i < scaleValue.size(); i++){
                    sound.setScale(i, ofToFloat(scaleValue[i]));
                }
                //set up synths
                sound.setup(&disc);
            }
            
            else if (title == "playerInfo" ){
                Player* _player = new Player();
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "IP") _player->setIP(playerData[1]);
                    if (playerData[0] == "color") _player->setColor(ofFromString<ofColor>(playerData[1]));
                    if (playerData[0] == "life") _player->setLife(ofToFloat(playerData[1]));
                    if (playerData[0] == "index") _player->setDiscIndex(ofToInt(playerData[1]));
                }
                _player->setConnection(true);
                me = _player;
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
                            else continue;
                        }
                        if (exists == false){
                         otherPlayers.push_back(_player);
                            _player->setIP(playerData[1]);
                        }
                    }
                    if (playerData[0] == "color") _player->setColor(ofFromString<ofColor>(playerData[1]));
                    if (playerData[0] == "life") _player->setLife(ofToFloat(playerData[1]));
                    if (playerData[0] == "index") _player->setDiscIndex(ofToInt(playerData[1]));
                }
                groove.setup(&disc, me, otherPlayers);
            }
            
            else if (title == "otherPlayersIndex"){
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    int thisPlayer;
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
                    cout<< received[i] <<endl;
                }
            }
            
            else if (title == "life"){
                for(int i = 1; i < received.size(); i++ ){
                    vector<string> playerData;
                    int thisPlayer;
                    playerData = ofSplitString(received[i], ": ");
                    if (playerData[0] == "IP"){
                        for (int j = 0; j < otherPlayers.size(); j++) {
                            if(playerData[1] == otherPlayers[j]->getIP()) {
                                thisPlayer = j;
                                break;
                            }
                        }
                    }
                    if (playerData[0] == "life") otherPlayers[thisPlayer]->setLife(ofToFloat(playerData[1]));
                    cout<< received[i] <<endl;
                }
            }
            
            else if (title == "rotationSpeed"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setRotationSpeed(index, ofToFloat(nameValue[1]));
                
                //change sound
                float netSpeed = abs(disc.getNetRotationSpeed(index));
                float beat = ofMap(netSpeed, 0, 10, 0, 1000);
                soundChange("bpm", index, beat);
                
                //update ui
                ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[index]);
                ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(index+1)));
                slider->setValue(disc.getNetRotationSpeed(index));
            }
            
            else if (title == "radius"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int index = ofToInt(nameValue[0]);
                disc.setThickness(index, ofToFloat(nameValue[1]));
                
                //change sound
                float q = ofMap(disc.getRadius(index)-disc.getRadius(index-1), 15, 100, 10, 0);
                soundChange("q", ofToInt(nameValue[0]), q);
                
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("radius"+ofToString(index+1)));
                slider->setValue(disc.getRadius(index)-disc.getRadius(index-1));
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
                soundChange("pulseLength", index, pulseRatio);
                
                //update ui
                ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
                ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("density"+ofToString(index+1)));
                slider->setValue(disc.getDensity(index));
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
                if(disc.getTexture(index) == 0) toggle0->setValue(true);
                else toggle0->setValue(false);
                if(disc.getTexture(index) == 1) toggle1->setValue(true);
                else toggle1->setValue(false);
                if(disc.getTexture(index) == 2) toggle2->setValue(true);
                else toggle2->setValue(false);
                if(disc.getTexture(index) == 3) toggle3->setValue(true);
                else toggle3->setValue(false);
                if(disc.getTexture(index) == 4) toggle4->setValue(true);
                else toggle4->setValue(false);
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
            }
            
            else if (title == "move"){
                vector<string> nameValue;
                nameValue = ofSplitString(received[1], ": ");
                int thisDisc = ofToInt(nameValue[0]);
                disc.setMoving(thisDisc, ofToInt(nameValue[1]));
            }
            
            else if (title == "moveReset"){
                int thisDisc = ofToInt(received[1]);
                disc.resetPerlin[thisDisc] = 1;
            }
            
            else if (title == "moveAll"){
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMoveAll->setValue(true);
                    toggleMove->setValue(true);
                    disc.setMoving(i, 1);
                }
            }
            
            else if (title == "stopAll"){
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMoveAll->setValue(false);
                    toggleMove->setValue(false);
                    disc.setMoving(i, 0);
                }
            }
            
            else if (title == "resetAll"){
                for(int i = 0; i<disc.getDiscIndex(); i++){
                    disc.resetPerlin[i] = 1;
                    ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
                    ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
                    ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
                    toggleMoveAll->setValue(false);
                    toggleMove->setValue(false);
                }
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
                disc.setCounter(ofToInt(nameValue[0]), ofToInt(nameValue[1]));
            }
            
            else if (title == "chat"){
                
                conversation = received[1] + conversation;
                ofxUITextArea *history = (ofxUITextArea *) chat->getWidget("chat");
                history->setTextString(conversation);
                
            }
            
            else if (title == "goodbye"){
                for (int i = 0; i < otherPlayers.size(); i++) {
                    if(received[1] == otherPlayers[i]->getIP()) {
                        otherPlayers[i]->setConnection(false);
                        otherPlayers[i]->setDiscIndex(-1);
                    }
                    else continue;
                }
            }
        
        }
    }
    
    groove.update();
}
//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(255, 233);
    glEnable(GL_DEPTH_TEST);
    
    ofPushMatrix();
    
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    cam.begin();
    groove.draw();
    cam.end();
    
    ofSetColor(me->getColor());
    ofFill();
    ofRect(groove.lifeBar[0]);
    
    for(int i = 0; i < otherPlayers.size(); i++){
        ofSetColor(otherPlayers[i]->getColor());
        ofFill();
        ofRect(groove.lifeBar[i+1]);
    }
    
    for(int i = 0; i < updateButtonsArray.size(); i++){
        updateButtonsArray[i]->draw();
    }
    
    ofPopMatrix();
    
    glDisable(GL_DEPTH_TEST);
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(chat->hasKeyboardFocus())
    {
        return;
    }
    
    if(key == ' ') groove.turn = !groove.turn;
    if(key == 'p'){
        if (disc.isMoving(me->getDiscIndex()) == 0) disc.setMoving(me->getDiscIndex(), 1);
        else disc.setMoving(me->getDiscIndex(), 0);
    }
    if(key == 'o') disc.resetPerlin[me->getDiscIndex()] = 1;
    
    if(key == 'c') chat->toggleVisible();
    
    if(key == 'a' && me->getDiscIndex() != -1) {
        
        if(disc.getLife() > 0) {
            disc.setLife(costRotation);     // reduce life
            disc.setRotationSpeed(me->getDiscIndex(), +.05);
            
            //update ui
            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(me->getDiscIndex()+1)));
            slider->setValue(disc.getNetRotationSpeed(me->getDiscIndex()));
            
            //change sound
            float netSpeed = abs(disc.getNetRotationSpeed(me->getDiscIndex()));
            float beat = ofMap(netSpeed, 0, 10, 0, 1000);
            sound.synth.setParameter("bpm"+ofToString(me->getDiscIndex()), beat);
            
            //send to server
            float newRotation = slider->getScaledValue()-disc.getNetRotationSpeed(me->getDiscIndex());
            string change = "rotationSpeed//"+ ofToString(me->getDiscIndex())+": "+ ofToString(+0.05);
            client.send(change);
        }
    }
    
    if(key == 'd' && me->getDiscIndex() != -1 ) {
        
        if(disc.getLife() > 0) {
            disc.setLife(costRotation);     // reduce life
            disc.setRotationSpeed(me->getDiscIndex(), -.05);
            
            //update ui
            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(me->getDiscIndex()+1)));
            slider->setValue(disc.getNetRotationSpeed(me->getDiscIndex()));
            
            //change sound
            float netSpeed = abs(disc.getNetRotationSpeed(me->getDiscIndex()));
            float beat = ofMap(netSpeed, 0, 10, 0, 1000);
            sound.synth.setParameter("bpm"+ofToString(me->getDiscIndex()), beat);
            
            //send to server
            float newRotation = slider->getScaledValue()-disc.getNetRotationSpeed(me->getDiscIndex());
            string change = "rotationSpeed//"+ ofToString(me->getDiscIndex())+": "+ ofToString(-0.05);
            client.send(change);
        }
    }
    
    if(key == 'w'){
        int jump = 1;
        bool occupied = true;
        while(occupied == true){
            occupied = false;
            for(int i = 0; i < otherPlayers.size(); i++){
                int destination = me->getDiscIndex() + jump;
                if( destination > disc.getDiscIndex()) destination -= disc.getDiscIndex();
                if(otherPlayers[i]->getDiscIndex() == destination) {
                    occupied = true;
                    jump++;
                    break;
                }
                else continue;
            }
        }
        
        if(me->getDiscIndex() + jump < disc.getDiscIndex()) me->setDiscIndex(me->getDiscIndex() + jump);
        else me->setDiscIndex(me->getDiscIndex() + jump - disc.getDiscIndex());
        
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
    
    if(key == 's'){
        int jump = 1;
        bool occupied = true;
        while(occupied == true){
            occupied = false;
            for(int i = 0; i < otherPlayers.size(); i++){
                int destination = me->getDiscIndex() - jump;
                if( destination > disc.getDiscIndex()) destination += disc.getDiscIndex();
                if(otherPlayers[i]->getDiscIndex() == destination) {
                    occupied = true;
                    jump++;
                    break;
                }
                else continue;
            }
        }
        if(me->getDiscIndex() - jump > -1) me->setDiscIndex(me->getDiscIndex() - jump);
        else me->setDiscIndex(me->getDiscIndex() - jump + disc.getDiscIndex());
        
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
    
    if(key == OF_KEY_BACKSPACE) {
        ui[me->getDiscIndex()]->setVisible(false);
        me->setDiscIndex(-1);
        
        //send change to server
        string changeDisc = "otherPlayersIndex//";
        changeDisc += "IP: "+ofToString(me->getIP()) + "//";
        changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
        client.send(changeDisc);
    }
    
    if(key == 'f') {
        fullScreen = !fullScreen;
        ofSetFullscreen(fullScreen);
    }
    
    if(key == 'i' && me->getDiscIndex() != -1 ) {
        disc.setPosition(me->getDiscIndex(), disc.getPosition(me->getDiscIndex())+1);
    }
    if(key == 'k' && me->getDiscIndex() != -1 ) {
        disc.setPosition(me->getDiscIndex(), disc.getPosition(me->getDiscIndex())-1);
    }
    if(key == 'm' && me->getDiscIndex() != -1 ) {
        if(me->getLife() > 0){
            disc.setLife(costMute);
            if(disc.isMute(me->getDiscIndex()) == 0) {
                disc.setMute(me->getDiscIndex(),1); //mute on
                soundChange("envelope", me->getDiscIndex(), 0);
            }
            else{
                disc.setMute(me->getDiscIndex(),0); //mute off
                disc.setEnvelope(me->getDiscIndex(), disc.getTexture(me->getDiscIndex()));
                soundChange("envelope", me->getDiscIndex(), disc.getTexture(me->getDiscIndex()));
            }
            string change = "mute//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.isMute(me->getDiscIndex()));
            client.send(change);
        }
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
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    mReleased = true;
    
    if(radiusChanged) {
        radiusChanged = false;
        me->setLife(me->getLife()-costRadius);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
        client.send(lifeUpdate);
        
        //update buttons
//        ofxUILabelToggle* toggle = new ofxUILabelToggle("Groove"+ofToString(me->getDiscIndex()+1)+" radius changed to "+ofToString(disc.getThickness(me->getDiscIndex())), false, 200, 50, -ofGetWidth()/2, updateButtonsArray.size()*55);
//        toggle->setColorBack(me->getColor());
//        updateButtonsArray.push_back(toggle);
//        ofxUICanvas* canvas = (ofxUICanvas*) toggle;
////        ofAddListener(canvas->newGUIEvent, this, &ofApp::guiEvent);
//        cout<< updateButtonsArray.size() <<endl;
        
//        ofxUIWidget* widget = (ofxUIWidget*) toggle;
//        widget->setPosition(OFX_UI_WIDGET_POSITION_UP);
        
        
//        updateButtons->addLabelToggle("Groove "+ofToString(me->getDiscIndex()+1)+" radius\n\nchanged to "+ofToString((int)disc.getThickness(me->getDiscIndex())), false, 200, 50)->setColorBack(me->getColor());
        
        
    }
    else if(densityChanged){
        densityChanged = false;
        me->setLife(me->getLife()-costDensity);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
        client.send(lifeUpdate);
    }
    else if(textureChanged){
        textureChanged = false;
        me->setLife(me->getLife()-costTexture);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
        client.send(lifeUpdate);
    }
    else if(rotationChanged){
        rotationChanged = false;
        me->setLife(me->getLife()-costRotation);
        
        //update server
        string lifeUpdate = "life//";
        lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
        lifeUpdate += "life: "+ofToString(me->getLife()) + "//";
        client.send(lifeUpdate);
    }
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

//--------------------------------------------------------------
//float ofApp::triangleWave(float frequency){
//
//    float phaseStep = frequency/44100;
//    phase += phaseStep;
//    return asin(sin(TWO_PI*phase))/PI;
//
//}
//
////--------------------------------------------------------------
//float ofApp::squareWave(float frequency){
//
//    float phaseStep = frequency/44100.;
//    phase += phaseStep;
//    if(phase > (1)) phase = phase - (1);
//    float y;
//    if(phase < .5) y = 1;
//    else y = -1;
//    return y;
//
//}
//
////--------------------------------------------------------------
//float ofApp::sawWave(float frequency){
//
//    float phaseStep = frequency/44100;
//    phase += phaseStep;
//    if(phase > (1)) phase = phase - (1);
//    return 1 - ( phase/.5 );
//
//}

//--------------------------------------------------------------
void ofApp::audioOut( float * output, int bufferSize, int nChannels ) {
    
    sound.synth.fillBufferOfFloats(output, bufferSize, nChannels);
    
    
    //    for(int i = 0; i < bufferSize; i++){
    //        float value = 0.1 *sawWave(220);
    //        output[2*i] = value;
    //        output[2*i+1] = value;
    //    }
    
}

//--------------------------------------------------------------
void ofApp::soundChange(string name, int index, float value) {
    
    if(name == "envelope"){
        if(disc.isMute(index) == 0){
            disc.setEnvelope(index, value);
            sound.synth.setParameter("attack"+ofToString(index),disc.getEnvelope(index, 0));
            sound.synth.setParameter("decay"+ofToString(index),disc.getEnvelope(index, 1));
            sound.synth.setParameter("sustain"+ofToString(index),disc.getEnvelope(index, 2));
            sound.synth.setParameter("release"+ofToString(index),disc.getEnvelope(index, 3));
        }
        else{
            disc.setEnvelope(index, 0);
            sound.synth.setParameter("attack"+ofToString(index),disc.getEnvelope(index, 0));
            sound.synth.setParameter("decay"+ofToString(index),disc.getEnvelope(index, 1));
            sound.synth.setParameter("sustain"+ofToString(index),disc.getEnvelope(index, 2));
            sound.synth.setParameter("release"+ofToString(index),disc.getEnvelope(index, 3));
        }
    }
    
    else sound.synth.setParameter(name+ofToString(index), value);
    
}
