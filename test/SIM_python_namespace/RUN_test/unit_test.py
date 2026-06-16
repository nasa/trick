from trick.unit_test import *


def main():
    trick.stop(1.0)
    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name(
        os.getenv("TRICK_HOME") + "/trick_test/SIM_python_namespace.xml"
    )
    trick_utest.unit_tests.set_test_name("PythonNamespace")
    test_suite = "python_namespace"

    # normal class methods from S_define
    ball.foo_food.print_me()
    ball.foo_inner_food.print_me()
    ball.bar_food.print_me()
    ball.foo_yummyfood.print_me()
    print

    # global typedef struct (not in a namespace)
    ball.trick_food.globalFoodItems = trick.alloc_type(1, "GlobalFoodItem")
    ball.trick_food.globalFoodItems[0].itemName = "Hotdog"
    ball.trick_food.globalFoodItems[0].price = 3.50
    TRICK_EXPECT_EQ(
        str(ball.trick_food.globalFoodItems[0].itemName),
        "Hotdog",
        test_suite,
        "global typedef struct",
    )
    TRICK_EXPECT_EQ(
        ball.trick_food.globalFoodItems[0].price,
        3.50,
        test_suite,
        "global typedef struct",
    )

    # global same typedef struct (creal64_T) declared in two different headers (SignalA.hh and SignalB.hh) 
    # regression test for duplicate castAs* symbol
    ball.signal_a.value_a_array = trick.alloc_type(1, "creal64_T")
    ball.signal_b.value_b_array = trick.alloc_type(1, "creal64_T")
    ball.signal_a.value_a_array[0].re = 1.0
    ball.signal_a.value_a_array[0].im = 2.0
    ball.signal_b.value_b_array[0].re = 3.0
    ball.signal_b.value_b_array[0].im = 4.0
    TRICK_EXPECT_EQ( ball.signal_a.value_a_array[0].re , 1.0, test_suite , "duplicate typedef struct in two headers" )
    TRICK_EXPECT_EQ( ball.signal_a.value_a_array[0].im , 2.0, test_suite , "duplicate typedef struct in two headers" )
    TRICK_EXPECT_EQ( ball.signal_b.value_b_array[0].re , 3.0, test_suite , "duplicate typedef struct in two headers" )
    TRICK_EXPECT_EQ( ball.signal_b.value_b_array[0].im , 4.0, test_suite , "duplicate typedef struct in two headers" )
    ball.signal_a.value.re = 1.0
    ball.signal_a.value.im = 2.0
    ball.signal_b.value.re = 3.0
    ball.signal_b.value.im = 4.0
    TRICK_EXPECT_EQ( ball.signal_a.value.re , 1.0, test_suite , "duplicate typedef struct in two headers" )
    TRICK_EXPECT_EQ( ball.signal_a.value.im , 2.0, test_suite , "duplicate typedef struct in two headers" )
    TRICK_EXPECT_EQ( ball.signal_b.value.re , 3.0, test_suite , "duplicate typedef struct in two headers" )
    TRICK_EXPECT_EQ( ball.signal_b.value.im , 4.0, test_suite , "duplicate typedef struct in two headers" )

    # struct in a namespace
    ball.foo_yummyfood.foo_food_in_struct = trick.alloc_type(2, "Foo::FooFoodInStruct")
    ball.foo_yummyfood.foo_food_in_struct[0].foodName = "Pizza"
    ball.foo_yummyfood.foo_food_in_struct[0].calories = 100.0
    ball.foo_yummyfood.foo_food_in_struct[1].foodName = "Ice Cream"
    ball.foo_yummyfood.foo_food_in_struct[1].calories = 200.0
    TRICK_EXPECT_EQ(
        str(ball.foo_yummyfood.foo_food_in_struct[0].foodName),
        "Pizza",
        test_suite,
        "typedef struct in a namespace",
    )
    TRICK_EXPECT_EQ(
        ball.foo_yummyfood.foo_food_in_struct[0].calories,
        100.0,
        test_suite,
        "typedef struct in a namespace",
    )
    TRICK_EXPECT_EQ(
        str(ball.foo_yummyfood.foo_food_in_struct[1].foodName),
        "Ice Cream",
        test_suite,
        "typedef struct in a namespace",
    )
    TRICK_EXPECT_EQ(
        ball.foo_yummyfood.foo_food_in_struct[1].calories,
        200.0,
        test_suite,
        "typedef struct in a namespace",
    )

    # struct in a namespace without typedef
    ball.foo_yummyfood.foo_food_in_struct_no_typedef = trick.alloc_type(
        2, "Foo::FooFoodInStructNoTypedef"
    )
    ball.foo_yummyfood.foo_food_in_struct_no_typedef[0].foodName = "Pizza"
    ball.foo_yummyfood.foo_food_in_struct_no_typedef[0].calories = 100.0
    ball.foo_yummyfood.foo_food_in_struct_no_typedef[1].foodName = "Ice Cream"
    ball.foo_yummyfood.foo_food_in_struct_no_typedef[1].calories = 200.0
    TRICK_EXPECT_EQ(
        str(ball.foo_yummyfood.foo_food_in_struct_no_typedef[0].foodName),
        "Pizza",
        test_suite,
        "struct without typedef in a namespace",
    )
    TRICK_EXPECT_EQ(
        ball.foo_yummyfood.foo_food_in_struct_no_typedef[0].calories,
        100.0,
        test_suite,
        "struct without typedef in a namespace",
    )
    TRICK_EXPECT_EQ(
        str(ball.foo_yummyfood.foo_food_in_struct_no_typedef[1].foodName),
        "Ice Cream",
        test_suite,
        "struct without typedef in a namespace",
    )
    TRICK_EXPECT_EQ(
        ball.foo_yummyfood.foo_food_in_struct_no_typedef[1].calories,
        200.0,
        test_suite,
        "struct without typedef in a namespace",
    )

    # new class from Foo.Food
    food = trick.Foo.Food()
    food.print_me()
    TRICK_EXPECT_EQ(food.fast, 0, test_suite, "first level python namespace")
    food.fast = trick.Foo.Burger
    TRICK_EXPECT_EQ(food.fast, 2, test_suite, "first level python namespace")

    # new class from Foo.Food.Inner
    foodinner = trick.Foo.Inner.Food()
    foodinner.print_me()
    TRICK_EXPECT_EQ(foodinner.fast, 1, test_suite, "second level python namespace")
    foodinner.fast = trick.Foo.Inner.Burger
    TRICK_EXPECT_EQ(foodinner.fast, 0, test_suite, "second level python namespace")

    # new class from Foo.Food.Inner
    bar = trick.Bar.Food()
    bar.print_me()
    TRICK_EXPECT_EQ(bar.fast, 2, test_suite, "another first level python namespace")
    bar.fast = trick.Bar.Burger
    TRICK_EXPECT_EQ(bar.fast, 1, test_suite, "another first level python namespace")

    # new class from Foo.Food.Inner
    yummy = trick.Foo.YummyFood()
    yummy.print_me()
    TRICK_EXPECT_EQ(yummy.yummy, 1, test_suite, "additional file in same namespace")
    yummy.yummy = trick.Foo.Doughnuts
    TRICK_EXPECT_EQ(yummy.yummy, 2, test_suite, "additional file in same namespace")

    # new class from TrickFood
    trickfood = trick.Food()
    trickfood.print_me()
    TRICK_EXPECT_EQ(trickfood.fast, 2, test_suite, "blank python_module statement")
    trickfood.fast = trick.Pizza
    TRICK_EXPECT_EQ(trickfood.fast, 0, test_suite, "blank python_module statement")


if __name__ == "__main__":
    main()
