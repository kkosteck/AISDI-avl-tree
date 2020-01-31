#include <iostream>
#include <fstream>
#include <chrono>
#include <utility>
#include <map>
#include <vector>

/*!
 *  Narzedzie do zliczania czasu
 *
 *  Sposob uzycia:
 *
 *  {
 *    Benchmark<std::chrono::nanoseconds> b;
 *    // kod do zbadania
 *    size_t elapsed = b.elapsed();
 *  }
 *
 *  lub
 *
 *  {
 *    Benchmark<std::chrono::milliseconds> b(true);
 *    // kod do zbadania
 *  } // obiekt wypisze wartosc czasu w podanych jednostkach na stderr
 */
template<typename D = std::chrono::microseconds>
class Benchmark {
public:

    Benchmark(bool printOnExit = false) : m_print(printOnExit) {
        start = std::chrono::high_resolution_clock::now();
    }
    typename D::rep elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto result = std::chrono::duration_cast<D>(end-start);
        return result.count();
    }
    ~Benchmark() {
        auto result = elapsed();
        if (m_print)
        {
            std::cerr << "Czas: " << result << "\n";
        }
    }
private:
    std::chrono::high_resolution_clock::time_point start;
    bool m_print = true;
};




/*
 * Slownik
 *
 */
template<typename KeyType, typename ValueType>
class TreeMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;

    TreeMap() {
        treeRoot = NULL;
    }    // konstruktor trywialny

    ~TreeMap() {
        deleteTree(treeRoot);
    }   // destruktor trywialny

    /*!
     * true jezeli slownik jest pusty
     */
    bool isEmpty() const {
        //throw std::runtime_error("TODO: isEmpty");
        return treeRoot == NULL;
    }

    /*!
     * dodaje wpis do slownika
     */
    void insert(const key_type& key, const mapped_type &value)
    {
        //throw std::runtime_error("TODO: insert");
        if (treeRoot == NULL) {
            treeRoot = new Node(key, value);
            return;
        }
        Node* temp = findKey(treeRoot, key);
        if (temp->key == key) {
            if (temp->value != 0) {
                temp->value = value;
            } else {
                temp->value += 1;
            }
        } else if (temp->key > key) {
            temp->left = new Node(key, value, temp);
            balance(temp->left);
        } else {
            temp->right = new Node(key, value, temp);
            balance(temp->right);
        }
    }
	void print(){
		print(treeRoot);
	}


    /*!
     * dodaje wpis do slownika przez podanie pary klucz-wartosc
     */
    void insert(const value_type &key_value) {
        //throw std::runtime_error("TODO: insert");
        insert(key_value.first, key_value.second);
    }

    /*!
     * zwraca referencje na wartosc dla podanego klucza
     *
     * jezeli elementu nie ma w slowniku, dodaje go
     */
    mapped_type& operator[](const key_type& key) {
        //throw std::runtime_error("TODO: operator[]");
        Node* temp = findExactlyKey(treeRoot, key);
        if (temp != NULL){
            return temp->value;
        }
        insert(key, 0);
        temp = findExactlyKey(treeRoot, key);
        return temp->value;
    }

    /*!
     * zwraca wartosc dla podanego klucza
     */
    const mapped_type& value(const key_type& key) const {
        //throw std::runtime_error("TODO: value");
        Node* temp = findExactlyKey(treeRoot, key);
        if (temp == NULL) {
            throw std::runtime_error("There is no key like that.");
        }
        return temp->value;
    }

    /*!
     * zwraca informacje, czy istnieje w slowniku podany klucz
     */
    bool contains(const key_type& key) const {
        //throw std::runtime_error("TODO: contains");
        Node* temp = findExactlyKey(treeRoot, key);
        if (temp != NULL){
            return true;
        }
        return false;
    }

    /*!
     * zwraca liczbe wpisow w slowniku
     */
    size_t size() const {
        //throw std::runtime_error("TODO: size");
        return size(treeRoot);
    }

private:
    class Node {
    public:
        Node *up;
        Node *left;
        Node *right;
        KeyType key;
        ValueType value;
        int balance;

        Node() = default;

        Node(KeyType key, ValueType value, Node* parent = NULL) : key(key), value(value), up(parent), left(NULL), right(NULL), balance(0) {}
    };

    Node *treeRoot = NULL;

    void deleteTree(Node *node) {
        if (node != NULL){
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

    Node* findKey(Node* root, const key_type key) const {
        Node* temp = root;
        Node* tempParent = NULL;

        if (root == NULL) {
            return NULL;
        }
        while (temp != NULL && temp->key != key) {
            tempParent = temp;
            temp = (temp-> key > key) ? temp->left : temp->right;
        }
        if (temp == NULL){
            return tempParent;
        }
        return temp;
    }

    Node* findExactlyKey(Node* root, const key_type key) const {
        Node* temp = root;
        while (temp != NULL && temp->key != key) {
            temp = (temp-> key > key) ? temp->left : temp->right;
        }
        return temp;
    }

    void leftRotation (Node* p) {
        Node* q = p->right;
        p->right = q->left;
        q->left = p;

        if (p->right != NULL) {
            p->right->up = p;
        }
        q->up = p->up;
        if (q->up != NULL) {
            if (q->up->left == p) {
                q->up->left = q;
            } else {
                q->up->right = q;
            }
        }
        p->up = q;
        if (treeRoot == p){
            treeRoot = q;
        }
    }

    void rightRotation (Node* q) {
        Node* p = q->left;
		q->left = p->right;
		p->right = q;

		if (q->left != NULL)
			(q->left)->up = q;
		p->up = q->up;
		if (p->up != NULL) {
			if (p->up->left == q)
				p->up->left = p;
			else
				p->up->right = p;
		}
		q->up = p;

		if (treeRoot == q)
			treeRoot = p;
    }

	void RR(Node* node) {
		leftRotation(node->up);
		if(node->balance == 1) 
			node->left->balance = node->balance = 0;
		else{
			node->left->balance  = 1; 
			node->balance = -1;
		}
	}

	void LL(Node* node) {
		rightRotation(node->up);
		if(node->balance == -1) 
			node->right->balance = node->balance = 0;
		else{
			node->right->balance = -1; 
			node->balance = 1;
		}
	}

	void LR(Node* node) {
		leftRotation(node->up);
		rightRotation(node->up);
		if( node->balance == -1 ) 
			node->left->balance =  1; 
		else 
			node->left->balance = 0;
		if( node->balance==  1 ) 
			node->right->balance = -1; 
		else 
			node->right->balance = 0;

		node->balance = 0;
	}

	void RL(Node* node) {
		rightRotation(node->up);
		leftRotation(node->up);
		if( node->balance == 1 ) 
			node->left->balance =  -1; 
		else 
			node->left->balance = 0;
		if( node->balance==  -1 ) 
			node->right->balance = 1; 
		else 
			node->right->balance = 0;

		node->balance = 0;
	}

    void balance(Node* c) {
        for (Node* p = c->up; p != NULL; p = c->up) {
            if (c == p->right) {  //R
                if (p->balance > 0) { //R
                    if (c->balance < 0){ //RL
                        RL(c->left);
                    } else { //RR
                        RR(c);
                    }
                } else {
                    if (p->balance < 0) {
                        p->balance = 0;
                        break;
                    }
                    p->balance = 1;
                    c = p;
                    continue;
                }
            } else { //L
                if (p->balance < 0) { //L
                    if (c->balance > 0){ //LR
                        LR(c->right);
                    } else { //LL
                        LL(c);
                    }
                } else {
                    if (p->balance > 0) {
                        p->balance = 0;
                        break;
                    }
                    p->balance = -1;
                    c = p;
                    continue;
                }
            }
            break;
        }
    }

    size_t size(Node *node) const {
        if (node == NULL) {
            return 0;
        } else {
            return 1 + size(node->left) + size(node->right);
        }
    }
    
	void print(Node* node){
		if(node != NULL){
			std::cout<<node->key<<" ";
			print(node->left);
			print(node->right);
		}
	}
};

#include "tests.h"

int main()
{
    unit_test();
    

	std::cout<<"WORDS  "<<"INSERT  "<<"AVL  "<<"MAP  "<<"SEARCH  "<<"AVL  "<<"MAP"<<std::endl;
	for (int i = 1000; i < 50000; i += 1000) {
		
		class TreeMap<std::string, int> tree;
		std::map<std::string, int> stdMap;
		
		std::ifstream file;
		file.open("pan-tadeusz.txt");
		if(!file.is_open())
			return 1;
		
		std::string word;
		int wordCount = 0;
		
		Benchmark<std::chrono::nanoseconds> b;
		while(file>>word && wordCount < i){ //tree insertion time
			tree.insert(word, wordCount);
			++wordCount;
		}
		size_t elapsedTree1 = b.elapsed();
		file.close();
		
		file.open("pan-tadeusz.txt");
		if(!file.is_open())
			return 1;
		
		wordCount = 0;
		
		Benchmark<std::chrono::nanoseconds> c;
		while(file>>word && wordCount < i){ //map insertion time
			stdMap.insert(std::make_pair(word, wordCount));
			++wordCount;
		}
		size_t elapsedMap1 = c.elapsed();
		file.close();
		
		file.open("pan-tadeusz.txt");
		if(!file.is_open())
			return 1;
		
		wordCount = 0;
		std::vector<std::string> vec;
			
		while(file>>word && wordCount < i){
			vec.push_back(word);
			++wordCount;
		}
		file.close();
		
		size_t elapsedTree2 = 0;
		size_t elapsedMap2 = 0;
		int forsize = 0;
		
		for (int k = 0; k < i; k += 100) {
			
			forsize++;
		
			Benchmark<std::chrono::nanoseconds> d;
			tree.contains(vec[k]);
			elapsedTree2 += d.elapsed();
			
			Benchmark<std::chrono::nanoseconds> e;
			stdMap.find(vec[k]);
			elapsedMap2 += e.elapsed();
		
		}
		
		elapsedTree2 = elapsedTree2/forsize;
		elapsedMap2 = elapsedMap2/forsize;
		
		std::cout<<i<<"  "<<elapsedTree1<<"  "<<elapsedMap1<<"  ";
		std::cout<<elapsedTree2<<"  "<<elapsedMap2<<std::endl;

	}
    return 0;
}
