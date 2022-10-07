#include <map>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <set>
#include <stack>
#include <array>
#include <string>

class UserClassStl {
    public:
        int a;
        std::vector<int> vec;
};

class UserClass {
    public:
        UserClass () : d(NULL) {}
        int a[5];
        long long b;
        std::string c;
        UserClass * d = NULL;

        bool operator== (const UserClass& other) const {
            for (int i = 0; i < 5; i++) {
                if (a[i] != other.a[i])
                    return false;
            }

            if (d == NULL ^ other.d == NULL) return false;
            return b == other.b && c == other.c && (d == NULL || *d == *(other.d));
        }

        bool operator != (const UserClass& other) const {
            return !(*this == other);
        }

};

class STLTestbed {
public:
    // This class is intended to test the checkpoint, restore, and delete functions for each of the supported STL types.
    // These live in the include/trick directory in checkpoint_<containername>.hh
    // Each STL type has its own implementation of each function, so they must be tested separately.
    // In addition, if the template argument is another STL container, a separate implementation is enabled to 
    // support recursion. (intrinsic:i vs stl:s in var names here)
    // Therefore, each of the 11 supported types presents 3 * 2^(num template args) implementations to test,
    // plus strings and pairs are sometimes handled strangely so those should probably be included here too

    std::vector<int> i_vec;
    std::vector<std::vector<int>> i_s_vec;
    std::vector<std::pair<int,int>> s_vec;
    std::vector<std::string> string_vec;

    std::pair<double,float> i_i_pair;
    std::pair<double,std::queue<bool>> i_s_pair;
    std::pair<std::queue<bool>,double> s_i_pair;
    std::pair<std::vector<int>,std::stack<float>> s_s_pair;
    std::pair<double,std::vector<bool>> i_v_pair;
    std::pair<std::pair<int,int>, int> pair_pair;

    std::map<int, double> i_i_map;
    std::map<int, std::stack<std::string>> i_s_map;
    std::map<std::set<int>, std::string> s_i_map;
    std::map<std::pair<int,int>, std::vector<int>> s_s_map;

    std::queue<int> i_queue;
    std::queue<std::pair<int,double>> s_queue;
    std::queue<std::list<float>> nested_list_queue;

    std::stack<long long> i_stack;
    std::stack<std::pair<short,double>> s_stack;
    std::stack<std::list<float>> nested_list_stack;

    std::set<char> i_set;
    std::set<std::vector<int>> vector_set;
    std::set   <std::pair<int,int>> s_set;
    std::set<std::map<short,double>> nested_map_set;

    // this one will as well, if we put any STL containers in there
    std::multiset<int> i_multiset;
    std::multiset<std::vector<int>> vector_multiset;
    std::multiset   <std::pair<int,int>> s_multiset;
    std::multiset<std::map<short,double>> nested_map_multiset;

    std::array<char, 10> i_array;
    std::array<std::pair<int,int>, 10> pair_array;
    std::array<std::string, 10> string_array;
    std::array<std::vector<int>, 10> vec_array;

    std::vector<UserClass> vec_user_defined;

    // This does not currently work in the unit tests
    std::vector<UserClassStl> vec_user_defined_stl;


    std::array<std::map<std::pair<int, int>,std::vector<std::stack<std::string>>>, 5> recursive_nightmare;
};
