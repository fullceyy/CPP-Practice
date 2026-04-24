#include <cstdint>
#include <memory>
#pragma once

template <typename P, typename D>
class CtrlBlock
{ // is there a reason to hide ctrl block internals via access speicifier if the CtrlBlock itself is an internal for SPtr.
public:
    P* mPurePtr;
    D mDeleter;
};

template <typename T>
class SharedPointer
{
private:
    T* mUnderlyingPtr {nullptr};
    uint32_t* mCount {nullptr};
public:
    void cleanup()
    {
        delete mUnderlyingPtr;
        delete mCount;
    }

    void release()
    {
        if(mCount)
        {
            (*mCount)--;
            if(*mCount == 0)
                cleanup();
        }
        mUnderlyingPtr = nullptr;
        mCount = nullptr;
    }

    T* get() const
    {
        return mUnderlyingPtr;
    }

    uint32_t& UseCount() const
    {
        return *mCount;
    }
    // or 
    // uint32_t UseCount() const
    // {
    //     return *mCount;
    // }

    T* operator->() const noexcept
    {
        return get();
    }

    T& operator*() const noexcept
    {
        return *get();
    }

    SharedPointer() {}
    ~SharedPointer() { release(); }

    SharedPointer(T* ptr)
    {
        if(ptr)
        {
            mUnderlyingPtr = ptr;
            mCount = new uint32_t(1);
            return;
        } 

        throw std::invalid_argument("passed ptr cannot be null!");
    }

    SharedPointer(const SharedPointer<T>& other)
    {
        if(other.mUnderlyingPtr)
        {
            mUnderlyingPtr = other.mUnderlyingPtr;
            mCount = other.mCount;
            (*mCount)++;
        }
    }

    SharedPointer& operator=(const SharedPointer<T>& other)
    {
        if((&other) == this)
            return *this;

        if(mUnderlyingPtr)
            this->release();
        
        if(other.mUnderlyingPtr)
        {
            mUnderlyingPtr = other.mUnderlyingPtr;
            mCount = other.mCount;
            (*mCount)++;
        }
        return *this;
    }

    SharedPointer(SharedPointer<T>&& other)
    {
        if(other.mUnderlyingPtr)
        {
            mUnderlyingPtr = other.mUnderlyingPtr;
            mCount = other.mCount;
            (*mCount)++;
        }

        other.mUnderlyingPtr = nullptr;
        other.mCount = nullptr;
    }

    SharedPointer&& operator=(SharedPointer<T>&& other)
    {
        if((&other) == this)
            return *this;
        
        if(mUnderlyingPtr)
            this->release();
        
        if(other.mUnderlyingPtr)
        {
            mUnderlyingPtr = other.mUnderlyingPtr;
            mCount = other.mCount;
            (*mCount)++;

            other.release();
        }
        return *this;
    }
};

template<typename T, typename ...Args>
SharedPointer<T> MakeShared(Args&& ...args)
{
    return SharedPointer<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
SharedPointer<T> MakeSharedArray(std::size_t size)
{
    using U = std::remove_extent_t<T>;
    U* data = new U[size]();

    return SharedPointer<T>(data, std::default_delete<U[]>());
}