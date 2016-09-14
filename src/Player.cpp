///
//  Player.cpp
//  varianet_server
//
//  Created by cemcakmak on 10/04/15.
//
//

#include "Player.h"

void Player::setup(){
    
	connected = false;
    life = 100;
    
    
}

bool Player::isConnected() const{
    
    return connected;
    
}

void Player::setConnection(bool connection){

    if(connection == false) discIndex = -1;
    connected = connection;
    
}

string Player::getIP() const{
    
    return IP;
    
}

void Player::setIP(string newIP) {
    
    IP = newIP;
    
}

ofColor Player::getColor() const{
    
    return color;
    
}

void Player::setColor(ofColor newColor){
    
    color.set(newColor);
}

void Player::setColorByIndex(int index){
    // 161,24,87 - purple
    // 255,153,0 - orange
    // 50,153,187 - blue
    // 103,209,88 - green
    switch (index) {
        case 1:
            color.set(50,153,187);
            break;
        case 2:
            color.set(224,232,0);
            break;
        case 3:
            color.set(161,24,87);
            break;
        case 4:
            color.set(103,209,88);
            break;
            
        default:
            break;
    }
    
}

float Player::getLife() const{
    
    return life;
    
}

void Player::setLife(float newLife){
    
    if(newLife > 100) newLife = 100;
    if(newLife < 0) newLife = 0;
    life = newLife;
    
}

void Player::changeLife(float amount){
    
    if(life + amount > 100) life = 100;
    else if(life + amount < 0) life = 0;
    else life += amount;
    
}

int Player::getDiscIndex() const{
    
    return discIndex;
    
}

void Player::setDiscIndex(int index){
    
    discIndex = index;
    
}

string Player::getNick() const{
    
    return nick;
}

void Player::setNick(string nickname){
    
    nick = nickname;
}
