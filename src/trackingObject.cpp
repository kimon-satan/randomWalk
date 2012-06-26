#include "trackingObject.h"

trackingObject::trackingObject()
{
    reset();
}

trackingObject::~trackingObject()
{
    //dtor
}

void trackingObject::reset(){
objChanged = true;
present = false;
assigned = false;
scrNearX = 0;
scrNearY = 0;
scrFarX = 0;
scrFarY = 0;
projCentX = 0;
projCentY = 0;
fakeObj = false;
fakeSpeed = 0;
history.clear();
ghostReset();

}

float trackingObject::getScrFarX(){
 return scrFarX;
}

void trackingObject::setScrFarX(float temp){

scrFarX = temp;

}

float trackingObject::getScrFarY(){
 return scrFarY;
}

void trackingObject::setScrFarY(float temp){

scrFarY = temp;

}


float trackingObject::getScrNearX(){
 return scrNearX;
}

void trackingObject::setScrNearX(float temp){

scrNearX = temp;

}

float trackingObject::getScrNearY(){
 return scrNearY;
}

void trackingObject::setScrNearY(float temp){

scrNearY = temp;

}

float trackingObject::getProjCentX(){
 return projCentX;
}

void trackingObject::setProjCentX(float temp){
projCentX = temp;
}

float trackingObject::getProjCentY(){
 return projCentY;
}

void trackingObject::setProjCentY(float temp){
projCentY = temp;
}



int trackingObject::getColorNumber(){
 return colNum;
}

void trackingObject::setColorNumber(int temp){

colNum = temp;

}

bool trackingObject::getAssigned(){
 return assigned;
}

void trackingObject::setAssigned(bool temp){
assigned = temp;
}

bool trackingObject::getPresent(){
 return present;
}

void trackingObject::setPresent(bool temp){
present = temp;
}


bool trackingObject::getChanged(){
 return objChanged;
}

void trackingObject::setChanged(bool temp){
objChanged = temp;
}


void trackingObject::updateHistory(){

ofVec2f temp;

temp.set(getProjCentX(), getProjCentY());

history.push_back(temp);

    if (history.size() > 10){

    history.erase(history.begin());

    }
}

ofVec2f trackingObject::getAvPos(){

float x = 0;
float y = 0;
ofVec2f pos;


    for(int i =0; i < history.size(); i++){

        pos = history[i];

        x += pos.x;
        y += pos.y;

    }
//printf("%f,%f \n", x,y);
x = x/history.size();
y = y/history.size();

pos.x = x;
pos.y = y;

return pos;

}


float trackingObject::getSpeed()
{

    float tempTotal = 0;
    float speed;
    float thisSpeed;
    float tempDiff;
    int counter = 1;

    if(history.size()>1){
        for(int i = 0; i < history.size()-1; i++){

            for(int j = i+1; j < history.size()-1; j++){
            tempDiff = history.at(i).distance(history.at(j));


            tempTotal += tempDiff;
            }

    counter++;

    }

  //  printf("tempTotal %f \n",tempTotal);
    if(tempTotal > 0.005){
    speed = tempTotal/counter;
    }else{speed = 0;}
    }else{speed = 0;}

if(fakeObj){return fakeSpeed;} else{return speed;}

}

void trackingObject::setFakeSpeed(float temp){

fakeSpeed = temp;

}

void trackingObject::setFake(bool temp){

fakeObj = temp;

}

void trackingObject::ghostIncrement(){

ghostCounter += 1;

}

void trackingObject::ghostReset(){

ghostCounter = 0;

}

int trackingObject::getGhost(){

 return ghostCounter;

}
