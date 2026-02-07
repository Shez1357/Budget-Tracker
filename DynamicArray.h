#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <iostream>

template <typename T>
class DynamicArray{
    T* array;
    int capacity;
    int current;
    public:

    DynamicArray(){
        array = new T[1];
        capacity = 1;
        current = 0;
    }

    ~DynamicArray(){
        delete[] array;
    }

    void push_back(T data){
        if (current == capacity){
             T* temp = new T[2* capacity];

             for (int i =0; i < capacity; i++){
                temp[i] = array[i];
             }

            delete[] array;
            capacity *= 2;
            array = temp;
        }
        array[current] = data;
        current++;
    }

    void erase(int index){
        if (index < 0 || index >= current){
            std::cerr << "Error: Index out of bounds!" << std::endl;
            return;
        }
         
        for (int i = index; i < current - 1; i++){
            array[i] = array[i+1];
        }
        current--;
    }
    
    void clear(){
        current = 0;
    }

    T* begin(){
        return array;
    }

    T* end(){
        return array + current;
    }

    int size(){
        return current;
    }

    T& operator[](int index){
        return array[index];
    }


};

#endif
