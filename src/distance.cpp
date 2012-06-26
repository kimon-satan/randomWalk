#include "distance.h"

distance::distance(int tempNPerf, int tempFPerf, float tempDistance)
{
    nPerf = tempNPerf;
    fPerf = tempFPerf;
    dist = tempDistance;
}

int distance::getNperf(){
return nPerf;
}

int distance::getFperf(){
return fPerf;
}

float distance::getDistance(){
return dist;
}

