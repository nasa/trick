
#include "test_ip/include/ClassOfEverything.hh"

double ClassOfEverything::sdaa[3][3] ;
const double ClassOfEverything::scd = 1.2345 ;
const double ClassOfEverything::csd = 6.7890 ;
const double ClassOfEverything::sdc = 9.8765 ;

ClassOfEverything::ClassOfEverything() : cir(i), icr(i) {

    unsigned int ii ;

    ep = NULL ;
    epp = NULL ;

    for ( ii = 0 ; ii < 4 ; ii++ ) {
        eap[ii] = NULL ;
    }

}

  double & ClassOfEverything::get_d()
  {
    return d;
  }
   double (&ClassOfEverything::get_da())[3]
   {
    return da;
   }
//   double * ClassOfEverything::get_d_flat()
//   {
//     return &d;
//   }
//    double * ClassOfEverything::get_da_flat()
//    {
//     return da;
//    }
   double * ClassOfEverything::get_dp()
   {
    return dp;
   }


    double (&ClassOfEverything::get_daa())[2][3]
    {
        return daa;
    }
//    double * ClassOfEverything::get_daa_flat()
//    {
//     return &daa[0][0];
//    }
   double* (&ClassOfEverything::get_dap())[4]
   {
    return dap;
   }
   double ** ClassOfEverything::get_dap_2d()
   {
     return &dap[0];
   }
   double ** ClassOfEverything::get_dap_2d_pl_2()
   {
    return &dap[2];
   }
   double ** ClassOfEverything::get_dpp()
   {
    return dpp;
   }
   double *** ClassOfEverything::get_dppp()
   {
    return dppp;
   }
   double (&ClassOfEverything::get_daaa())[2][3][4]
   {
    return daaa;
   }
   double (&ClassOfEverything::get_daaaa())[2][3][4][5]
   {
    return daaaa;
   }

     float & ClassOfEverything::get_f()
  {
    return f;
  }
   float * ClassOfEverything::get_fa()
   {
    return fa;
   }
   float * ClassOfEverything::get_fp()
   {
    return fp;
   }
   float * ClassOfEverything::get_faa()
   {
    return &faa[0][0];
   }
   float ** ClassOfEverything::get_fap()
   {
    return fap;
   }
   float ** ClassOfEverything::get_fpp()
   {
    return fpp;
   }
   float *** ClassOfEverything::get_fppp()
   {
    return fppp;
   }
   float * ClassOfEverything::get_faaa()
   {
    return &faaa[0][0][0];
   }

double ClassOfEverything::get_double()
{
   return 4732992000.0;
}
