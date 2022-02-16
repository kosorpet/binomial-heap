#include <iostream>

using namespace std;

class BinomialTree {
public:
    struct Node {
        Node *m_Parent = nullptr;
        uint32_t m_Key = 0;
        Politician m_Politician;
        Node *m_LeftSibling = nullptr;
        Node *m_RightmostSon = nullptr;
        int m_Order = 0;
    };

    static void printTree(Node *node) {
        while (node) {
            cout << "[ " << node->m_Key << " " << node->m_Politician.m_Name << " counter: " << node->m_Politician.counter << " ] ";
            printTree(node->m_RightmostSon);
            node = node->m_LeftSibling;
        }
    }

    static void deleteTree(Node *node) {
        if (!node)
            return;
        deleteTree(node->m_RightmostSon);
        deleteTree(node->m_LeftSibling);
        delete node;
    }

    static Node *mergeTrees(Node *root1, Node *root2) {
        if (root1->m_Order != root2->m_Order) {
            throw invalid_argument("Order of trees must match");
        }

        if (root1->m_Key > root2->m_Key) {
            root2->m_LeftSibling = root1->m_RightmostSon;
            root1->m_RightmostSon = root2;
            root2->m_Parent = root1;

            root1->m_Order++;
            return root1;
        }
        if(root1->m_Key == root2->m_Key){
            if(root1->m_Politician.counter > root2->m_Politician.counter){
                root2->m_LeftSibling = root1->m_RightmostSon;
                root1->m_RightmostSon = root2;
                root2->m_Parent = root1;

                root1->m_Order++;
                return root1;
            }
            else{
                root1->m_LeftSibling = root2->m_RightmostSon;
                root2->m_RightmostSon = root1;
                root1->m_Parent = root2;
                root2->m_Order++;
                return root2;
            }
        }
        root1->m_LeftSibling = root2->m_RightmostSon;
        root2->m_RightmostSon = root1;
        root1->m_Parent = root2;
        root2->m_Order++;
        return root2;
    }
};

/*----------------------------------------------------------*/
class BinomialHeap {
public:
    BinomialHeap() = default;

    explicit BinomialHeap(BinomialTree::Node *root) {
        begin = root;
        max = root;
    }


    ~BinomialHeap();

    void merge(BinomialHeap &other);

    bool empty() {
        return begin == nullptr;
    }

    BinomialTree::Node * getMax() const {
        return max;
    }

    BinomialTree::Node * getBegin(){
        return begin;
    }

    void insert(BinomialTree::Node *node);

    BinomialTree::Node * extractMax();

    static BinomialHeap createHeap(BinomialTree::Node *root);

    void printHeap() {
        cout << "-----------------" << endl;
        auto tmp = begin;
        while (tmp) {
            cout << "ORDER: " << tmp->m_Order << endl;
            cout << "[ " << tmp->m_Key << " " << tmp->m_Politician.m_Name << " counter:  "  << tmp->m_Politician.counter << " ]" << endl;
            BinomialTree::printTree(tmp->m_RightmostSon);
            cout << endl;
            tmp = tmp->m_LeftSibling;
        }
        cout << "-----------------" << endl;
    }

    void increaseKey(BinomialTree::Node *node, uint32_t newKey);

    void deleteNode(BinomialTree::Node *node);

    void changeKey(BinomialTree::Node *node, uint32_t newKey);

    void deleteMax();

    void decreaseKey(BinomialTree::Node *node, uint32_t newKey);

private:
    void add(BinomialTree::Node *node);

    void append(BinomialTree::Node *node);

    void findMax(){
        if(!begin)
            max = nullptr;

        auto curr = begin;
        max = begin;
        while(curr){
            if(curr->m_Key > max->m_Key){
                max = curr;
            }
            if(curr->m_Key == max->m_Key){
                if(curr->m_Politician.counter > max->m_Politician.counter)
                    max = curr;
            }
            curr = curr->m_LeftSibling;
        }
    }

    BinomialTree::Node *begin = nullptr;

    BinomialTree::Node *max = nullptr;

};

/*----------------------------------------------------------*/
BinomialHeap::~BinomialHeap() {
    BinomialTree::Node *tmp;
    while (begin) {
        tmp = begin;
        begin = begin->m_LeftSibling;
        BinomialTree::deleteTree(tmp->m_RightmostSon);
        delete tmp;
    }
}

void BinomialHeap::insert(BinomialTree::Node *node) {
    if(!node)
        return;
    node->m_Politician.counter = changeCounter;
    changeCounter ++;
    BinomialHeap h = BinomialHeap(node);
    merge(h);
}

void BinomialHeap::merge(BinomialHeap &other) {
    if (this == &other)
        return;
    if (empty()) {
        swap(begin, other.begin);
        swap(max, other.max);
        return;
    } else if (other.empty())
        return;

    BinomialHeap merged;
    BinomialTree::Node *addends[3] = {nullptr, nullptr, nullptr};
    BinomialTree::Node *carry = nullptr;
    int notEmpty = 2;
    int currentOrder = 0;
    while (notEmpty >= 2) {
        notEmpty = 0;
        int count = 0;
        if (!empty()) {
            notEmpty++;
            BinomialTree::Node *a = begin;
            if (a->m_Order == currentOrder) {
                addends[count] = a;
                count++;
                begin = begin->m_LeftSibling;
                a->m_LeftSibling = nullptr;
            }
        }
        if (!other.empty()) {
            notEmpty++;
            BinomialTree::Node *b = other.begin;
            if (b->m_Order == currentOrder) {
                addends[count] = b;
                count++;
                other.begin = other.begin->m_LeftSibling;
                b->m_LeftSibling = nullptr;
            }
        }
        if (carry) {
            notEmpty++;
            addends[count] = carry;
            count++;
            carry = nullptr;
        }

        if (count == 3) {
            merged.add(addends[2]);
            carry = BinomialTree::mergeTrees(addends[0], addends[1]);
        }
        if (count == 2) {
            carry = BinomialTree::mergeTrees(addends[0], addends[1]);
        }
        if (count == 1) {
            merged.add(addends[0]);
        }
        currentOrder++;
    }

    if (begin) {
        merged.append(begin);
        begin = nullptr;
    }

    if (other.begin) {
        merged.append(other.begin);
        other.begin = nullptr;
    }

    merged.findMax();

    other.max = nullptr;
    other.begin = nullptr;

    begin = merged.begin;
    max = merged.max;

    merged.begin = nullptr;
    merged.max = nullptr;
}

BinomialTree::Node *BinomialHeap::extractMax() {
    if (!begin)
        return nullptr;

    BinomialTree::Node *curr = begin;
    BinomialTree::Node *prev = nullptr;
    if (curr && curr == max) {
        begin = curr->m_LeftSibling;
    } else {
        while (curr && curr != max) {
            prev = curr;
            curr = curr->m_LeftSibling;
        }
    }

    if(!curr){
        throw runtime_error("node not in heap");
    }
    if (prev)
        prev->m_LeftSibling = curr->m_LeftSibling;

    BinomialHeap newHeap = createHeap(max);

    max->m_LeftSibling = nullptr;
    max->m_RightmostSon = nullptr;
    max->m_Order = 0;

    BinomialTree::Node *oldMax = max;

    findMax();

    merge(newHeap);
    return oldMax;
}

BinomialHeap BinomialHeap::createHeap(BinomialTree::Node *root) {
    //Remove root as parent
    BinomialTree::Node *son = root->m_RightmostSon;
    BinomialTree::Node *sons = son;
    while (sons) {
        sons->m_Parent = nullptr;
        sons = sons->m_LeftSibling;
    }

    BinomialHeap newHeap;
    //Reverse top layer
    BinomialTree::Node *curr = son;
    BinomialTree::Node *prev = nullptr;
    BinomialTree::Node *next = nullptr;
    while (curr) {
        next = curr->m_LeftSibling;
        curr->m_LeftSibling = prev;
        prev = curr;
        curr = next;
    }
    //Find new max
    newHeap.begin = prev;
    newHeap.findMax();

    return newHeap;
}

void BinomialHeap::add(BinomialTree::Node *node) {
    if (!begin || node->m_Order < begin->m_Order) {
        node->m_LeftSibling = begin;
        begin = node;
        return;
    }

    auto curr = begin;
    while (curr->m_LeftSibling && node->m_Order > curr->m_LeftSibling->m_Order) {
        curr = curr->m_LeftSibling;
    }
    curr->m_LeftSibling = node;
}

void BinomialHeap::append(BinomialTree::Node *node) {
    if (!begin) {
        begin = node;
        return;
    }
    auto curr = begin;
    while (curr->m_LeftSibling) {
        curr = curr->m_LeftSibling;
    }
    curr->m_LeftSibling = node;
}

void BinomialHeap::deleteNode(BinomialTree::Node *node) {
    if (empty())
        return;
    increaseKey(node, UINT32_MAX);
    auto toDelete = extractMax();
    delete toDelete;
}

void BinomialHeap::changeKey(BinomialTree::Node *node, uint32_t newKey) {
    if(!node)
        return;

    node->m_Politician.counter = changeCounter;
    changeCounter ++;

    if (node->m_Key > newKey)
        decreaseKey(node, newKey);
    else
        increaseKey(node, newKey);
}

void BinomialHeap::decreaseKey(BinomialTree::Node *node, uint32_t newKey) {
    if (!node || empty())
        return;
    increaseKey(node, UINT32_MAX);
    auto toChange = extractMax();
    toChange->m_Key = newKey;
    toChange->m_Politician.m_Popularity = newKey;
    toChange->m_Parent = nullptr;
    toChange->m_RightmostSon = nullptr;
    toChange->m_LeftSibling = nullptr;
    toChange->m_Order = 0;
    insert(toChange);
}

void BinomialHeap::increaseKey(BinomialTree::Node *node, uint32_t newKey) {
    if(!node || empty())
        return;
    node->m_Key = newKey;
    node->m_Politician.m_Popularity = newKey;

    while (node->m_Parent && node->m_Parent->m_Key <= node->m_Key) {
        if(node->m_Parent->m_Key == node->m_Key && node->m_Politician.counter < node->m_Parent->m_Politician.counter)
            break;

        //loop through siblings in my node and set their parent to node
        auto parent = node->m_Parent;
        auto grandParent = parent->m_Parent;
        auto nodeLayerRightmost = parent->m_RightmostSon;

        BinomialTree::Node *parentLayerRightmost = nullptr;
        if (grandParent)
            parentLayerRightmost = grandParent->m_RightmostSon;
        auto i = nodeLayerRightmost;
        while (i) {
            if (i != node)
                i->m_Parent = node;
            i = i->m_LeftSibling;
        }
        //set new parent for node
        node->m_Parent = parent->m_Parent;
        //if grandparent is present, set node as his son, if parent was rigthmost
        if (parent->m_Parent && parent->m_Parent->m_RightmostSon == parent)
            parent->m_Parent->m_RightmostSon = node;
        //set new parent for parent
        parent->m_Parent = node;
        //set son for parent
        parent->m_RightmostSon = node->m_RightmostSon;
        if (node->m_RightmostSon) {
            //if node has sons set parents as parent
            i = node->m_RightmostSon;
            while (i) {
                i->m_Parent = parent;
                i = i->m_LeftSibling;
            }
            node->m_RightmostSon->m_Parent = parent;
        }

        //fix node layer sibling connections
        swap(node->m_LeftSibling, parent->m_LeftSibling);
        i = nodeLayerRightmost;
        while (i) {
            if (i == node)
                break;
            if (i->m_LeftSibling == node) {
                i->m_LeftSibling = parent;
                break;
            }
            i = i->m_LeftSibling;
        }
        //fix parent layer sibling connections
        if (grandParent) {
            i = parentLayerRightmost;
            while (i) {
                if (i == parent)
                    break;
                if (i->m_LeftSibling == parent) {
                    i->m_LeftSibling = node;
                    break;
                }
                i = i->m_LeftSibling;
            }
        }
        //set node new son
        if (node == nodeLayerRightmost) {
            node->m_RightmostSon = parent;
        } else
            node->m_RightmostSon = nodeLayerRightmost;

        swap(parent->m_Order, node->m_Order);
    }
    //set new max
    if (node->m_Key >= max->m_Key)
        max = node;
    //travel up if begin changed
    while (begin->m_Parent) {
        begin = begin->m_Parent;
    }
    //fix top layer connections
    auto curr = begin;
    while (curr) {
        while (curr->m_LeftSibling && curr->m_LeftSibling->m_Parent) {
            curr->m_LeftSibling = curr->m_LeftSibling->m_Parent;
        }
        curr = curr->m_LeftSibling;
    }
}

void BinomialHeap::deleteMax() {
    auto oldMax = extractMax();
    delete oldMax;
}

