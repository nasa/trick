#!/usr/bin/python

import commands
import subprocess

def runSim(case):
  print("Running %s case"%case)
  ( stat, output ) = commands.getstatusoutput( "./S_main_Linux_4.8_x86_64.exe %s/input.py "%case )
  if( stat == 0 ):
    print "Run succeeded"
  else:
    print "Run failed"

def compareRange(case, col, low, high):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_range.py', case, col, low, high], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

def compareRangeOnRow(case, row, col, low, high):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_range_on_row.py', case, row, col, low, high], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

def compareSD(case, col, mean, sd):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_for_sd.py', case, col, mean, sd], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

def compareDist(case, col, low, high):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_distrubition.py', case, col, low, high], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

def compareValue(case, col, val):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_value.py', case, col, val], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

def compareValueAtRow(case, col, row, val):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_row_for_value.py', case, col, row, val], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

def compareBool(case, col):
  proc = subprocess.Popen(['python', 'validation/read_csv_and_check_boolean.py', case, col], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  print proc.communicate()[0]

# run the sims...
runSim("RUN_ERROR_file_inconsistent_skip")
runSim("RUN_ERROR_invalid_call")
runSim("RUN_ERROR_invalid_name") # produces no output variables
runSim("RUN_ERROR_invalid_sequence")
runSim("RUN_ERROR_out_of_domain_error")
runSim("RUN_ERROR_random_value_truncation")
runSim("RUN_file_sequential")
runSim("RUN_file_skip")
runSim("RUN_file_skip2")
runSim("RUN_generate_meta_data_early")
runSim("RUN_nominal")
runSim("RUN_random_normal_truncate_abs")
runSim("RUN_random_normal_truncate_rel")
runSim("RUN_random_normal_truncate_sd")
runSim("RUN_random_normal_untruncated")
runSim("RUN_random_normal__untruncate")
runSim("RUN_random_uniform")
runSim("RUN_remove_variable")
runSim("RUN_WARN_config_error")
runSim("RUN_WARN_invalid_name")
runSim("RUN_WARN_overconstrained_config")


print("")

# run the compares...
# check these case via range
compareValueAtRow('RUN_ERROR_file_inconsistent_skip', '1', '0', '2')
compareValueAtRow('RUN_ERROR_file_inconsistent_skip', '2', '0', '1')
compareValueAtRow('RUN_ERROR_file_inconsistent_skip', '3', '0', '0')

compareValue('RUN_ERROR_invalid_call', '1', '2')

compareRange('RUN_ERROR_invalid_sequence', '1', '10', '20')

compareSD('RUN_ERROR_out_of_domain_error', '1', '0', '1.0')

compareSD('RUN_ERROR_random_value_truncation', '1', '10', '3.5')
compareSD('RUN_ERROR_random_value_truncation', '2', '10', '3.5')
compareRange('RUN_ERROR_random_value_truncation', '3', '-inf', '10')

compareRangeOnRow('RUN_file_sequential', '0', '3', '0', '2')
compareRangeOnRow('RUN_file_sequential', '1', '3', '10', '12')
compareRangeOnRow('RUN_file_sequential', '2', '3', '20', '22')
compareRangeOnRow('RUN_file_sequential', '3', '3', '30', '32')
compareRangeOnRow('RUN_file_sequential', '4', '3', '0', '2')
compareRangeOnRow('RUN_file_sequential', '5', '3', '10', '12')
compareRangeOnRow('RUN_file_sequential', '6', '3', '20', '22')
compareRangeOnRow('RUN_file_sequential', '7', '3', '30', '32')
compareRangeOnRow('RUN_file_sequential', '8', '3', '0', '2')
compareRangeOnRow('RUN_file_sequential', '9', '3', '10', '12')

# this one skips randomly (by design); no way to predict the contents
# of any line. so check for the whole range in each column.
compareRange('RUN_file_skip', '1',  '2.0', '32.0')
compareRange('RUN_file_skip', '2',  '1.0', '31.0')
compareRange('RUN_file_skip', '3',  '0.0', '30.0')

compareRange('RUN_file_skip2', '1',  '1.0', '15.0')
compareRange('RUN_file_skip2', '2', '16.0', '30.0')
compareRange('RUN_file_skip2', '3', '31.0', '55.0')

compareDist('RUN_generate_meta_data_early', '1', '10', '20')
compareSD('RUN_generate_meta_data_early', '2', '10', '2.0')
compareRange('RUN_generate_meta_data_early', '3',  '9.0', '11.0')
compareRange('RUN_generate_meta_data_early', '4',  '9.5', '10.7')
compareRange('RUN_generate_meta_data_early', '5',  '9.9', '11.0')
compareRange('RUN_generate_meta_data_early', '6',  '9.9', 'inf')
compareRange('RUN_generate_meta_data_early', '7',  '-inf', '4')
compareSD('RUN_generate_meta_data_early', '8', '10', '2.0')
compareRange('RUN_generate_meta_data_early', '9',  '0', '2')
compareBool('RUN_generate_meta_data_early', '10')
compareValueAtRow('RUN_generate_meta_data_early', '11', '0', '2')
compareValueAtRow('RUN_generate_meta_data_early', '12', '0', '1')
compareValueAtRow('RUN_generate_meta_data_early', '13', '0', '0')
compareValue('RUN_generate_meta_data_early', '14', '7')
compareValue('RUN_generate_meta_data_early', '15', '7')
compareValue('RUN_generate_meta_data_early', '16', '2')

compareDist('RUN_nominal', '1', '10', '20')
compareSD('RUN_nominal', '2', '10', '2.0')
compareRange('RUN_nominal', '3',  '9.0', '11.0')
compareRange('RUN_nominal', '4',  '9.5', '10.7')
compareRange('RUN_nominal', '5',  '9.9', '11.0')
compareRange('RUN_nominal', '6',  '9.9', 'inf')
compareRange('RUN_nominal', '7',  '-inf', '4')
compareSD('RUN_nominal', '8', '10', '2.0')
compareRange('RUN_nominal', '9',  '0', '2')
compareBool('RUN_nominal', '10')
compareValueAtRow('RUN_nominal', '11', '0', '2')
compareValueAtRow('RUN_nominal', '11', '1', '12')
compareValueAtRow('RUN_nominal', '12', '0', '1')
compareValueAtRow('RUN_nominal', '12', '1', '11')
compareValueAtRow('RUN_nominal', '13', '0', '0')
compareValueAtRow('RUN_nominal', '13', '1', '10')
compareValue('RUN_nominal', '14', '7')
compareValue('RUN_nominal', '15', '7')
compareValue('RUN_nominal', '16', '2')

compareRange('RUN_random_normal_truncate_abs', '1', '-10.0', '10.0')
compareRange('RUN_random_normal_truncate_abs', '2',  '72.5', '85.0')
compareRange('RUN_random_normal_truncate_abs', '3',  '90.0', 'inf')
compareRange('RUN_random_normal_truncate_abs', '4', '-inf', '135.0')

compareRange('RUN_random_normal_truncate_rel', '1', '-10.0', '10.0')
compareRange('RUN_random_normal_truncate_rel', '2',  '72.5', '85.0')
compareRange('RUN_random_normal_truncate_rel', '3',  '90.0', 'inf')
compareRange('RUN_random_normal_truncate_rel', '4', '-inf', '135.0')

compareRange('RUN_random_normal_truncate_sd', '1', '-10.0', '10.0')
compareRange('RUN_random_normal_truncate_sd', '2',  '72.5', '85.0')
compareRange('RUN_random_normal_truncate_sd', '3',  '90.0', 'inf')
compareRange('RUN_random_normal_truncate_sd', '4', '-inf', '135.0')

# check this case via standard_distribution
compareSD('RUN_random_normal_untruncated', '1', '0', '5.0')
compareSD('RUN_random_normal_untruncated', '2', '75', '5.0')
compareSD('RUN_random_normal_untruncated', '3', '100.0', '5.0')
compareSD('RUN_random_normal_untruncated', '4', '125.0', '5.0')

compareSD('RUN_random_normal__untruncate', '1', '10.0', '2.0')
compareSD('RUN_random_normal__untruncate', '2', '10.0', '2.0')

compareDist('RUN_random_uniform', '1', '100.0', '100000.0')
compareDist('RUN_random_uniform', '2', '100', '100000')

compareRange('RUN_remove_variable/RUN_both_variables', '1', '10', '20')
compareValue('RUN_remove_variable/RUN_both_variables', '2', '7')

compareRange('RUN_remove_variable/RUN_one_variable', '1', '10', '20')

compareRange('RUN_WARN_config_error', '1', '7.0', '13.0')

compareRange('RUN_WARN_invalid_name', '1', '10', '20')

compareValue('RUN_WARN_overconstrained_config', '1', '14')

