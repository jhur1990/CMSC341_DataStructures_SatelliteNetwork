/**********************************************
 ** File: satnet.cpp
 ** Project: CMSC 341 Project 2, Fall 2023
 ** Author: Joshua Hur
 ** Date: 10/24/23
 ** Section: 2
 ** E-mail: jhur1@umbc.edu
 **
 ** This program creates a mock satellite network by inserting and removing satellites.
 ** The program randomly assigns satellite ID numbers when satellites are created and
 ** builds a Binary Search Tree data structure based on satellite ID numbers.
 ************************************************************************/

#include "satnet.h"

// SatNet()
// The default constructor performs the required initializations
SatNet::SatNet(){
    m_root = nullptr;
}

// ~SatNet()
// The destructor that deallocates the memory and re-initializations
SatNet::~SatNet(){
    clear();
}

// clear()
// Deallocate all memory and convert to an empty object
void SatNet::clear(){
    destroyTree(m_root);
    m_root = nullptr;
}

// destroyTree(Sat* node)
// Recursive helper function of clear() to deallocates all memory of the tree
void SatNet::destroyTree(Sat* node) {
    if (node != nullptr) {
        destroyTree(node -> m_left);
        destroyTree(node -> m_right);
        delete node;
        node = nullptr;
    }
}

// insert(const Sat& satellite)
// Add a node to the tree
void SatNet::insert(const Sat& satellite){
    
    // Check if desired ID number is within range
    if (satellite.m_id >= MINID && satellite.m_id <= MAXID) {
        m_root = insert(satellite, m_root);
    }
}

// insert(const Sat& satellite, Sat* node)
// Recursive helper function of insert(const Sat& satellite) adds a node in the tree
Sat* SatNet::insert(const Sat& satellite, Sat* node) {
    if (node == nullptr) {
        node = new Sat(satellite);
    
    // If the satellite ID is less than the current node's ID, insert into the left subtree
    } else if (satellite.m_id < node -> m_id) {
        node -> m_left = insert(satellite, node -> m_left);
        
        // Check if after insertion, the left subtree became too tall
        // If so, perform rotations
        if (height (node -> m_left) - height (node -> m_right) == 2) {
            if (satellite.m_id < node -> m_left -> m_id) {
                node = singleRotateWithLeft(node);
                
            } else {
                node = doubleRotateWithLeft(node);
            }
        }
        
    // If the satellite ID is greater than the current node's ID, insert into the right subtree
    } else if (satellite.m_id > node -> m_id) {
        node -> m_right = insert(satellite, node -> m_right);
        
        // Check if after insertion, the right subtree became too tall
        // If so, perform rotations
        if (height (node -> m_right) - height (node -> m_left) == 2) {
            if (satellite.m_id > node -> m_right -> m_id) {
                node = singleRotateWithRight(node);
                
            } else {
                node = doubleRotateWithRight(node);
            }
        }
    }
    
    // Update the height of the current node
    node -> m_height = maxValue(height(node -> m_left), height(node -> m_right)) + 1;
    return node;
}

// remove(int id)
// Traverse the tree to find a node and remove it from the tree
void SatNet::remove(int id){

    // Check if the desired ID is in the tree
    if (findSatellite(id)) {
        m_root = remove(id, m_root);
    }
}

// remove(int nodeId, Sat* node)
// Recursive helper function of remove(int id)
Sat* SatNet::remove(int nodeId, Sat* node) {
    
    // Check if desired ID to remove is nullptr
    if (node == nullptr) {
        return nullptr;
    }
    
    // Traverse the tree by comparing the value of ID
    if (nodeId < node -> m_id) {
        node -> m_left = remove(nodeId, node -> m_left);
        
    } else if (nodeId > node -> m_id) {
        node -> m_right = remove (nodeId, node -> m_right);
        
    } else if (node -> m_left && node -> m_right) {
        node -> m_id = findMin(node -> m_right) -> m_id;
        node -> m_right = remove (node -> m_id, node -> m_right);
        
    } else {
        // If found the exact ID, remove it from the tree
        Sat* oldNode = node;
        
        if (node -> m_left) {
            node = node -> m_left;
        } else {
            node = node -> m_right;
        }
        
        delete oldNode;
    }
    
    // Perform rotations the tree if certain removal causes an imbalance of the tree
    if (node) {
        node -> m_height = maxValue(height(node -> m_left), height(node -> m_right)) + 1;
        
        if (height(node -> m_left) - height(node -> m_right) == 2) {
            if (height(node -> m_left -> m_left) < height (node -> m_left -> m_right)){
                node = doubleRotateWithLeft(node);
            } else {
                node = singleRotateWithLeft(node);
            }
        }
        
        if (height(node -> m_right) - height(node -> m_left) == 2) {
            if (height(node -> m_right -> m_right) < height(node -> m_right -> m_left)) {
                node = doubleRotateWithRight(node);
            } else {
                node = singleRotateWithRight(node);
            }
        }
    }
    
    return node;
}

// listSatellites() const
// List all the current IDs with ascending order
void SatNet::listSatellites() const {
    
    // Helper function to list IDs in inorder
    inOrderList(m_root);
}

// inOrderList(Sat* node) const
// Recursive helper function of listSatellites() const
void SatNet::inOrderList(Sat* node) const {
    if (!node) {
        return;
    }
    
    inOrderList(node -> m_left);
    
    cout << "ID: " << node -> m_id << " Active: " << node -> getAltStr() << " degrees: " << node -> getInclinStr() << " miles" << endl;
    
    inOrderList(node -> m_right);
}

// setState(int id, STATE state)
// Find a node and set node's m_state member variable to state
bool SatNet::setState(int id, STATE state){
    
    // Helper function to find a node
    Sat* sat = find(m_root, id);
    
    // If the node exists, set its state
    if (sat) {
        sat -> setState(state);
        return true;
    }
    
    return false;
}

// removeDeorbited()
// Traverse the tree, find all satellites with DEORBITED state, and remove all
void SatNet::removeDeorbited(){
    
    // Recursive helper function to find satellites with DEORBITED states in the tree
    m_root = removeDeorbitedHelper(m_root);
}

// removeDeorbitedHelper(Sat* node)
// Recursive helper function of removeDeorbited()
Sat* SatNet::removeDeorbitedHelper(Sat* node) {
    if (!node) {
        return nullptr;
    }
    
    node -> m_left = removeDeorbitedHelper(node -> m_left);
    node -> m_right = removeDeorbitedHelper(node -> m_right);
    
    // Check if node's m_states is DEORBITED and remove all from the tree
    if (node -> m_state == DEORBITED) {
        Sat* temp;
        
        if (!node -> m_left) {
            temp = node -> m_right;
            delete node;
            return temp;
            
        } else if (!node -> m_right) {
            temp = node -> m_left;
            delete node;
            return temp;
        }
        
        temp = findMin(node -> m_right);
        node -> m_id = temp -> m_id;
        node -> m_state = temp -> m_state;
        node -> m_inclin = temp -> m_inclin;
        node -> m_altitude = temp -> m_altitude;
        node -> m_right = remove (temp -> m_id, node -> m_right);
    }
    
    // Re-adjust the height of each node after removal
    node -> m_height = maxValue(height(node -> m_left), height(node -> m_right)) + 1;
    
    // Perform rotations if needed after removal
    if (height(node -> m_left) - height(node -> m_right) == 2) {
        if (height(node -> m_left -> m_left) < height (node -> m_left -> m_right)){
            node = doubleRotateWithLeft(node);
        } else {
            node = singleRotateWithLeft(node);
        }
    }
    
    if (height(node -> m_right) - height(node -> m_left) == 2) {
        if (height(node -> m_right -> m_right) < height(node -> m_right -> m_left)) {
            node = doubleRotateWithRight(node);
        } else {
            node = singleRotateWithRight(node);
        }
    }
    
    return node;
}

// countSatellites(INCLIN degree) const
// Count all satellites with the specified inclination by degree
int SatNet::countSatellites(INCLIN degree) const{
    return countByInclination(m_root, degree);
}

// countByInclination(Sat* node, INCLIN degree) const
// Recursive helper function of countSatellites(INCLIN degree) const that counts all satellites with the specified inclination by degree
int SatNet::countByInclination(Sat* node, INCLIN degree) const{
    int count;
    
    // Check if tree is empty
    if (node == nullptr) {
        return 0;
    }
    
    // If specified inclination found, the count is happening
    if (node -> m_inclin == degree) {
        count = 1;
        
    } else {
        count = 0;
    }
    
    // Recursive return to add counts
    return count + countByInclination(node -> m_left, degree) + countByInclination(node -> m_right, degree);
}

// findSatellite(int id) const
// Find a node and check if it is in the tree or not
bool SatNet::findSatellite(int id) const {
    return find(m_root, id) != nullptr;
}

// operator=(const SatNet & rhs)
// Overloaded assignment operator that creates an exact deep copy of another object
const SatNet & SatNet::operator=(const SatNet & rhs){
    
    // Check for self-assignment
    if (this == &rhs) {
        return *this;
    }
    
    clear();
    
    // Make a deep copy from the root of the tree
    m_root = deepCopy(rhs.m_root);
    
    return *this;
}

// deepCopy(Sat* node)
// Recursive helper function of operator=(const SatNet & rhs) that performs a deep copy of the tree
Sat* SatNet::deepCopy(Sat* node) {
    if (node == nullptr) {
        return nullptr;
    }
    
    Sat* newNode = new Sat(*node);
    newNode -> m_left = deepCopy(node -> m_left);
    newNode -> m_right = deepCopy(node -> m_right);
    return newNode;
}

// height(Sat* node)
// Helper function that measures each node's height in the tree
int SatNet::height(Sat* node) {
    if (node != nullptr) {
        return node -> m_height;
    }
    
    return -1;
}

// maxValue(int a, int b)
// Helper function that compares two values and gives the larger value
int SatNet::maxValue(int a, int b) {
    if (a > b) {
        return a;
    }
    
    return b;
}

// findMin(Sat* node)
// Helper function that traverses from the desired node to find the node with minimum ID in the sub-tree
Sat* SatNet::findMin(Sat* node) {
    if (!node) {
        return nullptr;
    }
    
    while (node -> m_left) {
        node = node -> m_left;
    }
    
    return node;
}

// singleRotateWithLeft(Sat* k2)
// Helper function that performs a simple left rotation to balance the tree
Sat* SatNet::singleRotateWithLeft(Sat* k2) {
    if (k2 == nullptr) {
        return nullptr;
    }
    
    // Get the left child of the node
    Sat* k1 = k2 -> m_left;
    if (k1 == nullptr) {
        return k2;
    }
    
    // Perform the left rotation
    k2 -> m_left = k1 -> m_right;
    k1 -> m_right = k2;
    
    // Re-adjust the height of the tree after the rotation
    k2 -> m_height = maxValue(height(k2 -> m_left), height(k2 -> m_right)) + 1;
    k1 -> m_height = maxValue(height(k1 -> m_left), k2 -> m_height) + 1;
    
    return k1;
}

// singleRotateWithRight(Sat* k1)
// Helper function that performs a simple right rotation to balance the tree
Sat* SatNet::singleRotateWithRight(Sat* k1) {
    if (k1 == nullptr) {
        return nullptr;
    }
    
    // Get the right child of the node
    Sat* k2 = k1 -> m_right;
    if (k2 == nullptr) {
        return k1;
    }
    
    // Perform the right rotation
    k1 -> m_right = k2 -> m_left;
    k2 -> m_left = k1;
    
    // Re-adjust the height of the tree after the rotation
    k1 -> m_height = maxValue(height(k1 -> m_left), height(k1 -> m_right)) + 1;
    k2 -> m_height = maxValue(height(k2 -> m_right), k1 -> m_height) + 1;
    
    return k2;
}

// doubleRotateWithLeft(Sat* k3)
// Helper function that performs a left rotation twice
Sat* SatNet::doubleRotateWithLeft(Sat* k3) {
    k3 -> m_left = singleRotateWithRight(k3 -> m_left);
    return singleRotateWithLeft(k3);
}

// doubleRotateWithRight(Sat* k3)
// Helper function that performs a right rotation twice
Sat* SatNet::doubleRotateWithRight(Sat* k3) {
    k3 -> m_right = singleRotateWithLeft(k3 -> m_right);
    return singleRotateWithRight(k3);
}

// getBalance(Sat* node)
// Recursive helper function of isBalanced(Sat* node) that calculates the balance factor of each node
int SatNet::getBalance(Sat* node) {
    if (node == nullptr) {
        return 0;
    }
    
    return height(node -> m_left) - height(node -> m_right);
}

// isBalanced(Sat* node)
// Check each node has valid balance factor range -1 ~ 1 for a balanced tree
bool SatNet::isBalanced(Sat* node) {
    if (node == nullptr) {
        return true;
    }
    
    int balance = getBalance(node);
    
    if (balance > 1 || balance < -1) {
        return false;
    }
    
    return isBalanced(node -> m_left) && isBalanced(node -> m_right);
}

// find(Sat* node, int nodeId) const
// Recursive helper function to locate the specific ID node
Sat* SatNet::find(Sat* node, int nodeId) const{
    if (node == nullptr) {
        return nullptr;
    }
    
    if (nodeId < node -> m_id) {
        return find(node -> m_left, nodeId);
        
    } else if (nodeId > node -> m_id) {
        return find(node -> m_right, nodeId);
        
    } else {
        return node;
    }
}

// traverseAndCheck(Sat* node)
// Recursive helper function of tester.testRemoveDeorbited(SatNet &network) that checks if a node with DERORBITED in the tree or not
bool SatNet::traverseAndCheck(Sat* node) {
    if (node == nullptr) {
        return true;
    }

    // If there is a node with DEORBITED state, gives a flag
    if (node->m_state == DEORBITED) {
        return false;
    }

    // Otherwise, continue the check on the left and right child nodes in the tree
    return traverseAndCheck(node->m_left) && traverseAndCheck(node->m_right);
}

// dumpTree() const
// Visualize a satellite network 
void SatNet::dumpTree() const {
    dump(m_root);
}

// dump(Sat* satellite) const
// Helper function of dumpTree() const to visualize a tree by traversing
void SatNet::dump(Sat* satellite) const{
    if (satellite != nullptr){
        cout << "(";
        dump(satellite -> m_left); // first visit the left child
        cout << satellite -> m_id << ":" << satellite -> m_height; // second visit the node itself
        dump(satellite -> m_right); // third visit the right child
        cout << ")";
    }
}
