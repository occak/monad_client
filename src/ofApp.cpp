#include "ofApp.h"

/*


*/

ofApp::ofApp() {
	moveChanged = false;
	TCPsetup = false;
    timer = false;
	radiusChanged = false;
	textureChanged = false;
	densityChanged = false;
	rotationChanged = false;
	positionChanged = false;
	moveChanged = false;
	moveReset = false;
	moveAllChanged = false;
	muteChanged = false;
	me = NULL;
}


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetDataPathRoot("../Resources/data/");

	ofSetVerticalSync(true);
	ofBackground(255);
	ofSetFrameRate(30);
	if( me == NULL){
		initialize = new ofxUICanvas();
		initialize->setPosition(ofGetWidth()/2-125, ofGetHeight()/2-50);
        initialize->setFont(OF_TTF_MONO);
        initialize->setDimensions(250, 200);
        initialize->addTextArea("welcome", "     Welcome to Monad.", 0);
		initialize->addLabel("Server IP :");
		initialize->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
		initialize->addTextInput("IP", "");
		initialize->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        initialize->addLabel("Nickname  :");
        initialize->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        initialize->addTextInput("nick", "");
		initialize->autoSizeToFitWidgets();
		ofAddListener(initialize->newGUIEvent, this, &ofApp::guiEvent);
	}

	//set up gui
	noDisc = new ofxUICanvas();
    noDisc->setFont(OF_TTF_MONO);
	noDisc->addMultiImageToggle("inner","butonlar/buton-06.png", false, 20, 20, OFX_UI_ALIGN_LEFT);
	noDisc->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	noDisc->addLabel("None Selected",1);
	ofxUILabel *label = (ofxUILabel*) noDisc->getWidget("None Selected");
	noDisc->addWidgetPosition(label,OFX_UI_WIDGET_POSITION_RIGHT ,OFX_UI_ALIGN_CENTER);
	noDisc->addMultiImageToggle("outer", "butonlar/buton-07.png",false, 20, 20);
	ofxUIMultiImageToggle *toggle = (ofxUIMultiImageToggle*) noDisc->getWidget("outer");
	noDisc->addWidgetPosition(toggle,OFX_UI_WIDGET_POSITION_RIGHT ,OFX_UI_ALIGN_RIGHT);
	noDisc->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	noDisc->addSpacer();
	noDisc->addToggle("move all", false);
	noDisc->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	noDisc->addButton("reset all", false);
	noDisc->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	noDisc->addSpacer();
	noDisc->addLabelToggle("chat", true);
	noDisc->autoSizeToFitWidgets();
	if (me == NULL || me->getDiscIndex() != -1) noDisc->setVisible(false);
	if (me != NULL) noDisc->setColorBack(me->getColor());
	ofAddListener(noDisc->newGUIEvent, this, &ofApp::guiEvent);


	if(me == NULL) {
		// set up values of objects
		disc.setup();

		//set up synths
		sound.setup(&disc);

		for(int i = 0; i < disc.getDiscIndex(); i++){

			ofxUICanvas *_ui;

			_ui = new ofxUICanvas();
            _ui->setFont(OF_TTF_MONO);
			_ui->addMultiImageToggle("inner","butonlar/buton-06.png", false, 20, 20, OFX_UI_ALIGN_LEFT);
			_ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
			_ui->addLabel("Groove " + ofToString(i+1),0);
			ofxUILabel *label = (ofxUILabel*) _ui->getWidget("Groove " + ofToString(i+1));
			_ui->addWidgetPosition(label,OFX_UI_WIDGET_POSITION_RIGHT ,OFX_UI_ALIGN_CENTER);
			_ui->addMultiImageToggle("outer", "butonlar/buton-07.png",false, 20, 20);
			ofxUIMultiImageToggle *toggle = (ofxUIMultiImageToggle*) _ui->getWidget("outer");
			_ui->addWidgetPosition(toggle,OFX_UI_WIDGET_POSITION_RIGHT ,OFX_UI_ALIGN_RIGHT);
			_ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
			_ui->addSpacer();

			_ui->addLabel("texture", 1);
			if(disc.getTexture(i)==0) _ui->addMultiImageButton("blank","butonlar/buton-01.png", true, 35,35);
			else _ui->addMultiImageButton("blank","butonlar/buton-01.png", false, 35,35);
			_ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
			if(disc.getTexture(i)==1) _ui->addMultiImageButton("line", "butonlar/buton-02.png", true, 35,35);
			else _ui->addMultiImageButton("line", "butonlar/buton-02.png", false, 35,35);
			if(disc.getTexture(i)==2) _ui->addMultiImageButton("tri", "butonlar/buton-03.png", true, 35,35);
			else _ui->addMultiImageButton("tri", "butonlar/buton-03.png", false, 35,35);
			if(disc.getTexture(i)==3) _ui->addMultiImageButton("saw", "butonlar/buton-04.png", true, 35,35);
			else _ui->addMultiImageButton("saw", "butonlar/buton-04.png", false, 35,35);
			if(disc.getTexture(i)==4) _ui->addMultiImageButton("rect", "butonlar/buton-05.png", true, 35,35);
			else _ui->addMultiImageButton("rect", "butonlar/buton-05.png", false, 35,35);
			_ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

            _ui->addLabel("rotation speed",1);
            _ui->addBiLabelSlider("rotation" + ofToString(i+1), "CCW", "CW", 10, -10, disc.getNetRotationSpeed(i));
            _ui->addLabel("density",1);
            _ui->addBiLabelSlider("density" + ofToString(i+1), "sparse", "dense", 30, 3, disc.getDensity(i));
            _ui->addLabel("size",1);
            _ui->addBiLabelSlider("radius" + ofToString(i+1), "small", "large", 15, 100, disc.getRadius(i)-disc.getRadius(i-1));

            
            _ui->addLabel("z-motion",1);
            _ui->addToggle("move", disc.isMoving(i));
            _ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            _ui->addButton("reset", disc.resetPerlin[i]);
			_ui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            _ui->addLabelToggle("mute", disc.isMute(i));
			_ui->addSpacer();
			_ui->addLabelToggle("chat", true);

			_ui->autoSizeToFitWidgets();
			if(me != NULL && i == me->getDiscIndex()) _ui->setVisible(true);
			else _ui->setVisible(false);
			ofAddListener(_ui->newGUIEvent, this, &ofApp::guiEvent);

			ui.push_back(_ui);

		}

	}

	if( me != NULL){
		for(int i = 0; i < disc.getDiscIndex(); i++){
			ui[i]->setColorBack(me->getColor());
		}
	}
	updateButtons = new ofxUICanvas();
	updateButtons->setPosition(0, ofGetHeight()/2-70);
	updateButtons->setDrawBack(false);
	updateButtons->setFontSize(OFX_UI_FONT_SMALL, 7);
	ofAddListener(updateButtons->newGUIEvent, this, &ofApp::guiEvent);

	chat = new ofxUICanvas();
    chat->setFont(OF_TTF_MONO);
	chat->setDrawBack(false);
	int chatWidth = 900;
	chat->setPosition(noDisc->getGlobalCanvasWidth()+10, 0);
	chat->setDimensions(chatWidth, ofGetHeight());
	chat->setColorFill(ofxUIColor(50,50,50,150));
	conversation = "";
	chat->addTextInput("chatInput", "", OFX_UI_FONT_MEDIUM)->setAutoUnfocus(false);
	chat->addTextArea("chat", "", OFX_UI_FONT_MEDIUM);
	if (me == NULL) chat->setVisible(false);
    chat->autoSizeToFitWidgets();

	ofAddListener(chat->newGUIEvent, this, &ofApp::guiEvent);

	//set up audio stream & synth network
	if (me != NULL) ofSoundStreamSetup(2, 0); // 2 out, 0 in

	//        sound.setup(&disc);

//	// set up game costs
//	costRadius = 1;
//	costDensity = 1;
//	costRotation = 1;
//	costTexture = 1;
//	costMute = 1;
//	costMove = 1;
//	reward = 1;

}
//--------------------------------------------------------------
void ofApp::exit(){

	me->setConnection(false);
	string playerOut;
	playerOut = "goodbye//"+me->getNick();
	client.send(playerOut);


	for(int i = 0; i < disc.getDiscIndex(); i++){
		delete ui[i];
	}
	delete noDisc;
	delete chat;
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
			client.setMessageDelimiter("varianet");

            Player* _player = new Player();
            _player->setNick(nick);
            me = _player;
			// ask for server state
			client.send("hello//"+me->getNick());
		}
	}
    
	else{
		for(int i = 0; i < disc.getDiscIndex(); i++){
			if(e.getName() == "rotation" + ofToString(i+1)){
				ofxUISlider *slider = e.getSlider();
				if(me->getLife() > 0 && mReleased == false && disc.getTexture(i) != 0) {
					rotationChanged = true;
					float newRotation = slider->getScaledValue()-disc.getNetRotationSpeed(i);
					disc.setRotationSpeed(i, newRotation);

					//change sound
					float netSpeed = abs(disc.getNetRotationSpeed(i));
                    float frequency;
                    if(netSpeed <= 5){
                        frequency = ofMap(netSpeed, 0, 5, 50, 500);
                    }
					else frequency = ofMap(netSpeed, 5, 10, 500, 2500);
					float beatSpeed = ofMap(netSpeed, 0, 10, 0, 250);
					float beatDensity = ofMap(disc.getDensity(i), 1, 30, 30, 2);
					soundChange("freq", i, frequency);
					soundChange("bpm", i, beatSpeed*beatDensity);

				}
				else if (disc.getTexture(i) == 0) slider->setValue(0);
			}
			else if(e.getName() == "radius" + ofToString(i+1)){
				ofxUISlider *slider = e.getSlider();
				if(me->getLife() > 0 && mReleased == false) {
					radiusChanged = true;
					disc.setThickness(i, slider->getScaledValue());

					//change sound
					float q = ofMap(disc.getThickness(i), 15, 100, 10, 0);
					soundChange("q", i, q);

					//send to server
					//                string change = "radius//"+ofToString(i)+": "+ofToString(slider->getScaledValue());
					//                client.send(change);
				}
			}
			else if(e.getName() == "density" + ofToString(i+1)){
				ofxUISlider *slider = e.getSlider();
				if(me->getLife()> 0 && mReleased == false) {
					densityChanged = true;
					disc.setDensity(i, slider->getScaledValue());

					//change envelope
					float envelopeCoeff = ofMap(disc.getDensity(i), 1, 30, 1, 5);
					float pulseRatio = ofMap(disc.getDensity(i), 1, 30, 0.005, 1);
					soundChange("envelopeWidth", i, envelopeCoeff);
					//                    soundChange("pulseLength", i, pulseRatio);

					//change metronome
					float netSpeed = abs(disc.getNetRotationSpeed(i));
					float beatSpeed = ofMap(netSpeed, 0, 10, 0, 250);
					float beatDensity = ofMap(disc.getDensity(i), 1, 30, 30, 2);
					soundChange("bpm", i, beatSpeed*beatDensity);

					//send to server
					//                string change = "density//"+ ofToString(i)+": "+ ofToString(slider->getScaledValue());
					//                client.send(change);

				}
			}
		}

		//update buttons
		if(e.getKind() == OFX_UI_WIDGET_LABELTOGGLE && e.getName() != "mute" && e.getName() != "chat"){

			ofxUILabelToggle *updateButton = static_cast <ofxUILabelToggle*> (e.getToggle());
			if(updateButton->getValue() == true){

				Player* _player = new Player();
				for(int i = 0; i < otherPlayers.size(); i++){
					if(otherPlayers[i]->getColor() == updateButton->getColorBack()) _player = otherPlayers[i];
				}
				_player->setLife(_player->getLife()+reward);
				//send update
				string lifeUpdate = "life//";
				lifeUpdate += "IP: "+ofToString(_player->getIP()) + "//";
				lifeUpdate += "lifeChange: "+ofToString(_player->getLife()) + "//";
				client.send(lifeUpdate);

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
					if( destination < 0 ) destination += disc.getDiscIndex();
					if(otherPlayers[i]->getDiscIndex() == destination) {
						occupied = true;
						jump++;
						break;
					}
				}
			}
			if(me->getDiscIndex() - jump > -1) me->setDiscIndex(me->getDiscIndex() - jump);
			else if(me->getDiscIndex() != -1) me->setDiscIndex(me->getDiscIndex() - jump + disc.getDiscIndex());
			else me->setDiscIndex(me->getDiscIndex() - jump + disc.getDiscIndex()+1);

			button->setValue(false);

			//change ui
			noDisc->setVisible(false);
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
				}
			}

			if(me->getDiscIndex() + jump < disc.getDiscIndex()) me->setDiscIndex(me->getDiscIndex() + jump);
			else me->setDiscIndex(me->getDiscIndex() + jump - disc.getDiscIndex());
			button->setValue(false);

			//change ui
			noDisc->setVisible(false);
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
			if(me->getLife()> 0 && texture != 0 && toggle->getValue() == true) {
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

		}
		else if(e.getName() == "line"){
			ofxUIToggle *toggle = e.getToggle();
			int texture = disc.getTexture(me->getDiscIndex());
			if(me->getLife()> 0 && texture != 1 && toggle->getValue() == true) {
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
			}
			else toggle->setValue(true);

		}
		else if(e.getName() == "tri"){
			ofxUIToggle *toggle = e.getToggle();
			int texture = disc.getTexture(me->getDiscIndex());
			if(me->getLife() > 0 && texture != 2 && toggle->getValue() == true) {
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
			}
			else toggle->setValue(true);

		}
		else if(e.getName() == "saw"){
			ofxUIToggle *toggle = e.getToggle();
			int texture = disc.getTexture(me->getDiscIndex());
			if(me->getLife() > 0 && texture != 3 && toggle->getValue() == true) {
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
			}
			else toggle->setValue(true);

		}
		else if(e.getName() == "rect"){
			ofxUIToggle *toggle = e.getToggle();
			int texture = disc.getTexture(me->getDiscIndex());
			if(me->getLife() > 0 && texture != 4 && toggle->getValue() == true) {
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
			}
			else toggle->setValue(true);
		}
		else if(e.getName() == "move"){
			ofxUIToggle *toggle = e.getToggle();
			if(me->getLife()>0){
				moveChanged = true;
				if (toggle->getValue() == true) disc.setMoving(me->getDiscIndex(), 1);
				else disc.setMoving(me->getDiscIndex(), 0);
			}
		}
		else if(e.getName() == "reset"){
			ofxUIButton *button = e.getButton();
			if(me->getLife()>0){
				moveReset = true;
				disc.resetPerlin[me->getDiscIndex()] = 1;
				ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
				ofxUIToggle *toggle = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
				toggle->setValue(false);
			}
		}
		else if(e.getName() == "move all"){
			ofxUIToggle *toggle = e.getToggle();
			if(me->getLife()>0){
				moveAllChanged = true;
				if(toggle->getValue() == true){

					int costFactor = 0;
					for(int j = 0; j<disc.getDiscIndex(); j++){
						ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
						ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
						if(toggleMove->getValue()==false){
							toggleMove->setValue(true);
							disc.setMoving(j, 1);
							costFactor++;
						}
					}
					me->setLife(me->getLife()-(costMove*costFactor));
					string moveAll = "moveAll//"+me->getIP();
					client.send(moveAll);

					//update server
					string lifeUpdate = "life//";
					lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
					lifeUpdate += "lifeChange: "+ofToString(costMove*costFactor) + "//";
					client.send(lifeUpdate);
				}
				if(toggle->getValue() == false){

					int costFactor = 0;
					string zPositionAll = "zPositionAll//";

					for(int i = 0; i<disc.getDiscIndex(); i++){
						ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
						ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
						if(toggleMove->getValue()==true){
							toggleMove->setValue(false);
							disc.setMoving(i, 0);
							costFactor++;
						}
						zPositionAll += ofToString(i)+": "+ofToString(disc.getPosition(i))+"//";
					}
					me->setLife(me->getLife()-(costMove*costFactor));
					string stopAll = "stopAll//"+me->getIP();
					client.send(stopAll);
					client.send(zPositionAll);

					//update server
					string lifeUpdate = "life//";
					lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
					lifeUpdate += "lifeChange: "+ofToString(costMove*costFactor) + "//";
					client.send(lifeUpdate);
				}
			}
		}
		else if(e.getName() == "reset all"){
			ofxUIButton *button = e.getButton();
			if(me->getLife()>0){
				if(button->getValue() == true){
					int costFactor = 0;
					for(int j = 0; j<disc.getDiscIndex(); j++){
						ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[j]);
						ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
						if(disc.getPosition(j) != 0){
							toggleMove->setValue(false);
							disc.resetPerlin[j] = 1;
							costFactor++;
						}
					}
					ofxUICanvas *canvas = static_cast <ofxUICanvas*> (noDisc);
					ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (canvas->getWidget("move all"));
					toggleMoveAll->setValue(false);

					me->setLife(me->getLife()-(costMove*costFactor));
					string resetAll = "resetAll//"+me->getIP();
					client.send(resetAll);

					//update server
					string lifeUpdate = "life//";
					lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
					lifeUpdate += "lifeChange: "+ofToString(costMove*costFactor) + "//";
					client.send(lifeUpdate);
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
			}
			else toggle->setValue(false);
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
}

//--------------------------------------------------------------
void ofApp::update(){

	if(moveChanged){
		moveChanged = false;
		me->setLife(me->getLife()-costMove);

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
		me->setLife(me->getLife()-costMute);

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
		float amountFreq = ofMap(abs(disc.getNetRotationSpeed(i)), 0, 10, 0, 5000);
		float amountMod = ofMap(abs(disc.getPosition(i)), 0, 100, 0, 10000);
		soundChange("amountFreq", i, amountFreq);
		soundChange("amountMod", i, amountMod);
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
						if(nameValue[0] == "rotation"+ofToString(i)) {
							disc.setRotation (i, ofToFloat(nameValue[1]));
						}
						if(nameValue[0] == "rotationSpeed"+ofToString(i)) {
							disc.setNetRotationSpeed (i, ofToFloat(nameValue[1]));
							//sound
							float netSpeed = abs(disc.getNetRotationSpeed(i));
                            float frequency;
                            if(netSpeed <= 5){
                                frequency = ofMap(netSpeed, 0, 5, 50, 500);
                            }
                            else frequency = ofMap(netSpeed, 5, 10, 500, 2500);
							float beatSpeed = ofMap(netSpeed, 0, 10, 0, 250);
							float beatDensity = ofMap(disc.getDensity(i), 1, 30, 30, 2);
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
							//                            soundChange("pulseLength", i, pulseRatio);

							float netSpeed = abs(disc.getNetRotationSpeed(i));
//                            float frequency;
//                            if(netSpeed <= 5){
//                                frequency = ofMap(netSpeed, 0, 5, 50, 500);
//                            }
//                            else frequency = ofMap(netSpeed, 5, 10, 500, 2500);
							float beatSpeed = ofMap(netSpeed, 0, 10, 0, 250);
							float beatDensity = ofMap(disc.getDensity(i), 1, 30, 30, 2);
//							soundChange("freq", i, frequency);
							soundChange("bpm", i, beatSpeed*beatDensity);

							//ui
							ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[i]);
							ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("density"+ofToString(i+1)));
							slider->setValue(disc.getDensity(i));
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
				ofxUIToggle *moveAll = (ofxUIToggle*) noDisc->getWidget("move all");
				moveAll->setValue(allMoving);

				me->setConnection(true);
				TCPsetup = true;
				initialize->setVisible(false);
                
                loginMinute = ofGetMinutes();
                loginSecond = ofGetElapsedTimef();

				setup();
				//                sound.setup(&disc);
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
                        ofxUITextArea *history = (ofxUITextArea *) chat->getWidget("chat");
                        history->setTextString(conversation);
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

			else if (title == "life"){
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

					if (playerData[0] == "lifeChange" && _player != NULL) _player->changeLife(ofToFloat(playerData[1]));
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
                    frequency = ofMap(netSpeed, 0, 5, 50, 500);
                }
                else frequency = ofMap(netSpeed, 5, 10, 500, 2500);
				float beatSpeed = ofMap(netSpeed, 0, 10, 0, 250);
				float beatDensity = ofMap(disc.getDensity(index), 1, 30, 30, 2);
				soundChange("freq", index, frequency);
				soundChange("bpm", index, beatSpeed*beatDensity);

				//update ui
				ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[index]);
				ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(index+1)));
				slider->setValue(disc.getNetRotationSpeed(index));

				///////////update buttons/////////////

				ofxUILabelToggle *toggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" rotation\nset to "+ofToString(disc.getNetRotationSpeed(index)), false, 200, 50);
				updateButtonsArray.push_back(toggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				toggle->setColorBack(_player->getColor());

				refreshUpdateButtons();

				///////////////////////////////////////////
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

				///////////update buttons/////////////

				ofxUILabelToggle *toggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" size\nset to "+ofToString(disc.getThickness(index)), false, 200, 50);
				updateButtonsArray.push_back(toggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				toggle->setColorBack(_player->getColor());

				refreshUpdateButtons();

				///////////////////////////////////////////
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
                float beatSpeed = ofMap(netSpeed, 0, 10, 0, 250);
                float beatDensity = ofMap(disc.getDensity(index), 1, 30, 30, 2);
                soundChange("bpm", index, beatSpeed*beatDensity);

				//update ui
				ofxUICanvas *canvas = static_cast<ofxUICanvas*>(ui[index]);
				ofxUISlider *slider = static_cast<ofxUISlider*>(canvas->getWidget("density"+ofToString(index+1)));
				slider->setValue(disc.getDensity(index));

				///////////update buttons/////////////

				ofxUILabelToggle *toggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" density\nset to "+ofToString(disc.getDensity(index)), false, 200, 50);
				updateButtonsArray.push_back(toggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				toggle->setColorBack(_player->getColor());

				refreshUpdateButtons();

				///////////////////////////////////////////
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

				///////////update buttons/////////////

				string texture;
				if(disc.getTexture(index) == 0) texture = "blank";
				if(disc.getTexture(index) == 1) texture = "line";
				if(disc.getTexture(index) == 2) texture = "tri";
				if(disc.getTexture(index) == 3) texture = "saw";
				if(disc.getTexture(index) == 4) texture = "rect";

				ofxUILabelToggle *toggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" texture\nset to "+texture, false, 200, 50);
				updateButtonsArray.push_back(toggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				toggle->setColorBack(_player->getColor());

				refreshUpdateButtons();

				///////////////////////////////////////////
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

				///////////update buttons/////////////
				string change;
				if(ofToInt(nameValue[1]) == 0) change = "off";
				if(ofToInt(nameValue[1]) == 1) change = "on";
				ofxUILabelToggle *newToggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" mute\ntoggled "+ change, false, 200, 50);
				updateButtonsArray.push_back(newToggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				newToggle->setColorBack(_player->getColor());

				refreshUpdateButtons();
				///////////////////////////////////////////
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

				///////////update buttons/////////////
				string change;
				if(ofToInt(nameValue[1]) == 0) change = "off";
				if(ofToInt(nameValue[1]) == 1) change = "on";
				ofxUILabelToggle *newToggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" z-motion\ntoggled "+ change, false, 200, 50);
				updateButtonsArray.push_back(newToggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				newToggle->setColorBack(_player->getColor());

				refreshUpdateButtons();
				///////////////////////////////////////////
			}

			else if (title == "moveReset"){
				int index = ofToInt(received[1]);
				disc.resetPerlin[index] = 1;
				//ui
				ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[index]);
				ofxUIToggle *toggle = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
				toggle->setValue(false);

				///////////update buttons/////////////
				ofxUILabelToggle *newToggle = new ofxUILabelToggle("Groove "+ofToString(index+1)+" z-motion\nreset", false, 200, 50);
				updateButtonsArray.push_back(newToggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[2] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				newToggle->setColorBack(_player->getColor());

				refreshUpdateButtons();
				///////////////////////////////////////////

			}

			else if (title == "moveAll"){
				ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (noDisc->getWidget("move all"));
				toggleMoveAll->setValue(true);
				for(int i = 0; i<disc.getDiscIndex(); i++){
					ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
					ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
					toggleMove->setValue(true);
					disc.setMoving(i, 1);
				}

				///////////update buttons/////////////
				ofxUILabelToggle *newToggle = new ofxUILabelToggle("Moving all grooves", false, 200, 50);
				updateButtonsArray.push_back(newToggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[1] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				newToggle->setColorBack(_player->getColor());

				refreshUpdateButtons();
				///////////////////////////////////////////
			}

			else if (title == "stopAll"){
				ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (noDisc->getWidget("move all"));
				toggleMoveAll->setValue(false);
				for(int i = 0; i<disc.getDiscIndex(); i++){
					ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
					ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
					toggleMove->setValue(false);
					disc.setMoving(i, 0);
				}

				///////////update buttons/////////////
				ofxUILabelToggle *newToggle = new ofxUILabelToggle("Stopping all grooves", false, 200, 50);
				updateButtonsArray.push_back(newToggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[1] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				newToggle->setColorBack(_player->getColor());

				refreshUpdateButtons();
				///////////////////////////////////////////
			}

			else if (title == "resetAll"){
				ofxUIToggle *toggleMoveAll = static_cast <ofxUIToggle*> (noDisc->getWidget("move all"));
				toggleMoveAll->setValue(false);
				for(int i = 0; i<disc.getDiscIndex(); i++){
					disc.resetPerlin[i] = 1;
					ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[i]);
					ofxUIToggle *toggleMove = static_cast <ofxUIToggle*> (canvas->getWidget("move"));
					toggleMove->setValue(false);
				}

				///////////update buttons/////////////
				ofxUILabelToggle *newToggle = new ofxUILabelToggle("Reset all groove depths", false, 200, 50);
				updateButtonsArray.push_back(newToggle);

				Player* _player = NULL;
				for(int i = 0; i < otherPlayers.size(); i++){
					if(received[1] == otherPlayers[i]->getIP()) _player = otherPlayers[i];
				}
				if(_player == NULL) cout<< "_player is not matched" <<endl;
				newToggle->setColorBack(_player->getColor());

				refreshUpdateButtons();
				///////////////////////////////////////////
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
				ofxUITextArea *history = (ofxUITextArea *) chat->getWidget("chat");
				history->setTextString(conversation);

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
				ofxUITextArea *history = (ofxUITextArea *) chat->getWidget("chat");
				history->setTextString(conversation);
			}

		}
	}

	if(TCPsetup) groove.update();
}
//--------------------------------------------------------------
void ofApp::draw(){

	ofBackgroundGradient(230, 250);
	glEnable(GL_DEPTH_TEST);

	ofPushMatrix();

	ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

	if(TCPsetup){
		cam.begin();
		groove.draw();

		if(cam.getDistance() > 100){
//			cout<< cam.getDistance() <<endl;
			float wetLevel = ofMap(cam.getDistance(), 500, 9000, 0., .4);
			float masterLevel = ofMap(cam.getDistance(), 100, 9000, .999, 0.);
			ofClamp(wetLevel, 0., .4);
			ofClamp(masterLevel, 0., .999);

			sound.synth.setParameter("wet", wetLevel);
			sound.synth.setParameter("master", masterLevel);
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
        if(timer) {
            ofSetColor(0);
            ofDrawBitmapString(ofToString(abs(ofGetElapsedTimef()-loginSecond)), ofGetWidth()/2 - 120, ofGetHeight()/2 - 10);
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

	//    if(key == ' ') groove.turn = !groove.turn;
	//    if(key == 'p'){
	//        if (disc.isMoving(me->getDiscIndex()) == 0) disc.setMoving(me->getDiscIndex(), 1);
	//        else disc.setMoving(me->getDiscIndex(), 0);
	//    }
	//    if(key == 'o') disc.resetPerlin[me->getDiscIndex()] = 1;

	if(key == 'c') chat->toggleVisible();

	//    if(key == 'a' && me->getDiscIndex() != -1) {

	//        if(disc.getLife() > 0) {
	//            disc.setLife(costRotation);     // reduce life
	//            disc.setRotationSpeed(me->getDiscIndex(), +.05);
	//
	//            //update ui
	//            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
	//            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(me->getDiscIndex()+1)));
	//            slider->setValue(disc.getNetRotationSpeed(me->getDiscIndex()));
	//
	//            //change sound
	//            float netSpeed = abs(disc.getNetRotationSpeed(me->getDiscIndex()));
	//            float beat = ofMap(netSpeed, 0, 10, 0, 1000);
	//            sound.synth.setParameter("bpm"+ofToString(me->getDiscIndex()), beat);
	//
	//            //send to server
	//            float newRotation = slider->getScaledValue()-disc.getNetRotationSpeed(me->getDiscIndex());
	//            string change = "rotationSpeed//"+ ofToString(me->getDiscIndex())+": "+ ofToString(+0.05);
	//            client.send(change);
	//        }
	//    }

	//    if(key == 'd' && me->getDiscIndex() != -1 ) {

	//        if(disc.getLife() > 0) {
	//            disc.setLife(costRotation);     // reduce life
	//            disc.setRotationSpeed(me->getDiscIndex(), -.05);
	//
	//            //update ui
	//            ofxUICanvas *canvas = static_cast <ofxUICanvas*> (ui[me->getDiscIndex()]);
	//            ofxUISlider *slider = static_cast <ofxUISlider*> (canvas->getWidget("rotation"+ofToString(me->getDiscIndex()+1)));
	//            slider->setValue(disc.getNetRotationSpeed(me->getDiscIndex()));
	//
	//            //change sound
	//            float netSpeed = abs(disc.getNetRotationSpeed(me->getDiscIndex()));
	//            float beat = ofMap(netSpeed, 0, 10, 0, 1000);
	//            sound.synth.setParameter("bpm"+ofToString(me->getDiscIndex()), beat);
	//
	//            //send to server
	//            float newRotation = slider->getScaledValue()-disc.getNetRotationSpeed(me->getDiscIndex());
	//            string change = "rotationSpeed//"+ ofToString(me->getDiscIndex())+": "+ ofToString(-0.05);
	//            client.send(change);
	//        }
	//    }

	if(key == 'w'){
		int jump = 1;
		bool occupied = true;
		while(occupied == true){
			occupied = false;
			for(int i = 0; i < otherPlayers.size(); i++){
				int destination = me->getDiscIndex() + jump;
				if( destination > disc.getDiscIndex()) destination -= disc.getDiscIndex();
				if( otherPlayers[i]->getDiscIndex() == destination) {
					occupied = true;
					jump++;
					break;
				}
			}
		}

		if(me->getDiscIndex() + jump < disc.getDiscIndex()) me->setDiscIndex(me->getDiscIndex() + jump);
		else me->setDiscIndex(me->getDiscIndex() + jump - disc.getDiscIndex());

		//change ui
		noDisc->setVisible(false);
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
				if( destination < 0 ) destination += disc.getDiscIndex();
				if( otherPlayers[i]->getDiscIndex() == destination) {
					occupied = true;
					jump++;
					break;
				}
			}
		}
		if(me->getDiscIndex() - jump > -1) me->setDiscIndex(me->getDiscIndex() - jump);
		else if(me->getDiscIndex() != -1) me->setDiscIndex(me->getDiscIndex() - jump + disc.getDiscIndex());
		else me->setDiscIndex(me->getDiscIndex() - jump + disc.getDiscIndex() + 1);

		//change ui
		for(int i = 0; i < disc.getDiscIndex(); i++){
			ui[i]->setVisible(false);
		}
		ui[me->getDiscIndex()]->toggleVisible();
		noDisc->setVisible(false);

		//send change to server
		string changeDisc = "otherPlayersIndex//";
		changeDisc += "IP: "+ofToString(me->getIP()) + "//";
		changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
		client.send(changeDisc);
	}

	if(key == OF_KEY_BACKSPACE) {
		if(me->getDiscIndex() != -1){
			ui[me->getDiscIndex()]->setVisible(false);
			me->setDiscIndex(-1);
			noDisc->setVisible(true);

			//send change to server
			string changeDisc = "otherPlayersIndex//";
			changeDisc += "IP: "+ofToString(me->getIP()) + "//";
			changeDisc += "index: "+ofToString(me->getDiscIndex()) + "//";
			client.send(changeDisc);
		}
	}

	if(key == 'f') {
		fullScreen = !fullScreen;
		ofSetFullscreen(fullScreen);
	}
    
    if(key == 't') {
        timer = !timer;
    }

}

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
        
        if(disc.isMute(index) == 0) disc.setEnvelope(index, value);
        else disc.setEnvelope(index, 0);
        
        sound.synth.setParameter("attack"+ofToString(index),disc.getEnvelope(index, 0));
        sound.synth.setParameter("decay"+ofToString(index),disc.getEnvelope(index, 1));
        sound.synth.setParameter("sustain"+ofToString(index),disc.getEnvelope(index, 2));
        sound.synth.setParameter("release"+ofToString(index),disc.getEnvelope(index, 3));
        
        
        float volCoeff = 1;
        if(disc.getTexture(index) == 1) volCoeff = 1.1;
        else if(disc.getTexture(index) == 2) volCoeff = .6;
        else if(disc.getTexture(index) == 3) volCoeff = .45;
        else if(disc.getTexture(index) == 4) volCoeff = .1;
        
        sound.synth.setParameter("volBalance"+ofToString(index), volCoeff);
        
        //ui
        ofxUICanvas *canvas1 = static_cast<ofxUICanvas*>(ui[index]);
        ofxUISlider *slider1 = static_cast <ofxUISlider*> (canvas1->getWidget("rotation"+ofToString(index+1)));
        ofxUISlider *slider2 = static_cast <ofxUISlider*> (canvas1->getWidget("density"+ofToString(index+1)));
        ofxUISlider *slider3 = static_cast <ofxUISlider*> (canvas1->getWidget("radius"+ofToString(index+1)));
        
        if(disc.getTexture(index) == 0){
            slider1->setVisible(false);
            slider2->setVisible(false);
            slider3->setVisible(false);
        }
        else {
            slider1->setVisible(true);
            slider2->setVisible(true);
            slider3->setVisible(true);
        }
        
        
    }
    
    else sound.synth.setParameter(name+ofToString(index), value);
    
}

//--------------------------------------------------------------
void ofApp::refreshUpdateButtons(){
    
    if(updateButtonsArray.size() > 8){
        ofxUILabelToggle *toggle0 = updateButtonsArray.front();
        updateButtons->removeWidget(toggle0);
        updateButtonsArray.erase(updateButtonsArray.begin());
    }
    updateButtons->clearWidgets();
    for(int i = updateButtonsArray.size()-1; i >= 0; i--){
        ofxUILabelToggle *thisToggle = updateButtonsArray[i];
        
        thisToggle = updateButtons->addLabelToggle(thisToggle->getName(), thisToggle->getValue(), 200, 50);
        if( thisToggle->getValue() == false){
            thisToggle->setColorBack(updateButtonsArray[i]->getColorBack());
        }
        updateButtonsArray[i] = thisToggle;
    }
    updateButtons->autoSizeToFitWidgets();
    
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
		me->setLife(me->getLife()-costRadius);

		//update server
		string lifeUpdate = "life//";
		lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
		lifeUpdate += "lifeChange: "+ofToString(costRadius) + "//";
		client.send(lifeUpdate);

		string change = "radius//"+ofToString(me->getDiscIndex())+": "+ofToString(disc.getThickness(me->getDiscIndex()))+"//"+me->getIP();
		client.send(change);

	}
	else if(densityChanged){
		densityChanged = false;
		me->setLife(me->getLife()-costDensity);

		//update server
		string lifeUpdate = "life//";
		lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
		lifeUpdate += "lifeChange: "+ofToString(costDensity) + "//";
		client.send(lifeUpdate);

		string change = "density//"+ ofToString(me->getDiscIndex())+": "+ ofToString(disc.getDensity(me->getDiscIndex()))+"//"+me->getIP();
		client.send(change);
	}
	else if(textureChanged){
		textureChanged = false;
		me->setLife(me->getLife()-costTexture);

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
		me->setLife(me->getLife()-costRotation);

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
	}
	
	//    moveChanged is in update()

	else if(moveReset){
		moveReset = false;
		me->setLife(me->getLife()-costMove);

		//update server
		string lifeUpdate = "life//";
		lifeUpdate += "IP: "+ofToString(me->getIP()) + "//";
		lifeUpdate += "lifeChange: "+ofToString(costMove) + "//";
		client.send(lifeUpdate);

		string movementReset = "moveReset//"+ofToString(me->getDiscIndex())+"//"+me->getIP();
		client.send(movementReset);
	}
	// cost calculation and server notifications of move/stop/resetAll button are handled in guiEvent
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


