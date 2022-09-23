#include <iostream>
#include "trick/trick_math.h"
#include "trick/reference_frame.h"
using namespace std;

class EulerQuatTest {

   public:

      TRANSFORM init_data;
      bool pass;
      int num_tests;
      int num_pass;
      int method;
      double quat[4];
      double mat[3][3];

      EulerQuatTest() {
          this->num_tests = 1080;
          this->num_pass = 0;
          this->pass = false;
      }
      ~EulerQuatTest() {}

      void compare_euler(double eul[3]){
          double eul_error[3];
          double mat_test[3][3];
          double mat_error[3][3];

          M_IDENT(mat_error);
          M_IDENT(mat_test);
          V_INIT(eul_error);
          euler_matrix(eul, mat_test, 0 , this->init_data.euler_sequence);
          MxMt(mat_error, mat_test, this->mat);
          euler_matrix(eul_error, mat_error, 0 , this->init_data.euler_sequence);
          if(V_MAG(eul_error) < 0.00001){
             pass = true;
          } else{
             pass = false;
             printf("EulerAngle = %lf  %lf  %lf\n",this->init_data.euler_angles[0],
                                                   this->init_data.euler_angles[1],
                                                   this->init_data.euler_angles[2]);
             this->num_tests = 0;
          }
          return;
      }
      void compare_quat(double Q[4]){
          double Q_error[4];
          double mat_error[3][3];
          double eul_error[3];

          M_IDENT(mat_error);
          V_INIT(eul_error);
          QxQt(Q_error, Q, quat);
          quat_to_mat(mat_error, Q_error);
          euler_matrix(eul_error, mat_error, 0 , this->init_data.euler_sequence);
          if(V_MAG(eul_error) < 0.00001){
             this->pass = true;
          } else{
             this->pass = false;
          }
          return;
      }
      void test_euler_to_quat(){
          double Q[4];
          euler_quat(this->init_data.euler_angles, Q, 0,  this->init_data.euler_sequence);
          compare_quat( Q );
          return;
      }
      void test_quat_to_euler(int test){
          double eul[3];
          this->method = test;
          if(this->method == 1){
              euler_quat(eul, this->quat, this->method, this->init_data.euler_sequence);
          }
          else {
             switch (this->init_data.euler_sequence) {
                 case Roll_Pitch_Yaw:
                     euler123_quat( eul, this->quat, 2, this->init_data.euler_angles );
                     break;
                 case Roll_Yaw_Pitch:
                     euler132_quat( eul, this->quat, 2, this->init_data.euler_angles );
                     break;
                 case Pitch_Yaw_Roll:
                     euler231_quat( eul, this->quat, 2, this->init_data.euler_angles );
                     break;
                 case Pitch_Roll_Yaw:
                     euler213_quat( eul, this->quat, 2, this->init_data.euler_angles );
                     break;
                 case Yaw_Roll_Pitch:
                     euler312_quat( eul, this->quat, 2, this->init_data.euler_angles );
                     break;
                 case Yaw_Pitch_Roll:
                     euler321_quat( eul, this->quat, 2, this->init_data.euler_angles );
                     break;
             }
          }
          compare_euler(eul);
          return;
      }
};
