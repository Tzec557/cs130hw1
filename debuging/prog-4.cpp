#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

struct node
{
    node * prev;
    node * next;
    std::string data;

    node(node * p, node * n, const std::string& s)
        :prev(p), next(n), data(s)
    {
    }
};

struct list
{
    node * head;
    node * tail;
    size_t n;
    
    list()
        :head(0), tail(0), n(0)
    {
    }

    ~list()
    {
        // for(node * n = head; n; n = n->next)
        //     delete n;
        node* current = head;
        while (current != nullptr) {
            node* next_node = current->next; // Save the next pointer first
            delete current;                  // Now it's safe to delete
            current = next_node;             // Move to the saved pointer
        }
    }

    size_t size() const
    {
        return n;
    }

    node * append(const std::string& str)
    {
        if(tail)
        {
            tail = tail->next = new node(tail, 0, str);
        }
        else
        {
            tail = head = new node(0, 0, str);
        }
        return tail;
    }

    node * add_after(node* n, const std::string& str)
    {
        node * a = new node(n, n->next, str);
        if (n->next != nullptr)    // <- check if next exists
            n->next->prev = a;
        n->next = a;
        return a;
    }

    // void remove(node* n)
    // {
    //     n->prev->next = n->next;
    //     n->next->prev = n->prev;
    //     delete n;
    // }
    
    void remove(node* n)
    {
        if (n->prev != nullptr)
            n->prev->next = n->next;
        else
            head = n->next;      // removed first node → update head

        if (n->next != nullptr)
            n->next->prev = n->prev;
        else
            tail = n->prev;      // removed last node → update tail

        delete n;
        this->n--;
    }


    void print()
    {
        for(node * n = head; n; n = n->next)
            std::cout << n->data << std::endl;
    }
};

int main()
{
    list L;

    node * a = L.append("A");
    node * c = L.append("C");
    node * e = L.append("E");
    L.print();

    node * b = L.add_after(a, "B");
    node * d = L.add_after(c, "D");
    node * f = L.add_after(e, "F");
    L.print();

    L.remove(a);
    L.remove(d);
    
    L.print();

    return 0;
}

