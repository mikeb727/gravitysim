#ifndef LIST_H
#define LIST_H

template <class T>
class List {
public:
	struct Node {
		T* data;
		Node* next;
		Node(T* t){
			data = t;
			next = 0;
		}
	};
	class Iterator {
	public:
		Iterator(List* lst){
			l = lst;
			reset();
		}
		void reset(){
			nextNode = 0;
		}
		bool isEnd(){
			if (nextNode->next = NULL){
				return true;
			}
			return false;
		}
		T* next(){
			if (isEnd()){
				reset();
			}
			else {
				next = nextNode->next;
			}
			return nextNode->data;
		}
	private:
		List* l;
		List::Node* nextNode;
	};
	List(){
		head = 0;
		tail = 0;
	}
	~List(){
		Node* temp = head;
		while (temp != 0){
			head = head->next;
			delete temp;
			temp = head;
		}
		tail = 0;
	}
	void addItem(T* t){
		if (head == 0){
			head = tail = new Node(t);
		}
		else {
			tail->next = new Node(t);
			tail = tail->next;
		}
	}
	void removeItem(T* t){
		Node* temp = head;
		Node* prev = 0;
		while (temp != 0){
			if (temp->data == t){
				if (temp == head){
					head = head->next;
					delete temp;
				}
				else if (temp == tail){
					tail = prev;
					prev->next = 0;
					delete temp;
				}
				else {
					prev->next = temp->next;
					delete temp;
				}
			}
			prev = temp;
			temp = temp->next;
		}
	}
	T* first(){
		return head->data;
	}
	T* getFirst(){
		Node* temp = head;
		T* result = head->data;
		temp->data = 0;
		head = head->next;
		delete temp;
		return result;
	}
	void clear(){
		Node* temp = head;
		while (temp != 0){
			head = head->next;
			delete temp;
			temp = head;
		}
		tail = 0;
	}
	Iterator* getIterator() {return new Iterator(*this);}
private:
	Node* head;
	Node* tail;
};



#endif
