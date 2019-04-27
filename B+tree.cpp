#include <iostream>
#include <vector>
#define d 2 //degree
using namespace std;
int flag = 0;
class TreeNode{

public:

	int m;
	int pm;
	vector<int>key;
	vector<TreeNode*>pointer;
	bool isLeaf;
	TreeNode * left;
	TreeNode * right;

	TreeNode(bool isLeaf){

		m = 0;
		pm = 0;
		key.resize(2 * d + 1); // one for buffer
		pointer.resize(2 * d + 2,NULL); // one for buffer
		this -> isLeaf = isLeaf;
		left = right = NULL;

	}

	void insertKey(const int & value){

		int i;
		for(i = m ; i >= 1 ; i -- ){
			if(key[i - 1] > value){
				key[i] = key[i-1];
			}
			else{
				key[i] = value;
				break;
			}
		}
		if(!i) key[i] = value;
		m ++ ;

	}

	void insertPointer(TreeNode * t){

		int i;
		for(i = pm ; i >= 1 ; i -- ){
			if(pointer[i - 1] -> key[0] > t -> key[0]){
				pointer[i] = pointer[i - 1];
			}
			else{
				pointer[i] = t;
				break;
			}
		}
		if(!i) pointer[i] = t;
		pm ++;

	}

	int popKey(){

		m -- ;
		return key[m];

	}

	int popFrontKey(){

		int frontKey = key[0];
		for(int i = 1 ; i < m ; i ++ )
			key[i-1] = key[i];
		m -- ;
		return frontKey;

	}

	TreeNode * popPointer(){

		pm -- ;
		return pointer[pm];

	}

	//insert after t1.
	void makeLink(TreeNode * t2){

		t2 -> left = this;
		t2 -> right = this -> right;
		this -> right = t2;
		if(t2 -> right)
			t2 -> right -> left = t2;

	}

	bool deleteKey(const int & t){

		bool found = 0;
		for(int i = 0 ; i < m ; i ++ ){
			if(found){
				key[i-1] = key[i];
				continue;
			}
			if(key[i] == t){
				found = 1;
			}
		}
		m -= found;
		return found;

	}

	int backKey(){

		return key[m - 1];

	}

	int frontKey(){

		return key[0];

	}

};


class BPlusTree{

private:

	bool default_search(const int & key,TreeNode * ptr){

		if(ptr -> isLeaf){
			for(int i = 0 ; i < ptr -> m ; i ++ )
				if(ptr -> key[i] == key)
					return true;
			return false;
		}

		if(!ptr -> m)
			return false;

		if(key < ptr -> key[0])
			return default_search(key,ptr -> pointer[0]);

		if(key > ptr -> key[ptr -> m])
			return default_search(key,ptr -> pointer[ptr -> m]);

		for(int i = 0 ; i < ptr -> m - 1; i ++ )
			if(key >= ptr -> key[i] && key < ptr -> key[i + 1])
				return default_search(key,ptr -> pointer[i + 1]);

		return false;

	}

	TreeNode * defalut_insert(const int & key,TreeNode * ptr){

		if(ptr -> isLeaf){ //root initial ?

			ptr -> insertKey(key);

			if( ptr -> m <= 2 * d ) 
				return NULL;

			TreeNode * t = new TreeNode(true);
			for(int i = 0 ; i < d + 1 ; i ++ )
				t -> insertKey(ptr -> popKey());
			ptr -> makeLink(t);
			return t; // 

		}

		//non-leaf node
		TreeNode * tmp;
		if(key < ptr -> key[0])
			tmp = defalut_insert(key, ptr -> pointer[0]);
		else if(key > ptr -> key[ptr -> m - 1])
			tmp = defalut_insert(key, ptr -> pointer[ptr -> m]);
		else
			for(int i = 0 ; i < ptr -> m - 1 ; i ++ )
				if(key >= ptr -> key[i] && key < ptr -> key[i+1] ){
					tmp = defalut_insert(key, ptr -> pointer[i + 1]);
					break;
				}
		
		if(!tmp)
			return tmp;

		//child split
		ptr -> insertKey(tmp -> key[0]);
		ptr -> insertPointer(tmp);
		if(ptr -> m <= 2 * d)
			return NULL; // can be put into current node

		// split current node
		TreeNode * newT = new TreeNode(false);
		for(int i = 0 ; i < d ; i ++ ){
			newT -> insertKey(ptr -> popKey());
			newT -> insertPointer(ptr -> popPointer());
		}
		newT -> insertPointer(ptr -> popPointer());

		TreeNode * newNode = new TreeNode(false);
		newNode -> insertKey(ptr -> popKey());
		newNode -> insertPointer(ptr);
		newNode -> insertPointer(newT);

		if(ptr == root)
			root = newNode;

		return newNode;

	}	

/*	TreeNode * defalut_delete(const int & key, TreeNode * ptr, TreeNode * par){

		if(ptr -> isLeaf){ //root -> isLeaf recover
			if(!ptr -> deleteKey(key) || ptr -> m >= d || ptr == root)
				return NULL;
			for(int i = 0 ; i < par -> pm ; i ++ ){
				if(par -> pointer[i] == ptr -> left && ptr -> left -> m > d){
					ptr -> insertKey(ptr -> left -> popKey());
					par -> key[i+1] = ptr -> frontKey();
					return NULL;
				}
				else if(par -> pointer[i] == ptr -> right && ptr -> right -> m > d){
					ptr -> insertKey(ptr -> right -> popFrontKey());
					par -> key[i-1] = ptr -> right -> frontKey();
					return NULL;
				}
			}


		}

	}*/

public:

	TreeNode * root;

	BPlusTree(){

		root = new TreeNode(true);

	}

	void bulkLoading(const vector<int> & data){
		//TODO
	}

	bool search(const int & key){

		return default_search(key,root);

	}

	void insert(const int & key){

		if(root -> isLeaf && root -> m == 2 * d)
			root -> isLeaf = false; //
		defalut_insert(key,root);

	}

	/*void Delete(const int & key){
		
		default_delete(key,root,NULL);

	}*/


};

int main(){

	BPlusTree T;

	TreeNode * tt[5];
	for(int i = 0 ; i < 5 ; i ++ )
		tt[i] = new TreeNode(true);
	tt[0] -> insertKey(5);
	tt[1] -> insertKey(10);
	tt[2] -> insertKey(20);
	tt[3] -> insertKey(30);
	tt[4] -> insertKey(40);

	T.root -> insertKey(10);
	T.root -> insertKey(20);
	T.root -> insertKey(30);
	T.root -> insertKey(40);
	T.root -> insertPointer(tt[0]);
	T.root -> insertPointer(tt[1]);
	T.root -> insertPointer(tt[2]);
	T.root -> insertPointer(tt[4]);
	T.root -> insertPointer(tt[3]);

	T.insert(5);
	T.insert(6);
	T.insert(7);
	T.insert(8);
	T.insert(9);
	T.insert(12);
	T.insert(13);
	T.insert(14); 
	/*test case problem: the node may have element less than d*/

/*
	cout << T.root -> key[0] << endl;
	cout << T.root -> pointer[0] -> key[0] << endl;
	cout << T.root -> pointer[0] -> key[1] << endl;
	cout << T.root -> pointer[1] -> key[0] << endl;
	cout << T.root -> pointer[1] -> key[1] << endl;*/

}