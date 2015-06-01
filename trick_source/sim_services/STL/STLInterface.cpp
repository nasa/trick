
#include <iostream>

#include "trick/STLInterface.hh"
#include "trick/STLManager.hh"
#include "trick/STLUtilities.hh"

Trick::STLInterface::STLInterface() : restore(false) {
    stl_id = Trick::STLManager::getSTLManager()->addSTL(*this) ;
}

Trick::STLInterface::~STLInterface() {
    Trick::STLManager::getSTLManager()->removeSTL(*this) ;
}

void Trick::STLInterface::post_checkpoint( std::string obj_name ) {
    Trick::delete_trick_stl(obj_name, stl_id) ;
}
