#pragma once
#include "Actors.h"

using namespace std;

struct Node {
    Actors *a;
    Node *prev = nullptr;
    Node *next = nullptr;
};

class CircLinkedList {
    private:
        int size = 0;
        Node *head = nullptr;
        Node *tail = nullptr;
        Node *current = nullptr;
    public:
        CircLinkedList() = default;
        ~CircLinkedList() {
            while (head != tail) {
                Node *temp = head;
                head = head->next;
                delete temp;
            }
            delete head;
        }
        void push_back(Actors *a) {
            if (!size) {
                head = tail= current = new Node {a};
                size++;
            }
            else {
                Node *temp = new Node {a};
                tail->next = temp;
                temp->prev = tail;
                tail = temp;
                tail->next = head;
                head->prev = tail;
                size++;
            }
        }

        Node get_current() {
            return *current;
        }

        void set_current() {
            while(true) {
                current = current->next;
                if (!current->a->get_dead()) break;
            }
        }
};
