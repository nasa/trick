/*
PURPOSE:
    ( Job Class Identifiers )
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
    (((John Penn)(L3)   (2010)))
*/

#ifndef INTEGJOBCLASSID_HH
#define INTEGJOBCLASSID_HH

namespace Trick {
    enum {
        DerivativeJobClassId      = 2000,
        IntegrationJobClassId     = 2001,
        DynamicEventJobClassId    = 2002,
        PostIntegrationJobClassId = 2003,
        PreIntegrationJobClassId  = 2004
    };
}
#endif
