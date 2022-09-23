exec(open("Modified_data/dr_typesASCII.dr").read())
exec(open("Modified_data/dr_typesBINARY.dr").read())
exec(open("Modified_data/dr_bitfASCII.dr").read())
exec(open("Modified_data/dr_bitfBINARY.dr").read())

trick_utest.unit_tests.enable() ;
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_dr.xml" ) ;


trick.stop(1.0)
