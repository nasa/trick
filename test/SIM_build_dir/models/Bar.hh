// @trick_parse{everything}

#include "trick/Event.hh"

class Bar : public Trick::Event {

    int  process(long long) {return 0;}
    void add() {}
    void remove() {}
    void restart() {}

};
