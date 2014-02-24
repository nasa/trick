#include "dougnode.h"

#ifdef USE_DOUG_H

DougNode::DougNode() :
    _id(-1)
{
}

DougNode::DougNode(const string &node_name)
{
    if( node_name.length() == 0 ) {
        _id = -1;
    } else {
        _id = DSF_GetNodeID( const_cast<char *>(node_name.c_str()) ) ;
    }
}

string DougNode::name() const
{
    if ( isNull() ) {
        return string();
    } else {
        return string(DSV_scene->node_list[_id].name );
    }
}

bool DougNode::isNull() const
{
    if ( _id == -1 ) {
        return true;
    } else {
        return false;
    }
}

void DougNode::global_matrix(DST_MATRIX &mat)
{
    if ( ! isNull() ) {
        DSF_GetGlobalMat(_id, mat );
    } else {
        mat[0][0] = 1.0; mat[0][1] = 0.0; mat[0][2] = 0.0; mat[0][3] = 0.0;
        mat[1][0] = 0.0; mat[1][1] = 1.0; mat[1][2] = 0.0; mat[1][3] = 0.0;
        mat[2][0] = 0.0; mat[2][1] = 0.0; mat[2][2] = 1.0; mat[2][3] = 0.0;
        mat[3][0] = 0.0; mat[3][1] = 0.0; mat[3][2] = 0.0; mat[3][3] = 1.0;
    }
}

void DougNode::xyz(GLfloat (&vec)[3])
{

    if ( isNull() ) {
        vec[0] = 0.0;
        vec[1] = 0.0;
        vec[2] = 0.0;
    } else {
        vec[0] = DSV_scene->node_list[_id].x;
        vec[1] = DSV_scene->node_list[_id].y;
        vec[2] = DSV_scene->node_list[_id].z;
    }
}

void DougNode::pyr(GLfloat (&vec)[3])
{
    if ( isNull() ) {
        vec[0] = 0.0;
        vec[1] = 0.0;
        vec[2] = 0.0;
    } else {
        vec[0] = DSV_scene->node_list[_id].P;
        vec[1] = DSV_scene->node_list[_id].Y;
        vec[2] = DSV_scene->node_list[_id].R;
    }
}

void DougNode::set_xyz(const GLfloat (&vec)[3])
{
    if ( isNull() ) {
        return ;
    }

    DSF_BgnModifyScene( DSV_scene );

    DSV_scene->node_list[_id].x = vec[0];
    DSV_scene->node_list[_id].y = vec[1];
    DSV_scene->node_list[_id].z = vec[2];

    DSF_ModifiedNode( _id, DSD_XYZ_MODIFIED );
    DSF_EndModifyScene( DSV_scene );
}

void DougNode::set_pyr(const GLfloat (&vec)[3])
{
    if ( isNull() ) {
        return ;
    }

    DSF_BgnModifyScene( DSV_scene );

    DSV_scene->node_list[_id].P = vec[0];
    DSV_scene->node_list[_id].Y = vec[1];
    DSV_scene->node_list[_id].R = vec[2];

    DSF_ModifiedNode( _id, DSD_PYR_MODIFIED );
    DSF_EndModifyScene( DSV_scene );
}

//
// C interface for DougNode Class
//
extern "C" {

CDougNode* dougnode_create()
{
    CDougNode* c = NULL;
    try {
        c = new CDougNode();
    } catch (...) {}
    return c;
}

void dougnode_destroy(CDougNode* c)
{
    if ( c == 0 || c == NULL ) return;

    delete c;

    c = 0 ;
}

}

#endif // use doug
