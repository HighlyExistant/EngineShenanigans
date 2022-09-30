#include <iostream>

struct Struct1
{
    int val1;
    int val2;
    static void function() 
    {
        std::cout << "hello world";
    }
};
struct Struct2 : Struct1
{
};

int main(int argc, char const *argv[])
{
    Struct1 struct1;
    
    Struct2 struct2;
    // struct2.
    return 0;
}
