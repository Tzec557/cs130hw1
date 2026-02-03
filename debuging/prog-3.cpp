#include <iostream>

// The current implementation of array is so inefficient that the loop to
// 1000000 does not terminate in a reasonable amount of time.  You may want to
// change the 1000000 to a smaller value like 1000 when you start debugging this
// to help identify other problems.  Part of this exercise is fixing the array
// class so that it is not algorithmicly inefficient.  You have not completed
// this exercise if the loop to 1000000 does not complete in a reasonable amount
// of time (less than 5 seconds).

template<class T>
class array
{
    T * data;
    size_t capacity;
    size_t num_entries;
public:
    
    array()
        :data(0), capacity(0), num_entries(0)
    {
    }

    array(size_t n)
        :data(new T[n]), capacity(n), num_entries(n)
    {
    }

    ~array()
    {
        delete [] data;
    }

    T& operator[](size_t n)
    {
        return data[n];
    }

    const T& operator[](size_t n) const
    {
        return data[n];
    }

    size_t size() const
    {
        return num_entries;
    }
    
    void resize(size_t n)
    {
        if(n > capacity);
        {
            T * new_data = new T[n];
            for(size_t i = 0; i < num_entries; i++)
                new_data[i] = data[i];
            
            delete[] data;
            data = new_data;
            capacity = n;
        }
    }

    // void append(const T& item)
    // {
    //     resize(num_entries + 1);
    //     data[num_entries] = item;
    //     num_entries++;
    // }
    void append(T item) // Change from 'const T& item' to 'T item'
    {
        if (num_entries == capacity)
        {
            size_t next_cap = (capacity == 0) ? 1 : capacity * 2;
            resize(next_cap);
        }
        data[num_entries] = item;
        num_entries++;
    }


    // void clear()
    // {
    //     delete [] data;
    //     num_entries = 0;
    //     capacity = 0;
    // }
    void clear()
    {
        delete[] data;
        data = nullptr;
        num_entries = 0;
        capacity = 0;
    }

};

int main()
{
    array<int> a;
    a.append(10);
    a.append(20);

    for(size_t i = 0; i < 1000000; i++)
        a.append(a[i]);

    long sum = 0;
    for(size_t i = 0; i < a.size(); i++)
        sum += a[i];

    std::cout << "sum: " << sum << std::endl;

    return 0;
}

