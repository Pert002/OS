#include "tree.h"
#include <vector>
#include <iostream>

using namespace std;

node::node(){
	left = NULL;
	right = NULL;
}

node::node(int value){
	val = value;
	left = NULL;
	right = NULL;
}

int node::get_val(){
	return val;
}

node*& node::get_left(){
	return left;
}

node*& node::get_right(){
	return right;
}

binary_tree::binary_tree(){
	head = NULL;
}

binary_tree::binary_tree(int val){
	node *n = new node(val);
	head = n;
}

void binary_tree::add_(node*& n, int value){
	if (n == NULL){
		n = new node(value);
	}
	else if(value >= n->get_val()){
		return add_(n->get_right(), value);
	}
	else if(value < n->get_val()){
		return add_(n->get_left(), value);
	}
}

void binary_tree::add(int value){
	add_(head, value);
}

bool binary_tree::find_(node* node, int value){
	if(node == NULL)
		return false;
	else if(value > node->get_val())
		return find_(node->get_right(), value);
	else if(value < node->get_val())
		return find_(node->get_left(), value);
	return true;
}

bool binary_tree::find(int value){
	return find_(head, value);
}

void binary_tree::print_(node *node, int h){
	if(node == NULL)
		return;
	print_(node->get_left(), h + 1);
	for(int i = 0; i < h; i++)
		cout << "  ";
	cout << node->get_val() << endl;
	print_(node->get_right(), h + 1);
}

void binary_tree::print(){
	print_(head, 0);
}

void binary_tree::vectorize_values(node *node, vector <int>& v){
	if (node == NULL)
		return;
	vectorize_values(node->get_left(), v);
	v.push_back(node->get_val());
	vectorize_values(node->get_right(), v);
}

vector <int> binary_tree::get_all_values(){
	vector <int> v;
	vectorize_values(head, v);
	return v;
}

void binary_tree::delete_tree(node*& n){
	if(!n) 
		return;
	delete_tree(n->get_left());
	delete_tree(n->get_right());
	delete n;
}

void binary_tree::delete_el_(node*& n, int val){
	if(!n) 
		return;
	if(val < n->get_val()){
		delete_el_(n->get_left(), val);
	} 
	else if(val > n->get_val()){
		delete_el_(n->get_right(), val);
	} 
	else{
		delete_tree(n);
		n = NULL;
	}
}

void binary_tree::delete_el(int val){
	delete_el_(head, val);
}

int binary_tree::get_parent(node*& n, int val){
	if(val < n->get_val()){
		if(n->get_left() == nullptr) 
			return n->get_val();
		return get_parent(n->get_left(), val);
	} else if(val > n->get_val()){
		if(n->get_right() == nullptr) 
			return n->get_val();
		return get_parent(n->get_right(), val);
	}
	return -1;
}

int binary_tree::get_place(int val){
	return get_parent(head, val);
}



