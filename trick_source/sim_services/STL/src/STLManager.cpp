
#include <algorithm>
#include <iostream>
#include "sim_services/STL/include/STLManager.hh"

Trick::STLManager * trick_stl_manager = NULL ;

Trick::STLManager::STLManager() : count(1) {
    trick_stl_manager = this ;
}

Trick::STLManager::~STLManager() {
}

Trick::STLManager * Trick::STLManager::getSTLManager() {
    if ( trick_stl_manager == NULL ) {
        trick_stl_manager = new STLManager ;
    }
    return trick_stl_manager ;
}

void Trick::STLManager::checkpoint() {
    std::vector<STLInterface *>::iterator it  ;
    for ( it = stlObjects.begin() ; it != stlObjects.end() ; it++ ) {
        (*it)->checkpoint(std::string("trick_stl_manager")) ;
    }
}

void Trick::STLManager::post_checkpoint() {
    std::vector<STLInterface *>::iterator it  ;
    for ( it = stlObjects.begin() ; it != stlObjects.end() ; it++ ) {
        (*it)->post_checkpoint(std::string("trick_stl_manager")) ;
    }
}

void Trick::STLManager::restart() {
    std::vector<STLInterface *>::iterator it  ;

    // Mark all of the stls that currently are registered.  These are the ones to restore.
    for ( it = stlObjects.begin() ; it != stlObjects.end() ; it++ ) {
        (*it)->restore = true ;
    }
    // restore the STLs that existed before we called restart.
    // The restore flag will be false for newly created STLs
    for ( it = stlObjects.begin() ; it != stlObjects.end() ; it++ ) {
        if ( (*it)->restore ) {
            (*it)->restart(std::string("trick_stl_manager")) ;
        }

    }

    // Run the post_checkpoint routine to remove MemoryManager allocations.
    for ( it = stlObjects.begin() ; it != stlObjects.end() ; it++ ) {
        (*it)->post_checkpoint(std::string("trick_stl_manager")) ;
    }
}

unsigned int Trick::STLManager::addSTL( STLInterface & stlIn ) {
    //TODO: This needs mutex protection
    stlObjects.push_back(&stlIn) ;
    return count++ ;
}

void Trick::STLManager::removeSTL( STLInterface & stlIn ) {
    //TODO: This needs mutex protection
    std::vector<STLInterface *>::iterator it = find( stlObjects.begin(), stlObjects.end(), &stlIn );
    stlObjects.erase( it );
}
