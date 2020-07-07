#include "Control/include/PIDController.hh"

PIDController::PIDController(double kp, double ki, double kd, double omax, double omin, double dt, double tc) {
  Kprop = kp;
  Kinteg = ki;
  Kderv = kd;
  Dt = dt;
  k = dt/tc;
  error = 0.0;
  integral = 0;
  out_max = omax;
  out_min = omin;
  previous_error = 0.0;
  prev_setpoint_value = 0.0;
  integration_enabled = true;
}

double PIDController::getOutput(double setpoint_value, double measured_value) {

  double error_unfiltered = setpoint_value - measured_value;

  // Low Pass Filter
  error = error + (k * (error_unfiltered - error));

  // Check for drastic changes
  if (prev_setpoint_value != setpoint_value) {
    previous_error = error;
  }

  // integration
  if (integration_enabled == true) {
    integral = integral + (error * Dt);
  }

  // Update derivative (rate of change of error)
  double derivative = ((error - previous_error) / Dt);

  double proportionalterm = Kprop * error;
  double integralterm = Kinteg * integral;
  double derivateterm = Kderv * derivative;
  double output = proportionalterm + integralterm + derivateterm;

  // Clamping Actuator

  // Sign Check
  bool same_sign;
  if ((error * output) > 0.0) {
    same_sign = 1;
  } else {
    same_sign = 0;
  }

  //Saturation Check
  bool output_limited;
  if (output > out_max) {
    output = out_max;
    output_limited = 1;
  }
  if (output < out_min) {
    output = out_min;
    output_limited = 1;
  }

  //AND Gate Check
  if (output_limited && same_sign) {
    integration_enabled = false;
  } else {
    integration_enabled = true;
  }

  // Prepare for next calcuation
  previous_error = error;
  prev_setpoint_value = setpoint_value;

  return (output);

}
