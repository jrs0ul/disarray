/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2010
 -------------------------------------------
 Dynamic array
 mod. 2010.09.27
 */
#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


template <class T>
class DArray{
    unsigned long _count;
    T* mas;
    unsigned long batch;
    unsigned long capacity;
public:


    DArray(){
        _count=0; mas=0; batch = 1000;
        capacity = batch;

    }
    //----------------------------------
    /*DArray(DArray const & copy){
        this->destroy();
        for (unsigned long i=0;i<copy.count();i++){
            this->add(copy[i]);
        }
    }*/
    //---------------------
    void * getData(){
        return mas;
    }

    //---------------------------
    void add(const T& newitem){

        _count++;

        if (_count == 1)
        {
            mas = (T *)malloc(capacity*sizeof(T));
        }
        else if ( _count > capacity )
        {
            capacity += batch;
            mas = (T *)realloc(mas, sizeof(T) * capacity);
        }

        mas[_count - 1] = newitem;

    }
    //---------------------------
    bool remove(unsigned long index){

        if (index >= _count)
            return false;

        if (_count == 1) {
            this->destroy();

            return true;
        }
        else{
            memmove(mas + index, mas + index + 1, sizeof(T)*((_count - 1) - index));

            _count--;
            if ((capacity - _count) > batch){
                capacity -= batch;
                mas = (T *)realloc(mas, sizeof(T)*capacity);
            }
            return true;

        }

    }
    //---------------------------
    void destroy(){
        if (mas){
            free(mas);
            mas=0;
            _count=0;
            capacity = batch;

        }
    }
    //---------------------------
    unsigned long count(){return _count;}

    //-----------------------------------------------
    T& operator[](unsigned long index) {

        if (index < _count)
            return mas[index];
        else
            return mas[_count - 1];
    }



};



#endif //_DARRAY_H_


