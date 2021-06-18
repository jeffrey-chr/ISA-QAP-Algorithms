#include <stdlib.h>
#include <iostream>

int function1()
{
    std::cout << "foo\n";
}

int function2()
{
    std::cout << "bar\n";
    return 0;
}

int main()
{
    function2();
    
    function1();
    
    return 0;
}

