#include <iostream>
#include <assert.h>


template<class T>
struct defaultDelete {
    void operator()(T* ptr)
    {
        delete ptr;
    }
};

template<class T>
struct arrayDelete {
    void operator()(T* ptr)
    {
        delete[] ptr;
    }
};

template<class T>
struct debugDelete {
    void operator()(T* ptr)
    {
        std::cout << "Object at memory address <" << ptr << "> being deleted now.\n";
        delete ptr;
    }
};

template<class T, class D = defaultDelete<T>>
class UniquePtr
{
private:
    T* pointedElement;
    D deleter;

public:
    T* release();
    void reset();
    T* swap(T* other);

    explicit UniquePtr(T*);
    explicit UniquePtr(T*, D);
    UniquePtr();
    UniquePtr(const UniquePtr &other) = delete;
    UniquePtr(UniquePtr&& other) noexcept;

    ~UniquePtr();

    UniquePtr& operator=(const UniquePtr &other) = delete;
    UniquePtr& operator=(UniquePtr&& other) noexcept;

    T* operator->();
    T operator*() const;
    operator bool() const;

};

/// Constructors
// Standard constructor
template<class T, class D>
UniquePtr<T, D>::UniquePtr(T* pointTo) : pointedElement(pointTo), deleter(defaultDelete<T>()) {}

template<class T, class D>
UniquePtr<T, D>::UniquePtr(T* pointTo, D customDeleter) : pointedElement(pointTo), deleter(customDeleter) {}

template<class T, class D>
UniquePtr<T, D>::UniquePtr() : deleter(defaultDelete<T>()) {}

// Copy constructor
// Should not be used, thus ``= delete;``

// Move constructor
template<class T, class D>
UniquePtr<T, D>::UniquePtr(UniquePtr &&other) noexcept : pointedElement(other.pointedElement), deleter(other.deleter)
{
    other.pointedElement = nullptr;
}


/// Destructor
template<class T, class D>
UniquePtr<T, D>::~UniquePtr()
{
    deleter(pointedElement);
}


/// Operators
// Copy assignment operator
// Should not be used, thus delete


// Move assignment operator
template<class T, class D>
UniquePtr<T, D> &UniquePtr<T, D>::operator=(UniquePtr &&other) noexcept
{
    if(this != &other)
    {
        pointedElement = other.pointedElement;
        deleter = other.deleter;

        other.pointedElement = nullptr;
    }

    return *this;
}

// Operator overloads
template<class T, class D>
T* UniquePtr<T, D>::operator->()
{
    assert(pointedElement != nullptr);

    return pointedElement;
}

template<class T, class D>
UniquePtr<T, D>::operator bool() const
{
    return pointedElement != nullptr;
}

template<class T, class D>
T UniquePtr<T, D>::operator*() const
{
    assert(pointedElement != nullptr);

    return (*pointedElement);
}


/// Member functions
template<class T, class D>
T *UniquePtr<T, D>::release()
{
    T* temp = pointedElement;
    pointedElement = nullptr;

    return temp;
}

/*
 * The assessment's point on this function was kind of confusing ...
 * I thought just resetting the pointer to nullptr would be kind of stupid as you wouldn't deallocate the memory and thus
 * it would be ease for programmer's to cause memory leaks. This is why i have decided to call for the deleter.
 * I researched on cpp-reference where it was done like this.
 */
template<class T, class D>
void UniquePtr<T, D>::reset()
{
    deleter(pointedElement);
    pointedElement = nullptr;
}

/*
 * This point was not very clear to me as well. for me a swap would take the argument of another UniquePtr and then
 * truly swapping their pointed elements. I now interpreted it as it should return the current pointed object and take
 * the new parameter as the member pointer.
 */
template<class T, class D>
T* UniquePtr<T, D>::swap(T* other)
{

    T* temp = pointedElement;
    pointedElement = other;

    return temp;
}


int main() {
    struct Entity {
        int id = -1;
    };

    {
        UniquePtr<Entity, arrayDelete<Entity>> array(new Entity[4], arrayDelete<Entity>());
        UniquePtr<Entity, defaultDelete<Entity>> noArray(new Entity, defaultDelete<Entity>());
        UniquePtr<Entity, debugDelete<Entity>> debug(new Entity, debugDelete<Entity>());

    }

    {
        // Testing basic usage
        std::cout << "Testing constructor" << std::endl << std::endl;
        UniquePtr<Entity> entityPointer(new Entity);
        UniquePtr<std::string> stringPointer(new std::string("Hello World"));

        // Testing -> operator
        std::cout << "Testing \"->\" operator" << std::endl << std::endl;
        std::cout << entityPointer->id << std::endl;
        std::cout << stringPointer->c_str() << std::endl;
        std::cout << std::endl << std::endl;

        // Testing . operator
        std::cout << "Testing \".\" operator" << std::endl << std::endl;
        std::cout << (*entityPointer).id << std::endl;
        std::cout << (*stringPointer).c_str() << std::endl;
        std::cout << std::endl << std::endl;

        // Testing bool operator
        std::cout << "Testing \"bool\" operator" << std::endl << std::endl;
        if (entityPointer && stringPointer) {
            std::cout << "Valid unique_ptrs" << std::endl;
        }

        Entity *ptr = entityPointer.release();
        delete ptr;
        if (entityPointer && stringPointer) {
            std::cout << "Valid unique_ptrs" << std::endl;
        } else {
            std::cout << "Invalid unique_ptrs" << std::endl;
        }
        std::cout << std::endl << std::endl;
    }

    {
        UniquePtr<Entity> entityPointer(new Entity);
        UniquePtr<std::string> stringPointer(new std::string("Hello World"));

        // Testing release function
        std::cout << "Testing release function" << std::endl << std::endl;
        UniquePtr<std::string> releasedPointer(stringPointer.release());
        std::cout << releasedPointer->c_str() << std::endl;
        std::cout << releasedPointer << std::endl;
        std::cout << stringPointer << std::endl;
        std::cout << std::endl << std::endl;

        // Testing reset function
        std::cout << "Testing reset function" << std::endl << std::endl;
        auto* entity2 = new Entity;
        UniquePtr<Entity> entityPointer2(entity2);
        std::cout << "Pre-reset: " << entityPointer2 << std::endl;
        entityPointer2.reset();
        std::cout << "Post-reset: " << entityPointer2 << std::endl;
        std::cout << entity2->id << std::endl;
        std::cout << std::endl << std::endl;


        // Testing swap function
        std::cout << "Testing swap function" << std::endl << std::endl;
        std::cout << stringPointer << std::endl;
        stringPointer.swap(new std::string("Hello Moon"));
        std::cout << stringPointer->c_str() << std::endl;
        //stringPointer.swap(*releasedPointer);
        std::cout << stringPointer->c_str() << std::endl;
        std::cout << std::endl << std::endl;

    }

    {
        UniquePtr<Entity> entityPointer(new Entity);
        UniquePtr<std::string> stringPointer(new std::string("Hello World"));
        auto* entity = new Entity;
        entity->id = 3;
        UniquePtr<Entity> entityPointer2(entity);
        UniquePtr<std::string> stringPointer2(new std::string("Hello Moon"));

        // Testing copy constructor and assignment
        /*std::cout << "Testing  copy constructor and assignment" << std::endl << std::endl;
        UniquePtr<Entity> otherEntPointer(entityPointer);
        std::cout << otherEntPointer->id << std::endl;
        std::cout << entityPointer->id << std::endl;

        otherEntPointer = entityPointer2;
        std::cout << otherEntPointer->id << std::endl;
        std::cout << entityPointer2->id << std::endl;
        std::cout << std::endl << std::endl;*/

        // Testing move constructor and assignment
        std::cout << "Testing  move constructor and assignment" << std::endl << std::endl;
        UniquePtr<Entity> otherEntPointer(new Entity);
        otherEntPointer->id = 5;

        UniquePtr<Entity> movedEntPointer = std::move(otherEntPointer);
        std::cout << movedEntPointer->id << std::endl;
        std::cout << otherEntPointer << std::endl;

        movedEntPointer = std::move(entityPointer);
        std::cout << movedEntPointer->id << std::endl;
        std::cout << entityPointer << std::endl;

    }

    // Testing with examples from cpp-refernce
    {
        /*{
            UniquePtr<int> foo(new int);
            UniquePtr<int> bar(new int(100));

            *foo = *bar * 2;

            std::cout << "foo: " << *foo << '\n';
            std::cout << "bar: " << *bar << '\n';

        }*/

        {
            struct C {
                int a;
                int b;
            };

            UniquePtr<C> foo(new C);
            UniquePtr<C> bar(new C);

            foo->a = 10;
            foo->b = 20;

            bar = std::move(foo);

            if (foo) std::cout << "foo: " << foo->a << ' ' << foo->b << '\n';
            if (bar) std::cout << "bar: " << bar->a << ' ' << bar->b << '\n';
        }

        {
            UniquePtr<int> foo;
            UniquePtr<int> bar (new int(12));

            if (foo) std::cout << "foo points to " << *foo << '\n';
            else std::cout << "foo is empty\n";

            if (bar) std::cout << "bar points to " << *bar << '\n';
            else std::cout << "bar is empty\n";
        }

        /*{
            UniquePtr<int> auto_pointer (new int);
            int * manual_pointer;

            *auto_pointer=10;

            manual_pointer = auto_pointer.release();
            // (auto_pointer is now empty)

            std::cout << "manual_pointer points to " << *manual_pointer << '\n';

            delete manual_pointer;
        }
        /*
        {
            UniquePtr<int> up;  // empty

            up.reset (new int);       // takes ownership of pointer
            *up=5;
            std::cout << *up << '\n';

            up.reset (new int);       // deletes managed object, acquires new pointer
            *up=10;
            std::cout << *up << '\n';

            up.reset();               // deletes managed object
        }

        {
            std::unique_ptr<int> foo (new int(10));
            std::unique_ptr<int> bar (new int(20));

            foo.swap(bar);

            std::cout << "foo: " << *foo << '\n';
            std::cout << "bar: " << *bar << '\n';
        }
        */
    }


    return 0;

}