| [Home](/trick) → [How‐To Guides](How‐To-Guides) → Use Inherited Templates |
|---------------------------------------------------------------------|

# How do I use inherited templates in the input file?

First, here is some documentation on how [SWIG handles templates](http://www.swig.org/Doc1.3/SWIGPlus.html#SWIGPlus_nn30).
When inheriting from a template, we need to tell SWIG about the template using the SWIG %template
statement.  The %template statement gives each template/argument combination a name.  The name can
be anything, it only has to be unique.  In some cases Trick will write these %template lines out
for the user.  In other cases, the user will have manually write the %template statement themselves.

Here are some examples of inheriting from templates and shows where Trick will automatically write
the %template statements and when the user is responsible from writing them.

Below we have some classes and templates.  Class A is a normal class type.  Class B is a template
inheriting from A.  Class Ca is another template inheriting from B.  Class Cb is a normal class
inheriting from B.

When instantiating template typed variables, Trick will automatically create a %template statement
for the immediate type it can see.  Trick does not attempt to determine any lower level inheritance
to determine if further %template statements.  So in the below example in class D Trick will write
out %template statements for the top level of templates for variables my\_b (B<double>)and my\_ca
(Ca<float>). Trick will not follow the inheritance path of my\_ca and will not write out the lower
level (B<float>) that Ca<float> inherits from.  Therefore we have to write out a %template statement
for B<float> manually.

Additionally if we inherit from a specific template directly as is the case for class Cb, then again
Trick will not automatically write out the %template statement and it is the user responsibility
to write out the template statement.

```c++
class A {
    public:
        A(): a(2) {}
        int run_A() {
            return 0 ;
        }
        int a ;
} ;

template < typename T > class B : public A {
    public:
        B() : b(3) {}
        T run_B() {
            return b ;
        }
        T b ;
} ;

template < typename U > class Ca : public B<U> {
    public:
        Ca() : ca(4) {}
        U run_Ca() {
            return ca ;
        }
        U ca ;
} ;

// Trick does not currently write out the %template statement for specifically inherited template
#ifdef SWIG
%template(Bshort) B<short> ;
#endif

class Cb : public B<short> {
    public:
        Cb() : cb(5) {}
        double run_Cb() {
            return cb ;
        }
        double cb ;
} ;

// Trick does not create %template statements for lower level inherited classes.  B<float>
// is created by Cb<float>
#ifdef SWIG
%template(Bfloat) B<float> ;
#endif

class D {
    public:
        // Trick sees these template variables and auto generates
        // %template(Bdouble) B< double >
        // %template(Cafloat) Ca< float >
        B< double > my_b;
        Ca< float > my_ca;
        Cb my_cb;
} ;
```
