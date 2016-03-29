/*
    PURPOSE: (Test if item is an STL)
*/

#ifndef CHECKPOINT_IS_STL_CONTAINER
#define CHECKPOINT_IS_STL_CONTAINER

#include <vector>
#include <list>
#include <deque>
#include <set>

template <typename T>
struct is_stl_container {
  static const bool value = false;
};

template <typename T,typename Alloc>
struct is_stl_container<std::vector<T,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Alloc>
struct is_stl_container<std::list<T,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Alloc>
struct is_stl_container<std::deque<T,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Compare,typename Alloc>
struct is_stl_container<std::set<T,Compare,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Compare,typename Alloc>
struct is_stl_container<std::multiset<T,Compare,Alloc> > {
  static const bool value = true;
};

#endif

