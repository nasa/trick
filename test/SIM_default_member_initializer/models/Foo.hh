template <class T>
class Foo {

    public:

    Foo() {}

    Foo(int i) {
        (void)i;
    }

    private:
    void operator=(const Foo&);

};
