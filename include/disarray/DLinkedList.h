#ifndef _DLINKED_LIST_H
#define _DLINKED_LIST_H


//

template <class T>
class DLinkedList;

template <class T>
class DLinkedListIterator;
//=========================================
template <class T>
class Link{
    friend class DLinkedList<T>;
    friend class DLinkedListIterator<T>;
protected:
    T* var;
    long id;
    Link<T>* next;
    Link<T>* prev;


    Link(T& nvar, Link<T>* nxt, Link<T>* prv){
        var=new T;
        *var=nvar;
        next=nxt;
        prev=prv;
    }

    Link(){
        prev=next=this;
    }

    ~Link(){
        if (var)
         delete var;
        var=0;
        next=0;
        prev=0;
    }

};

//=========================================
template <class T>
class DLinkedList{

friend class DLinkedListIterator<T>;

Link<T> * head;



 Link<T>* findById(long sid){
    long nid=-1;

    Link<T>* block=head;
    nid=head->id;
    while ((sid!=nid)&&(block)){
        block=block->next;
        if (block)
            nid=block->id;
    }
    if (block)
        return block;
    return 0;
 
}

//---------------------------------
 void removeByAddress(Link<T>* current){

    Link<T>* target=current;
    Link<T>* previous=0;
    Link<T>* nextone=0;
    if (target->prev)
     previous=target->prev;
    if (target->next)
     nextone=target->next;
    if (previous)
     previous->next=nextone;
    if (nextone)
     nextone->prev=previous;
    
    if (target){
     delete target;

     if (target==head)
      head=nextone;
     
    }

 }



public:

 DLinkedList(){head=0;}

 //---------------------------------
 void add(T& newvar){

     if (!head){
        head=new Link<T>(newvar,0,0);
        head->id=0;
     }
    else{
        
        Link<T>* end=head;
        while (end->next)
            end=end->next;
        end->next=new Link<T>(newvar,0,end);
        end->next->id=(end->id)+1;
        
    }
 }
 //---------------------------------
 T * get(long id){
    Link<T>* target=findById(id);
    if (target)
        return target->var;
    return 0;
 }
 //---------------------------------
 void removeByID(long id){
    Link<T>* target=findById(id);
    Link<T>* previous=0;
    Link<T>* nextone=0;
    if (target->prev)
     previous=target->prev;
    if (target->next)
     nextone=target->next;
    if (previous)
     previous->next=nextone;
    if (nextone)
     nextone->prev=previous;
    
    if (target){
     delete target;

     if (target==head)
      head=nextone;
    }
 }

//---------------------------------
 T& operator[](long index) { 
    
    long ind=0;
    Link<T>* end=head;
    while ((end->next)&&(ind!=index)){
        ind++;
        end=end->next;
    }
    return end; 
}

 //---------------------------------
 void destroy(){
    while (head){
        removeByID(head->id);
    }
    head=0;
 }
 //-----------------------------
 
};

//==========================================
template <class T>
class DLinkedListIterator{
    Link<T>* link;
    DLinkedList<T> *list;
public:
    //---------------------
    DLinkedListIterator(){
        link=0;
    }
    //---------------------------
    void reset(DLinkedList<T> * _list){
        link=(list=_list)->head;
    }
    //---------------------------
    T* getvalue(){
        if (link)
            return link->var;
        else
            return 0;
    }
    //---------------------------
    bool nextStep(){
        if (link->next){
            link=link->next;
            return true;
        }
        return false;
    }
    //---------------------------
    bool prevStep(){
        if (link->prev){
            link=link->prev;
            return true;
        }
        return false;

    }
    //---------------------------
    void removeThis(){
        bool isLast=false;
        Link<T>* linkas=0;
        if (nextStep()){
            linkas=link->prev;
        }
        else{
            linkas=link;
            isLast=true;
        }

        if (linkas)
            list->removeByAddress(linkas);
        if (isLast)
            link=list->head;
        
        
    }

};






#endif //_DLINKED_LIST_H
