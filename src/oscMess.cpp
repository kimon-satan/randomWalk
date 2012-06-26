#include "oscMess.h"

oscMess::oscMess()
{
sender.setup( HOST, PORT );

}

void oscMess::sendInitialise(int temp)
{

ofxOscMessage m;
m.setAddress( "/init" );
m.addIntArg(temp);
sender.sendMessage(m);
m.clear();

}

void oscMess::sendMoving(int temp)
{

ofxOscMessage m;
m.setAddress( "/isMoving_set" );
m.addIntArg(temp);
sender.sendMessage(m);
m.clear();

}

void oscMess::sendProximities(vector<float> temp){

ofxOscMessage m;
m.setAddress( "/proximities_set" );

for(int i = 0; i < temp.size(); i++){

m.addFloatArg(temp.at(i));

}

sender.sendMessage(m);
m.clear();

}

void oscMess::sendPoints(vector<bool> temp){

ofxOscMessage m;
m.setAddress( "/aPoints_set" );

for(int i = 0; i < temp.size(); i++){

m.addIntArg((int)temp.at(i));

}

sender.sendMessage(m);
m.clear();


}

void oscMess::sendNextLevel(int temp){

ofxOscMessage m;
m.setAddress( "/nextLevel" );

m.addIntArg(temp);

sender.sendMessage(m);
m.clear();

}

void oscMess::sendGameOver(){

ofxOscMessage m;
m.setAddress( "/gameOver" );

sender.sendMessage(m);
m.clear();
}

oscMess::~oscMess()
{
    //dtor
}
