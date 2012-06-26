#include "display.h"


display::display()
{

	counter = 0;
	stillcount = 0;
	currLev = 10;
	levPoints = 1;
	light.setPosition(600, 400, 100);

}

void display::setup(){

	timeFont.loadFont("datalatin.ttf",30);
	bigFont.loadFont("datalatin.ttf",45);
	cam1.setDistance(100);
	cam1.disableMouseInput();
	cam2.setDistance(50);
	cam2.disableMouseInput();
}

void display::updateCounter(){
	
	counter = ofGetElapsedTimeMillis();
	
	for(int i = 0; i < 10; i++){
		smallCounts[i] += 0.02 * i;
		if(smallCounts[i] > 360)smallCounts[i] = 0;
	}
	
	
}

void display::drawInfo(float time, float score, int level){
	
	
	string levelCol;
	
	if(level == 0)levelCol = "red";
	if(level == 1)levelCol = "aqua";
	if(level == 2)levelCol = "yellow";
	if(level == 3)levelCol = "blue";
	if(level == 4)levelCol = "white";
	glPushMatrix();
	glTranslated(750, 100, 0);
	ofSetHexColor(0x66CC66);
	timeFont.drawString("Time: " + ofToString(time,3),0,0);
	timeFont.drawString("Score: " + ofToString(score,3),0,30);
	timeFont.drawString("Level: " + levelCol,0,60);
	glPopMatrix();
	ofSetColor(255,255,255);
	
}

void display::showGameOver(string highScores, bool allComp){
	
	if(goCount < 100){
		
		if(allComp){bigFont.drawString("all points found !", 100 ,1000);}
		
		bigFont.drawString("Game Over",200,1150);
		goCount += 1;}else{
			
			timeFont.drawString("High Scores", 200,1050);
			timeFont.drawString(highScores, 50,1090);
		}
	
	
}

void display::drawPlayerPos(ofVec2f playerPos){
	
	ofSetHexColor(0x66CC66);
	
	string levelCol;
	
	if(currLev == 0)levelCol = "red";
	if(currLev == 1)levelCol = "aqua";
	if(currLev == 2)levelCol = "yellow";
	if(currLev == 3)levelCol = "blue";
	if(currLev == 4)levelCol = "white";
	
	if(goCount == 0){
		if(levelComplete && stillcount < 30){
			stillcount += 1;
			bigFont.drawString(levelCol + " level complete",200,450);
			
		}else if(newPoint && stillcount < 30){
			
			bigFont.drawString("Well Done",200,450);
			stillcount += 1;
			
		}else if(newPoint && stillcount >= 30 && !levelComplete){
			
			timeFont.drawString("keep moving to find the next point", 100, 450);
			
		}else if(levelComplete && stillcount >= 30 && currLev < 4){
			
			timeFont.drawString("next level has " + ofToString(levPoints, 0) + " points to find", 100, 450);
			stillcount +=1;
			
		}else if(!levelComplete || !newPoint){stillcount = 0;}
	}
	
	
	cam1.begin(ofRectangle(650,200,450,450));
	glPushMatrix();

	glRotatef(45,1,0,0);
	glRotatef(0,0,1,0);
	glRotatef(-45,0,0,1);
	
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofNoFill();
	ofRect(0,0,50,50);
	ofSetRectMode(OF_RECTMODE_CORNER);
	
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
	light.enable();
	
		glTranslatef(playerPos.x -25, 25 - playerPos.y,0);
		glRotatef(90, 1, 0, 0);
		glRotatef((counter/10)%360,0,1,0);
		ofFill();
		ofSetConeResolution(6, 4);
		ofCone(0,0,0,3,6);
	
	light.disable();
	ofDisableLighting();
	glDisable(GL_DEPTH_TEST);
	
	glPopMatrix();
	cam1.end();
	ofSetColor(255,255,255);
}


void display::setLeveldata(int thisLev, vector< vector<mazePoint> > tempPoints){
	
    if(thisLev == 100 || thisLev < currLev){
		
        stillcount = 0;
        levelComplete = false;
        newPoint = false;
        goCount = 0;
        shapeTypes.clear();
        stillChain.clear();
        levelComplete = false;
        vector<int> tempTypes;
        int shapeStart = rand()%6;
		
		
        for(int lev = 0; lev < tempPoints.size(); lev++){
			
            shapeStart = rand()%6;
			
            for (int j = 0; j< tempPoints[lev].size(); j++){
				
				tempTypes.push_back((j + shapeStart)%6);
				
            }
			
			std::random_shuffle( tempTypes.begin(), tempTypes.end() );
			shapeTypes.push_back(tempTypes);
        }
		
    };
	
    if(thisLev != currLev){
		
		levelComplete = false;
		if(thisLev !=100){currLev = thisLev; }else{currLev = 0;}
		
		
		if(currLev != 4)levPoints = tempPoints[currLev +1].size();
        xmults.clear();
        ymults.clear();
        xrots.clear();
        yrots.clear();
        zrots.clear();
		
        for(int i = 0; i < tempPoints[currLev].size(); i ++){
			
			float x = (tempPoints[currLev][i].location.x - 25)/1.25;
			float y = (tempPoints[currLev][i].location.y - 25)/1.25;
			
			xmults.push_back(x); ymults.push_back(y);
			
			xrots.push_back(3 + (rand()%20));
			yrots.push_back(3 + (rand()%20));
			zrots.push_back(3 + (rand()%20));
			
        }
		
		
		
    }
	
	
	
}



void display::drawMoveShapes(vector< vector<mazePoint> > tempPoints, ofVec2f playerPos){
	
	newPoint = false;
	
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
	light.enable();
	cam2.begin(ofRectangle(-50,0,800,800));
	
	glPushMatrix();
	glRotatef(playerPos.x * 7.2, 0,0,1);
	glRotatef(playerPos.y * -7.2, 0,0,1);
	
	
	for(int lev = 0; lev < tempPoints.size(); lev++){
		
		for (int j = 0; j< tempPoints[lev].size(); j++){
			
			if(!tempPoints[lev][j].isActive){
				
				if(lev == 0){ofSetHexColor(0xFF0000);}
				if(lev == 1){ofSetHexColor(0x00FFFF);}
				if(lev == 2){ofSetHexColor(0xFFFF00);}
				if(lev == 3){ofSetHexColor(0x0033FF);}
				if(lev == 4){ofSetHexColor(0xFFFFFF);}
				
				float x, y, z;
				
				if(lev == currLev){
					x = tempPoints[lev][j].proximity * xmults[j];
					y = tempPoints[lev][j].proximity * ymults[j];
					z = tempPoints[lev][j].proximity * - 80;
				}else{
					
					x = tempPoints[lev][j].location.x * xmults[j];
					y = tempPoints[lev][j].location.y * ymults[j];
					z = -3500 + (tempPoints[lev][j].sequential * 20);
				};
				
				
				glPushMatrix();
				glTranslated(x, y, -100 + z);
				
				
				glPushMatrix();
				ofFill();
				glTranslatef(5,5,5);
				glRotatef((counter/xrots[j])%360, 1, 0, 0);
				glRotatef((counter/yrots[j])%360, 0, 1, 0);
				glRotatef((counter/zrots[j])%360, 0, 0, 1);
				
				switch (shapeTypes[lev][j]){
						
					case 0:
						ofBox(0,0,0,40);
						break;
						
					case 1:
						ofCone(0, 0, 0, 15, 40);
						break;
						
					case 2:
						ofTorus(0, 0, 0, 20, 10);
						break;
						
					case 3:
						ofCylinder(0, 0, 0, 20, 40, false);
						break;
						
					case 4:
						ofSphere(0, 0, 0, 20);
						break;
						
					case 5:
						ofCapsule(0, 0, 0, 20, 40);
						break;
						
				}
				
				
				ofFill();
				ofSetColor(255,255,255);
				
				glPopMatrix();
				glPopMatrix();
				
			}
		}
	}
	
	glPopMatrix();
	light.disable();
	ofDisableLighting();
	glDisable(GL_DEPTH_TEST);
	cam2.end();
	
}

void display::drawStillShapes(vector<vector<mazePoint> >tempPoints, ofVec2f playerPos){
	
	
	
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
	light.enable();
	cam2.begin(ofRectangle(-50,0,800,800));
	
	glPushMatrix();
	
	
    for(int i = 0; i < tempPoints[currLev].size(); i++){
		
		if(tempPoints[currLev][i].isActive && tempPoints[currLev][i].isOccupied){
			
            vector<int> temp;
            temp.push_back(currLev);
            temp.push_back(i);
			
            bool found = false;
			
			for(int x = 0; x < stillChain.size(); x++){
				
				if(stillChain[x][0] == currLev && stillChain[x][1] == i){found = true;}
				
			}
			
			if(!found){stillChain.push_back(temp);newPoint = true;}
		}
	}
	
	
	int levelCount = 0;
	
	if(newPoint){
		glTranslated(0, 0, -180);
		glRotatef(-30,1,0,0);
		glRotatef(20,0,1,0);
		glRotatef(-15,0,0,1);
		
		
		for(int i = stillChain.size() - 1; i > -1; i -- ){
			
			int lev = stillChain[i][0];
			int point = stillChain[i][1];
			
			if(lev == currLev){levelCount +=1;};
			
			glPushMatrix();
			glTranslatef(sin(smallCounts[1]),sin(smallCounts[3]),sin(smallCounts[2]));
			glRotatef((xrots[i]*5)%360, 1, 0, 0);
			glRotatef((yrots[i]*5)%360, 0, 1, 0);
			glRotated((zrots[i]*5)%360, 0, 0, 1);
			
			if(lev == 0){ofSetHexColor(0xFF0000);}
			if(lev == 1){ofSetHexColor(0x00FFFF);}
			if(lev == 2){ofSetHexColor(0xFFFF00);}
			if(lev == 3){ofSetHexColor(0x0033FF);}
			if(lev == 4){ofSetHexColor(0xFFFFFF);}
			
			float growx = 1 + (sin(smallCounts[4])*0.03);
			float growy = 1 + (sin(smallCounts[5])*0.03);
			float growz = 1 + (sin(smallCounts[6])*0.03);
			
			glScalef(growx, growy, growz);
			
			switch (shapeTypes[lev][point]){
					
				case 0:
					ofBox(0,0,0,40);
					break;
					
				case 1:
					ofCone(0, 0, 0, 15, 40);
					break;
					
				case 2:
					ofTorus(0, 0, 0, 20, 10);
					break;
					
				case 3:
					ofCylinder(0, 0, 0, 20, 40, false);
					break;
					
				case 4:
					ofSphere(0, 0, 0, 20);
					break;
					
				case 5:
					ofCapsule(0, 0, 0, 20, 40);
					break;
					
			}
			
			glPopMatrix();
			
			
			glRotatef(-5 + sin(smallCounts[xrots[point]%5]),1,0,0);
			glRotatef(25 + sin(smallCounts[yrots[point]%5]),0,1,0);
			glRotatef(-30 + sin(smallCounts[zrots[point]%5]),0,0,1);
			ofSetColor(120,120,120);
			
			
			
			if(i != 0){
				ofLine(0,0,0,150 + (sin(smallCounts[xrots[i]%4]) * xrots[point]));
				ofSetColor(255,0,0);
				glTranslatef(0,150 + (sin(smallCounts[xrots[i]%4]) * xrots[point]),0);}
			
			
			
			
			
			
		}
		
	}
	
	glPopMatrix();
	
	light.disable();
	ofDisableLighting();
	glDisable(GL_DEPTH_TEST);
	cam2.end();
	
	ofSetColor(255,255,255);
	
	
	if(levelCount == tempPoints[currLev].size())levelComplete = true;
	
	
	
}



display::~display()
{
    //dtor
}
