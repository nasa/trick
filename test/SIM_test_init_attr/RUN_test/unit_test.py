from trick.unit_test import *

trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name(
        os.getenv("TRICK_HOME") + "/trick_test/SIM_test_init_attr.xml"
    )
trick_utest.unit_tests.set_test_name("CallInitAttr")

suite_name = "init_attr"
attr_map = trick.AttributesMap.attributes_map()
outer_attr = attr_map.get_attr("Outer")
TRICK_EXPECT_TRUE(outer_attr != None, suite_name, "outer_attr_table_found")

if outer_attr != None:
    # Returns the name of the first attribute of the Outer struct, which is Inner inner
    field_name = getattr(outer_attr, "name", None)
    # Returns the type of the first attribute of the Outer struct, which is TRICK_STRUCTURED
    field_type = getattr(outer_attr, "type", None)
    # Returns the type name of the first attribute of the Outer struct, which is Inner
    field_type_name = getattr(outer_attr, "type_name", None)

    # Without PR# 2166 and if there is no instance of Outer declared in the header file or S_define, 
    # the following line will return None for nested_attr thus nested_attr_pointer_present test would fail
    nested_attr = getattr(outer_attr, "attr", None)

    inner_attr = attr_map.get_attr("Inner")
    inner_name = getattr(inner_attr, "name", None)
    inner_type_name = getattr(inner_attr, "type_name", None)

    another_inner_attr = attr_map.get_attr("AnotherInner")
    another_inner_name = getattr(another_inner_attr, "name", None)
    another_inner_type_name = getattr(another_inner_attr, "type_name", None)

    TRICK_EXPECT_TRUE(field_name == "inner", suite_name, "first_outer_attr_field_name")
    TRICK_EXPECT_TRUE(field_type == trick.TRICK_STRUCTURED, suite_name, "first_outer_attr_field_type")
    TRICK_EXPECT_TRUE(field_type_name == "Inner", suite_name, "first_outer_attr_field_type_name")
    TRICK_EXPECT_TRUE(nested_attr != None, suite_name, "nested_attr_pointer_present")
    TRICK_EXPECT_TRUE(inner_attr != None, suite_name, "inner_type_attr_present")
    TRICK_EXPECT_TRUE(inner_name == "i", suite_name, "inner_name")
    TRICK_EXPECT_TRUE(inner_type_name == "int", suite_name, "inner_type_name")
    TRICK_EXPECT_TRUE(another_inner_attr != None, suite_name, "another_inner_type_attr_present")
    TRICK_EXPECT_TRUE(another_inner_name == "j", suite_name, "another_inner_name")
    TRICK_EXPECT_TRUE(another_inner_type_name == "int", suite_name, "another_inner_type_name")

trick_utest.unit_tests.write_output()

trick.stop(1.0)


