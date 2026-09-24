#include <iostream>

struct MoveNode {
    int disk;
    char from;
    char to;
    MoveNode* prev;
    MoveNode* next;
};

struct MoveList {
    MoveNode* head = nullptr;
    MoveNode* tail = nullptr;
};

void addMove(MoveList& list, int disk, char from, char to) {
    MoveNode* node = new MoveNode{disk, from, to, nullptr, nullptr};
    if (list.tail == nullptr) {
        list.head = node;
        list.tail = node;
    } else {
        node->prev = list.tail;
        list.tail->next = node;
        list.tail = node;
    }
}

void hanoi(int n, char from, char aux, char to, MoveList& list) {
    if (n == 0) {
        return;
    }
    hanoi(n - 1, from, to, aux, list);
    addMove(list, n, from, to);
    hanoi(n - 1, aux, from, to, list);
}

void printList(const MoveList& list) {
    int step = 1;
    MoveNode* current = list.head;
    while (current != nullptr) {
        std::cout << step << ": disk " << current->disk
                   << " from " << current->from
                   << " to " << current->to << std::endl;
        current = current->next;
        step++;
    }
}

void freeList(MoveList& list) {
    MoveNode* current = list.head;
    while (current != nullptr) {
        MoveNode* next = current->next;
        delete current;
        current = next;
    }
    list.head = nullptr;
    list.tail = nullptr;
}

int main() {
    const int disks = 8;
    MoveList list;

    hanoi(disks, 'A', 'B', 'C', list);
    printList(list);
    freeList(list);

    return 0;
}
