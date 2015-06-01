
/*
  PURPOSE:                     (Monte carlo structures)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Keith Vetter) (LinCom) (7/2003))
*/

#ifndef REMOTESHELL_HH
#define REMOTESHELL_HH

namespace Trick {

    enum RemoteShell {
        TRICK_SSH,              /* Secure encrypted shell(Default) */
        TRICK_RSH,              /* Insecure rlogin shell */
        TRICK_USER_REMOTE_SH    /* user defined remote shell */
    } ;

} ;

//void write_monte_output(MonteWork *MW);

#endif
