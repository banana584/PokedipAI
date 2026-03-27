#include "nn.hpp"

int main() {
    Matrix A(3, 2);
    Matrix B(2, 3);

    // Fill A
    A.Set(0,0,1);
    A.Set(0,1,2);
    A.Set(0,2,3);
    A.Set(1,0,4);
    A.Set(1,1,5);
    A.Set(1,2,6);

    // Fill B
    B.Set(0,0,7);
    B.Set(0,1,8);
    B.Set(1,0,9);
    B.Set(1,1,10);
    B.Set(2,0,11);
    B.Set(2,1,12);

    A.Print();
    std::cout << "\n";
    B.Print();
    std::cout << "\n";

    Matrix C = A * B;

    C.Print();
    std::cout << "\n";

    return 0;
}