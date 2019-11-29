#include <iostream>


template<class T>
struct defaultDelete {
    void operator()(T* ptr)
    {
        delete ptr;
    }

private:
    T* pointedElement;
};

template<class T, class D = defaultDelete<T>>
class UniquePtr
{
private:
    T* pointedElement;

public:
    T* release();
    void reset();
    bool swap(T* other);

    explicit UniquePtr(T *);
    UniquePtr();
    UniquePtr(UniquePtr &other);
    UniquePtr(const UniquePtr &other);
    UniquePtr(UniquePtr&& other);

    ~UniquePtr();

    UniquePtr& operator=(UniquePtr &other);
    UniquePtr& operator=(const UniquePtr &other);
    UniquePtr& operator=(UniquePtr&& other);

    T* operator->();
    T& operator*();
    operator bool();

};

/// Constructors
// Standard constructor
template<class T, class D>
UniquePtr<T, D>::UniquePtr(T *pointTo) : pointedElement(pointTo){}

template<class T, class D>
UniquePtr<T, D>::UniquePtr() {}

// Copy constructor
template<class T, class D>
UniquePtr<T, D>::UniquePtr(UniquePtr &other)
        : pointedElement(other.pointedElement) {}

template<class T, class D>
UniquePtr<T, D>::UniquePtr(const UniquePtr &other)
        : pointedElement(other.pointedElement) {}

// Move constructor
template<class T, class D>
UniquePtr<T, D>::UniquePtr(UniquePtr &&other) : pointedElement(other.pointedElement)
{
    other.pointedElement = nullptr;
}


/// Destructor
template<class T, class D>
UniquePtr<T, D>::~UniquePtr()
{
    D(pointedElement);
}


/// Operators
// Copy assignment operators
template<class T, class D>
UniquePtr<T, D> &UniquePtr<T, D>::operator=(UniquePtr &other)
{
    if(this != &other)
    {
        pointedElement = other.pointedElement;
    }

    return *this;
}

template<class T, class D>
UniquePtr<T, D> &UniquePtr<T, D>::operator=(const UniquePtr &other)
{
    if(this != &other)
    {
        pointedElement = other.pointedElement;
    }

    return *this;
}

// Move assignment operator
template<class T, class D>
UniquePtr<T, D> &UniquePtr<T, D>::operator=(UniquePtr &&other)
{
    if(this != &other)
    {
        pointedElement = other.pointedElement;
        other.pointedElement = nullptr;
    }

    return *this;
}

// Operator overloads
template<class T, class D>
T* UniquePtr<T, D>::operator->()
{
    return pointedElement;
}

template<class T, class D>
UniquePtr<T, D>::operator bool()
{
    return pointedElement != nullptr;
}

template<class T, class D>
T& UniquePtr<T, D>::operator*()
{
    if(pointedElement)
        return *pointedElement;
}


/// Member functions
template<class T, class D>
T *UniquePtr<T, D>::release()
{
    T* temp = pointedElement;
    pointedElement = nullptr;

    return temp;
}

template<class T, class D>
void UniquePtr<T, D>::reset()
{
    pointedElement = nullptr;
}

template<class T, class D>
bool UniquePtr<T, D>::swap(T *other)
{
    if (other)
    {
        pointedElement = other;

        return  true;
    }

    return false;
}


int main() {
    struct Entity {
        int id = -1;
    };

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
        std::cout << "Testing  copy constructor and assignment" << std::endl << std::endl;
        UniquePtr<Entity> otherEntPointer(entityPointer);
        std::cout << otherEntPointer->id << std::endl;
        std::cout << entityPointer->id << std::endl;

        otherEntPointer = entityPointer2;
        std::cout << otherEntPointer->id << std::endl;
        std::cout << entityPointer2->id << std::endl;
        std::cout << std::endl << std::endl;

        // Testing move constructor and assignment
        std::cout << "Testing  copy constructor and assignment" << std::endl << std::endl;
        otherEntPointer = entityPointer2;
        UniquePtr<Entity> movedEntPointer = std::move(otherEntPointer);
        std::cout << movedEntPointer->id << std::endl;
        std::cout << otherEntPointer << std::endl;

        movedEntPointer = std::move(entityPointer);
        std::cout << movedEntPointer->id << std::endl;
        std::cout << entityPointer << std::endl;

    }

    // Testing with examples from cpp-refernce
    {
        {
            UniquePtr<int> foo(new int);
            UniquePtr<int> bar(new int(100));

            *foo = *bar * 2;

            std::cout << "foo: " << *foo << '\n';
            std::cout << "bar: " << *bar << '\n';

        }

        {
            struct C {
                int a;
                int b;
            };

            UniquePtr<C> foo;
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

        {
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