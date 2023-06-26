
class Foo {
    public:
    int a; 
    int b;
    int q;

    void init () {
        a = 0;
        b = 0;
        q = 0;
    }

    void increment () {
        a++;
        b+=2;
        if(a%3 == 0) q++;
    }
};