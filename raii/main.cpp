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
    auto& [x, y] = *msTest;
    std::cout << x << " : " << y << std::endl;
    

    SharedPointer<float[]> floatArr = MakeSharedArray<float[]>(64);
    std::cout << floatArr[55] << std::endl; 
    // o in normal mode to go line below and eter insert mode.
    SharedPointer<float> floatTest = MakeShared<float>(11.2f);
    // std::cout << floatTest[1] << std::endl;
    // swap test
    // Ignore these type of comments, I'm trying to learn vim navigations.
    // to remove single character press x in normal mode.
    SharedPointer<int> first = MakeShared<int>(15);
    // di( -> to remove inside of the parentheses of the current line.
    // ci( -> clears inside of the parentheses' and puts you into insert mode.
    SharedPointer<int> second = MakeShared<int>(20);
    // yyp to yy -> yank / copy the whole line, p -> paste it below.
    std::cout << *first << std::endl;
    std::cout << *second << std::endl;

    swap(first, second);
    // capital a, A -> in order to move to the end of line and enter insert mode.
    // 2yy to copy the lines below, which were couts earlier and p to paste it below swap.
    std::cout << *first << std::endl;
    std::cout << *second << std::endl;

    first = second;
    if(first == second)
        std::cout << "first is second!" << std::endl;
    else
        std::cout << "first is not second!" << std::endl;

    
    return 0;
}