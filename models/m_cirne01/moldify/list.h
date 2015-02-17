/**
 *  sss - Space Shared Scheduler
 *  List
 **/


#ifndef __LIST_H
#define __LIST_H


#include <stdlib.h>

#include "util.h"
#include "random.h"


template <class Elem>
class Node {
public:
	Elem* n_elem;
	Node<Elem>* n_next;
	Node<Elem>* n_prev;
	Node( Elem* elem, Node<Elem>* next = NULL, Node<Elem>* prev = NULL ){
		n_elem = elem;
		n_next = next;
		n_prev = prev;
	}
};


template <class Elem>
class List
{

	Node<Elem>* l_list;
	Node<Elem>* l_current;
	int l_numelems;

public:

	List() {
		l_list = new Node<Elem>( NULL );
		l_list->n_next = l_list->n_prev = l_list;
		l_current = NULL;
		l_numelems = 0;
	}

	void add( Elem* newelem );

	Elem* find( int id );
	Elem* pickrandomly();
	Elem* getfirst();
	Elem* getnext();

	Elem* removefirst();
	Elem* removerandomly();

	void replace( Elem* oldelem, Elem* newelem );

	int numelems(){
		return l_numelems;
	}

};


template <class Elem> 
void List<Elem>::add( Elem* newelem )
{
	Node<Elem>* newnode = new Node<Elem>( newelem );
	Node<Elem>* node = l_list->n_next;
	while( node != l_list && node->n_elem->rank() < newelem->rank() ){
		node = node->n_next;
	}
	node->n_prev->n_next = newnode;
	newnode->n_prev = node->n_prev;
	node->n_prev = newnode;
	newnode->n_next = node;
	l_numelems++;
}


template <class Elem> 
Elem* List<Elem>::find( int id )
{
	Node<Elem>* node = l_list->n_next;
	while( node != l_list && node->n_elem->id() != id ){
		node = node->n_next;
	}
	if( node == l_list ){
		return NULL;
	}
	else {
		return node->n_elem;
	}
}


template <class Elem> 
Elem* List<Elem>::pickrandomly()
{
	if( l_list->n_next == l_list ){
		return NULL;
	}
	int picked = UniformRandom( 0, l_numelems - 1 );
	Node<Elem>* node = l_list->n_next;
	while( picked > 0 ){
		picked--;
		node = node->n_next;
		if( node == l_list ){
			fatal( "broken invariant in List<Elem>::pickrandomly()" );
		}
	}
	return node->n_elem;
}


template <class Elem> 
Elem* List<Elem>::getfirst()
{
	l_current = l_list->n_next;
	return getnext();
}


template <class Elem> 
Elem* List<Elem>::getnext()
{
	if( l_current == l_list ){
		return NULL;
	}
	else {
		Elem* result = l_current->n_elem;
		l_current = l_current->n_next;
		return result;
	}
}


template <class Elem> 
Elem* List<Elem>::removefirst()
{
	if( l_list->n_next == l_list ){
		return NULL;
	}
	else {
		Node<Elem>* node = l_list->n_next;
		Elem* result = node->n_elem;
		l_list->n_next = node->n_next;
		node->n_next->n_prev = l_list;
		delete node;
		l_numelems--;
		return result;
	}
}


template <class Elem> 
Elem* List<Elem>::removerandomly()
{
	if( l_list->n_next == l_list ){
		return NULL;
	}
	int picked = UniformRandom( 0, l_numelems - 1 );
	Node<Elem>* node = l_list->n_next;
	while( picked > 0 ){
		picked--;
		node = node->n_next;
		if( node == l_list ){
			fatal( "broken invariant in List<Elem>::removerandomly()" );
		}
	}
	Elem* result = node->n_elem;
	node->n_prev->n_next = node->n_next;
	node->n_next->n_prev = node->n_prev;
	delete node;
	l_numelems--;
	return result;
}


template <class Elem> 
void List<Elem>::replace( Elem* oldelem, Elem* newelem ){
	Node<Elem>* node = l_list->n_next;
	while( node->n_elem != oldelem && node->n_next != l_list ){
		node = node->n_next;
	}
	if( node->n_elem == oldelem ){
		node->n_elem = newelem;
	}
}


#endif

