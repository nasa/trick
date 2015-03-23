#ifndef SCENEELEMENT_HH
#define SCENEELEMENT_HH

class SceneElement {

    public:
    SceneElement(const char* model_name);

    void init();
    void display();

    double pos[3];
    double vel[3];
    double R[4][4];

    private:
    void * dl_handle ;
    void (*model_display_func)(void);
    void (*model_init_func)(void);
    SceneElement();
};
#endif
