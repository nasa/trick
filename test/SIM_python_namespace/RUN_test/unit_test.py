
from trick.unit_test import *

def main():
    trick.stop(1.0)
    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_python_namespace.xml" )
    trick_utest.unit_tests.set_test_name( "PythonNamespace" )
    test_suite = "python_namespace"

    # normal class methods from S_define
    ball.foo_food.print_me()
    ball.foo_inner_food.print_me()
    ball.bar_food.print_me()
    ball.foo_yummyfood.print_me()
    print

    # struct in a namespace
    ball.foo_yummyfood.foo_food_in_struct = trick.alloc_type(2, "Foo::FooFoodInStruct")
    ball.foo_yummyfood.foo_food_in_struct[0].foodName = "Pizza"
    ball.foo_yummyfood.foo_food_in_struct[0].calories = 100.0
    ball.foo_yummyfood.foo_food_in_struct[1].foodName = "Ice Cream"
    ball.foo_yummyfood.foo_food_in_struct[1].calories = 200.0
    TRICK_EXPECT_EQ( str(ball.foo_yummyfood.foo_food_in_struct[0].foodName) , "Pizza", test_suite , "struct in a namespace" )
    TRICK_EXPECT_EQ( ball.foo_yummyfood.foo_food_in_struct[0].calories , 100.0, test_suite , "struct in a namespace" )
    TRICK_EXPECT_EQ( str(ball.foo_yummyfood.foo_food_in_struct[1].foodName) , "Ice Cream", test_suite , "struct in a namespace" )
    TRICK_EXPECT_EQ( ball.foo_yummyfood.foo_food_in_struct[1].calories , 200.0, test_suite , "struct in a namespace" )

    # new class from Foo.Food
    food = trick.Foo.Food()
    food.print_me()
    TRICK_EXPECT_EQ( food.fast , 0, test_suite , "first level python namespace" )
    food.fast = trick.Foo.Burger
    TRICK_EXPECT_EQ( food.fast , 2, test_suite , "first level python namespace" )

    # new class from Foo.Food.Inner
    foodinner = trick.Foo.Inner.Food()
    foodinner.print_me()
    TRICK_EXPECT_EQ( foodinner.fast , 1, test_suite , "second level python namespace" )
    foodinner.fast = trick.Foo.Inner.Burger
    TRICK_EXPECT_EQ( foodinner.fast , 0, test_suite , "second level python namespace" )

    # new class from Foo.Food.Inner
    bar = trick.Bar.Food()
    bar.print_me()
    TRICK_EXPECT_EQ( bar.fast , 2, test_suite , "another first level python namespace" )
    bar.fast = trick.Bar.Burger
    TRICK_EXPECT_EQ( bar.fast , 1, test_suite , "another first level python namespace" )

    # new class from Foo.Food.Inner
    yummy = trick.Foo.YummyFood()
    yummy.print_me()
    TRICK_EXPECT_EQ( yummy.yummy , 1, test_suite , "additional file in same namespace" )
    yummy.yummy = trick.Foo.Doughnuts
    TRICK_EXPECT_EQ( yummy.yummy , 2, test_suite , "additional file in same namespace" )

    # new class from TrickFood
    trickfood = trick.Food()
    trickfood.print_me()
    TRICK_EXPECT_EQ( trickfood.fast , 2, test_suite , "blank python_module statement" )
    trickfood.fast = trick.Pizza
    TRICK_EXPECT_EQ( trickfood.fast , 0, test_suite , "blank python_module statement" )

if __name__ == "__main__":
    main()


