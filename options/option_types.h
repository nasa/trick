/*
 * This is a hack that I'm too embarrassed to explain
 */

#include "dougbool.h"
#include "dougcolor.h"
#include "doughost.h"
#include "dougnode.h"
#include "dougstring.h"

typedef unsigned int uint;

#if defined FOREACH_TYPE || defined FOREACH_C_TYPE
#ifdef FOREACH_C_TYPE
FOREACH_C_TYPE(double)
FOREACH_C_TYPE(int)
FOREACH_C_TYPE(uint)
FOREACH_C_TYPE(CDougBool)
FOREACH_C_TYPE(CDougString)
#ifdef USE_DOUG_H
FOREACH_C_TYPE(CDougNode)
#endif
FOREACH_C_TYPE(CDougHost)
FOREACH_C_TYPE(CDougColor)
#else
FOREACH_TYPE(int)
FOREACH_TYPE(uint)
FOREACH_TYPE(double)
FOREACH_TYPE(bool)
FOREACH_TYPE(DougBool)
FOREACH_TYPE(CDougBool)
FOREACH_TYPE(DougString)
FOREACH_TYPE(CDougString)
#ifdef USE_DOUG_H
FOREACH_TYPE(DougNode)
FOREACH_TYPE(CDougNode)
#endif
FOREACH_TYPE(DougHost)
FOREACH_TYPE(CDougHost)
FOREACH_TYPE(DougColor)
FOREACH_TYPE(CDougColor)
#endif
#undef FOREACH_TYPE
#undef FOREACH_C_TYPE
#endif
