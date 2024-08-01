// @trick_parse{everything}

#ifndef Classes_HH
#define Classes_HH

template <class T>
class Foo {};

namespace peer {
    template <class T>
    class Bar {};

    struct Leek {
        template <class T>
        class Eggplant {};
    };

    namespace nested {
        template <class T>
        class Broccoli {};

        struct Carrot {
            template <class T>
            class Celery {};
        };
    }
}

namespace prime {

    template<class T>
    class Potato {};

    struct Turnip {
        template <class T>
        class Radish {};
    };

    namespace nested {
        template <class T>
        class Baz {};

        struct Artichoke {
            template <class T>
            class Asparagus {};
        };
    }

    struct Onion {
// ======================= WORKING =============================================
        /**
         * We assume a qualified template instantiation is fully qualified
         * starting from the global namespace, and we put the corresponding
         * %template directive in the global namespace, which works if our
         * assumption is correct.
         */
        ::Foo<int>                                    fooInt;

          peer::Bar<int>                              barInt;
        ::peer::Bar<double>                           barDouble;

          peer::Leek::Eggplant<int>                   eggplantInt;
        ::peer::Leek::Eggplant<double>                eggplantDouble;

          peer::nested::Broccoli<int>                 broccoliInt;
        ::peer::nested::Broccoli<double>              broccoliDouble;

          peer::nested::Carrot::Celery<int>           celeryInt;
        ::peer::nested::Carrot::Celery<double>        celeryDouble;

          prime::Potato<int>                          potatoInt;
        ::prime::Potato<double>                       potatoDouble;

          prime::Turnip::Radish<int>                  radishInt;
        ::prime::Turnip::Radish<double>               radishDouble;

          prime::nested::Baz<int>                     bazInt;
        ::prime::nested::Baz<double>                  bazDouble;

          prime::nested::Artichoke::Asparagus<int>    asparagusint;
        ::prime::nested::Artichoke::Asparagus<double> asparagusDouble;

        /**
         * We put the %template directive for unqualified template
         * instantiations in the namespace containing the current class,
         * which only works if the template is defined in that namespace as
         * well.
         */
        Potato<bool> potatoBool;

// ======================= BROKEN ==============================================
        /**
         * Because Foo is declared in the global namespace, its %template
         * directive must be in that scope as well. However, because the
         * template instation here is unqualified, we put the %template
         * directive in the containing namespace, which is an error.
         *
         * Error: 'Foo' resolves to '::Foo' and was incorrectly
         * instantiated in scope 'prime' instead of within scope ''.
         */
//        Foo<double> fooDouble;

        /**
         * Because these template instantiations are qualified, their %template
         * directives are placed in the global namespace. However, because they
         * are not _fully_ qualified, the generated SWIG input code is invalid.
         * For partially-qualified instantiations, the directive should either be:
         * 1. Declared in the global namespace and qualified with prime::
         * 2. Declared in the prime namespace
         *
         * We don't track enough information to do either.
         *
         * Error: Template 'Turnip::Radish' undefined.
         * Error: Template 'nested::Baz' undefined.
         * Error: Template 'nested::Artichoke::Asparagus' undefined.
         */
//        Turnip::Radish<bool>               radishBool;
//        nested::Baz<bool>                  bazBool;
//        nested::Artichoke::Asparagus<bool> asparagusBool;

        template<class T>
        class InnerOnion {}; // onions have layers!

        /**
         * All %template directives are placed before the class containing their
         * corresponding template instantiations. For member templates
         * (templates declared within the class being processed), this results
         * in the %template directive referencing the member template before it
         * has been declared, which is an error.
         *
         * Error: Template 'InnerOnion' undefined.
         * Error: Template 'Onion::InnerOnion' undefined.
         * Error: Template 'prime::Onion::InnerOnion' undefined.
         * Error: Template '::prime::Onion::InnerOnion' undefined.
         */
//                        InnerOnion<int>    layer1;
//                 Onion::InnerOnion<double> layer2;
//          prime::Onion::InnerOnion<bool>   layer2;
//        ::prime::Onion::InnerOnion<short>  layer4;
    };

    // Same tests as above, but within a nested namespace.
    namespace nested {
        class Corn {
            ::Foo<peer::Leek>                                 fooLeek;

              peer::Bar<Foo<long>>                            barFoo;
            ::peer::Bar<peer::Leek>                           barLeek;

              peer::Leek::Eggplant<long>                      eggplantLong;
            ::peer::Leek::Eggplant<peer::Leek>                eggplantLeek;

              peer::nested::Broccoli<long>                    broccoliLong;
            ::peer::nested::Broccoli<peer::Leek>              broccoliLeek;

              peer::nested::Carrot::Celery<long>              celeryLong;
            ::peer::nested::Carrot::Celery<peer::Leek>        celeryLeek;

              prime::Potato<long>                             potatoLong;
            ::prime::Potato<peer::Leek>                       potatoLeek;

              prime::Turnip::Radish<long>                     radishLong;
            ::prime::Turnip::Radish<peer::Leek>               radishLeek;

              prime::nested::Baz<long>                        bazLong;
            ::prime::nested::Baz<peer::Leek>                  bazLeek;

              prime::nested::Artichoke::Asparagus<long>       asparagusLong;
            ::prime::nested::Artichoke::Asparagus<peer::Leek> asparagusLeek;

            // These fail in Onion, but work here because Onion has already been processed.
              prime::Onion::InnerOnion<long>                  onionLong;
            ::prime::Onion::InnerOnion<peer::Leek>            onionLeek;

            // These fail for the same reason as in Onion.
//          Foo<float> fooFloat;
//          Turnip::Radish<float>               radishFloat;
//          nested::Baz<float>                  bazFloat;
//          nested::Artichoke::Asparagus<float> asparagusFloat;
//          Onion::InnerOnion<float>            innerOnionFloat;
        };
    }
}

#endif
