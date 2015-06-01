
#ifndef STL_MANAGER_HH
#define STL_MANAGER_HH

#include <vector>
#include "trick/STLInterface.hh"

namespace Trick {

class STLManager {
    public:
        STLManager() ;
        virtual ~STLManager() ;

        static STLManager * getSTLManager() ;

        void checkpoint() ;
        void post_checkpoint() ;
        void restart() ;

        unsigned int addSTL( STLInterface & stlIn ) ;
        void removeSTL( STLInterface & stlIn ) ;

    private:
        //TODO: This needs to be a set, not a vector
        std::vector<STLInterface *> stlObjects ;
        unsigned int count ;

};

} ;

#endif
