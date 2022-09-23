
#ifndef SCENE_HH
#define SCENE_HH

#include "Camera.hh"
#include "SceneElement.hh"
#include <vector>

typedef struct {
    double pos[3];
    double R[3][3];
} ElementState;

typedef struct {
    ElementState satellite;
//    ElementState satellite2;
} Comm_packet;

#endif
