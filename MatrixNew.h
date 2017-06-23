#pragma once
#include <vector>
#include <functional>
#include <initializer_list>
#include <algorithm>
#include <memory>
#include <iostream>
#include <map>
#include <list>
#include <functional>
#include <iso646.h>

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
	vector<size_t> ax = vector<size_t>(d,0);

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

	bool adjacents(Coord<d> c2)
	{
		//-- coords are adjacents only if they their difference is 1
		int diff = 0;
		for (int i = 0; i < d; i++)
		{
			diff += abs(this.ax[i] - c2.ax[i]);
		}
		return diff == 1;
	}

	auto begin() const  { return ax.begin(); }
	auto end() const { return ax.end(); }

	bool operator<(const Coord<d>& other) {
		for(size_t i = 0; i< ax.size(); i++)
		{
			if (ax[i] < other.ax[i])
				return true;
			if (ax[i] > other.ax[i])
				return false;
		}
		return true;
	}
};

using Crd2 = Coord<2>;
using Crd3 = Coord<3>;


template<class T, size_t DIMENSIONS>
class Matrix;

template<class T, size_t DIMENSIONS>
struct MatrixCopier {
	static void copy(T* dest, size_t dest_size, const size_t* dest_dimensions, const T* source, size_t source_size, const size_t* source_dimensions) {
		size_t dest_size0 = dest_dimensions[0] ? dest_size / dest_dimensions[0] : 0;
		size_t source_size0 = source_dimensions[0] ? source_size / source_dimensions[0] : 0;
		for (size_t i = 0; i < source_dimensions[0]; ++i) {
			MatrixCopier<T, DIMENSIONS - 1>::copy(dest + (i * dest_size0), dest_size0, dest_dimensions + 1, source + (i * source_size0), source_size0, source_dimensions + 1);
		}
	}
};

template<class T>
struct MatrixCopier<T, 1> {
	static void copy(T* dest, size_t dest_size, const size_t* dest_dimensions, const T* source, size_t source_size, const size_t* source_dimensions) {
		for (size_t i = 0; i < source_size; ++i) {
			dest[i] = source[i];
		}
	}
};



template<class T, size_t DIMENSIONS>
struct MatrixPrinter {
	static void print(const T* values, size_t size, const size_t* dimensions, std::ostream& out = cout) {
		out << '{';
		size_t size0 = size / dimensions[0];
		MatrixPrinter<T, DIMENSIONS - 1>::print(values, size0, dimensions + 1, out);
		for (size_t i = 1; i < dimensions[0]; ++i) {
			out << ',';
			MatrixPrinter<T, DIMENSIONS - 1>::print(values + (i*size0), size0, dimensions + 1, out);
		}
		out << '}';
	}
};

template<class T>
struct MatrixPrinter<T, 1> {
	static void print(const T* values, size_t size, const size_t* dimensions, std::ostream& out = cout) {
		out << '{';
		out << values[0];
		for (size_t i = 1; i < size; ++i) {
			out << ',' << values[i];
		}
		out << '}';
	}
};







/* Matrix*/

template<typename T, size_t DIMENSIONS>
class Matrix {
	constexpr static size_t NUM_DIMENSIONS = DIMENSIONS;
	std::unique_ptr<T[]> _array = nullptr;
	size_t _dimensions[DIMENSIONS] = {};
	const size_t _size = 0;
	friend class Matrix<T, DIMENSIONS + 1>;
public:
	size_t size() const { return _size; }
	Matrix() {}

	// DIMENSIONS == 1
	// We want here a ctor with this signature:
	//    Matrix(const std::initializer_list<T>& values) {
	// but SFINAE is needed to block this ctor from being used by Matrix with DIMENSIONS > 1
	// The SFINAE results with the exact signature we want, but only for cases DIMENSIONS == 1
	template<typename G = T>
	Matrix(const std::initializer_list<typename std::enable_if_t<DIMENSIONS == 1, G>>& values) {
		const_cast<size_t&>(_size) = values.size();
		_dimensions[0] = _size;
		_array = std::make_unique<T[]>(_size);
		size_t i = 0;
		for (auto& val : values) {
			_array[i++] = val;
		}
	}
	// DIMENSIONS > 1
	// We want here a ctor with this signature:
	//    Matrix(const std::initializer_list<Matrix<T, DIMENSIONS-1>>& values) {
	// although this ctor is not useful and won't be used by Matrix<T, 1> it will still be in class
	// and thus would compile as part of Matrix<T, 1> resulting with a parameter of type:
	//          const std::initializer_list< Matrix<T, 0> >& values
	// having Matrix<T, 0> as a parameter - even for a function that is not in use, inside a class that is used
	// would require the compiler to instantiate Matrix<T, 0> class which results with a warning
	// the SFINAE below solves this warning.
	// The SFINAE results with the exact signature we want, but only for cases DIMENSIONS > 1
	template<typename G = T>
	Matrix(const std::initializer_list<Matrix<typename std::enable_if_t<(DIMENSIONS > 1), G>, DIMENSIONS - 1>>& values) {
		_dimensions[0] = values.size();
		/* for each sub Matrix in of size DIMENSTION-1 */
		for (auto& m : values) {
			/* for every dimenstion that is small then DIMENSIONS find the maximum dimension of the sub Matrix */
			for (size_t dim = 0; dim < DIMENSIONS - 1; ++dim) {
				if (m._dimensions[dim] > _dimensions[dim + 1]) {
					_dimensions[dim + 1] = m._dimensions[dim];
				}
			}
		}
		size_t size = 1;
		for (size_t dim = 0; dim < DIMENSIONS; ++dim) {
			size *= _dimensions[dim];
		}

		const_cast<size_t&>(_size) = size;
		/* _array is now an array of type T with the total size of the Matrix (e.g. 5X3X4, then _array is one dimensional array of size 60
		initialized to all zeros */
		_array = std::make_unique<T[]>(_size); // "zero initialized" - T()
		size_t i = 0;
		size_t dest_size = _size / _dimensions[0];
		for (auto& m : values) {
			MatrixCopier<T, DIMENSIONS - 1>::copy(&(_array[i * dest_size]), dest_size, _dimensions + 1, m._array.get(), m._size, m._dimensions);
			++i;
		}
	}

	Matrix(Matrix&& m) {
		*this = std::move(m);
	}

	auto& operator=(Matrix&& m) {
		std::swap(_array, m._array);
		std::swap(const_cast<size_t&>(_size), const_cast<size_t&>(m._size));
		std::swap(_dimensions, m._dimensions);
		return *this;
	}

	size_t getDimension(size_t i) const {
		return _dimensions[i];
	}


	int coordToArrayIndex(Coord<DIMENSIONS> crd)
	{
		for (int i = 0; i < DIMENSIONS; i++)
		{
			if (crd.ax[i] > _dimensions[i] - 1)
			{
				throw exception("index out of range");
			}
		}
		int offset = 0;
		for (int layer = 0; layer < DIMENSIONS; layer++) //-- layer depth
		{
			int layer_prog = crd.ax[layer];
			for (int j = layer + 1; j < DIMENSIONS; j++)
			{
				layer_prog *= _dimensions[j];
			}
			offset += layer_prog;
		}
		return offset;
	}

	T& getElem(Coord<DIMENSIONS> crd)
	{
		return _array[coordToArrayIndex(crd)];
	}

	bool isInMatrix(Coord<DIMENSIONS> crd)
	{
		for(size_t ax = 0; ax<crd.ax.size(); ax++)
		{
			if (crd.ax[ax] >= _dimensions[ax] or crd.ax[ax] < 0)
				return false;
		}
		return true;
	}


	template<typename GroupingFunc>
	using GroupingType = result_of_t<GroupingFunc(T)>;


	template <typename GroupingFunc>
	void revealSurroundings(GroupingFunc groupingFunc, Coord<DIMENSIONS> crd, GroupingType<GroupingFunc> crd_func_val, vector<pair<Coord<DIMENSIONS>, bool>>& coordinates_mapped, vector<Coord<DIMENSIONS>>& coords)
	{
		if (groupingFunc(getElem(crd)) == crd_func_val)
		{
			coordinates_mapped[coordToArrayIndex(crd)].second = true;
			coords.push_back(crd);
			
			for(size_t ax = 0 ; ax < crd.ax.size() ; ax++)
			{
				auto tmp_crd = Coord<DIMENSIONS>(crd);
				tmp_crd.ax[ax] = tmp_crd.ax[ax]-1;
				if (isInMatrix(tmp_crd) and !coordinates_mapped[coordToArrayIndex(tmp_crd)].second)
					revealSurroundings<GroupingFunc>(groupingFunc, tmp_crd, crd_func_val, coordinates_mapped, coords);
				tmp_crd.ax[ax] = tmp_crd.ax[ax] + 2;
				if (isInMatrix(tmp_crd) and !coordinates_mapped[coordToArrayIndex(tmp_crd)].second)
					revealSurroundings<GroupingFunc>(groupingFunc, tmp_crd, crd_func_val, coordinates_mapped, coords);
				
			}
		}
	}



	template<typename GroupingFunc>
	auto groupValues(GroupingFunc groupingFunc) {
		auto coordinates_mapped = vector<pair<Coord<DIMENSIONS>,bool>>();
		auto coordinates_components = vector<vector<size_t>>(_size);
		// initialize coordinates_components
		int num_of_reaccurance = 1;
		size_t sub_matrix_size = _size;
		for (size_t& dim : _dimensions)
		{
			sub_matrix_size /= dim;
			int index = 0;
			for (int i = 1; i <= num_of_reaccurance; i++)
			{
				for (size_t j = 0; j < dim; j++)
				{
					for (size_t k = 0 ; k <sub_matrix_size; k++)
					{						
						coordinates_components[index++].push_back(j);
					}
				}
			}
			num_of_reaccurance *= dim;			
		}
		//initialize coordinates_mapped
		for(auto& crd : coordinates_components)
		{
			//TODO: emplace...
			coordinates_mapped.push_back(make_pair(Coord<DIMENSIONS>(crd), false));
		}

		std::map<GroupingType<GroupingFunc>, std::vector<vector<Coord<DIMENSIONS>>>> groups;

		//find groups
		for (auto& crd_obj : coordinates_mapped)
		{
			if (!crd_obj.second) //if have not been revealed yet
			{
				auto coords = vector<Coord<DIMENSIONS>>();
				auto crd_val = getElem(crd_obj.first);
				auto crd_func_val = groupingFunc(crd_val);
				revealSurroundings<GroupingFunc>(groupingFunc, crd_obj.first, crd_func_val, coordinates_mapped, coords);
				groups[crd_func_val].push_back(coords);
			}
		}

		//sort groups
		for(auto& groups_by_type : groups)
		{
			for (auto& group : groups_by_type.second)
				sort(group.begin(), group.end());

		}
		
		return groups;
	}



	friend std::ostream& operator<<(std::ostream& out, const Matrix& m) {
		MatrixPrinter<T, DIMENSIONS>::print(m._array.get(), m._size, m._dimensions, out);
		return out;
	}
};

// defining Matrix2d<T> as Matrix<T, 2>
template<class T>
using Matrix2d = Matrix<T, 2>;

// defining Matrix3d<T> as Matrix<T, 3>
template<class T>
using Matrix3d = Matrix<T, 3>;

// defining IntVector as Matrix<int, 1>
using IntVector = Matrix<int, 1>;

/*int main() {
	Matrix<int, 1> m0 = { 25, 10, 15 };
	cout << m0 << endl;

	Matrix<int, 3> m1 = { { { 10, 20, 30 },{ 10, 20 },{ 11, 22, 33 } },{ { 40, 50, 60 },{ 80, 90 },{ 51, 52, 53 } } };
	cout << m1 << endl;

	// move-assignment
	Matrix3d<int> m2;
	m2 = std::move(m1);
	cout << m2 << endl;

	// move-ctor
	auto m3 = std::move(m2);
	cout << m3 << endl;

	IntVector m4 = std::move(m0);
	cout << m4 << endl;

	Matrix<int, 2> m5 = { {},{ 25, 10, 15 },{},{ 3 } };
	cout << m5 << endl;

}*/

