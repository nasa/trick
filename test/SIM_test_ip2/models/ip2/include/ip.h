 /*********************************** TRICK HEADER **************************
PURPOSE:                     (Test ip)
REFERENCES:                  (None)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
PROGRAMMERS:                 ((lin) (Thu Feb 27 14:57:16 CST 2003))
***************************************************************************/

#ifndef IP_H
#define IP_H

#include "sim_services/include/Flag.h"

typedef struct {

    char c ;                        /**< -- Test integer */
    char c2 ;                       /**< -- Test integer */
    char c3 ;                       /**< -- Test integer */

    char ca[6] ;                    /**< -- Test integer */
    char ca2[6] ;                   /**< -- Test integer */
    char ca3[6] ;                   /**< -- Test integer */

    char caa[8][8] ;                /**< -- Test integer */
    char caa2[8][8] ;               /**< -- Test integer */
    char caa3[8][8] ;               /**< -- Test integer */

    char *cp ;                      /**< -- Test integer */
    char *cp2 ;                     /**< -- Test integer */
    char *cp3 ;                     /**< -- Test integer */

    char **cpp ;                    /**< -- Test integer */
    char **cpp2 ;                   /**< -- Test integer */
    char **cpp3 ;                   /**< -- Test integer */

    char ***cppp ;                  /**< -- Test integer */
    char ***cppp2 ;                 /**< -- Test integer */
    char ***cppp3 ;                 /**< -- Test integer */

} CHAR_STR ;

typedef struct {

    unsigned char uc ;              /**< -- Test integer */
    unsigned char uc2 ;             /**< -- Test integer */
    unsigned char uc3 ;             /**< -- Test integer */

    unsigned char uca[6] ;          /**< -- Test integer */
    unsigned char uca2[6] ;         /**< -- Test integer */
    unsigned char uca3[6] ;         /**< -- Test integer */

    unsigned char ucaa[8][8] ;      /**< -- Test integer */
    unsigned char ucaa2[8][8] ;     /**< -- Test integer */
    unsigned char ucaa3[8][8] ;     /**< -- Test integer */

    unsigned char *ucp ;            /**< -- Test integer */
    unsigned char *ucp2 ;           /**< -- Test integer */
    unsigned char *ucp3 ;           /**< -- Test integer */

    unsigned char **ucpp ;          /**< -- Test integer */
    unsigned char **ucpp2 ;         /**< -- Test integer */
    unsigned char **ucpp3 ;         /**< -- Test integer */

    unsigned char ***ucppp ;        /**< -- Test integer */
    unsigned char ***ucppp2 ;       /**< -- Test integer */
    unsigned char ***ucppp3 ;       /**< -- Test integer */

} UCHAR_STR ;

typedef struct {

    short s ;                       /**< -- Test integer */
    short s2 ;                      /**< -- Test integer */
    short s3 ;                      /**< -- Test integer */

    short sa[6] ;                   /**< -- Test integer */
    short sa2[6] ;                  /**< -- Test integer */
    short sa3[6] ;                  /**< -- Test integer */

    short saa[8][8] ;               /**< -- Test integer */
    short saa2[8][8] ;              /**< -- Test integer */
    short saa3[8][8] ;              /**< -- Test integer */

    short *sp ;                     /**< -- Test integer */
    short *sp2 ;                    /**< -- Test integer */
    short *sp3 ;                    /**< -- Test integer */

    short **spp ;                   /**< -- Test integer */
    short **spp2 ;                  /**< -- Test integer */
    short **spp3 ;                  /**< -- Test integer */

    short ***sppp ;                 /**< -- Test integer */
    short ***sppp2 ;                /**< -- Test integer */
    short ***sppp3 ;                /**< -- Test integer */

} SHORT_STR ;

typedef struct {

    unsigned short us ;             /**< -- Test integer */
    unsigned short us2 ;            /**< -- Test integer */
    unsigned short us3 ;            /**< -- Test integer */

    unsigned short usa[6] ;         /**< -- Test integer */
    unsigned short usa2[6] ;        /**< -- Test integer */
    unsigned short usa3[6] ;        /**< -- Test integer */

    unsigned short usaa[8][8] ;     /**< -- Test integer */
    unsigned short usaa2[8][8] ;    /**< -- Test integer */
    unsigned short usaa3[8][8] ;    /**< -- Test integer */

    unsigned short *usp ;           /**< -- Test integer */
    unsigned short *usp2 ;          /**< -- Test integer */
    unsigned short *usp3 ;          /**< -- Test integer */

    unsigned short **uspp ;         /**< -- Test integer */
    unsigned short **uspp2 ;        /**< -- Test integer */
    unsigned short **uspp3 ;        /**< -- Test integer */

    unsigned short ***usppp ;       /**< -- Test integer */
    unsigned short ***usppp2 ;      /**< -- Test integer */
    unsigned short ***usppp3 ;      /**< -- Test integer */

} USHORT_STR ;

typedef struct {

    int i ;                         /**< -- Test integer */
    int i2 ;                        /**< -- Test integer */
    int i3 ;                        /**< -- Test integer */

    int ia[6] ;                     /**< -- Test integer */
    int ia2[6] ;                    /**< -- Test integer */
    int ia3[6] ;                    /**< -- Test integer */

    int iaa[8][8] ;                 /**< -- Test integer */
    int iaa2[8][8] ;                /**< -- Test integer */
    int iaa3[8][8] ;                /**< -- Test integer */

    int *ip ;                       /**< -- Test integer */
    int *ip2 ;                      /**< -- Test integer */
    int *ip3 ;                      /**< -- Test integer */

    int **ipp ;                     /**< -- Test integer */
    int **ipp2 ;                    /**< -- Test integer */
    int **ipp3 ;                    /**< -- Test integer */

    int ***ippp ;                   /**< -- Test integer */
    int ***ippp2 ;                  /**< -- Test integer */
    int ***ippp3 ;                  /**< -- Test integer */

} INT_STR ;

typedef struct {

    unsigned int ui ;               /**< -- Test integer */
    unsigned int ui2 ;              /**< -- Test integer */
    unsigned int ui3 ;              /**< -- Test integer */

    unsigned int uia[6] ;           /**< -- Test integer */
    unsigned int uia2[6] ;          /**< -- Test integer */
    unsigned int uia3[6] ;          /**< -- Test integer */

    unsigned int uiaa[8][8] ;       /**< -- Test integer */
    unsigned int uiaa2[8][8] ;      /**< -- Test integer */
    unsigned int uiaa3[8][8] ;      /**< -- Test integer */

    unsigned int *uip ;             /**< -- Test integer */
    unsigned int *uip2 ;            /**< -- Test integer */
    unsigned int *uip3 ;            /**< -- Test integer */

    unsigned int **uipp ;           /**< -- Test integer */
    unsigned int **uipp2 ;          /**< -- Test integer */
    unsigned int **uipp3 ;          /**< -- Test integer */

    unsigned int ***uippp ;         /**< -- Test integer */
    unsigned int ***uippp2 ;        /**< -- Test integer */
    unsigned int ***uippp3 ;        /**< -- Test integer */

} UINT_STR ;

typedef struct {

    long l ;                        /**< -- Test long */
    long l2 ;                       /**< -- Test long */
    long l3 ;                       /**< -- Test long */

    long la[6] ;                    /**< -- Test long */
    long la2[6] ;                   /**< -- Test long */
    long la3[6] ;                   /**< -- Test long */

    long laa[8][8] ;                /**< -- Test long */
    long laa2[8][8] ;               /**< -- Test long */
    long laa3[8][8] ;               /**< -- Test long */

    long *lp ;                      /**< -- Test long */
    long *lp2 ;                     /**< -- Test long */
    long *lp3 ;                     /**< -- Test long */

    long **lpp ;                    /**< -- Test long */
    long **lpp2 ;                   /**< -- Test long */
    long **lpp3 ;                   /**< -- Test long */

    long ***lppp ;                  /**< -- Test long */
    long ***lppp2 ;                 /**< -- Test long */
    long ***lppp3 ;                 /**< -- Test long */

} LONG_STR ;

typedef struct {

    unsigned long ul ;              /**< -- Test long */
    unsigned long ul2 ;             /**< -- Test long */
    unsigned long ul3 ;             /**< -- Test long */

    unsigned long ula[6] ;          /**< -- Test long */
    unsigned long ula2[6] ;         /**< -- Test long */
    unsigned long ula3[6] ;         /**< -- Test long */

    unsigned long ulaa[8][8] ;      /**< -- Test long */
    unsigned long ulaa2[8][8] ;     /**< -- Test long */
    unsigned long ulaa3[8][8] ;     /**< -- Test long */

    unsigned long *ulp ;            /**< -- Test long */
    unsigned long *ulp2 ;           /**< -- Test long */
    unsigned long *ulp3 ;           /**< -- Test long */

    unsigned long **ulpp ;          /**< -- Test long */
    unsigned long **ulpp2 ;         /**< -- Test long */
    unsigned long **ulpp3 ;         /**< -- Test long */

    unsigned long ***ulppp ;        /**< -- Test long */
    unsigned long ***ulppp2 ;       /**< -- Test long */
    unsigned long ***ulppp3 ;       /**< -- Test long */

} ULONG_STR ;

typedef struct {

    long long ll ;                  /**< -- Test long */
    long long ll2 ;                 /**< -- Test long */
    long long ll3 ;                 /**< -- Test long */

    long long lla[6] ;              /**< -- Test long */
    long long lla2[6] ;             /**< -- Test long */
    long long lla3[6] ;             /**< -- Test long */

    long long llaa[8][8] ;          /**< -- Test long */
    long long llaa2[8][8] ;         /**< -- Test long */
    long long llaa3[8][8] ;         /**< -- Test long */

    long long *llp ;                /**< -- Test long */
    long long *llp2 ;               /**< -- Test long */
    long long *llp3 ;               /**< -- Test long */

    long long **llpp ;              /**< -- Test long */
    long long **llpp2 ;             /**< -- Test long */
    long long **llpp3 ;             /**< -- Test long */

    long long ***llppp ;            /**< -- Test long */
    long long ***llppp2 ;           /**< -- Test long */
    long long ***llppp3 ;           /**< -- Test long */

} LONG_LONG_STR ;

typedef struct {

    unsigned long long ull ;        /**< -- Test long */
    unsigned long long ull2 ;       /**< -- Test long */
    unsigned long long ull3 ;       /**< -- Test long */

    unsigned long long ulla[6] ;    /**< -- Test long */
    unsigned long long ulla2[6] ;   /**< -- Test long */
    unsigned long long ulla3[6] ;   /**< -- Test long */

    unsigned long long ullaa[8][8] ;/**< -- Test long */
    unsigned long long ullaa2[8][8];/**< -- Test long */
    unsigned long long ullaa3[8][8];/**< -- Test long */

    unsigned long long *ullp ;      /**< -- Test long */
    unsigned long long *ullp2 ;     /**< -- Test long */
    unsigned long long *ullp3 ;     /**< -- Test long */

    unsigned long long **ullpp ;    /**< -- Test long */
    unsigned long long **ullpp2 ;   /**< -- Test long */
    unsigned long long **ullpp3 ;   /**< -- Test long */

    unsigned long long ***ullppp ;  /**< -- Test long */
    unsigned long long ***ullppp2 ; /**< -- Test long */
    unsigned long long ***ullppp3 ; /**< -- Test long */

} ULONG_LONG_STR ;

typedef struct {

    float f ;                       /**< -- Test integer */
    float f2 ;                      /**< -- Test integer */
    float f3 ;                      /**< -- Test integer */

    float fa[6] ;                   /**< -- Test integer */
    float fa2[6] ;                  /**< -- Test integer */
    float fa3[6] ;                  /**< -- Test integer */

    float faa[8][8] ;               /**< -- Test integer */
    float faa2[8][8] ;              /**< -- Test integer */
    float faa3[8][8] ;              /**< -- Test integer */

    float *fp ;                     /**< -- Test integer */
    float *fp2 ;                    /**< -- Test integer */
    float *fp3 ;                    /**< -- Test integer */

    float **fpp ;                   /**< -- Test integer */
    float **fpp2 ;                  /**< -- Test integer */
    float **fpp3 ;                  /**< -- Test integer */

    float ***fppp ;                 /**< -- Test integer */
    float ***fppp2 ;                /**< -- Test integer */
    float ***fppp3 ;                /**< -- Test integer */

} FLOAT_STR ;

typedef struct {

    double d ;                      /**< -- Test integer */
    double d2 ;                     /**< -- Test integer */
    double d3 ;                     /**< -- Test integer */

    double da[6] ;                  /**< -- Test integer */
    double da2[6] ;                 /**< -- Test integer */
    double da3[6] ;                 /**< -- Test integer */

    double daa[8][8] ;              /**< -- Test integer */
    double daa2[8][8] ;             /**< -- Test integer */
    double daa3[8][8] ;             /**< -- Test integer */

    double *dp ;                    /**< -- Test integer */
    double *dp2 ;                   /**< -- Test integer */
    double *dp3 ;                   /**< -- Test integer */

    double **dpp ;                  /**< -- Test integer */
    double **dpp2 ;                 /**< -- Test integer */
    double **dpp3 ;                 /**< -- Test integer */

    double ***dppp ;                /**< -- Test integer */
    double ***dppp2 ;               /**< -- Test integer */
    double ***dppp3 ;               /**< -- Test integer */

} DOUBLE_STR ;

typedef struct {

    CHAR_STR cs ;
    CHAR_STR csa[6] ;
    CHAR_STR csaa[6][6] ;
    CHAR_STR *csp ;
    CHAR_STR *csp1 ;
    CHAR_STR *csp2 ;
    CHAR_STR **cspp ;
    CHAR_STR **cspp1 ;
    CHAR_STR **cspp2 ;
    CHAR_STR ***csppp ;
    CHAR_STR ***csppp1 ;
    CHAR_STR ***csppp2 ;

    UCHAR_STR ucs ;
    UCHAR_STR ucsa[6] ;
    UCHAR_STR ucsaa[6][6] ;
    UCHAR_STR *ucsp ;
    UCHAR_STR *ucsp1 ;
    UCHAR_STR *ucsp2 ;
    UCHAR_STR **ucspp ;
    UCHAR_STR **ucspp1 ;
    UCHAR_STR **ucspp2 ;
    UCHAR_STR ***ucsppp ;
    UCHAR_STR ***ucsppp1 ;
    UCHAR_STR ***ucsppp2 ;

    SHORT_STR ss ;
    SHORT_STR ssa[6] ;
    SHORT_STR ssaa[6][6] ;
    SHORT_STR *ssp ;
    SHORT_STR *ssp1 ;
    SHORT_STR *ssp2 ;
    SHORT_STR **sspp ;
    SHORT_STR **sspp1 ;
    SHORT_STR **sspp2 ;
    SHORT_STR ***ssppp ;
    SHORT_STR ***ssppp1 ;

    USHORT_STR uss ;
    USHORT_STR ussa[6] ;
    USHORT_STR ussaa[6][6] ;
    USHORT_STR *ussp ;
    USHORT_STR *ussp1 ;
    USHORT_STR *ussp2 ;
    USHORT_STR **usspp ;
    USHORT_STR **usspp1 ;
    USHORT_STR **usspp2 ;
    USHORT_STR ***ussppp ;
    USHORT_STR ***ussppp1 ;

    INT_STR _is ;
    INT_STR isa[6] ;
    INT_STR isaa[6][6] ;
    INT_STR *isp ;
    INT_STR *isp1 ;
    INT_STR *isp2 ;
    INT_STR **ispp ;
    INT_STR **ispp1 ;
    INT_STR **ispp2 ;
    INT_STR ***isppp ;
    INT_STR ***isppp1 ;
    INT_STR ***isppp2 ;

    UINT_STR uis ;
    UINT_STR uisa[6] ;
    UINT_STR uisaa[6][6] ;
    UINT_STR *uisp ;
    UINT_STR *uisp1 ;
    UINT_STR *uisp2 ;
    UINT_STR **uispp ;
    UINT_STR **uispp1 ;
    UINT_STR **uispp2 ;
    UINT_STR ***uisppp ;
    UINT_STR ***uisppp1 ;
    UINT_STR ***uisppp2 ;

    LONG_STR ls ;
    LONG_STR lsa[6] ;
    LONG_STR lsaa[6][6] ;
    LONG_STR *lsp ;
    LONG_STR *lsp1 ;
    LONG_STR *lsp2 ;
    LONG_STR **lspp ;
    LONG_STR **lspp1 ;
    LONG_STR **lspp2 ;
    LONG_STR ***lsppp ;
    LONG_STR ***lsppp1 ;
    LONG_STR ***lsppp2 ;

    ULONG_STR uls ;
    ULONG_STR ulsa[6] ;
    ULONG_STR ulsaa[6][6] ;
    ULONG_STR *ulsp ;
    ULONG_STR *ulsp1 ;
    ULONG_STR *ulsp2 ;
    ULONG_STR **ulspp ;
    ULONG_STR **ulspp1 ;
    ULONG_STR **ulspp2 ;
    ULONG_STR ***ulsppp ;
    ULONG_STR ***ulsppp1 ;
    ULONG_STR ***ulsppp2 ;

    LONG_LONG_STR lls ;
    LONG_LONG_STR llsa[6] ;
    LONG_LONG_STR llsaa[6][6] ;
    LONG_LONG_STR *llsp ;
    LONG_LONG_STR *llsp1 ;
    LONG_LONG_STR *llsp2 ;
    LONG_LONG_STR **llspp ;
    LONG_LONG_STR **llspp1 ;
    LONG_LONG_STR **llspp2 ;
    LONG_LONG_STR ***llsppp ;
    LONG_LONG_STR ***llsppp1 ;
    LONG_LONG_STR ***llsppp2 ;

    ULONG_LONG_STR ulls ;
    ULONG_LONG_STR ullsa[6] ;
    ULONG_LONG_STR ullsaa[6][6] ;
    ULONG_LONG_STR *ullsp ;
    ULONG_LONG_STR *ullsp1 ;
    ULONG_LONG_STR *ullsp2 ;
    ULONG_LONG_STR **ullspp ;
    ULONG_LONG_STR **ullspp1 ;
    ULONG_LONG_STR **ullspp2 ;
    ULONG_LONG_STR ***ullsppp ;
    ULONG_LONG_STR ***ullsppp1 ;
    ULONG_LONG_STR ***ullsppp2 ;

    FLOAT_STR fs ;
    FLOAT_STR fsa[6] ;
    FLOAT_STR fsaa[6][6] ;
    FLOAT_STR *fsp ;
    FLOAT_STR *fsp1 ;
    FLOAT_STR *fsp2 ;
    FLOAT_STR **fspp ;
    FLOAT_STR **fspp1 ;
    FLOAT_STR **fspp2 ;
    FLOAT_STR ***fsppp ;
    FLOAT_STR ***fsppp1 ;
    FLOAT_STR ***fsppp2 ;

    DOUBLE_STR ds ;
    DOUBLE_STR dsa[6] ;
    DOUBLE_STR dsaa[6][6] ;
    DOUBLE_STR *dsp ;
    DOUBLE_STR *dsp1 ;
    DOUBLE_STR *dsp2 ;
    DOUBLE_STR **dspp ;
    DOUBLE_STR **dspp1 ;
    DOUBLE_STR **dspp2 ;
    DOUBLE_STR ***dsppp ;
    DOUBLE_STR ***dsppp1 ;
    DOUBLE_STR ***dsppp2 ;

    Flag fl ;
    Flag fla[10] ;
    Flag * flp ;
} EVERYTHING ;

typedef struct LinkedList {

    int i ;
    struct LinkedList * next ;
    struct LinkedList * prev ;

} LINKED_LIST ;

#endif
