//#pragma once
//template <int d, typename T>
//class ClassA {
//
//    size_t n_[d] = { 0 };
//    T* data_;
//
//    template <int D, typename U>
//    struct Initializer_list 
//    {
//        typedef std::initializer_list<typename Initializer_list<D - 1, U>::list_type > list_type;
//        Initializer_list(list_type l, ClassA& a, size_t s, size_t idx) 
//        {
//            a.n_[d - D] = l.size();
//
//            size_t j = 0;
//            for (const auto& r : l)
//            {
//                Initializer_list<D - 1, U> pl(r, a, s*l.size(), idx + s*j++);
//            }
//        }
//    };
//
//    template <typename U>
//    struct Initializer_list<1, U> 
//    {
//        typedef std::initializer_list<T> list_type;
//        Initializer_list(list_type l, ClassA& a, size_t s, size_t i) 
//        {
//            a.n_[d - 1] = l.size();
//            if (!a.data_)
//            {
//                a.data_ = new T[s*l.size()];
//            }
//
//            size_t j = 0;
//            for (const auto& r : l)
//            {
//                a.data_[i + s*j++] = r;
//            }
//        }
//    };
//                
//    typedef typename Initializer_list<d, T>::list_type initializer_type;
//                
//public:
//
//    // initializer list constructor
//    ClassA(initializer_type l) : data_(nullptr) {
//        Initializer_list<d, T> r(l, *this, 1, 0);
//    }
//
//    size_t size() const {
//        size_t n = 1;
//        for (size_t i = 0; i<4; ++i)
//            n *= n_[i];
//        return n;
//    }
//
//    friend std::ostream& operator<<(std::ostream& os, const ClassA& a) {
//        for (int i = 0; i<a.size(); ++i)
//            os << " " << a.data_[i];
//        return os << std::endl;
//    }
//};
//
//
//template<class T>
//using Eyal = ClassA<2, T>;