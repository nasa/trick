#include <vector>

namespace a {
    template<class> class A {
        private:
        void operator=(const A&);
    };
}

namespace b {

    template<class> class A {};
    template<class> class B {};

    class C {
        public:
        std::vector<int> v;
        a::A<int> a;
        a::A<int> a2;
        A<int> a3;
        A<int> a4;
        B<int> b;
        B<int> b2;
        b::B<int> b3;
        b::B<int> b4;
        B<a::A<int> > ba;
        B<a::A<int> > ba2;
        b::B<a::A<int> > ba3;
        b::B<a::A<int> > ba4;
        B<A<int> > ba5;
        B<A<int> > ba6;

        // These don't work. Because the type is qualified, convert_swig assumes
        // it's fully qualified and puts the %template directive in the global
        // namespace. However, there is no A in the global namespace, so the wrapper
        // code fails to compile. Bonus points if you can fix this without breaking
        // something else.
        //b::B<A<int> > ba7;
        //b::B<A<int> > ba8;
    };

    // This class is the same as the previous. Seems weird, but it reveals scoping
    // errors under SWIG 2. The replication is not necessary in SWIG 4, which has
    // better error detection.
    class D {
        public:
        std::vector<int> v;
        a::A<int> a;
        a::A<int> a2;
        A<int> a3;
        A<int> a4;
        B<int> b;
        B<int> b2;
        b::B<int> b3;
        b::B<int> b4;
        B<a::A<int> > ba;
        B<a::A<int> > ba2;
        b::B<a::A<int> > ba3;
        b::B<a::A<int> > ba4;
        B<A<int> > ba5;
        B<A<int> > ba6;
        //b::B<A<int> > ba7;
        //b::B<A<int> > ba8;
    };
}

class E {
    public:
        std::vector<int> v;
        a::A<int> a;
        a::A<int> a2;
        b::B<int> b3;
        b::B<int> b4;
        b::B<a::A<int> > ba3;
        b::B<a::A<int> > ba4;
};

namespace a {
    class B {
        public:
        std::vector<int> v;
        a::A<int> a;
        a::A<int> a2;
        A<int> a3;
        A<int> a4;
        b::B<int> b3;
        b::B<int> b4;
        b::B<a::A<int> > ba3;
        b::B<a::A<int> > ba4;
        //b::B<A<int> > ba7;
        //b::B<A<int> > ba8;
    };
}
