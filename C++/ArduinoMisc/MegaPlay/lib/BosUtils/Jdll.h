#ifndef JDLL_H
#define JDLL_H

namespace bosley
{

template<class T>
class JDLL
{
public:

    JDLL();
    ~JDLL();

    void pushBack(T data);
    void removeItem(T data);
    void removeIndex(int);
    int retrieveIndex(T data);
    T retrieveDataByIndex(int);

    int size;

private:

    typedef struct n
    {
        T data;
        n* _next;
        n* _prev;
    }* node;

    node _head;
    node _temp;
    node _tail;
};


template<class T>
JDLL<T>::JDLL()
{
    this->_head = nullptr;
    this->_temp = nullptr;
    this->_tail = nullptr;
    this->size = 0;
}

template<class T>
JDLL<T>::~JDLL()
{
}

template<class T>
void JDLL<T>::pushBack(T data)
{
    node nn = new n;
    nn->_next = nullptr;
    nn->_prev = nullptr;
    nn->data = data;

    if(this->_tail == nullptr)
    {
        this->_head = nn;
        this->_tail = nn;
    }
    else
    {
        this->_tail->_next = nn;
        nn->_prev = this->_tail;
        this->_tail = nn;
    }
    this->size++;
}

template<class T>
void JDLL<T>::removeItem(T data)
{
    node dn = new n;
    this->_temp = this->_head;
    while(this->_temp->_next != nullptr)
    {
        if(this->_temp->data == data)
        {
            dn = this->_temp;
            if(this->_temp == this->_head)
                this->_head = this->_temp->_next;
            else if (this->_temp == this->_tail)
                this->_tail = _temp->_prev;
            else
            {
                this->_temp->_next->_prev = this->_temp->_prev;
                this->_temp->_prev->_next = this->_temp->_next;
            }
        }
        this->_temp = this->_temp->_next;
        delete dn;
    }
}

template<class T>
void JDLL<T>::removeIndex(int index)
{
    if( index > -1 && index <= this->size)
    {
        int i = 0;
        node dn = new n;
        this->_temp = this->_head;
        while(i != index)
        {
            this->_temp = this->_temp->_next;
            i++;
        }
        dn = this->_temp;

        if(this->_temp == this->_head)
            this->_head = this->_temp->_next;
        else if (this->_temp == this->_tail)
            this->_tail = _temp->_prev;
        else
        {
            this->_temp->_next->_prev = this->_temp->_prev;
            this->_temp->_prev->_next = this->_temp->_next;
        }

        this->size--;
        delete dn;
    }
}

template<class T>
int JDLL<T>::retrieveIndex(T data)
{
    int i = 0;
    this->_temp = this->_head;
    while(this->_temp->_next != nullptr)
    {
        if(this->_temp->data == data)
        {
            return i;
        }
        this->_temp = this->_temp->_next;
        i++;
    }
    return -1;
}

template<class T>
T JDLL<T>::retrieveDataByIndex(int index)
{
    if( index > -1 && index <= this->size)
    {
        int i = 0;
        this->_temp = this->_head;
        while(i != index)
        {
            this->_temp = this->_temp->_next;
            i++;
        }
        return this->_temp->data;
    }
    return T();
}

}
#endif // JDLL_H