#ifndef DOUGNODE_H
#define DOUGNODE_H

#ifdef USE_DOUG_H

#ifdef __cplusplus

#include <string>
using namespace std;
#include "doug.h"

class DougNode
{
public:
     DougNode();
     DougNode(const string& node_name);

     string name() const;
     bool isNull() const;

     void global_matrix(DST_MATRIX& mat);
     void xyz(GLfloat (&vec)[3]);
     void pyr(GLfloat (&vec)[3]);

     void set_xyz(const GLfloat (&vec)[3]);
     void set_pyr(const GLfloat (&vec)[3]);

  private:
     int _id;
};
#endif


/* C interface for DougNode class */

#ifdef __cplusplus
struct CDougNode : public DougNode {
     CDougNode() : DougNode() {}
     CDougNode(const string& node_name) : DougNode(node_name) {}
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle thing */
struct CDougNode;
typedef struct CDougNode CDougNode;

CDougNode* dougnode_create();
void dougnode_destroy(CDougNode* c);

#ifdef __cplusplus
}
#endif

#endif // use doug

#endif // DOUGNODE_H
