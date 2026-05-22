#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <unordered_map>

#define STACK_SIZE   100
#define QUEUE_SIZE   100
#define CQ_CAPACITY  5

using namespace std;
using namespace std::chrono;

// SORTING ALGORITHMS

void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int partition(int numbers[], int i, int k) {
    int midpoint = i + (k - i) / 2;
    int pivot = numbers[midpoint];
    int l = i;
    int h = k;
    bool done = false;

    while (!done) {
        while (numbers[l] < pivot) {
            ++l;
        }
        while (pivot < numbers[h]) {
            --h;
        }
        if (l >= h) {
            done = true;
        }
        else {
            int temp = numbers[l];
            numbers[l] = numbers[h];
            numbers[h] = temp;
            ++l;
            --h;
        }
    }
    return h;
}

void quickSort(int numbers[], int i, int k) {
    int j;
    if (i >= k) {
        return;
    }
    j = partition(numbers, i, k);
    quickSort(numbers, i, j);
    quickSort(numbers, j + 1, k);
}

void merge(int numbers[], int i, int j, int k) {
    int mergedSize;
    int mergePos;
    int leftPos;
    int rightPos;
    int* mergedNumbers = nullptr;

    mergePos = 0;
    mergedSize = k - i + 1;
    leftPos = i;
    rightPos = j + 1;
    mergedNumbers = new int[mergedSize];

    while (leftPos <= j && rightPos <= k) {
        if (numbers[leftPos] <= numbers[rightPos]) {
            mergedNumbers[mergePos] = numbers[leftPos];
            ++leftPos;
        }
        else {
            mergedNumbers[mergePos] = numbers[rightPos];
            ++rightPos;
        }
        ++mergePos;
    }

    while (leftPos <= j) {
        mergedNumbers[mergePos] = numbers[leftPos];
        ++leftPos;
        ++mergePos;
    }

    while (rightPos <= k) {
        mergedNumbers[mergePos] = numbers[rightPos];
        ++rightPos;
        ++mergePos;
    }

    for (mergePos = 0; mergePos < mergedSize; ++mergePos) {
        numbers[i + mergePos] = mergedNumbers[mergePos];
    }
    delete[] mergedNumbers;
}

void mergeSort(int numbers[], int i, int k) {
    int j;
    if (i < k) {
        j = (i + k) / 2;
        mergeSort(numbers, i, j);
        mergeSort(numbers, j + 1, k);
        merge(numbers, i, j, k);
    }
}

// DATA GENERATION

vector<int> generateManyDuplicates(int n) {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(1, 1000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

vector<int> generateFewDuplicates(int n) {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(1, 10000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

vector<int> generateSorted(int n) {
    vector<int> v = generateFewDuplicates(n);
    sort(v.begin(), v.end());
    return v;
}

vector<int> generateReverseSorted(int n) {
    vector<int> v = generateSorted(n);
    reverse(v.begin(), v.end());
    return v;
}

double measureMs(vector<int> arr, void (*sortFn)(vector<int>&)) {
    auto start = high_resolution_clock::now();
    sortFn(arr);
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

double measureMsRaw(vector<int> arr, void (*sortFn)(int[], int, int)) {
    auto start = high_resolution_clock::now();
    sortFn(arr.data(), 0, (int)arr.size() - 1);
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

// SORTING EVALUATION

struct PatternResult {
    string label;
    vector<int>sizes;
    vector<double> bubble, insertion, merge, quick;
};

PatternResult runPattern(const string& label, vector<int>(*gen)(int), const vector<int>& sizes) {
    PatternResult res;
    res.label=label;
    res.sizes=sizes;

    cout << "\n--- " << label << " ---\n";
    cout << left << setw(14) << "n"
         << setw(14) << "Bubble(ms)"
         << setw(16) << "Insertion(ms)"
         << setw(14) << "Merge(ms)"
         << setw(14) << "Quick(ms)" << "\n";
    cout << string(72, '-') << "\n";

    for (int n : sizes) {
        vector<int> base = gen(n);

        double t1 = measureMs(base, bubbleSort);
        double t2 = measureMs(base, insertionSort);
        double t3 = measureMsRaw(base, mergeSort);
        double t4 = measureMsRaw(base, quickSort);

        res.bubble.push_back(t1);
        res.insertion.push_back(t2);
        res.merge.push_back(t3);
        res.quick.push_back(t4);

        cout << left << setw(14) << n
             << fixed << setprecision(3)
             << setw(14) << t1
             << setw(16) << t2
             << setw(14) << t3
             << setw(14) << t4 << "\n";
    }
    return res;
} 

void writeCSV(const vector<PatternResult>& results, const string& path) {
    ofstream f(path);
    f << "pattern,n,Bubble,Insertion,Merge,Quick\n";
    for (const auto& r : results) {
        for (int i = 0; i < (int)r.sizes.size(); i++) {
            f << fixed << setprecision(6)
              << r.label        << ","
              << r.sizes[i]     << ","
              << r.bubble[i]    << ","
              << r.insertion[i] << ","
              << r.merge[i]     << ","
              << r.quick[i]     << "\n";
        }
    }
    cout << "Timing data written to: " << path << "\n";
}

void sortingEvaluation() {
    cout << "\n========== SORTING EVALUATION ==========\n";
    vector<int> sizes = { 100, 1000, 5000, 10000 };

    vector<PatternResult> results;
    results.push_back(runPattern("Many Duplicates",generateManyDuplicates,sizes));
    results.push_back(runPattern("Few Duplicates",generateFewDuplicates,sizes));
    results.push_back(runPattern("Sorted",generateSorted,sizes));
    results.push_back(runPattern("Reverse Sorted",generateReverseSorted,sizes));

    cout << "\nNote: Bubble/Insertion are O(n^2); Merge/Quick are O(n log n).\n";
    cout << "Insertion sort is fastest on already-sorted data.\n";

    writeCSV(results, "sort_results.csv");

    int ret = system("python plot_graphs.py sort_results.csv");
    if (ret != 0)
        cout << "Warning: graph generation failed.\n";
}

// STACK

class Stack {
private:
    int top;
    int arr[STACK_SIZE];

public:
    Stack() {
        top = -1;
    }

    void push(int x) {
        if (top >= STACK_SIZE - 1) {
            cout << "Stack Overflow" << endl;
            return;
        }
        arr[++top] = x;
        cout << "The element " << x << " is pushed into the stack" << endl;
    }

    int pop() {
        if (top < 0) {
            cout << "Stack is empty" << endl;
            return -1;
        }
        int val = arr[top--];
        cout << "The element " << val << " is popped from the stack" << endl;
        return val;
    }

    int peek() {
        if (top < 0) {
            cout << "Stack is empty" << endl;
            return -1;
        }
        cout << "The top element is " << arr[top] << endl;
        return arr[top];
    }

    bool isEmpty() {
        return top == -1;
    }

    void display() {
        if (isEmpty()) {
            cout << "Stack is empty" << endl;
            return;
        }
        cout << "Stack (top -> bottom): ";
        for (int i = top; i >= 0; i--)
            cout << arr[i] << " ";
        cout << endl;
    }
};

void stackDemo() {
    cout << "\n========== STACK DEMONSTRATION ==========\n";
    Stack s;
    s.push(10);
    s.push(20);
    s.push(30);
    s.display();
    s.peek();
    s.pop();
    s.pop();
    s.display();
    s.pop();
    s.pop(); // underflow
}

// QUEUE

class Queue {
private:
    int front;
    int rear;
    int arr[QUEUE_SIZE];

public:
    Queue() {
        front = 0;
        rear  = -1;
    }

    void enqueue(int x) {
        if (isFull()) {
            cout << "Queue overflow" << endl;
            return;
        }
        arr[++rear] = x;
        cout << x << " is added to the queue" << endl;
    }

    int dequeue() {
        if (isEmpty()) {
            cout << "Queue underflow" << endl;
            return -1;
        }
        int val = arr[front++];
        if (front > rear) {
            front = 0;
            rear  = -1;
        }
        cout << val << " is removed from the queue" << endl;
        return val;
    }

    int begin() {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return -1;
        }
        return arr[front];
    }

    int back() {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return -1;
        }
        return arr[rear];
    }

    bool isEmpty() {
        return front > rear;
    }

    bool isFull() {
        return rear == QUEUE_SIZE - 1;
    }

    int size() {
        if (isEmpty()) return 0;
        return rear - front + 1;
    }

    void display() {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return;
        }
        cout << "Queue (front -> rear): ";
        for (int i = front; i <= rear; i++)
            cout << arr[i] << " ";
        cout << endl;
    }
};

void queueDemo() {
    cout << "\n========== QUEUE DEMONSTRATION ==========\n";
    Queue q;
    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);
    q.display();
    cout << "Front element: " << q.begin() << endl;
    cout << "Back element: "  << q.back()  << endl;
    cout << "Queue size: "    << q.size()  << endl;
    q.dequeue();
    q.display();
    cout << "Is queue empty? " << (q.isEmpty() ? "Yes" : "No") << endl;
}

// CIRCULAR QUEUE

class CircularQueue {
private:
    int front;
    int rear;
    int arr[CQ_CAPACITY];

public:
    CircularQueue() {
        front = -1;
        rear  = -1;
    }

    bool is_empty() {
        return front == -1;
    }

    bool is_full() {
        return (rear + 1) % CQ_CAPACITY == front;
    }

    void push(int x) {
        if (is_full()) {
            cout << "Queue is full; can't update" << endl;
            return;
        }
        if (is_empty())
            front = 0;
        rear = (rear + 1) % CQ_CAPACITY;
        arr[rear] = x;
        cout << x << " is added to the queue | front=" << front << " rear=" << rear << endl;
    }

    int pop() {
        if (is_empty()) {
            cout << "Nothing to remove." << endl;
            return -1;
        }
        int val = arr[front];
        if (front == rear) {
            front = -1;
            rear  = -1;
        } else {
            front = (front + 1) % CQ_CAPACITY;
        }
        cout << val << " is removed from the queue | front=" << front << " rear=" << rear << endl;
        return val;
    }

    void display() {
        if (is_empty()) {
            cout << "Circular queue is empty" << endl;
            return;
        }
        cout << "Circular Queue: ";
        int i = front;
        while (true) {
            cout << arr[i] << " ";
            if (i == rear) break;
            i = (i + 1) % CQ_CAPACITY;
        }
        cout << "(front=" << front << ", rear=" << rear << ")" << endl;
    }
};

void circularQueueDemo() {
    cout << "\n========== CIRCULAR QUEUE DEMONSTRATION ==========\n";
    CircularQueue cq;
    cq.push(1); cq.push(2); cq.push(3); cq.push(4); cq.push(5);
    cq.push(6); // full
    cq.display();
    cq.pop(); cq.pop();
    cq.display();
    cq.push(6); cq.push(7);
    cq.display();
}

// PRIORITY QUEUE (Min-Heap)

class PriorityQueue {
private:
    vector<int> heap;
 
    void min_heapify(vector<int>& arr, int n, int i) {
        int smallest = i;
        int left     = 2 * i + 1;
        int right    = 2 * i + 2;
 
        if (left  < n && arr[left]  < arr[smallest])
            smallest = left;
        if (right < n && arr[right] < arr[smallest])
            smallest = right;
 
        if (smallest != i) {
            swap(arr[i], arr[smallest]);
            min_heapify(arr, n, smallest);
        }
    }
 
    void build_heap() {
        int n = heap.size();
        for (int i = n / 2 - 1; i >= 0; i--)
            min_heapify(heap, n, i);
    }
 
public:
    void insert(int x) {
        heap.push_back(x);
        build_heap();
        cout << x << " is inserted into the priority queue" << endl;
    }
 
    int extractMin() {
        if (heap.empty()) {
            cout << "Priority queue is empty" << endl;
            return -1;
        }
        int val = heap[0];
        swap(heap[0], heap.back());
        heap.pop_back();
        if (!heap.empty()) min_heapify(heap, heap.size(), 0);
        cout << val << " is extracted (minimum)" << endl;
        return val;
    }
 
    int peek() {
        if (heap.empty()) {
            cout << "Priority queue is empty" << endl;
            return -1;
        }
        cout << "Minimum element is " << heap[0] << endl;
        return heap[0];
    }
 
    bool isEmpty() {
        return heap.empty();
    }
 
    void display() {
        if (isEmpty()) {
            cout << "Priority queue is empty" << endl;
            return;
        }
        cout << "Heap array: ";
        for (int x : heap) cout << x << " ";
        cout << endl;
    }
};

void priorityQueueDemo() {
    cout << "\n========== PRIORITY QUEUE (Min-Heap) DEMONSTRATION ==========\n";
    PriorityQueue pq;
    pq.insert(40); pq.insert(10); pq.insert(30); pq.insert(5); pq.insert(20);
    pq.display();
    pq.peek();
    pq.extractMin();
    pq.display();
    pq.extractMin();
    pq.display();
}

// BINARY SEARCH TREE

struct Node {
    int   data;
    Node* left;
    Node* right;
    Node(int val) {
        data  = val;
        left  = nullptr;
        right = nullptr;
    }
};

Node* insert_bst(Node* root, int val) {
    if (root == nullptr)
        return new Node(val);
    if (val < root->data)
        root->left  = insert_bst(root->left,  val);
    else if (val > root->data)
        root->right = insert_bst(root->right, val);
    return root;
}

Node* findMin(Node* root) {
    while (root != nullptr && root->left != nullptr)
        root = root->left;
    return root;
}

Node* delete_bst(Node* root, int key) {
    if (root == nullptr)
        return nullptr;
    if (root->data > key) {
        root->left  = delete_bst(root->left,  key);
    } else if (root->data < key) {
        root->right = delete_bst(root->right, key);
    } else {
        if (root->left == nullptr && root->right == nullptr) {
            cout << "  [Case 1: leaf node deleted]\n";
            delete root;
            return nullptr;
        }
        if (root->left == nullptr) {
            cout << "  [Case 2: one child (right)]\n";
            Node* temp = root->right;
            delete root;
            return temp;
        }
        if (root->right == nullptr) {
            cout << "  [Case 2: one child (left)]\n";
            Node* temp = root->left;
            delete root;
            return temp;
        }
        cout << "  [Case 3: two children, using inorder successor]\n";
        Node* successor = findMin(root->right);
        root->data  = successor->data;
        root->right = delete_bst(root->right, successor->data);
    }
    return root;
}

void in_order(Node* root) {
    if (root == nullptr) return;
    in_order(root->left);
    cout << root->data << " ";
    in_order(root->right);
}

void pre_order(Node* root) {
    if (root == nullptr) return;
    cout << root->data << " ";
    pre_order(root->left);
    pre_order(root->right);
}

void post_order(Node* root) {
    if (root == nullptr) return;
    post_order(root->left);
    post_order(root->right);
    cout << root->data << " ";
}

void bstDemo() {
    cout << "\n========== BINARY SEARCH TREE DEMONSTRATION ==========\n";
    Node* root = nullptr;
    int values[] = { 50, 30, 70, 20, 40, 60, 80, 35 };

    cout << "Inserting: ";
    for (int val : values) {
        cout << val << " ";
        root = insert_bst(root, val);
    }
    cout << "\n";

    cout << "Inorder:   "; in_order(root);   cout << "\n";
    cout << "Preorder:  "; pre_order(root);  cout << "\n";
    cout << "Postorder: "; post_order(root); cout << "\n";

    cout << "\n-- Deletion Examples --\n";
    cout << "Deleting 20 (leaf):\n";
    root = delete_bst(root, 20);
    cout << "Inorder: "; in_order(root); cout << "\n";

    cout << "Deleting 30 (two children):\n";
    root = delete_bst(root, 30);
    cout << "Inorder: "; in_order(root); cout << "\n";

    cout << "Deleting 70 (two children):\n";
    root = delete_bst(root, 70);
    cout << "Inorder: "; in_order(root); cout << "\n";
}

// AVL TREE

struct AVLNode {
    int      val;
    int      height;
    AVLNode* left;
    AVLNode* right;
    AVLNode(int v) {
        val    = v;
        height = 1;
        left   = nullptr;
        right  = nullptr;
    }
};

int avl_height(AVLNode* n) {
    return n ? n->height : 0;
}

int balance_factor(AVLNode* n) {
    return n ? avl_height(n->left) - avl_height(n->right) : 0;
}

void update_height(AVLNode* n) {
    if (n)
        n->height = 1 + max(avl_height(n->left), avl_height(n->right));
}

AVLNode* rotate_right(AVLNode* y) {
    cout << "  LL Rotation at node " << y->val << "\n";
    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    update_height(y);
    update_height(x);
    return x;
}

AVLNode* rotate_left(AVLNode* x) {
    cout << "  RR Rotation at node " << x->val << "\n";
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    update_height(x);
    update_height(y);
    return y;
}

AVLNode* avl_balance(AVLNode* node) {
    update_height(node);
    int bf = balance_factor(node);

    if (bf > 1 && balance_factor(node->left) >= 0)
        return rotate_right(node);
    if (bf > 1 && balance_factor(node->left) < 0) {
        cout << "  LR Rotation at node " << node->val << "\n";
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (bf < -1 && balance_factor(node->right) <= 0)
        return rotate_left(node);
    if (bf < -1 && balance_factor(node->right) > 0) {
        cout << "  RL Rotation at node " << node->val << "\n";
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}

AVLNode* insert_avl(AVLNode* node, int val) {
    if (node == nullptr)
        return new AVLNode(val);
    if (val < node->val)
        node->left  = insert_avl(node->left,  val);
    else if (val > node->val)
        node->right = insert_avl(node->right, val);
    else
        return node;
    return avl_balance(node);
}

void avl_inorder(AVLNode* node) {
    if (node == nullptr) return;
    avl_inorder(node->left);
    cout << node->val << "(h=" << node->height << ") ";
    avl_inorder(node->right);
}

void delete_avl(AVLNode* node) {
    if (node == nullptr) return;
    delete_avl(node->left);
    delete_avl(node->right);
    delete node;
}

void avlDemo() {
    cout << "\n========== AVL TREE DEMONSTRATION ==========\n";

    cout << "\n-- LL Rotation: Insert 30, 20, 10 --\n";
    AVLNode* t1 = nullptr;
    t1 = insert_avl(t1, 30); t1 = insert_avl(t1, 20); t1 = insert_avl(t1, 10);
    cout << "Inorder: "; avl_inorder(t1); cout << "\n";

    cout << "\n-- RR Rotation: Insert 10, 20, 30 --\n";
    AVLNode* t2 = nullptr;
    t2 = insert_avl(t2, 10); t2 = insert_avl(t2, 20); t2 = insert_avl(t2, 30);
    cout << "Inorder: "; avl_inorder(t2); cout << "\n";

    cout << "\n-- LR Rotation: Insert 30, 10, 20 --\n";
    AVLNode* t3 = nullptr;
    t3 = insert_avl(t3, 30); t3 = insert_avl(t3, 10); t3 = insert_avl(t3, 20);
    cout << "Inorder: "; avl_inorder(t3); cout << "\n";

    cout << "\n-- RL Rotation: Insert 10, 30, 20 --\n";
    AVLNode* t4 = nullptr;
    t4 = insert_avl(t4, 10); t4 = insert_avl(t4, 30); t4 = insert_avl(t4, 20);
    cout << "Inorder: "; avl_inorder(t4); cout << "\n";

    cout << "\n-- Full sequence: 50,25,75,10,30,60,80,5,15 --\n";
    AVLNode* t5 = nullptr;
    for (int v : {50, 25, 75, 10, 30, 60, 80, 5, 15})
        t5 = insert_avl(t5, v);
    cout << "Inorder: "; avl_inorder(t5); cout << "\n";

    delete_avl(t1); delete_avl(t2); delete_avl(t3); delete_avl(t4); delete_avl(t5);
}

// GRAPH - BFS & DFS

void addEdge(unordered_map<int, vector<int>>& adj, int u, int v) {
    adj[u].push_back(v);
    adj[v].push_back(u);
}

void bfs(unordered_map<int, vector<int>>& adj, int start, int V) {
    cout << "\nBFS from node " << start << ":\n";
    unordered_map<int, bool> visited;
    vector<int> order;
    vector<int> q;

    q.push_back(start);
    visited[start] = true;
    int front = 0;

    while (front < (int)q.size()) {
        int node = q[front++];
        order.push_back(node);
        for (int nb : adj[node]) {
            if (!visited[nb]) {
                visited[nb] = true;
                q.push_back(nb);
            }
        }
    }
    cout << "Traversal order: ";
    for (int x : order) cout << x << " ";
    cout << "\nTime Complexity: O(V + E) where V=" << V << "\n";
}

void dfs_helper(unordered_map<int, vector<int>>& adj, int node,
                unordered_map<int, bool>& visited, vector<int>& order) {
    visited[node] = true;
    order.push_back(node);
    for (int nb : adj[node])
        if (!visited[nb])
            dfs_helper(adj, nb, visited, order);
}

void dfs(unordered_map<int, vector<int>>& adj, int start, int V) {
    cout << "\nDFS from node " << start << ":\n";
    unordered_map<int, bool> visited;
    vector<int> order;
    dfs_helper(adj, start, visited, order);
    cout << "Traversal order: ";
    for (int x : order) cout << x << " ";
    cout << "\nTime Complexity: O(V + E) where V=" << V << "\n";
}

void graphDemo() {
    cout << "\n========== GRAPH BFS & DFS DEMONSTRATION ==========\n";
    int V = 7;
    unordered_map<int, vector<int>> adj;
    addEdge(adj, 0, 1); addEdge(adj, 0, 2);
    addEdge(adj, 1, 3); addEdge(adj, 1, 4);
    addEdge(adj, 2, 5); addEdge(adj, 2, 6);
    addEdge(adj, 3, 4);

    cout << "Adjacency List:\n";
    for (auto& [node, neighbors] : adj) {
        cout << "  " << node << " -> ";
        for (int nb : neighbors) cout << nb << " ";
        cout << "\n";
    }

    int startNode;
    cout << "\nEnter starting node (0-6): ";
    cin >> startNode;
    if (startNode < 0 || startNode > 6) startNode = 0;

    bfs(adj, startNode, V);
    dfs(adj, startNode, V);
}

// MAIN MENU

int main() {

    int choice;
    do {
        cout << "\n--- MAIN MENU ---\n";
        cout << "1. Sorting Evaluation\n";
        cout << "2. Stack Demonstration\n";
        cout << "3. Queue Demonstration\n";
        cout << "4. Circular Queue Demonstration\n";
        cout << "5. Priority Queue Demonstration\n";
        cout << "6. BST Operations\n";
        cout << "7. AVL Operations\n";
        cout << "8. BFS and DFS\n";
        cout << "9. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        if(cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: sortingEvaluation();  break;
            case 2: stackDemo();          break;
            case 3: queueDemo();          break;
            case 4: circularQueueDemo();  break;
            case 5: priorityQueueDemo();  break;
            case 6: bstDemo();            break;
            case 7: avlDemo();            break;
            case 8: graphDemo();          break;
            case 9: cout << "Exiting. Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 9);

    return 0;
}