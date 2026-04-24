#include <iostream>
#include "shared_ptr.h"

struct test
{
    int x, y;
    test(int u, int v) : x(u), y(v)
    {}
};

int main(void)
{
    SharedPointer<int> m(new int(10));
    std::cout << *m.get() << std::endl;
    std::cout << m.UseCount() << std::endl;
    
    SharedPointer<int> m2 = m;
    std::cout << *m2.get() << std::endl;
    std::cout << m2.UseCount() << std::endl;

    m2.release();
    // std::cout << *m.get() << std::endl;
    // std::cout << m.UseCount() << std::endl;

    SharedPointer<float> ms = MakeShared<float>(12.5f);
    std::cout << *ms.get() << std::endl;
    std::cout << ms.UseCount() << std::endl;

    std::cout << "Dereference operator: "<< *ms << std::endl;

    SharedPointer<test> msTest = MakeShared<test>(5, 2);
    std::cout << msTest->x << " : " << msTest->y << std::endl;

    SharedPointer<float[]> floatArr = MakeSharedArray<float[]>(64);
    return 0;
}