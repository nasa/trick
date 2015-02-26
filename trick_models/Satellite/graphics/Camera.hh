#ifndef _CAMERA_H_
#define _CAMERA_H_
class Camera {

public:
    Camera(); 
    void look();

    double pos[3];
    double tgt[3];
    double up[3];
};
#endif
