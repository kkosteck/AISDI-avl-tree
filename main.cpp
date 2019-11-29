#include <iostream>
#include <fstream>
#include <chrono>
#include <utility>
#include <map>

#define INPUT_FILENAME "pan-tadeusz.txt"

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

   /* void zig(Node* node) {
		if (node == (node->up)->left) {
			rightRotation(node->up);
		} else {
			leftRotation(node->up);
		}
		evalBalance(node);
	}

	void zigZig(Node* node) {
		zig(node->up);
		zig(node);
		evalBalance(node);
	}

	void zigZag(Node* node) {
		zig(node);
		zig(node);
		evalBalance(node);
	}*/
	
	void RR(Node* node) {
		leftRotation(node->up);
		evalBalance(node);
	}
	
	void LL(Node* node) {
		rightRotation(node->up);
		evalBalance(node);
	}
	
	void RL(Node* node) {
		LL(node);
		RR(node);
		evalBalance(node);
	}
	
	void LR(Node* node) {
		RR(node);
		LL(node);
		evalBalance(node);
	}

    void evalBalance(Node* node) {
		node->balance = (NULL != node->right) - (NULL != node->left);
		if (NULL != node->left) {
			node->left->balance = (NULL != node->left->right) - (NULL != node->left->left);
		}
		if (NULL != node->right) {
			node->right->balance = (NULL != node->right->right) - (NULL != node->right->left);
		}
	}

    void balance(Node* c) {
        for (Node* p = c->up; p != NULL; p = c->up) {
            if (c == p->right) {  //R
                if (p->balance > 0) { //R
                    if (c->balance < 0){ //LR
                        LR(c->left);
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
                    if (c->balance > 0){ //RL
                        RL(c->right);
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
            if (temp->value != NULL) {
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
        insert(key, NULL);
        temp = findExactlyKey(treeRoot, key);
        return temp->value;
    }

    /*!
     * zwraca wartosc dla podanego klucza
     */
    const mapped_type& value(const key_type& key) const {
        //throw std::runtime_error("TODO: value");
        Node* temp = findExactlyKey(treeRoot, key);
       /* if (temp == NULL) {
            std::cout<<"There is no key like "<<key;
        }*/
        std::cout<<temp->value<<std::endl;
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
        return sizeOfTree(treeRoot);
    }

    size_t sizeOfTree( Node *node ) const {
        if ( node == NULL) {
            return 0;
        } else {
            return 1 + sizeOfTree(node->left) + sizeOfTree(node->right);
        }
    }

private:

};

#include "tests.h"

int main()
{
    unit_test();

        /*class TreeMap<std::string, int> tree;
        std::map<std::string, int> tree2;
        std::ifstream file;
        file.open (INPUT_FILENAME);
        if (!file.is_open()) exit(EXIT_FAILURE);
        std::string word;
        int i = 0;
        while (file >> word && i < 1000) {
            //std::cout << word << std::endl;
            tree.insert(word, i);
            i++;
        }
        Benchmark<std::chrono::nanoseconds> t1;
        while (file >> word && i < 2000) {
            //std::cout << word << std::endl;
            tree.insert(word, i);
            i++;
        }
        size_t elapsed1 = t1.elapsed();
        file.close();
        file.open (INPUT_FILENAME);
        if (!file.is_open()) exit(EXIT_FAILURE);
        i = 0;
        while (file >> word && i < 1000) {
            tree2.insert( std::make_pair(word, i) );
            i++;
        }
        Benchmark<std::chrono::nanoseconds> t2;
        while (file >> word && i < 2000) {
            tree2.insert( std::make_pair(word, i) );
            i++;
        }
        size_t elapsed2 = t2.elapsed();
        file.close();
        file.open (INPUT_FILENAME);
        if (!file.is_open()) exit(EXIT_FAILURE);
        Benchmark<std::chrono::nanoseconds> t3;
        i=0;
        while (file >> word && i < 2000) {
            tree.contains( word );
            i++;
        }
        size_t elapsed3 = t3.elapsed();
        file.open (INPUT_FILENAME);
        if (!file.is_open()) exit(EXIT_FAILURE);
        Benchmark<std::chrono::nanoseconds> t4;
        i=0;
        while (file >> word && i < 2000) {
            tree2.find( word );
            i++;
        }
        size_t elapsed4 = t4.elapsed();
        std::cout << "AVL: " << elapsed1 << ", search: " << elapsed3 << std::endl;
        std::cout << "map: " << elapsed2 << ", search: " << elapsed4 << std::endl;*/

    return 0;
}
