from trick.unit_test import *

trick_utest.unit_tests.enable() 
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_dr.xml" ) 

trick_utest.unit_tests.set_test_name( "DRTest" )

has_dhf5 = False
if hasattr(trick, 'DRHDF5'):
    has_dhf5 = True

######################################################################################################################

test_suite = "drg api"

# Get the number of data recording groups before any drg is created
num_drgs = trick.get_num_data_record_groups()

# Check the result of trick.get_num_data_record_groups()
TRICK_EXPECT_EQ( num_drgs , 0 , test_suite , "0 drgs before any created" )

# The first item of each pair is the .dr file name and the second item of each pair is the drg name
if has_dhf5:
    dr_file_name_drg_name_tuple = (
                                   ('Modified_data/dr_typesASCII.dr', 'DR_typesASCII'), 
                                   ('Modified_data/dr_typesBINARY.dr', 'DR_typesBINARY'), 
                                   ('Modified_data/dr_typesHDF5.dr', 'DR_typesHDF5'),
                                   ('Modified_data/dr_bitfASCII.dr', 'DR_bitfieldsASCII'), 
                                   ('Modified_data/dr_bitfBINARY.dr', 'DR_bitfieldsBINARY'),
                                   ('Modified_data/dr_bitfHDF5.dr', 'DR_bitfieldsHDF5'),
                                   ('Modified_data/dr_typesASCII_multivarcheck.dr', 'DR_typesASCII_multivarcheck'), 
                                   ('Modified_data/dr_typesBINARY_multivarcheck.dr', 'DR_typesBINARY_multivarcheck'), 
                                   ('Modified_data/dr_typesHDF5_multivarcheck.dr', 'DR_typesHDF5_multivarcheck')
                                  )
else:
    dr_file_name_drg_name_tuple = (
                                   ('Modified_data/dr_typesASCII.dr', 'DR_typesASCII'), 
                                   ('Modified_data/dr_typesBINARY.dr', 'DR_typesBINARY'), 
                                   ('Modified_data/dr_bitfASCII.dr', 'DR_bitfieldsASCII'), 
                                   ('Modified_data/dr_bitfBINARY.dr', 'DR_bitfieldsBINARY'),
                                   ('Modified_data/dr_typesASCII_multivarcheck.dr', 'DR_typesASCII_multivarcheck'), 
                                   ('Modified_data/dr_typesBINARY_multivarcheck.dr', 'DR_typesBINARY_multivarcheck')
                                  )

num_files = len(dr_file_name_drg_name_tuple)
for i in range(num_files):
    exec(open(dr_file_name_drg_name_tuple[i][0]).read())

# Get the number of data recording groups created
num_drgs = trick.get_num_data_record_groups()

# Check the result of trick.get_num_data_record_groups()
if has_dhf5:
    TRICK_EXPECT_EQ( num_drgs , 9 , test_suite , "num of dr groups = 9" )
else:
    TRICK_EXPECT_EQ( num_drgs , 6 , test_suite , "num of dr groups = 6" )

# Test trick.get_data_record_group(<drg_name>) for getting the drg pointer by its name
# Check the name of the obtained drg instead of the drg pointer
for i in range(num_drgs):
    TRICK_EXPECT_EQ(  trick.get_data_record_group(dr_file_name_drg_name_tuple[i][1]).get_group_name(), dr_file_name_drg_name_tuple[i][1], test_suite , "get drg by name " + dr_file_name_drg_name_tuple[i][1] )

# Test trick.get_data_record_group_by_idx(<drg_idx) for getting the drg pointer by its id number
# Check the name of the obtained drg instead of the drg pointer
for i in range(num_drgs):
    TRICK_EXPECT_EQ(  trick.get_data_record_group_by_idx(i).get_group_name(), dr_file_name_drg_name_tuple[i][1], test_suite , "get drg by drg id " + str(i) )

is_null = False
if trick.get_data_record_group(dr_file_name_drg_name_tuple[0][1]+'test') is None :
    is_null = True
TRICK_EXPECT_TRUE(  is_null, test_suite , "null drg by nonexistent drg name" )

is_null = False
if trick.get_data_record_group_by_idx(num_drgs+1) is None :
    is_null = True
if has_dhf5:
    TRICK_EXPECT_TRUE(  is_null, test_suite , "null drg by drg id 10" )
else:
    TRICK_EXPECT_TRUE(  is_null, test_suite , "null drg by drg id 7" )

is_null = False
if trick.get_data_record_group_by_idx(-1) is None :
    is_null = True
TRICK_EXPECT_TRUE(  is_null, test_suite , "null drg by drg id -1" )

trick.stop(1.0)
