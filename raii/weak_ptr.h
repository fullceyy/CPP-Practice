#pragma once
#include <memory>

template <typename T>
class WeakPtr
{
private:
    using U = std::remove_extent_t<T>;
    U* mUnderlyingPtr {nullptr};
public:
    WeakPtr() {}
    ~WeakPtr() {}

    void reset()
    {
        mUnderlyingPtr = nullptr;
    }

    bool expired() const noexcept
    {
        return mUnderlyingPtr ? true : false;
    }
};

template <typename T>
void swap(WeakPtr<T>& first, WeakPtr<T>& second)
{ // jj is used as my esc to move around modes in vim
    //Ctrl+w      " deletes the whole word behind the cursor
   std::swap(first.mUnderlyingPtr, second.mUnderlyingPtr); 
}