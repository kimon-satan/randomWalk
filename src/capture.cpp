#include "capture.h"


//--------------------------------------------------------------
void capture::setup(){

camNo = 0;
camWidth = 640;
camHeight = 480;
//vidGrabber.close();
  //      vidGrabber.setDeviceID(camNo);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(640,480);
    ofSetFrameRate(30);
	

   cTrackCont.allocate(camWidth, camHeight);
   cTrackTexture.allocate(camWidth, camHeight, GL_LUMINANCE);

   cTrackPixels = new unsigned char [camWidth * camHeight];     //rangeImage

    colorImg.allocate(camWidth,camHeight);

    colorImageHSV.allocate(camWidth,camHeight);
    hueImage.allocate(camWidth,camHeight);       //Hue map
    satImage.allocate(camWidth,camHeight);        //saturation map
    briImage.allocate(camWidth,camHeight);
    bgImage.allocate(camWidth,camHeight);
    bgDiff.allocate(camWidth, camHeight);
    grImage.allocate(camWidth, camHeight);

	learnBg = true;
	threshold = 12;

	glDepthMask(true);
    //glEnable(GL_DEPTH_TEST);

    selectedItem = 0;
    selectedMenu = 0;
    selectedImage = 0;
	selectedColor = 0;
    changed = true;

    maskArray[0] = 0;
    maskArray[1] = 0;
    maskArray[2] = -100;
    maskArray[3] = -50;
    maskArray[4] = 0;
    maskArray[5] = 0;
    maskArray[6] = 1.0;

    roiArray[0] = 48;
    roiArray[1] = 118;
    roiArray[2] = 545;
    roiArray[3] = 300;

    maskHeight = 0;
    viewX = 0; viewY = 0;
    projScale = 0.5;
    counter = 0;

    gauss[0] = 20; gauss[1] = 20; gauss[2] = 20; gauss[3] = 20;
    minBlob = 50;

    hueThresh = 80;
    satThresh = 80;
    briThresh = 80;

    mouseTest = false;
    trackingObject temp;
    temp.setPresent(false);
    temp.setFake(true);
    mouseObj.push_back(temp);
    mouseObj.push_back(temp);
    bFrameDiff = true;
	
	displayText.allocate(1024, 768, GL_RGBA);
	
	thisDisplay.setup();


}

capture::~capture()
{
    vidGrabber.close();
}



//--------------------------------------------------------------
void capture::update(){

	ofBackground(100,100,100);

    bool bNewFrame = false;

   // if(viewMode > 0){learnBg = true;}

       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();

	if (bNewFrame){

        counter += 1; counter = counter%2;

        colorImg.setFromPixels(vidGrabber.getPixels(), camWidth,camHeight);

		colorImageHSV = colorImg;
		colorImageHSV.convertRgbToHsv();
		//colorImageHSV.blurGaussian(gauss);
		colorImageHSV.convertToGrayscalePlanarImages(hueImage, satImage, briImage);

        hueImage.resetROI();
        hueImage.setROI(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);
        hueImage.blurGaussian(gauss[0]);
        hueImage.resetROI();

        satImage.resetROI();
        satImage.setROI(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);
        satImage.blurGaussian(gauss[1]);
        satImage.resetROI();

        briImage.resetROI();
        briImage.setROI(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);
        briImage.blurGaussian(gauss[2]);
        briImage.resetROI();

		hueImage.flagImageChanged();
		satImage.flagImageChanged();
		briImage.flagImageChanged();

        nowBlobs.clear();

        colBlobString =  " Blobs \n";

        grImage = colorImg;


        bgImage.resetROI();
        bgImage.setROI(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);


        grImage.resetROI();
        grImage.setROI(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);
		// take the abs value of the difference between background and incoming and then threshold:
		bgDiff.resetROI();
		bgDiff.setROI(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);
		bgDiff.absDiff(bgImage, grImage);
		bgDiff.threshold(threshold);
		//cvErode(bgDiff.getCvImage(), bgDiff.getCvImage(),CV_SHAPE_RECT,1);
		bgDiff.erode();
		bgDiff.dilate();
		bgDiff.dilate();
		//cvDilate(bgDiff.getCvImage(), bgDiff.getCvImage(),CV_SHAPE_RECT,2);


		bgDiff.resetROI();
		bgImage.resetROI();
		grImage.resetROI();

		if(cTrackColor[selectedColor].enabled)colorTrack(projScale);

		thisObjectTracker.feedObjects(nowBlobs);

        if(!thisMaze.getGameOver()){
        if(!mouseTest){thisMaze.feedObjects(thisObjectTracker.getFinals());}else{
        thisMaze.feedObjects(mouseObj);
        }
        };

    	if (counter == 0 && learnBg && bFrameDiff){
			bgImage = grImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)

		}

	}
        thisMaze.updateTime();

   //printf("%f \n", ofGetFrameRate());
}

void capture::loadSettings(){

char fileName[80];
ofxXmlSettings XML;

cout<< "Open - Enter file name:";
gets(fileName);


if(XML.loadFile(fileName)){
cout <<"\n" << "opening: "<< fileName << "\n";
}else{

cout <<"\n" << "can't find file: "<< fileName << "\n";
return;
};


if( XML.pushTag("RWSETTINGS", 0) ){

    if( XML.pushTag("MAZE", 0) ){

    thisMaze.loadLevels(XML.getValue("LEVELS", 0));
    thisMaze.setSpeedThresh(XML.getValue("SPEEDTHRESH",0));
    thisMaze.setStillTime(XML.getValue("STILLTIME",0));
    thisMaze.setTotalTime(XML.getValue("TOTALTIME",0));


    for(int i = 0; i < thisMaze.getLevels(); i++){
        if( XML.pushTag("LEVEL", i) ){
            thisMaze.setMazeDensity(XML.getValue("MAZEDENSITY",0),i);
            thisMaze.setMinDistance(XML.getValue("MINDISTANCE",0),i);
            thisMaze.setMazeDiameter(XML.getValue("MAZEDIAMETER",0),i);

            XML.popTag();
        }


    }

    XML.popTag();

    }

    if( XML.pushTag("BLOBTRACK", 0) ){

        thisObjectTracker.setMaxDist(XML.getValue("MAXDIST", 0));
        thisObjectTracker.setGhostFrames(XML.getValue("GHOSTFRAMES", 0));

        XML.popTag();
    }

    if( XML.pushTag("MASK", 0) ){
        for(int i =0; i < 8; i++){
        maskArray[i] = XML.getValue("MASKARRAY",0,i);
        }
        maskHeight = XML.getValue("MASKHEIGHT", 0);
        viewX = XML.getValue("VIEWX", viewX, 0);
        viewY = XML.getValue("VIEWY", viewY, 0);
        changed = true;
        for(int i =0; i < 4; i++){
        roiArray[i] = XML.getValue("ROIARRAY",0,i);
        }
    }

    if( XML.pushTag("TRACK", 0) ){

         if( XML.pushTag("GENERAL", 0) ){
            threshold = XML.getValue("THRESHOLD", 0);

            for(int i = 0; i < 4; i++){
            gauss[i] = XML.getValue("GAUSSIAN", 0,i);
            }

            minBlob = XML.getValue("MINBLOB", 0);

            XML.popTag();
            }


        for(int i = 0; i < 2; i++){

            string tag;

            if(i == 0)tag = "COLOR0";
            if(i == 1)tag = "COLOR1";

            if( XML.pushTag(tag, 0) ){

                cTrackColor[i].hue = XML.getValue("HUE:ABS",cTrackColor[i].hue,0);
                cTrackColor[i].huePlus = XML.getValue("HUE:PLUS", cTrackColor[i].huePlus, 0);
                cTrackColor[i].hueMinus = XML.getValue("HUE:MINUS", cTrackColor[i].hueMinus, 0);

                cTrackColor[i].sat = XML.getValue("SAT:ABS",cTrackColor[i].sat,0);
                cTrackColor[i].satPlus = XML.getValue("SAT:PLUS", cTrackColor[i].satPlus, 0);
                cTrackColor[i].satMinus = XML.getValue("SAT:MINUS", cTrackColor[i].satMinus, 0);

                cTrackColor[i].bri = XML.getValue("BRI:ABS",cTrackColor[i].bri,0);
                cTrackColor[i].briPlus = XML.getValue("BRI:PLUS", cTrackColor[i].briPlus, 0);
                cTrackColor[i].briMinus = XML.getValue("BRI:MINUS", cTrackColor[i].briMinus, 0);

                XML.popTag();
            }

        }

    XML.popTag();

    }

XML.popTag();

}
thisMaze.initialiseMaze(50,50);
thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());

}


void capture::saveSettings(){

char fileName[80];
ofxXmlSettings XML;

cout<< "Save - Enter file name:";
gets(fileName);
cout <<"\n" << "saving: "<< fileName << "\n";


int tagNum = XML.addTag("RWSETTINGS");
if( XML.pushTag("RWSETTINGS", tagNum) ){

    tagNum	= XML.addTag("MAZE");
    if( XML.pushTag("MAZE", tagNum) ){

    XML.setValue("LEVELS", thisMaze.getLevels(), tagNum);
    XML.setValue("SPEEDTHRESH",thisMaze.getSpeedThresh(), tagNum);
    XML.setValue("STILLTIME",thisMaze.getStillTime(), tagNum);
    XML.setValue("TOTALTIME",thisMaze.getTotalTime(), tagNum);


    for(int i = 0; i < thisMaze.getLevels(); i++){
        tagNum	= XML.addTag("LEVEL");
        if( XML.pushTag("LEVEL", tagNum) ){
            XML.setValue("MAZEDENSITY", thisMaze.getMazeDensity(i), tagNum);
            XML.setValue("MINDISTANCE", thisMaze.getMinDistance(i), tagNum);
            XML.setValue("MAZEDIAMETER", thisMaze.getMazeDiameter(i), tagNum);

            XML.popTag();
        }


    }

    XML.popTag();
    }

    tagNum	= XML.addTag("BLOBTRACK");
    if( XML.pushTag("BLOBTRACK", tagNum) ){

    XML.setValue("MAXDIST", thisObjectTracker.getMaxDist(), tagNum);
    XML.setValue("GHOSTFRAMES", thisObjectTracker.getGhostFrames(), tagNum);

    XML.popTag();
    }

    tagNum	= XML.addTag("MASK");
    if( XML.pushTag("MASK", tagNum) ){
        for(int i =0; i < 8; i++){
        XML.setValue("MASKARRAY",maskArray[i],tagNum+i);
        }
        XML.setValue("MASKHEIGHT", maskHeight,tagNum);
        XML.setValue("VIEWX", viewX, tagNum);
        XML.setValue("VIEWY", viewY, tagNum);
        for(int i =0; i < 4; i++){
        XML.setValue("ROIARRAY",roiArray[i],tagNum+i);
        }
    }

    tagNum	= XML.addTag("TRACK");
    if( XML.pushTag("TRACK", tagNum) ){

        tagNum = XML.addTag("GENERAL");
            if( XML.pushTag("GENERAL", tagNum) ){

            XML.setValue("THRESHOLD", threshold, tagNum);

        for(int i = 0; i < 4; i++ ){
            XML.setValue("GAUSSIAN", gauss[i], tagNum + i);
            }

            XML.setValue("MINBLOB", minBlob, tagNum);

            XML.popTag();
        }

        for(int i = 0; i < 2; i++){

        string tag;

        if(i == 0)tag = "COLOR0";
        if(i == 1)tag = "COLOR1";

        tagNum = XML.addTag(tag);

            if( XML.pushTag(tag, tagNum) ){

                tagNum = XML.addTag("HUE");

                XML.setValue("HUE:ABS",cTrackColor[i].hue,tagNum);
                XML.setValue("HUE:PLUS", cTrackColor[i].huePlus, tagNum);
                XML.setValue("HUE:MINUS", cTrackColor[i].hueMinus, tagNum);

                tagNum = XML.addTag("SAT");

                XML.setValue("SAT:ABS",cTrackColor[i].sat,tagNum);
                XML.setValue("SAT:PLUS", cTrackColor[i].satPlus, tagNum);
                XML.setValue("SAT:MINUS", cTrackColor[i].satMinus, tagNum);

                tagNum = XML.addTag("BRI");

                XML.setValue("BRI:ABS",cTrackColor[i].bri,tagNum);
                XML.setValue("BRI:PLUS", cTrackColor[i].briPlus, tagNum);
                XML.setValue("BRI:MINUS", cTrackColor[i].briMinus, tagNum);

                XML.popTag();
            }

        }


    XML.popTag();
    }
XML.popTag();
}
XML.saveFile(fileName);

}

void capture::colorTrack(float scale){


    unsigned char * huePixels = hueImage.getPixels();                         //huePixels is now a raw array of pixels
    unsigned char * satPixels = satImage.getPixels();                         //satPixels is now a raw array of pixels  just like huePixels
    unsigned char * briPixels = briImage.getPixels();
    unsigned char * diffPixels = bgDiff.getPixels();


    int nPixels;

    nPixels = camWidth * camHeight;



    for (int i = 0; i < nPixels; i++){


    if(diffPixels[i] > 100){

                 //if the saturation and brightness is of a certain range

                if((satPixels[i] >= cTrackColor[selectedColor].sat - cTrackColor[selectedColor].satMinus &&
                    satPixels[i] <= cTrackColor[selectedColor].sat + cTrackColor[selectedColor].satPlus)
                    &&(briPixels[i] >= cTrackColor[selectedColor].bri - cTrackColor[selectedColor].briMinus &&
                       briPixels[i] <= cTrackColor[selectedColor].bri + cTrackColor[selectedColor].briPlus)){


                    if (satPixels[i] == 0 ||  //if the hue is of a certain range
                        (huePixels[i] >= cTrackColor[selectedColor].hue - cTrackColor[selectedColor].hueMinus
                         && huePixels[i] <= cTrackColor[selectedColor].hue + cTrackColor[selectedColor].huePlus)
                        || (huePixels[i]-255 >= cTrackColor[selectedColor].hue - cTrackColor[selectedColor].hueMinus
                         && huePixels[i]-255 <= cTrackColor[selectedColor].hue + cTrackColor[selectedColor].huePlus)
                        || (huePixels[i]+255 >= cTrackColor[selectedColor].hue - cTrackColor[selectedColor].hueMinus
                         && huePixels[i]+255<= cTrackColor[selectedColor].hue + cTrackColor[selectedColor].huePlus) )


                    {
                    cTrackPixels[i] = 255;   //mark this corresponding pixel white

                }else {cTrackPixels[i] = 0;}

                                           //if it doesn't fit then color the corresponding pixel black
            }else {cTrackPixels[i] = 0;}

        }else {cTrackPixels[i] = 0;}

       }

       cTrackCont.setFromPixels(cTrackPixels, camWidth, camHeight);  //set oneCont from the colorTrackedPixelsOne array so it's all clean and openCv operable

        cTrackCont.resetROI();
        cTrackCont.setROI(roiArray[0], roiArray[1], roiArray[2], roiArray[3]);
        cTrackCont.blurGaussian(gauss[3]);
        cTrackCont.resetROI();
       cTrackFinder.findContours(cTrackCont,minBlob,nPixels/5, 10, false, false);                  //lets find one (1) blob in the grayscale openCv image oneCont

       cTrackTexture.loadData(cTrackPixels, camWidth, camHeight, GL_LUMINANCE);


        glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);

        for (int i = 0; i < cTrackFinder.nBlobs; i++){

        int y = cTrackFinder.blobs[i].centroid.y * scale;
        int x = cTrackFinder.blobs[i].centroid.x * scale;

        //y = y + (camHeight * scale);

        GLfloat winX, winY, winZ;

        winX = (float)x;
        winY = (float)ofGetHeight() - (float)y;

        glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );


        if (winZ != 1.0f) {


            //printf("test \n");
        gluUnProject( winX, winY, winZ, mvmatrix, projmatrix, viewport, &position[0], &position[1],&position[2]);



        trackingObject temp;

        temp.setProjCentX((float)position[0]);
        temp.setProjCentY((float)position[1]);

        //fill the output string for chosen colNum


        colBlobString = colBlobString + "\nprojected (x,y): " + ofToString(position[0],1) + "," + ofToString(position[1],1);

        int boundX, boundY;

        boundX = cTrackFinder.blobs[i].boundingRect.x;
        boundY = cTrackFinder.blobs[i].boundingRect.y;

        temp.setScrNearX((float)boundX);
        temp.setScrNearY((float)boundY);
        temp.setScrFarX((float)boundX + (float)cTrackFinder.blobs[i].boundingRect.width);
        temp.setScrFarY((float)boundY + (float)cTrackFinder.blobs[i].boundingRect.height);

            if(position[0] >= 0 && position[1] >=0){
            nowBlobs.push_back(temp);
            }
        }



    }

    glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
}


void capture::reshape(int x, int y, int w, int h)
{

//glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);

GLfloat prop = (GLfloat)w/(GLfloat)h;

   glViewport (x - (w*0.25), y -(h*0.25), (GLsizei)w * 1.5, (GLsizei)h * 1.5);
   if(changed == true)glGetIntegerv (GL_VIEWPORT, viewport);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective (45.0, prop,1.0, 400.0);

   if(changed == true)glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

}

void capture::reload(){

glMatrixMode(GL_VIEWPORT);
glViewport(0,0,ofGetWidth(),ofGetHeight());


}


void capture::draw3DFrame(bool topFrame){


//glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();

if(topFrame){



        glPushMatrix();
		glTranslated(maskArray[0], maskArray[1] + maskHeight, maskArray[2]);
		glRotated(maskArray[3], 1, 0, 0);
		glRotated(maskArray[4], 0, 1, 0);
		glRotated(maskArray[5], 0, 0, 1);
		// this will be the same modelview matrix that the object is drawn with
		// so vertices will be in the same frame of reference.

        glPushMatrix();
        ofNoFill();
        glTranslatef(-25,-25,0);
        ofSetColor(255,0,0);
		ofRect(0,0,50,50);
		ofFill();
        ofSetColor(255,255,255);


        glPopMatrix();


        glPopMatrix();
   }

            glPushMatrix();
            glTranslated(maskArray[0], maskArray[1], maskArray[2]);
            glRotated(maskArray[3], 1, 0, 0);
            glRotated(maskArray[4], 0, 1, 0);
            glRotated(maskArray[5], 0, 0, 1);
            // this will be the same modelview matrix that the object is drawn with
            // so vertices will be in the same frame of reference.

            glPushMatrix();
            ofNoFill();
            glTranslatef(-25,-25,0);

            if(viewMode == 0)ofCircle(25,25,1);
            ofSetColor(255,255,255);
            ofRect(0,0,50,50);
            ofFill();




            glPopMatrix();


            glPopMatrix();



}

void capture::save3DMask(){


}

void capture::draw3DMask(){

//ofNoFill();
//ofRect(0,0,camWidth,camHeight);

//reshape(viewX,viewY + ofGetHeight()-(camHeight * scale) ,camWidth * scale,camHeight *scale);



        glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();

		glTranslated(maskArray[0], maskArray[1] + maskHeight, maskArray[2]);
		glRotated(maskArray[3], 1, 0, 0);
		glRotated(maskArray[4], 0, 1, 0);
		glRotated(maskArray[5], 0, 0, 1);


		// this will be the same modelview matrix that the object is drawn with
		// so vertices will be in the same frame of reference.

        glPushMatrix();
        ofFill();
        glTranslatef(-25,-25,0);
		ofRect(0,0,50,50*maskArray[6]);

		if(changed == true){glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix); changed = false;}

        glPopMatrix();

        glPopMatrix();



}

void capture::draw3DBlobs(){



  //  reshape(viewX + posX,viewY + ofGetHeight()-((camHeight*scale)+posY),camWidth*scale,camHeight*scale);

        //glLoadMatrixd(mvmatrix);
		// this will be the same modelview matrix that the object is drawn with
		// so vertices will be in the same frame of reference.
		glTranslated(maskArray[0], maskArray[1], maskArray[2]);
		glRotated(maskArray[3], 1, 0, 0);
		glRotated(maskArray[4], 0, 1, 0);
		glRotated(maskArray[5], 0, 0, 1);
        glTranslatef(-25,-25,0);
		ofSetColor(255,0,0);


        for(int i = 0; i < nowBlobs.size(); i++){

                float x = nowBlobs.at(i).getProjCentX();
                float y = nowBlobs.at(i).getProjCentY();

            ofRect(x-1.5f, y-1.5f,3,3);


        }

        ofSetColor(255,255,255);

   // reshape(viewX, viewY + ofGetHeight()-(camHeight * scale),camWidth * scale,camHeight *scale);

//glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
}

void capture::draw3DTrackedBlobs(){

	vector<trackingObject> trackedBlobs;

	if(!mouseTest){trackedBlobs = thisObjectTracker.getFinals();}else{
	trackedBlobs = mouseObj;

	}


    //reshape(viewX + posX,viewY + ofGetHeight()-((camHeight*scale)+posY),camWidth * scale,camHeight * scale);

        //glLoadMatrixd(mvmatrix);
		// this will be the same modelview matrix that the object is drawn with
		// so vertices will be in the same frame of reference.
		glPushMatrix();

		glTranslated(maskArray[0], maskArray[1], maskArray[2]);
		glRotated(maskArray[3], 1, 0, 0);
		glRotated(maskArray[4], 0, 1, 0);
		glRotated(maskArray[5], 0, 0, 1);
        glTranslatef(-25,-25,0);




        for(int i = 0; i < trackedBlobs.size(); i++){

           ofSetColor(255,0,0);

                float x = trackedBlobs.at(i).getProjCentX();
                float y = trackedBlobs.at(i).getProjCentY();

            if(trackedBlobs.at(i).getPresent()){ofRect(x-1.5f, y-1.5f,3,3);
            ofSetColor(255,255,255);
				glPushMatrix();
					glTranslatef(x +1.5,y+1.5, 0);
					glScalef(0.2, 0.2, 0);
					ofDrawBitmapString(ofToString(i,0),0,0);
				glPopMatrix();
				
            }



        }

        ofSetColor(255,255,255);
        glPopMatrix();

    //reshape(viewX,viewY + ofGetHeight()-(camHeight * scale),camWidth *scale,camHeight*scale);
//glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);

}

void capture::drawMaze(){

    vector<mazePoint> tempPoints = thisMaze.getMazePoints();

//glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);

//    reshape(viewX + posX,viewY + ofGetHeight()-((camHeight*scale)+posY),camWidth * scale,camHeight * scale);

        //glLoadMatrixd(mvmatrix);
		// this will be the same modelview matrix that the object is drawn with
		// so vertices will be in the same frame of reference.
		glTranslated(maskArray[0], maskArray[1], maskArray[2]);
		glRotated(maskArray[3], 1, 0, 0);
		glRotated(maskArray[4], 0, 1, 0);
		glRotated(maskArray[5], 0, 0, 1);
        glTranslatef(-25,-25,0);

    for(int i = 0; i < tempPoints.size(); i++){

       // int bval = 255 - (5 * (int)tempPoints.at(i).proximity);
        int rval = 5 * (int)tempPoints.at(i).proximity;


        ofSetColor(rval,rval,rval);

        ofCircle(tempPoints.at(i).location.x, tempPoints.at(i).location.y, thisMaze.getMazeDiameter());

    }

    ofSetColor(255,255,255);


//glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
//reshape(viewX,viewY + ofGetHeight()-(camHeight* scale),camWidth *scale,camHeight*scale);



}


void capture::draw(){

ofSetColor(0,0,0);


thisDisplay.setLeveldata(thisMaze.getCurrentLevel(), thisMaze.getAllMazePoints());
thisDisplay.updateCounter();

	displayText.begin();
	ofClear(0, 0, 0, 255);
	if(thisMaze.isMoving()){
		thisDisplay.drawMoveShapes(thisMaze.getAllMazePoints(), thisMaze.getPlayerPos());
	}else{
		thisDisplay.drawStillShapes(thisMaze.getAllMazePoints(), thisMaze.getPlayerPos());
	}
	
	
	thisDisplay.drawPlayerPos(thisMaze.getPlayerPos());
	thisDisplay.drawInfo(thisMaze.getTimeRemain(), thisMaze.getCurrentScore(), thisMaze.getCurrentLevel());
	
	if(thisMaze.getGameOver()){

		thisDisplay.showGameOver(thisMaze.getHighScores(), thisMaze.isAllComplete());
	}

	
	displayText.end();
	
	displayText.draw(1280,0);
	
    switch(viewMode){

    case 0:
    projScale = 1;
    draw0();
    glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
    reshape(viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    draw3DMask();
    glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
    reshape(camWidth + viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    draw3DFrame(true);
    draw3DBlobs();
    reshape(viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);



    break;

    case 1:
    projScale = 1;
    draw1();
    reshape(viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
    draw3DMask();
    glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
    reshape(camWidth + viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    draw3DFrame(false);
    draw3DTrackedBlobs();
    reshape(viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    break;

    case 2:
    projScale = 1;
    draw2();
    glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
    reshape(viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    draw3DMask();
    glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
    reshape(camWidth + viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    draw3DFrame(false);
    draw3DTrackedBlobs();
    drawMaze();
    reshape(viewX,viewY + ofGetHeight()-camHeight,camWidth,camHeight);
    break;

    }






}

//--------------------------------------------------------------
void capture::draw0(){

glPushMatrix();
//glTranslatef(camWidth,0,0);
ofNoFill();
ofSetColor(0,0,255);
ofRect(roiArray[0],roiArray[1],roiArray[2],roiArray[3]);
ofSetColor(255,255,255);
ofFill();
glPopMatrix();

glPushMatrix();
glTranslatef(10,500,0);

if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
ofDrawBitmapString("Mask Settings:",0,0);

  int maskMenuColors[14];

    for(int i = 0; i < 14; i++){

    if(selectedItem == i && selectedMenu == 0){
    maskMenuColors[i] = 0xff0000;
    }else{maskMenuColors[i] = 0xffffff;}

    }

glTranslatef(0,15,0);

ofSetHexColor(maskMenuColors[0]);
ofDrawBitmapString("tran x:" + ofToString(maskArray[0],0),0,0);
ofSetHexColor(maskMenuColors[1]);
ofDrawBitmapString("tran y:" + ofToString(maskArray[1],0),0,15);
ofSetHexColor(maskMenuColors[2]);
ofDrawBitmapString("tran z:" + ofToString(maskArray[2],0),0,30);
ofSetHexColor(maskMenuColors[3]);
ofDrawBitmapString("rot x:" + ofToString(maskArray[3],0),0,45);
ofSetHexColor(maskMenuColors[4]);
ofDrawBitmapString("rot y:" + ofToString(maskArray[4],0),0,60);
ofSetHexColor(maskMenuColors[5]);
ofDrawBitmapString("rot z:" + ofToString(maskArray[5],0),0,75);
ofSetHexColor(maskMenuColors[6]);
ofDrawBitmapString("prop (w*h):" + ofToString(maskArray[6],2),0,90);
ofSetHexColor(maskMenuColors[7]);
ofDrawBitmapString("mask height:" + ofToString(maskHeight,2),0,105);
ofSetHexColor(maskMenuColors[8]);
ofDrawBitmapString("viewX:" + ofToString(viewX,2),0,120);
ofSetHexColor(maskMenuColors[9]);
ofDrawBitmapString("viewY:" + ofToString(viewY,2),0,135);
ofSetHexColor(maskMenuColors[10]);
ofDrawBitmapString("ROIX:" + ofToString(roiArray[0],2),0,150);
ofSetHexColor(maskMenuColors[11]);
ofDrawBitmapString("ROIY:" + ofToString(roiArray[1],2),0,165);
ofSetHexColor(maskMenuColors[12]);
ofDrawBitmapString("ROIW:" + ofToString(roiArray[2],2),0,180);
ofSetHexColor(maskMenuColors[13]);
ofDrawBitmapString("ROIH:" + ofToString(roiArray[3],2),0,195);

ofSetColor(255,255,255);


//ofDrawBitmapString("x: " + ofToString(position[0],1) + "   y: " + ofToString(position[1],1) + "   z: " + ofToString(position[2],1),0,60 );
glPopMatrix();


    glPushMatrix();
    glTranslatef(camWidth,0,0);
    colorImg.draw(0,0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,20,0);

    glPushMatrix();
    glTranslatef(400,camHeight,0);
        glPushMatrix();
        glScalef(0.5,0.5,0);

        switch(selectedImage){

        case 0:
        bgDiff.draw(0,0);
        ofDrawBitmapString("bgDiff", 0, camHeight + 25);
        break;

        case 1:
        hueImage.draw(0,0);
        ofDrawBitmapString("hue", 0, camHeight + 25);
        break;

        case 2:
        satImage.draw(0,0);
        ofDrawBitmapString("sat", 0, camHeight + 25);
        break;

        case 3:
        briImage.draw(0,0);
        ofDrawBitmapString("bri", 0, camHeight + 25);
        break;

        case 4:
        colorImageHSV.draw(0,0);
        ofDrawBitmapString("hsv", 0, camHeight + 25);
        break;

        case 5:
        cTrackTexture.draw(0,0);
        ofDrawBitmapString("c-track", 0, camHeight + 25);
        break;

        case 6:
        ofFill();
        ofSetHexColor(0x333333);
        ofRect(0,0,camWidth,camHeight);
        ofSetHexColor(0xffffff);
        cTrackFinder.draw(0,0);
        ofDrawBitmapString("blobs", 0, camHeight + 25);
        break;
        }

        glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(200,camHeight ,0);

    if(selectedMenu == 1){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("ColorTrack Settings:",0,0);

    int ctMenuColors[13];

    for(int i = 0; i < 13; i++){

    if(selectedItem == i && selectedMenu == 1){
    ctMenuColors[i] = 0xff0000;
    }else{ctMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(ctMenuColors[0]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("color:" + ofToString(selectedColor),0,0);
    ofSetHexColor(ctMenuColors[1]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("enabled:" + ofToString(cTrackColor[selectedColor].enabled),0,0);
    ofSetHexColor(ctMenuColors[2]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("hue abs:" + ofToString(cTrackColor[selectedColor].hue,0),0,0);
    ofSetHexColor(ctMenuColors[3]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("hue-:" + ofToString(cTrackColor[selectedColor].hueMinus,0),0,0);
    ofSetHexColor(ctMenuColors[4]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("hue+:" + ofToString(cTrackColor[selectedColor].huePlus,0),0,0);
    ofSetHexColor(ctMenuColors[5]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("sat abs:" + ofToString(cTrackColor[selectedColor].sat,0),0,0);
    ofSetHexColor(ctMenuColors[6]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("sat-:" + ofToString(cTrackColor[selectedColor].satMinus,0),0,0);
    ofSetHexColor(ctMenuColors[7]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("sat+:" + ofToString(cTrackColor[selectedColor].satPlus,0),0,0);
    ofSetHexColor(ctMenuColors[8]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("bri abs:" + ofToString(cTrackColor[selectedColor].bri,2),0,0);
    ofSetHexColor(ctMenuColors[9]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("bri-:" + ofToString(cTrackColor[selectedColor].briMinus,2),0,0);
    ofSetHexColor(ctMenuColors[10]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("bri+:" + ofToString(cTrackColor[selectedColor].briPlus,2),0,0);
    ofSetHexColor(ctMenuColors[11]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("bg thresh:" + ofToString(threshold,2),0,0);
    ofSetHexColor(ctMenuColors[12]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("frameDiff:" + ofToString(bFrameDiff,0),0,0);
    ofSetColor(255,255,255);

    glPopMatrix();

 glPushMatrix();
    glTranslatef(200,camHeight + 220,0);

if(selectedMenu == 2){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Blob Settings:",0,0);

    int blobMenuColors[5];

    for(int i = 0; i < 5; i++){

    if(selectedItem == i && selectedMenu == 2){
    blobMenuColors[i] = 0xff0000;
    }else{blobMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(blobMenuColors[0]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("minBlob:" + ofToString(minBlob),0,0);
    ofSetHexColor(blobMenuColors[1]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("hue gauss:" + ofToString(gauss[0]),0,0);
    ofSetHexColor(blobMenuColors[2]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("sat gauss:" + ofToString(gauss[1]),0,0);
    ofSetHexColor(blobMenuColors[3]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("bri gauss:" + ofToString(gauss[2]),0,0);
    ofSetHexColor(blobMenuColors[4]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("text gauss:" + ofToString(gauss[3]),0,0);


    ofSetColor(255,255,255);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(800,camHeight,0);
    ofDrawBitmapString(colBlobString,0,0);
    glPopMatrix();





glPopMatrix();


}


void capture::draw1(){


   colorImg.draw(camWidth,0);

    glPushMatrix();

    glTranslatef(10,camHeight + 20,0);

    if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Tracking Settings:",0,0);

    int trackMenuColors[2];

    for(int i = 0; i < 2; i++){

    if(selectedItem == i && selectedMenu == 0){
    trackMenuColors[i] = 0xff0000;
    }else{trackMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(trackMenuColors[0]);
    ofDrawBitmapString("maxDist: " + ofToString(thisObjectTracker.getMaxDist(),1),0,15);
    ofSetHexColor(trackMenuColors[1]);
    ofDrawBitmapString("ghostFrames: " + ofToString(thisObjectTracker.getGhostFrames(),0),0,30);

    ofSetColor(255,255,255);

    glPopMatrix();

    vector<trackingObject> trackedObjects;
    trackedObjects = thisObjectTracker.getFinals();

    string trackedString = "Tracked Objects:  \n";

    for(int i = 0; i < 2; i++){

    trackedString = trackedString + "\nobject: " + ofToString(i,0) +
                "   present: " + ofToString(trackedObjects[i].getPresent()) +
                    "  projX: " + ofToString(trackedObjects[i].getProjCentX(),1) +
                    "   projY:" + ofToString(trackedObjects[i].getProjCentY(),1) +
                    "   speed:" + ofToString(trackedObjects[i].getSpeed(),3);

    }

    ofDrawBitmapString(trackedString, 300,camHeight + 20);


}

void capture::draw2(){


glPushMatrix();

    glTranslatef(360,camHeight + 20,0);

    if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Maze Settings:",0,0);

    int mazeMenuColors[6];

    for(int i = 0; i < 6; i++){

    if(selectedItem == i && selectedMenu == 0){
    mazeMenuColors[i] = 0xff0000;
    }else{mazeMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(mazeMenuColors[0]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("speedThresh: " +ofToString(thisMaze.getSpeedThresh(),2),0,0);
    ofSetHexColor(mazeMenuColors[1]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("stillTime: " +ofToString(thisMaze.getStillTime()),0,0);
    ofSetHexColor(mazeMenuColors[2]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("levels: " + ofToString(thisMaze.getLevels()) ,0,0);
    ofSetHexColor(mazeMenuColors[3]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("countDown: " + ofToString(thisMaze.getTotalTime()) ,0,0);
    ofSetHexColor(mazeMenuColors[4]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("mouseTest: "+ ofToString(mouseTest),0,0);
    ofSetHexColor(mazeMenuColors[5]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("playerName: "+ thisMaze.getPlayerName(),0,0);


    ofSetColor(255,255,255);


    glPopMatrix();


    glPushMatrix();

    glTranslatef(360,camHeight + 180,0);

    if(selectedMenu == 1){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Level Settings:",0,0);

    int levelMenuColors[4];

    for(int i = 0; i < 4; i++){

    if(selectedItem == i && selectedMenu == 1){
    levelMenuColors[i] = 0xff0000;
    }else{levelMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(levelMenuColors[0]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("currentLevel: " + ofToString(thisMaze.getCurrentLevel(), 0),0,0);
    ofSetHexColor(levelMenuColors[1]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("points: " + ofToString(thisMaze.getMazeDensity(),0),0,0);
    ofSetHexColor(levelMenuColors[2]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("pointDiameter: " + ofToString(thisMaze.getMazeDiameter(),2),0,0);
    ofSetHexColor(levelMenuColors[3]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("minDistance: " +ofToString(thisMaze.getMinDistance(),2),0,0);

    ofSetColor(255,255,255);

    glPopMatrix();


    vector<mazePoint> tempPoints = thisMaze.getMazePoints();
    string pointString = "Maze points: \n";

    for(int i = 0; i < tempPoints.size(); i++){
    pointString =  pointString + "sequential: " + ofToString(tempPoints.at(i).sequential) +
                    "   proximity: " + ofToString(tempPoints.at(i).proximity) + "\n";
    }

    ofDrawBitmapString(pointString,10,camHeight +20);

    ofDrawBitmapString(ofToString(thisMaze.getTimeRemain(),3),10,600);
    ofDrawBitmapString("score:  " + ofToString(thisMaze.getCurrentScore(),3),10,620);
    ofDrawBitmapString("High Scores: \n" + thisMaze.getHighScores(),10,640);

    glPushMatrix();
    ofEnableAlphaBlending();
    glTranslatef(camWidth, camHeight, 0);

    glPushMatrix();
    glScalef(5,5,0);
    ofSetColor(255,255,255,255);

    for(int i = 0; i < tempPoints.size(); i ++){
    if(tempPoints.at(i).isActive){ofFill();}else{ofNoFill();}
		
    ofCircle(tempPoints.at(i).location.x, 50 - tempPoints.at(i).location.y, thisMaze.getMazeDiameter());
		glPushMatrix();
		glTranslatef(tempPoints.at(i).location.x + thisMaze.getMazeDiameter(), 
					 50 -tempPoints.at(i).location.y+ thisMaze.getMazeDiameter(),0);
		glScalef(0.2, 0.2, 0);
		ofDrawBitmapString(ofToString(tempPoints.at(i).sequential,0),0,0);
		glPopMatrix();
    }
	
    glPopMatrix();

    if(thisMaze.isMoving()){ofSetColor(255,0,0,100);}else{ofSetColor(0,255,0,100);}
    ofFill();
    ofRect(0,0, 250,250);

    ofNoFill();
    glPopMatrix();
    ofDisableAlphaBlending();
    ofSetColor(255,255,255);

}


//--------------------------------------------------------------
void capture::keyPressed  (int key){

	switch (key){

        case '1':
        viewMode += 1;
        viewMode = viewMode%3;
        selectedMenu = 0;
        selectedItem = 0;
        changed = false;
        break;
			
		case 'f':
			ofToggleFullscreen();
			break;

	}
	


	if (viewMode == 0)viewKeys0(key);
	if (viewMode == 1)viewKeys1(key);
	if (viewMode == 2)viewKeys2(key);

}

void capture::viewKeys0(int key){

    switch(key){

        case '2':
        camNo +=1;camNo = camNo%2;
        vidGrabber.close();
        vidGrabber.setDeviceID(camNo);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        break;

        case '6':
        saveSettings();
        break;

        case '7':
        loadSettings();
        break;

        case ' ':
        if (learnBg == true){learnBg = false;} else{learnBg = true;}
        break;

        case OF_KEY_RETURN:
        selectedImage +=1; selectedImage = selectedImage%7;
        break;

        case '<':
        selectedMenu += 1;
        selectedMenu = selectedMenu%3;
        selectedItem = 0;
        break;

        case '>':
        selectedItem += 1;
        int items;
        if(selectedMenu == 0)items = 14;
        if(selectedMenu == 1)items = 13;
        if(selectedMenu == 2)items = 5;
        selectedItem = selectedItem%items;
        break;

        case OF_KEY_LEFT:
        if (selectedMenu == 0) maskArray[selectedItem] -= 1;changed = true;
        break;

        case OF_KEY_RIGHT:
        if (selectedMenu == 0)maskArray[selectedItem] += 1;changed = true;
        break;

        case OF_KEY_UP:
        if (selectedMenu == 0){
            changed = true;

            if(selectedItem < 6){maskArray[selectedItem] += 1;}
            else if(selectedItem == 6){maskArray[selectedItem] += 0.05;}
            else if(selectedItem == 7){maskHeight += 0.5;}
            else if(selectedItem == 8){viewX +=0.5;}
            else if(selectedItem == 9){viewY +=0.5;}
            else if(selectedItem > 9){roiArray[selectedItem - 10] +=1;}
        }
        if (selectedMenu == 1){
            if(selectedItem == 0)selectedColor += 1; selectedColor = selectedColor%2;
            if(selectedItem == 1)cTrackColor[selectedColor].enabled = true;
            if(selectedItem == 2)cTrackColor[selectedColor].hue += 1;
            if(selectedItem == 3)cTrackColor[selectedColor].hueMinus += 1;
            if(selectedItem == 4)cTrackColor[selectedColor].huePlus += 1;
            if(selectedItem == 5)cTrackColor[selectedColor].sat += 1;
            if(selectedItem == 6)cTrackColor[selectedColor].satMinus += 1;
            if(selectedItem == 7)cTrackColor[selectedColor].satPlus += 1;
            if(selectedItem == 8)cTrackColor[selectedColor].bri += 1;
            if(selectedItem == 9)cTrackColor[selectedColor].briMinus += 1;
            if(selectedItem == 10)cTrackColor[selectedColor].briPlus += 1;
            if(selectedItem == 11)threshold += 1;
             if(selectedItem == 12)bFrameDiff = true;
        }
        if (selectedMenu == 2){
            if(selectedItem == 0)minBlob +=1 ;
            if(selectedItem == 1)gauss[0] +=1;
            if(selectedItem == 2)gauss[1] +=1;
            if(selectedItem == 3)gauss[2] +=1;
            if(selectedItem == 4)gauss[3] +=1;


            }


        break;

        case OF_KEY_DOWN:
        if (selectedMenu == 0){
        changed = true;
            if(selectedItem < 6){maskArray[selectedItem] -= 1;}
            else if(selectedItem == 6){maskArray[selectedItem] -= 0.05;}
            else if(selectedItem == 7){maskHeight -= 0.5;}
            else if(selectedItem == 8){viewX -=0.5;}
            else if(selectedItem == 9){viewY -=0.5;}
            else if(selectedItem > 9){roiArray[selectedItem - 10] -=1;}

        }
        if (selectedMenu == 1){
            if(selectedItem == 0)selectedColor -= 1; selectedColor = max(0,selectedColor);
            if(selectedItem == 1)cTrackColor[selectedColor].enabled = false;
            if(selectedItem == 2)cTrackColor[selectedColor].hue -= 1;
            if(selectedItem == 3)cTrackColor[selectedColor].hueMinus -= 1;
            if(selectedItem == 4)cTrackColor[selectedColor].huePlus -= 1;
            if(selectedItem == 5)cTrackColor[selectedColor].sat -= 1;
            if(selectedItem == 6)cTrackColor[selectedColor].satMinus -= 1;
            if(selectedItem == 7)cTrackColor[selectedColor].satPlus -= 1;
            if(selectedItem == 8)cTrackColor[selectedColor].bri -= 1;
            if(selectedItem == 9)cTrackColor[selectedColor].briMinus -= 1;
            if(selectedItem == 10)cTrackColor[selectedColor].briPlus -= 1;
            if(selectedItem == 11)threshold -= 1;
            if(selectedItem == 12)bFrameDiff = false;
        }
        if (selectedMenu == 2){
            if(selectedItem == 0)minBlob -=1 ;
            if(selectedItem == 1)gauss[0] -=1;
            if(selectedItem == 2)gauss[1] -=1;
            if(selectedItem == 3)gauss[2] -=1;
            if(selectedItem == 4)gauss[3] -=1;
            }




        break;



	}


}

void capture::viewKeys1(int key){

changed = false;

    switch(key){



            case '>':
            selectedMenu += 1;
            selectedMenu = selectedMenu%1;
            selectedItem = 0;
            break;

            case '<':
            selectedItem += 1;
            int items;
            if(selectedMenu == 0)items = 2;
            selectedItem = selectedItem%items;
            break;

            case OF_KEY_UP:
            if (selectedMenu == 0){

                if(selectedItem == 0){thisObjectTracker.setMaxDist(thisObjectTracker.getMaxDist() + 1);}
                if(selectedItem == 1){thisObjectTracker.setGhostFrames(thisObjectTracker.getGhostFrames() + 1);}

            }
            break;

            case OF_KEY_DOWN:
            if (selectedMenu == 0){

                if(selectedItem == 0){thisObjectTracker.setMaxDist(thisObjectTracker.getMaxDist() - 1);}
                if(selectedItem == 1){thisObjectTracker.setGhostFrames(thisObjectTracker.getGhostFrames() - 1);}


            }
            break;

            case OF_KEY_LEFT:

            break;

            case OF_KEY_RIGHT:

            break;
    }
}

void capture::viewKeys2(int key){

changed = false;

    switch(key){

    case '6':
    thisMaze.saveHighScores();
    break;

    case '7':
    thisMaze.loadHighScores();
    break;

    case '0':
    thisMaze.initialiseMaze(50, (int)(50 *maskArray[6]));
    thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());
    break;

    case ' ':
    bFrameDiff = true;
    thisMaze.startTimer();
    thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());
    break;

    case '>':
    selectedMenu += 1;
    selectedMenu = selectedMenu%2;
    selectedItem = 0;
    break;

    case '<':
    selectedItem += 1;
    int items;

    if(selectedMenu == 0)items = 6;
    if(selectedMenu == 1)items = 4;
    selectedItem = selectedItem%items;
    break;

    case OF_KEY_UP:
            if (selectedMenu == 0){
                if(selectedItem == 0){thisMaze.setSpeedThresh(thisMaze.getSpeedThresh()+1);}
                if(selectedItem == 1)thisMaze.setStillTime(thisMaze.getStillTime()+1);
                if(selectedItem == 2){thisMaze.setLevels(thisMaze.getLevels()+1);
                                    thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());}

                if(selectedItem == 3)thisMaze.setTotalTime(thisMaze.getTotalTime()+1);
                if(selectedItem == 4)mouseTest = true;

            }
            if (selectedMenu == 1){

                if(selectedItem == 0){thisMaze.levelIncrement(false);}
                if(selectedItem == 1){thisMaze.setMazeDensity(thisMaze.getMazeDensity()+1);
                                      thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());}
                if(selectedItem == 2){thisMaze.setMazeDiameter(thisMaze.getMazeDiameter()+0.05);}
                if(selectedItem == 3){thisMaze.setMinDistance(thisMaze.getMinDistance()+1);}


            }
            break;

            case OF_KEY_DOWN:
            if (selectedMenu == 0){
                if(selectedItem == 0){thisMaze.setSpeedThresh(thisMaze.getSpeedThresh()-1);}
                if(selectedItem == 1)thisMaze.setStillTime(thisMaze.getStillTime()-1);
                if(selectedItem == 2){thisMaze.setLevels(thisMaze.getLevels()-1);
                                    thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());}
                if(selectedItem == 3)thisMaze.setTotalTime(thisMaze.getTotalTime()-1);
                if(selectedItem == 4)mouseTest = false;

            }
            if (selectedMenu == 1){

                //if(selectedItem == 0){thisMaze.setCurrentLevel(thisMaze.getCurrentLevel() -1);}
                if(selectedItem == 1){thisMaze.setMazeDensity(thisMaze.getMazeDensity()-1);
                                       thisDisplay.setLeveldata(100, thisMaze.getAllMazePoints());}
                if(selectedItem == 2){thisMaze.setMazeDiameter(thisMaze.getMazeDiameter()-0.05);}
                if(selectedItem == 3){thisMaze.setMinDistance(thisMaze.getMinDistance()-1);}


            }
            break;

        default:

        if(selectedMenu == 0 && selectedItem == 5){

        string str;
        str = thisMaze.getPlayerName();

       if(key==8 && str.size()>0) { // backspace
        str = str.substr(0, str.size()-1); // delete one character
        } else {
        str.append (1, (char)key );
        }

        thisMaze.setPlayerName(str);

    }

    }



}
//--------------------------------------------------------------
void capture::mouseMoved(int x, int y ){


}
//--------------------------------------------------------------
void capture::mouseDragged(int x, int y, int button){


if(viewMode == 2 && mouseTest){



 glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);



        GLfloat winX, winY, winZ;

        winX = (float)x;
        winY = (float)ofGetHeight() - (float)y;

        glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );


        if (winZ != 1.0f) {


            //printf("test \n");
        gluUnProject( winX, winY, winZ, mvmatrix, projmatrix, viewport, &position[0], &position[1],&position[2]);

        mouseObj[0].setProjCentX((float)position[0]);
        mouseObj[0].setProjCentY((float)position[1]);
        mouseObj[1].setProjCentX((float)position[0]);
        mouseObj[1].setProjCentY((float)position[1]);





        }



}




}

//--------------------------------------------------------------
void capture::mousePressed(int x, int y, int button){

x -= camWidth;

        if (y < camHeight && (x < camWidth && x > 0)){
        unsigned char * huePixels = hueImage.getPixels();  //teh hue
        unsigned char * satPixels = satImage.getPixels();  //teh saturation
        unsigned char * briPixels = briImage.getPixels();  //teh brightness*/ //unnecessary really, hue and sat should be enough

        //x = MIN(x,hueImage.width-1);     //find the smallest value out of those two so we don't crash if we click outside of the camera image
        //y = MIN(y,hueImage.height-1);


        if(selectedMenu == 1){

            if(button == 0) {
                cTrackColor[selectedColor].hue = huePixels[x+(y*hueImage.width)];  //set the hue
                cTrackColor[selectedColor].sat = satPixels[x+(y*satImage.width)];  //set the sat
                cTrackColor[selectedColor].bri = briPixels[x+(y*briImage.width)];
            }

        }




    }


if(viewMode == 2 && mouseTest){

    mouseObj[0].setPresent(true);
    mouseObj[1].setPresent(true);


}

}

//--------------------------------------------------------------
void capture::mouseReleased(int x, int y, int button){

if(viewMode == 2 && mouseTest){

mouseObj[0].setPresent(false);
mouseObj[1].setPresent(false);

}
}

//--------------------------------------------------------------
void capture::windowResized(int w, int h){
changed = true;
}

