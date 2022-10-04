#include <map>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <set>
#include <stack>
#include <array>
#include <string>

class SimpleWrapper {
    public:
        int a;
        std::vector<int> vec;
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
    // These fail in CP right now
    // std::set   <std::pair<int,int>> s_set;
    // std::set<std::map<int,int>> nested_map_set;

    // this one will as well, if we put any STL containers in there
    std::multiset<int> i_multiset;

    std::array<char, 10> i_array;
    std::array<std::pair<int,int>, 10> pair_array;
    std::array<std::string, 10> string_array;
    std::array<std::vector<int>, 10> vec_array;

    // This does not currently work in the unit tests
    std::vector<SimpleWrapper> vec_user_defined;

    std::array<std::map<std::pair<int, int>,std::vector<std::stack<std::string>>>, 5> recursive_nightmare;
};