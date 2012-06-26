#include "objectTracker.h"

bool comparebyDistance (const distListItem a, const distListItem b) {
        return (a.dist < b.dist);
 }

objectTracker::objectTracker(){

    maxDist = 10;
    ghostFrames = 3;

    //intialise finalObjects

    reset();

}



void objectTracker::setGhostFrames(int tempGhost){
ghostFrames = tempGhost;
}

int objectTracker::getGhostFrames(){
return ghostFrames;
}

void objectTracker::setMaxDist(int tempMaxDist){
maxDist = tempMaxDist;
}

int objectTracker::getMaxDist(){
return maxDist;
}


void objectTracker::reset(){

finalObjects.clear();

   int colCha = 0;

    for(int i = 0; i < 2; i ++){

    trackingObject temp;

    temp.setColorNumber(colCha);

    finalObjects.push_back(temp);

    }

}

void objectTracker::feedObjects(vector<trackingObject> nowObjects){



        for(int i = 0; i < finalObjects.size(); i++){

            finalObjects.at(i).setAssigned(false);

        }



//assign for any existing performers


        distanceList.clear();

        for(int i = 0; i < nowObjects.size(); i++){

            for(int j = 0; j < finalObjects.size(); j++){



            if(finalObjects.at(j).getPresent()){

                distListItem *tempItem = new distListItem;

                float distX = nowObjects.at(i).getProjCentX() - finalObjects.at(j).getProjCentX();
                float distY = nowObjects.at(i).getProjCentY() - finalObjects.at(j).getProjCentY();

               tempItem->dist = sqrt((distX * distX) + (distY * distY));

                tempItem->np = i;

                tempItem->fp = j;


                if(tempItem->dist < maxDist) {

                distanceList.push_back(*tempItem);

                }

                delete tempItem;

                }


            }

        }


      if(distanceList.size() > 1){

      sort(distanceList.begin(),distanceList.end(), comparebyDistance);

      }


          for(int i = 0; i < distanceList.size(); i++){

              trackingObject *nowObject;
              trackingObject *finalObject;


              if(nowObjects.at(distanceList.at(i).np).getAssigned() == false &&
                 finalObjects.at(distanceList.at(i).fp).getAssigned() == false
                 ){

                 nowObject = &nowObjects.at(distanceList.at(i).np);
                 finalObject = &finalObjects.at(distanceList.at(i).fp);

                 nowObject->setAssigned(true);
                 finalObject->setAssigned(true);

                    finalObject->ghostReset();

                    finalObject->setProjCentX(nowObject->getProjCentX());
                    finalObject->setProjCentY(nowObject->getProjCentY());
                    finalObject->setScrNearX(nowObject->getScrNearX());
                    finalObject->setScrNearY(nowObject->getScrNearY());
                    finalObject->setScrFarX(nowObject->getScrFarX());
                    finalObject->setScrFarY(nowObject->getScrFarY());
                    finalObject ->updateHistory();

                 if(distanceList.at(i).dist > changeThresh ){

                    finalObject->setChanged(true);


                 }else { finalObject -> setChanged(false);}

                 }

            }



//if nowPerformer still not allocated give empty assignment

    for(int i = 0; i < nowObjects.size(); i++){

            for(int j = 0; j < finalObjects.size(); j++){

                if(!finalObjects.at(j).getAssigned() && !nowObjects.at(i).getAssigned()){




                    nowObjects[i].setAssigned(true);
                    finalObjects.at(j).setAssigned(true);
                    finalObjects.at(j).setPresent(true);
                    finalObjects.at(j).setProjCentX(nowObjects[i].getProjCentX());
                    finalObjects.at(j).setProjCentY(nowObjects[i].getProjCentY());
                    finalObjects.at(j).setScrNearX(nowObjects[i].getScrNearX());
                    finalObjects.at(j).setScrNearY(nowObjects[i].getScrNearY());
                    finalObjects.at(j).setScrFarX(nowObjects[i].getScrFarX());
                    finalObjects.at(j).setScrFarY(nowObjects[i].getScrFarY());
                    finalObjects.at(j).updateHistory();


                }
            }

    }


 //now reset unallocated final performers

for(int j = 0; j < finalObjects.size(); j++){

            if(!finalObjects.at(j).getAssigned() && finalObjects.at(j).getPresent()){



                if(finalObjects.at(j).getGhost() > ghostFrames){
                    finalObjects.at(j).setAssigned(true);
                    finalObjects.at(j).reset();
                }else{
                    finalObjects.at(j).ghostIncrement();
                }

            }
        }

}




vector<trackingObject> objectTracker::getFinals(){

 return finalObjects;

}
