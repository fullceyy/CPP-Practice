#pragma once
#include <cstdint>
#include <memory>

template <typename T>
struct CustomSimpleDeleter
{
    using U = std::remove_extent_t<T>;
    void operator()(U* ptr) const noexcept 
    { 
    if constexpr(std::is_array_v<T>)
        delete[] ptr;
    else
        delete ptr;
    }
};

template <typename T, typename D = CustomSimpleDeleter<T>>
class ControlBlock
{
public:
    using U = std::remove_extent_t<T>;
    D mControlBlockDeleter;
    U* mControlBlockRawPtr {nullptr};
    uint32_t* mStrongCount {nullptr};
    uint32_t* mWeakCount   {nullptr};

    ControlBlock() {}
    ~ControlBlock() {}
};

template <typename T, typename D = CustomSimpleDeleter<T>>
class SharedSmartPointer
{
private:
    /*
        if passed float[] for example, it is not gonna strip away at it.
        it passes it to the control block which in return handles the stripping process.
        std::remove_extent_t I've meant.

        Maybe then we can actually hold two types of the data.
        Control Block makes sure to hold the right stripped away data so the pointer is set correctly.
        Shared Smart Pointer holds the actual type of data without it being stripped away, 
        maybe there is something to it.
    */
    ControlBlock<T, D>* mBlock {nullptr};
public:
    SharedSmartPointer() : mBlock(new ControlBlock<T, D>()) {}
    ~SharedSmartPointer() {}

    U* get() const noexcept
    {
        return static_cast<U*>(mBlock->mControlBlockRawPtr);
    }
    
    U* operator->() const noexcept
    {
        return get();
    }

    U* operator*() const noexcept
    {
        return *get();
    }

    SharedSmartPointer(T* ptr)
    {
        
    }
};

// does not support 2D arrays
template <typename T, typename D = CustomSimpleDeleter<T>>
class SharedPointer
{
private:
    using U = std::remove_extent_t<T>;
    U* mUnderlyingPtr {nullptr};
    uint32_t* mCount {nullptr};
    D mDeleter;

    void reset()
    {
        mUnderlyingPtr = nullptr;
        mCount = nullptr;
    }
public:

    bool operator==(const SharedPointer<T>& other)
    {
        return mUnderlyingPtr == other.mUnderlyingPtr; 
    }

    const U& operator[](int idx) requires std::is_array_v<T>
    {
        // if constexpr(std::is_array_v<T>)
            return mUnderlyingPtr[idx];
    }

    void cleanup()
    {
        mDeleter(mUnderlyingPtr);
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

    U* get() const
    {
        return mUnderlyingPtr;
    }

    uint32_t& UseCount() const
    {
        return *mCount;
    }

    U* operator->() const noexcept
    {
        return get();
    }

    U& operator*() const noexcept
    {
        return *get();
    }

    SharedPointer() {}
    ~SharedPointer() { release(); }

    SharedPointer(U* ptr)
    {
        if(ptr)
        {
            mUnderlyingPtr = ptr;
            mCount = new uint32_t(1);
            return;
        } 
        throw std::invalid_argument("passed ptr cannot be null!");
    }

    SharedPointer(U* ptr, D deleter)
    {
        if(ptr)
        {
            mUnderlyingPtr = ptr;
            mCount = new uint32_t(1);
            mDeleter = std::move(deleter);
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
        }

        other.reset();
    }

    SharedPointer& operator=(SharedPointer<T>&& other)
    {
        if((&other) == this)
            return *this;
        
        if(mUnderlyingPtr)
            this->release();
        
        if(other.mUnderlyingPtr)
        {
            mUnderlyingPtr = other.mUnderlyingPtr;
            mCount = other.mCount;

            other.reset();
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

    return SharedPointer<T>(data);
}

template <typename T>
void swap(SharedPointer<T>& first, SharedPointer<T>& second) noexcept
{
    std::swap(first, second);    
}