#pragma once
#include <vector>
#include <functional>
#include <initializer_list>
#include <algorithm>

using namespace std;

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

template <int d>
struct Coord
{
    size_t ax[d];

    Coord(vector<size_t>& crds)
    {
        int minLen = min(d, crds.size());
        for (int i = 0; i < minLen; ++i)
        {
            ax[i] = crds[i];
        }
    }

    bool operator==(Coord<d> other)
    {
        int diff = 0;
        for (int i = 0; i < d; i++)
        {
            diff += abs(static_cast<int>(ax[i]) - static_cast<int>(other.ax[i]));
        }
        return diff == 0;
    }

    bool adjacents(Coord<d> c1, Coord<d> c2)
    {
        //-- coords are adjacents only if they their difference is 1
        int diff = 0;
        for (int i = 0; i < d; i++)
        {
            diff += abs(c1.ax[i] - c2.ax[i]);
        }
        return diff == 1;
    }
};

using Crd2 = Coord<2>;

template <typename Ret, typename Inp>
struct callable
{
    function<Ret(Inp)> func;

    callable(function<Ret(Inp)> f) : func(f){}
    Ret operator()(Inp p)
    {
        return func(p);
    }
};


template <int d, typename T>
class Matrix 
{
    size_t n_[d] = { 0 };
    vector<T> data_;
    
    //------------------------------- Depth_detector --------------------------------//
    template <int D, typename U> //start with D == d
    struct Depth_detector
    {
        typedef std::initializer_list<typename Depth_detector<D - 1, U>::list_type > list_type;
        
        Depth_detector(list_type init_list, Matrix& mat)
        {
            mat.n_[d - D] = max(mat.n_[d - D], init_list.size());

            for (const auto& lst : init_list)
            {
                Depth_detector<D - 1, U> detect_lower_level_depth(lst, mat);
            }
        }
    };

    template <typename U>
    struct Depth_detector<1, U>
    {
        typedef std::initializer_list<T> list_type;
        Depth_detector(list_type init_list, Matrix& mat)
        {
            mat.n_[d - 1] = max(mat.n_[d - 1], init_list.size());
        }
    };

    typedef typename Depth_detector<d, T>::list_type depth_detector_type;

    //------------------------------- Initializer_list --------------------------------//
    template <int D, typename U> //-- start with D == d and end with D == 1
    struct Initializer_list
    {
        typedef std::initializer_list<typename Initializer_list<D - 1, U>::list_type > list_type;
        Initializer_list(list_type list, Matrix& mat, vector<int>& progress)
        {
            int layer_num = 0;
            for (const auto& layer : list)
            {
                progress[d - D] = layer_num;
                Initializer_list<D - 1, U> pl(layer, mat, progress);
                layer_num++;
            }
        }
    };

    template <typename U>
    struct Initializer_list<1, U>
    {
        typedef std::initializer_list<T> list_type;
        Initializer_list(list_type init_list, Matrix& mat, vector<int>& progress)
        {
            //-- get here to copy a "row" 
            /*
             * example:
             * progress = [1,1,0], n_ = [2,2,10]
             * so now we need to copy all 10 columns of the 2nd row (index 1)
             * offset is 1*2*10+1*10
             */

            int offset = 0;
            for (int layer = 0; layer < d-1; layer++) //-- layer depth
            {
                int layer_prog = progress[layer];
                for (int j = layer + 1; j < d; j++)
                {
                    layer_prog *= mat.n_[j];
                }
                offset += layer_prog;
            }

            int col = 0;
            for (const auto& r : init_list)
            {
                mat.data_[offset + col++] = r;
            }
        }
    };
    typedef typename Initializer_list<d, T>::list_type initializer_type;

public:
    // initializer list constructor
    Matrix(initializer_type l) : data_(vector<T>())
    {
        Depth_detector<d, T> depth_det(l, *this); //-- detects dimensions
        
        int num_of_elements = 1;
        for (int i = 0; i < d; i++)
        {
            num_of_elements *= n_[i];
        }
        data_ = vector<T>(num_of_elements, 0);

        auto progress = vector<int>(d, 0);
        Initializer_list<d, T> initialize(l, *this, progress); //-- copy values
    }

    T& getElem(Coord<d> crd)
    {
        for (int i = 0; i < d; i++)
        {
            if (crd.ax[i] > n_[i] - 1)
            {
                throw exception("index out of range");
            }
        }
        int offset = 0;
        for (int layer = 0; layer < d; layer++) //-- layer depth
        {
            int layer_prog = crd.ax[layer];
            for (int j = layer + 1; j < d; j++)
            {
                layer_prog *= n_[j];
            }
            offset += layer_prog;
        }
        return data_[offset];
    }

    template<typename R>
    vector<pair<R,vector<Coord<d>>>> groupValues(callable<R,T> f)
    {
        auto unmapped_soil = vector<Coord<d>>();
        vector<size_t> c1(d, 0);
        Coord<d> currCoord(c1);

        auto all_groups = vector<pair<R, vector<Coord<d>>>>();
        //auto q = f(getElem(currCoord));

        while (c1[d-1] < n_[d-1])
        {
            unmapped_soil.push_back(Coord<d>(c1));

            auto currInsert = 0;
            c1[currInsert] += 1;
            
            while (c1[currInsert] > n_[currInsert] && currInsert < d-1)
            {
                c1[currInsert++] = 0;
                c1[currInsert] += 1;
            }
        }

        while (unmapped_soil.size() > 0)
        {
            vector<Coord<d>> new_grp;
            auto begin_crd = unmapped_soil[0];
            //revealSurroundings(new_grp, begin_crd, f);

            for (const auto& crd : new_grp)
            {
                //-- todo: skeptic
                unmapped_soil.erase(std::remove(unmapped_soil.begin(), unmapped_soil.end(), crd), unmapped_soil.end());
            }
            //-- add group:
            bool has_group = false;
            auto it = all_groups.begin();
        }
        return all_groups;
    }
    
    template <typename R>
    void revealSurroundings(vector<Coord<d>>& grp_crds_vec, Coord<d>& crd, callable<R, T> f)
    {
        grp_crds_vec.push_back(crd);

        R grp_type = f(getElem(crd));
        
        for (int dim = 0; dim < d; ++dim)
        {
            if (crd.ax[dim] == 0) // only raise
            {
                crd.ax[dim] += 1;
                if ((std::find(grp_crds_vec.begin(), grp_crds_vec.end(), crd) != grp_crds_vec.end()) && (f(getElem(crd)) == grp_type))
                {
                    revealSurroundings(grp_crds_vec, crd, f);
                }
                crd.ax[dim] -= 1; //-- undo changes
            }
            else if (crd.ax[dim] == n_[dim] - 1) // only lower
            {
                crd.ax[dim] -= 1;
                if ((std::find(grp_crds_vec.begin(), grp_crds_vec.end(), crd) != grp_crds_vec.end()) && (f(getElem(crd)) == grp_type))
                {
                    revealSurroundings(grp_crds_vec, crd, f);
                }
                crd.ax[dim] += 1; //-- undo changes
            }
            else //both
            {
                crd.ax[dim] += 1;
                if ((std::find(grp_crds_vec.begin(), grp_crds_vec.end(), crd) != grp_crds_vec.end()) && (f(getElem(crd)) == grp_type))
                {
                    revealSurroundings(grp_crds_vec, crd, f);
                }
                crd.ax[dim] -= 2;
                if ((std::find(grp_crds_vec.begin(), grp_crds_vec.end(), crd) != grp_crds_vec.end()) && (f(getElem(crd)) == grp_type))
                {
                    revealSurroundings(grp_crds_vec, crd, f);
                }
                crd.ax[dim] += 1; //-- undo changes
            }
        }
    }


//void Board::revealSurroundings(Coordinate c, char ship_char, Board &board, vector<Coordinate> &coords)
//{
//    if (board.charAt(c) == ship_char)
//    {
//        board.setSlot(c, Board::SEA);
//        coords.push_back(c);
//        int rows[2] = { c.row + 1, c.row - 1 };
//        int cols[2] = { c.col + 1, c.col - 1 };
//        int depths[2] = { c.depth + 1, c.depth - 1 };
//        for (int i = 0; i < 2; i++)
//        {
//            if (board.isInBoard(rows[i], c.col, c.depth))
//                revealSurroundings(rows[i], c.col, c.depth, ship_char, board, coords);
//            if (board.isInBoard(c.row, cols[i], c.depth))
//                revealSurroundings(c.row, cols[i], c.depth, ship_char, board, coords);
//            if (board.isInBoard(c.row, c.col, depths[i]))
//                revealSurroundings(c.row, c.col, depths[i], ship_char, board, coords);
//        }
//    }
//}
};

template<class T>
using Matrix2d = Matrix<2, T>;

