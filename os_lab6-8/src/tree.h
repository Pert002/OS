#ifndef TREE_H
#define TREE_H

#include <vector>

using namespace std;

class node{
private:
	int val;
	node *left;
	node *right;
public:
	node();
	node(int value);
	int get_val();
	node*& get_left();
	node*& get_right();
};

class binary_tree{
private:
	node *head;
	bool find_(node *node, int value);
	void add_(node*& node, int value);
	void print_(node *node, int h);
	void vectorize_values(node *node, vector <int>& v);
	int get_parent(node*& cur, int val);
	void delete_el_(node*& n, int val);
	void delete_tree(node*& n);
public:
	void print();
	binary_tree();
	binary_tree(int val);
	void add(int val);
	void remove(int val);
	bool find(int val);
	void delete_el(int val);
	int get_place(int val);
	vector <int> get_all_values();
};

#endif