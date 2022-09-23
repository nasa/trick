#ifndef CAMERA_HH
#define CAMERA_HH
class Camera {

public:
    Camera();
    void look();

    double pos[3];
    double tgt[3];
    double up[3];
};
#endif
