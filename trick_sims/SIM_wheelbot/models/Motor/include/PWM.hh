#ifndef PWM_H
#define PWM_H
/********************************* TRICK HEADER *******************************
PURPOSE: ()
LIBRARY DEPENDENCY:
    ((PWM.o))
*******************************************************************************/

#include<stdexcept>

class PWM {

  public:

    double highVoltage;
    double lowVoltage;

    PWM( double HighVoltage,
         double LowVoltage,
         double DutyCycle) throw (std::logic_error);

    ~PWM() {}

    void   setDutyCycle( double DutyCycle) throw (std::logic_error);
    double getDutyCycle() const;
    double getAverageVoltage() const;

  private:

    PWM(){};
    double dutyCycle;
};

#endif
