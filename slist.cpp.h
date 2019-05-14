#include "slist.h"

template<class Object>
SList<Object>::SList( ) {
    init( );
}

template<class Object>
SList<Object>::SList( const SList &rhs ) {
    init( );
    *this = rhs;                                   // then assign rhs to this.
}

template<class Object>
void SList<Object>::init( ) {
    for ( int i = 0; i < LEVEL; i++ ) {  // for each level
        // create the left most dummy nodes;
        header[i] = new SListNode<Object>;
        header[i]->prev = NULL;
        header[i]->down = ( i > 0 ) ? header[i - 1] : NULL;
        header[i]->up = NULL;
        if ( i > 0 ) header[i - 1]->up = header[i];

        // create the right most dummy nodes;
        header[i]->next = new SListNode<Object>;
        header[i]->next->next = NULL;
        header[i]->next->prev = header[i];
        header[i]->next->down = ( i > 0 ) ? header[i - 1]->next : NULL;
        header[i]->next->up = NULL;
        if ( i > 0 ) header[i - 1]->next->up = header[i]->next;
    }

    // reset cost.
    cost = 0;
}

template<class Object>
SList<Object>::~SList( ) {
    clear( );                                      // delete items starting 1st
    for ( int i = 0; i < LEVEL; i++ ) {
        delete header[i]->next;                      // delete the right most dummy
        delete header[i];                            // delete the left most dummy
    }
}

template<class Object>
bool SList<Object>::isEmpty( ) const {
    return ( header[0]->next->next == NULL );
}

template<class Object>
int SList<Object>::size( ) const {
    SListNode<Object> *p = header[0]->next; // at least the right most dummy
    int size = 0;

    for ( ; p->next != NULL; p = p->next, ++size );
    return size;
}

template<class Object>
void SList<Object>::clear( ) {
    for ( int i = 0; i < LEVEL; i++ ) {        // for each level
        SListNode<Object> *p = header[i]->next;  // get the 1st item
        while ( p->next != NULL ) {              // if this is not the left most
            SListNode<Object> *del = p;
            p = p->next;                           // get the next item
            delete del;                            // delete the current item
        }

        header[i]->next = p;                     // p now points to the left most
    }                                          // let the right most point to it
}

template<class Object>
void SList<Object>::insert( const Object &obj ) {
    // right points to the level-0 item before which a new object is inserted.
    SListNode<Object> *right = searchPointer( obj );
    SListNode<Object> *up = NULL;

    if ( right->next != NULL && right->item == obj )
        // there is an identical object
        return;

    // Implemented by Daria Sykuleva /////////////////////////////////////////
    int level = 0;   //start from the bottom level
    bool exitIndicator = false;   //set a variable to exit a while loop
    SListNode<Object> *inserted = NULL;
    while (!exitIndicator) {
        inserted = insertToTheLeft(obj, right, inserted);
        if (rand() % 2 == 1) {   //randomly decide whether to copy the node to the next level
            SListNode<Object> *left = inserted->prev;
            while (left->up == NULL) {  //search for the next item with not NULL "up"
                left = left->prev;
            }
            left = left->up;
            level++;
            right = left->next;
            if (level >= LEVEL - 1) {  //condition of the top level
                exitIndicator = true;
            }
        } else {
            exitIndicator = true;
        }
    }
}

template<class Object>
SListNode<Object> *SList<Object>::insertToTheLeft(const Object &obj, SListNode<Object> *right,
                                                  SListNode<Object> *prevInserted) {
    SListNode<Object> *inserted = new SListNode<Object>;  //deleted in remove method
    inserted->item = obj;   //set an object to inserted node
    SListNode<Object> *left = right->prev;
    //redirect the pointers from left and right to inserted
    left->next = inserted;
    inserted->next = right;
    right->prev = inserted;
    inserted->prev = left;
    if (prevInserted != NULL) {  //connect the nodes in the up and down directions
        inserted->down = prevInserted;
        prevInserted->up = inserted;
    }
    return inserted;
}

template<class Object>
bool SList<Object>::find( const Object &obj ) {
    // p oints to the level-0 item close to a given object
    SListNode<Object> *p = searchPointer( obj );

    return ( p->next != NULL && p->item == obj );     // true if obj was found
}

template<class Object>
SListNode<Object> *SList<Object>::searchPointer( const Object &obj ) {
    SListNode<Object> *p = header[LEVEL - 1];     // start from the top left
    while ( p->down != NULL ) {                   // toward level 0
        p = p->down;                                // shift down once
        cost++;

        if ( p->prev == NULL ) {                    // at the left most item
            if ( p->next->next == NULL )              // no intermediate items
                continue;
            else {                                    // some intermadiate items
                if ( p->next->item <= obj )             // if 1st item <= obj
                    p = p->next;                          // shift right to item 1
                cost++;
            }
        }

        while ( p->next->next != NULL && p->next->item <= obj ) {
            // shift right through intermediate items as far as the item value <= obj
            p = p->next;
            cost++;
        }
    }

    // now reached the bottom. shift right once if the current item < obj
    if ( p->prev == NULL || p->item < obj ) {
        p = p->next;
        cost++;
    }
    return p; // return the pointer to an item >= a given object.
}

template<class Object>
void SList<Object>::remove( const Object &obj ) {
    // p points to the level-0 item to delete
    SListNode<Object> *p = searchPointer( obj );

    // validate if p is not the left most or right most and exactly contains the
    // item to delete
    if ( p->prev == NULL || p->next == NULL || p->item != obj )
        return;

    // Implemented by Daria Sykuleva /////////////////////////////////////////
    else {
        while (p != NULL) {  //deleting all the elements in the column (up list)
            SListNode<Object> *left = p->prev;
            SListNode<Object> *right = p->next;
            SListNode<Object> *current = p;
            //make the connections between left and right
            left->next = right;
            right->prev = left;
            p = p->up;    //move one level up
            delete current;   //delete node
        }
    }
}

template<class Object>
const SList<Object> &SList<Object>::operator=( const SList &rhs ) {
    if ( this != &rhs ) { // avoid self-assignment
        clear( );           // deallocate old items

        int index;
        SListNode<Object> *rnode;
        for ( index = 0, rnode = rhs.header[0]->next; rnode->next != NULL;
              rnode = rnode->next, ++index )
            insert( rnode->item );

        cost = rhs.cost;
    }
    return *this;
}

template<class Object>
int SList<Object>::getCost( ) const {
    return cost;
}

template<class Object>
void SList<Object>::show( ) const {
    cout << "contents:" << endl;
    for ( SListNode<Object> *col = header[0]; col != NULL; col = col->next ) {
        SListNode<Object> *row = col;
        for ( int level = 0; row != NULL && level < LEVEL; level++ ) {
            if ( row->prev == NULL )
                cout << "-inf\t";
            else if ( row->next == NULL )
                cout << "+inf\t";
            else
                cout << row->item << "\t";
            row = row->up;
        }
        cout << endl;
    }
}
