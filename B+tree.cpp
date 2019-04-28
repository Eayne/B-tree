#include <iostream>
#include <queue>
#include <vector>
#define d 2 //degree
using namespace std;
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

	bool popPointer(TreeNode * t){ //only move ,not free

		bool found = 0;
		for(int i = 0 ; i < pm ; i ++ ){
			if(found){
				pointer[i-1] = pointer[i];
				continue;
			}
			if(pointer[i] == t){
				found = 1;
			}
		}
		pm -= found;
		return found;

	}

	//insert after t1.
	void makeLink(TreeNode * t2){

		t2 -> left = this;
		t2 -> right = this -> right;
		this -> right = t2;
		if(t2 -> right)
			t2 -> right -> left = t2;

	}

	bool deleteKey(const int & t){ //free

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

	bool deletePointer(TreeNode * t){

		bool found = 0;
		for(int i = 0 ; i < pm ; i ++ ){
			if(found){
				pointer[i-1] = pointer[i];
				continue;
			}
			if(pointer[i] == t){
				found = 1;
				free(pointer[i]);
			}
		}
		pm -= found;
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

/*
	the insert operation mainly consists of 2 steps:copy up and push up

	in leaf level:
		the leaf has space:
			insert ,return NULL
		the leaf is full:
			insert ,split(d and d+1) ,and return the new node to the parent(copy up).
			the parent will insert the key and pointer to this new node

	in node level:
		if the return node is not empty:
			the node has space:
				insert a new key and pointer to the return node
				return NULL;
			the node is full:
				insert , and split(d and d) ,make a new node (1) to this two child split node, 
				and push the new node up(push up)

*/

	TreeNode * defalut_insert(const int & key,TreeNode * ptr){

		if(ptr -> isLeaf){ 
			ptr -> insertKey(key);

			if( ptr -> m <= 2 * d ) 
				return NULL;

			TreeNode * t = new TreeNode(true);
			for(int i = 0 ; i < d + 1 ; i ++ )
				t -> insertKey(ptr -> popKey());
			ptr -> makeLink(t);
			return t; 
		}

		//non-leaf node
		TreeNode * tmp;
		if(key < ptr -> key[0])
			tmp = defalut_insert(key, ptr -> pointer[0]);
		else if(key >= ptr -> key[ptr -> m - 1])
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
		if(!tmp -> isLeaf)
			tmp -> deleteKey(tmp -> frontKey());
		ptr -> insertPointer(tmp);

		if(ptr -> m <= 2 * d)
			return NULL; // can be put into current node

		// split current node
		TreeNode * newT = new TreeNode(false);
		for(int i = 0 ; i < d + 1; i ++ ){
			newT -> insertKey(ptr -> popKey());
			newT -> insertPointer(ptr -> popPointer());
		}

		if(ptr == root){
			TreeNode * newRoot = new TreeNode(false);
			newRoot -> insertKey(newT -> popFrontKey());
			newRoot -> insertPointer(ptr);
			newRoot -> insertPointer(newT);
			root = newRoot;
		}

		return newT;

	}	


/*
	leaf level:
		the leaf has space : delete ((return NULL))
		the leaf doesn't have enough space while the siblings have:
			delete , redistribute , update parent(copy up) (return NULL)
		the leaf doesn't have enoguh space and the siblings hasn't either: 
			delete , merge(sibling) , delete one parent node , and iterate to the parent to see whether the
			parent node has enough space

	non-leaf level:
		the node has space : return NULL
		the node doesn't has enough space while the siblings have:
			redistribute and update parent node(exchange this node and it's parent value),return NULL
		the node doesn't have enoguh space and the siblings hasn't either: 
			merge , pull down the parent node(copy and delete one of the parent key,delete one of the parent pointer),
			and iterate to the parent to see whether the parent node has enough space

	it's not just pull down and copy up process, while the update parent step propagate at most one level,
	and the pull down step may propogate to the root.

	merge:the right node is moved into the left node.

	caution:siling means the node who has the same parent as this node and they are adjacen(left or right)
*/

	TreeNode * default_Delete(const int & key, TreeNode * ptr, TreeNode * par){

		if(ptr -> isLeaf){ 
			if(!ptr -> deleteKey(key) || ptr -> m >= d || ptr == root)
				return NULL;
			//redistribution
			for(int i = 0 ; i < par -> pm ; i ++ ){
				if(par -> pointer[i] == ptr -> left && ptr -> left -> m > d){
					ptr -> insertKey(ptr -> left -> popKey());
					par -> key[i] = ptr -> frontKey();
					return NULL;
				}
				if(par -> pointer[i] == ptr -> right && ptr -> right -> m > d){
					ptr -> insertKey(ptr -> right -> popFrontKey());
					par -> key[i-1] = ptr -> right -> frontKey();
					return NULL;
				}
			}
			//merge
			for(int i = 0 ; i < par -> pm ; i ++ ){
				if(par -> pointer[i] == ptr -> left){
					int size = ptr -> m;
					for(int j = 0 ; j < size ; j ++ )
						ptr -> left -> insertKey(ptr -> popKey());
					ptr -> left -> right = ptr -> right;
					if(ptr -> right)
						ptr -> right -> left = ptr -> left;
					par -> deletePointer(ptr);
					par -> deleteKey(par -> key[i]);
					ptr = par -> pointer[i];
					break;
				}
				if(par -> pointer[i] == ptr -> right){
					int size = ptr -> right -> m;
					for(int j = 0 ; j < size ; j ++ ){
						ptr -> insertKey(ptr -> right -> popKey());
					}
					ptr -> right = ptr -> right -> right;
					if(ptr -> right)
						ptr  -> right -> left = ptr;
					par -> deletePointer(par -> pointer[i]);
					par -> deleteKey(par -> key[i-1]);
					break;
				}
			}
			return ptr;

		}

		TreeNode * child;
		if(key < ptr -> key[0])
			child = default_Delete(key,ptr -> pointer[0],ptr);		
		else if(key >= ptr -> key[ptr -> m - 1])
			child = default_Delete(key,ptr -> pointer[ptr -> m],ptr);
		else for(int i = 0 ; i < ptr -> m - 1 ; i ++ )
				if(key >= ptr -> key[i] && key < ptr -> key[i+1] ){
					child = default_Delete(key, ptr -> pointer[i + 1],ptr);
					break;
				}

		if(ptr == root){
			if(!root -> m){
				free(root);
				root = child;
			}
			return root;
		}

		//redistribution
		if(!child || ptr -> m >= d) return NULL;
		for(int i = 0 ; i < par -> pm ; i ++ ){
			if(par -> pointer[i+1] == ptr && par -> pointer[i] -> m > d){ //left
				ptr -> insertKey(par -> key[i]);
				ptr -> insertPointer(par -> pointer[i] -> popPointer());
				par -> deleteKey(par -> key[i]); 
				par -> insertKey(par -> pointer[i] -> popKey()); 
				return NULL;
			}
			if(par -> pointer[i-1] == ptr && par -> pointer[i] -> m > d){ //right
				ptr -> insertKey(par -> key[i-1]);
				ptr -> insertPointer(par -> pointer[i] -> pointer[0]);
				par -> deleteKey(par -> key[i-1]);
				par -> insertKey(par -> pointer[i] -> key[0]);
				par -> pointer[i] -> deleteKey(par -> pointer[i] -> key[0]);
				par -> pointer[i] -> popPointer(par -> pointer[i] -> pointer[0]);
				return NULL;
			}
		}

		//merge
		for(int i = 0 ; i < par -> pm ; i ++ ){
			if(i < par -> pm - 1 && par -> pointer[i+1] == ptr){ //left
				par -> pointer[i] -> insertKey(par -> key[i]);
				int size = ptr -> m;
				for(int j = 0 ; j < size ; j ++ )
					par -> pointer[i] -> insertKey(ptr -> popKey());
				size = ptr -> pm;
				for(int j = 0 ; j < size ; j ++ )
					par -> pointer[i] -> insertPointer(ptr -> popPointer());
				par -> deleteKey(par -> key[i]);
				par -> deletePointer(ptr);
				ptr = par -> pointer[i];
				break;
			}
			if(i >= 0 && par -> pointer[i-1] == ptr){ // right
				ptr -> insertKey(par -> key[i-1]);
				int size = par -> pointer[i] -> m;
				for(int j = 0 ; j < size ; j ++ )
					ptr -> insertKey(par -> pointer[i] -> popKey());
				size = par -> pointer[i] -> pm;
				for(int j = 0 ; j < size ; j ++ )
					ptr -> insertPointer(par -> pointer[i] -> popPointer());
				par -> deleteKey(par -> key[i-1]);
				par -> deletePointer(par -> pointer[i]);
				break;
			}
		}
		return ptr;

	}

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

		TreeNode * child = defalut_insert(key,root);
		if(root -> isLeaf && child && child != root){
			TreeNode * newRoot = new TreeNode(false);
			newRoot -> insertKey(child -> key[0]);
			newRoot -> insertPointer(root);
			newRoot -> insertPointer(child);
			root = newRoot;
		}

	}

	void Delete(const int & key){
		
		default_Delete(key,root,NULL);

	}

	void levelTraversal(){
		
		queue<TreeNode*>Q;
		Q.push(root);
		while(!Q.empty()){
			int s = Q.size();
			for(int i = 0 ; i < s ; i ++ ){
				for(int i = 0 ; i < Q.front() -> m ; i ++ )
					cout << Q.front() -> key[i] << " ";
				for(int i = 0 ; i < Q.front() -> pm ; i ++ )
					Q.push(Q.front() -> pointer[i]);
				if(i != s - 1)
				cout << "  ,  ";
				Q.pop();
			}
			cout << endl;
		}

	}
};

int main(){

	BPlusTree T;

	T.insert(2);
	T.insert(13);
	T.insert(17);
	T.insert(24);
	T.insert(30);

	T.insert(3);
	T.insert(5);
	T.insert(7);

	T.insert(14);
	T.insert(16);
	
	T.insert(19);
	T.insert(20);
	T.insert(22);

	T.insert(27);
	T.insert(29);
	
	T.insert(31);
	T.insert(18);
	T.insert(21);
	T.insert(23);
	T.insert(32);
	T.insert(25);
	T.insert(28);
	T.insert(33);
	T.insert(34);
	T.insert(35);

 	T.Delete(5);
 	T.Delete(13);
 	T.Delete(16);
 	T.Delete(30);
 	T.Delete(2);
 	T.Delete(31);
 	T.Delete(17);
 	T.Delete(3);
 	T.Delete(22);
 	T.Delete(14);
 	T.Delete(19);
 	T.Delete(20);
 	T.Delete(27);

 	T.insert(25);
 	T.insert(26);


 	
 
   T.levelTraversal();

}