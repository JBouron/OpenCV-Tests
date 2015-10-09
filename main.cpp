#include <iostream>

using namespace std;

int main() {
    const int a = 0;
    const_cast<int> (a) = 99;
    cout << a << endl;
    return 0;
}