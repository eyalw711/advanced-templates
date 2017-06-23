#include <iostream>
#include "MatrixNew.h"
#include <map>
#include <locale>

using namespace std;

template<typename Groups>
void print(const Groups& all_groups) {
	for (const auto& groupType : all_groups) {
		cout << groupType.first << ":" << endl;
		for (const auto& groupOfType : groupType.second) {
			for (const auto& coord : groupOfType) {
				cout << "{ ";
				for (auto i : coord) {   // was: for(int i : coord)
					cout << i << ' ';
				}
				cout << "} ";
			}
			cout << endl;
		}
	}
}


int main() {
    
	Matrix2d<char> m = { { 'a', 'A', 'a' },{ 'B', 'a', 'B' },{ 'B', 'a', 'B' } };
	auto all_groups = m.groupValues([](auto i) {return islower(i) ? "L" : "U"; });
	//Matrix3d<int> m = { {{0,1},{0,1}},{ { 0,1 },{ 0,1 } } };
	//Matrix2d<int> m = { { 1},{ 4, 8, 7 },{ 7, 1, 1, 1, 9, 9, 0 } };
    //auto all_groups = m.groupValues([](auto i) {return i % 2 == 0; });
    //auto all_groups = m.groupValues([](auto i){return islower(static_cast<int>(i)) ? "L" : "U"; });
    print(all_groups);
	Matrix3d<int> m2 = { { { 1, 2, 3 },{ 1, 2 },{ 1, 2 } },{ { 1, 2 },{ 1, 2, 3, 4 } } };
	auto groups = m2.groupValues([](auto i) {return i % 3 ? "!x3" : "x3"; });
	print(groups);

    return 0;
}



/////////////////////////////////////////////////////////////////////

//#include <iostream>
//#include "ClassA.h"
//
//int main()
//{
//    Eyal<char> e = { { 'a', 'A', 'a' },{ 'B', 'a', 'B' },{ 'B', 'a', 'B' } };
//    ClassA<4, double> TT = { 
//        {
//            { { 1. },{ 7. },{ 13. },{ 19 } },
//            { { 2 },{ 8 },{ 14 },{ 20 } },
//            { { 3 },{ 9 },{ 15 },{ 21 } }
//        },
//        { 
//            { { 4. },{ 10 },{ 16 },{ 22 } },
//            { { 5 },{ 11 },{ 17 },{ 23 } },
//            { { 6 },{ 12 },{ 18 },{ 24 } } 
//        } 
//    };
//    std::cout << "TT -> " << TT << std::endl;
//
//    return 0;
//}




