/**********************************************
 ** File: mytest.cpp
 ** Project: CMSC 341 Project 2, Fall 2023
 ** Author: Joshua Hur
 ** Date: 10/24/23
 ** Section: 2
 ** E-mail: jhur1@umbc.edu
 **
 ** This file contains test functions to test binary search tree properties.
 ************************************************************************/

#include "satnet.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>

enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution
};

class Tester{
    public:
    
    // testInsertNormal(SatNet &network)
    // Case: Verify insert function works in normal case
    // Expected result: Return true if the new node is successfully added in the tree, else return false
    bool testInsertNormal(SatNet &network) {
        Sat satellite(50000, MI215, I53);
        network.insert(satellite);
        
        // Check if the node added in the tree or not
        if (network.findSatellite(50000)) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // testInsertEdge(SatNet &network)
    // Case: Verify insert function works in edge case
    // Expected result: Return true if the new node is successfully added in the tree, else return false
    bool testInsertEdge(SatNet &network) {
        
        // Check if the node added in the tree or not
        if (network.findSatellite(MINID)) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // testInsertError(SatNet &network)
    // Case: Verify insert function works in error case
    // Expected result: Return true if the new node that is outside of range is not added in the tree, else return false
    bool testInsertError(SatNet &network) {
        
        // Check if the node added in the tree or not
        if (!network.findSatellite(MAXID + 10)) {
            return true;
            
        } else {
            return false;
        }
    }

    // testTreeBalance(SatNet &network)
    // Case: Verify the tree has a BST is balanced
    // Expected result: Return true if each node has valid balance factor from -1 to 1, else return false
    bool testTreeBalance(SatNet &network) {
        if (network.isBalanced(network.m_root)) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // isBST(const Sat* node, int min, int max)
    // Recursive helper function of testBSTProperty(SatNet &network) that checks each node's left child has lesser ID value and right child has greater ID value than the parent
    bool isBST(const Sat* node, int min, int max) {
        if (node == nullptr) {
            return true;
        }
        
        if (node -> m_id <= min || node -> m_id > max) {
            return false;
        }
        
        return isBST(node -> m_left, min, node -> m_id) && isBST(node->m_right, node->m_id, max);
    }
    
    // testBSTProperty(SatNet &network)
    // Case: Verify the tree has binary search tree property
    // Expected result: Return true if each node has left child that is lesser ID than the parent and right child is greater ID than the parent node, else return false
    bool testBSTProperty(SatNet &network) {
        if(isBST(network.m_root, MINID, MAXID)) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // testRemoveNormal(SatNet &network, int idToRemove)
    // Case: Verify remove function works in normal case
    // Expected result: Return true if desired ID was removed and cannot found in the tree anymore, else return false
    bool testRemoveNormal(SatNet &network, int idToRemove) {
        network.remove(idToRemove);
        if (network.findSatellite(idToRemove)) {
            return false;
            
        } else {
            return true;
        }
    }
    
    // testRemoveEdge(SatNet &network)
    // Case: Verify remove function works in edge case
    // Expected result: Return true if the largest ID in the tree was removed and cannot found in the tree anymore, else return false
    bool testRemoveEdge(SatNet &network) {
        Sat* root = network.getRoot();
        Sat* largestIDNode = root;
        
        // Find the node that has the lagest ID in the tree
        while (largestIDNode -> m_right != nullptr) {
            largestIDNode = largestIDNode -> m_right;
        }
        
        // Remove the node that has the largest ID
        int largestID = largestIDNode -> m_id;
        network.remove(largestID);
        
        // Check if the removed node is still existed in the tree or not
        if (network.findSatellite(largestID)) {
            return false;
            
        } else {
            return true;
        }
    }
    
    // testRemoveDeorbited(SatNet &network)
    // Case: Verify removeDeorbited function successfully removes all nodes with the DEORBITED state 
    // Expected result: Return true if there is still nodes with the DEORBITED state left in the tree after the removal, else return false
    bool testRemoveDeorbited(SatNet &network) {
        return !network.traverseAndCheck(network.getRoot());
    }
    
    // testCountSatellite(SatNet &network)
    // Case: Verify countSatellite function counts node with specified inclination
    // Expected result: Return true if all the counts with specific inclination is matched, else return false
    bool testCountSatellite(SatNet &network) {
        
        network.insert(Sat(10005, MI208, I48));
        network.insert(Sat(10006, MI215, I48));
        network.insert(Sat(10007, MI340, I53));
        network.insert(Sat(10008, MI350, I70));
        network.insert(Sat(10009, MI350, I70));
        network.insert(Sat(10010, MI350, I70));
        
        if(network.countSatellites(I48) == 2 &&
           network.countSatellites(I53) == 1 &&
           network.countSatellites(I70) == 3) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // testFindSatelliteNormal(SatNet &network, int id)
    // Case: Verify findSatellite function works in normal case
    // Expected result: Return true the function can found the desired existed node in the tree, else return false
    bool testFindSatelliteNormal(SatNet &network, int id) {
        if (network.findSatellite(id)) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // testFindSatelliteError(SatNet &network, int id)
    // Case: Verify findSatellite function works in error case
    // Expected result: Return true the function cannot found the desired non-existed node in the tree, else return false
    bool testFindSatelliteError(SatNet &network, int id) {
        if (!network.findSatellite(id)) {
            return true;
        } else {
            return false;
        }
    }
    
    // testAssignmentOperatorNormal(SatNet &copiedNet, int idOne, int idTwo, int idThree)
    // Case: Verify the assignment operator in normal case
    // Expected result: Return true the copied tree has the same nodes in the original, else return false
    bool testAssignmentOperatorNormal(SatNet &originalNet, SatNet &copiedNet) {
        return areTreesEqual(originalNet.getRoot(), copiedNet.getRoot());
    }
    
    // areTreesEqual(Sat* originalRoot, Sat* copiedRoot)
    // Recursive helper function of testAssignmentOperatorNormal(SatNet &originalNet, SatNet &copiedNet) that checks two trees have the same nodes and equal
    bool areTreesEqual(Sat* originalRoot, Sat* copiedRoot) {
        // Both nodes are NULL, trees are still identical
        if (originalRoot == nullptr && copiedRoot == nullptr) {
            return true;
        }

        // One of them is NULL, trees are not identical
        if (originalRoot == nullptr || copiedRoot == nullptr) {
            return false;
        }

        // If the values are different, trees are not identical
        if (originalRoot -> m_id != copiedRoot -> m_id) {
            return false;
        }

        // Recursively check left and right subtrees
        return areTreesEqual(originalRoot -> m_left, copiedRoot -> m_left) &&
               areTreesEqual(originalRoot -> m_right, copiedRoot -> m_right);
    }
    
    // testAssignmentOperatorError(SatNet &networkOne, SatNet &networkTwo)
    // Case: Verify the assignment operator in error case
    // Expected result: Return true the copied and the original tree are both empty, else return false
    bool testAssignmentOperatorError(SatNet &networkOne, SatNet &networkTwo) {
        if(networkOne.getRoot() == nullptr && networkTwo.getRoot() == nullptr) {
            return true;
            
        } else {
            return false;
        }
    }
    
    // testRemovalTimeComplexity()
    // Case: Verify remove operation performs in O(log n)
    // Expected result: Performs two different removal operations and compare the difference of the speed is within the acceptable ratio range
    bool testRemovalTimeComplexity(){
        Random idGen(MINID,MAXID);
        Random inclinGen(0,3);
        Random altGen(0,3);

        SatNet network1;
        SatNet network2;

        int size1 = 1000;
        int size2 = 2000;
        int tempIDOne[size1];
        int tempIDTwo[size2];

        const int ABSOLUTEERROR = 0.4;

        // Insert 1000 satellites into network1 and measure the time T1
        for(int i = 0;i < size1; i++){
            int id = idGen.getRandNum();
            tempIDOne[i] = id;
            Sat satellite(id,
                        static_cast<ALT>(altGen.getRandNum()),
                        static_cast<INCLIN>(inclinGen.getRandNum()));
            network1.insert(satellite);
        }
        
        clock_t start1 = clock();
        for(int i = 0; i < size1; i++) {
            network1.remove(tempIDOne[i]);
        }
        
        clock_t end1 = clock();
        double T1 = (double)(end1 - start1) / CLOCKS_PER_SEC;

        // Insert 2000 satellites into network2 and measure the time T2
        for(int i = 0;i < size2; i++){
            int id = idGen.getRandNum();
            tempIDTwo[i] = id;
            Sat satellite(id,
                        static_cast<ALT>(altGen.getRandNum()),
                        static_cast<INCLIN>(inclinGen.getRandNum()));
            network2.insert(satellite);
        }
        
        clock_t start2 = clock();
        for(int i = 0; i < size2; i++) {
            network2.remove(tempIDTwo[i]);
        }
        
        clock_t end2 = clock();
        double T2 = (double)(end2 - start2) / CLOCKS_PER_SEC;

        // Calculate the ratio T2 / 2T1
        double ratio = T2 / (2 * T1);

        // Check whether the ratio is within the acceptable range
        if (ratio >= (ratio - ABSOLUTEERROR) && ratio <= (ratio + ABSOLUTEERROR)) {
            return true;
            
        } else {
            return false;
        }
    }
    
    
    // testInsertionTimeComplexity()
    // Case: Verify insert operation performs in O(log n)
    // Expected result: Performs two different insert operations and compare the difference of the speed is within the acceptable ratio range
    bool testInsertionTimeComplexity(){
        Random idGen(MINID,MAXID);
        Random inclinGen(0,3);
        Random altGen(0,3);

        SatNet network1;
        SatNet network2;

        int size1 = 1000;
        int size2 = 2000;
        
        const int ABSOLUTEERROR = 0.4;

        // Insert 1000 satellites into network1 and measure the time T1
        clock_t start1 = clock();
        for(int i = 0;i < size1; i++){
            int id = idGen.getRandNum();
            Sat satellite(id,
                        static_cast<ALT>(altGen.getRandNum()),
                        static_cast<INCLIN>(inclinGen.getRandNum()));
            network1.insert(satellite);
        }
        clock_t end1 = clock();
        double T1 = (double)(end1 - start1) / CLOCKS_PER_SEC;

        // Insert 2000 satellites into network2 and measure the time T2
        clock_t start2 = clock();
        for(int i = 0;i < size2; i++){
            int id = idGen.getRandNum();
            Sat satellite(id,
                        static_cast<ALT>(altGen.getRandNum()),
                        static_cast<INCLIN>(inclinGen.getRandNum()));
            network2.insert(satellite);
        }
        clock_t end2 = clock();
        double T2 = (double)(end2 - start2) / CLOCKS_PER_SEC;

        // Calculate the ratio T2 / 2T1
        double ratio = T2 / (2 * T1);

        // Check whether the ratio is within the acceptable range
        if (ratio >= (ratio - ABSOLUTEERROR) && ratio <= (ratio + ABSOLUTEERROR)) {
            return true;

        } else {
            return false;
        }
    }

};

int main(){
    Tester tester;
    Random idGen(MINID,MAXID);
    Random inclinGen(0,3);  // there are 4 inclination
    Random altGen(0,3);     // there are 4 altitudes
    {
        SatNet network;
        int teamSize = 10;
        int tempID = 0;
        int ID = 0;
        {
            SatNet networkOne;
            
            for(int i = 0; i < teamSize; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkOne.insert(satellite);
            }
            
            if (tester.testInsertNormal(networkOne)) {
                cout << "Normal case test passed!" << endl;
                
            } else {
                cout << "Normal case test failed!" << endl;
            }
        }
        
        {
            SatNet networkTwo;
            for(int i = 0; i < teamSize; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkTwo.insert(satellite);
            }
            
            Sat satellite(MINID, MI215, I53);
            networkTwo.insert(satellite);
            
            if (tester.testInsertEdge(networkTwo)) {
                cout << "Edge case test passed!" << endl;
                
            } else {
                cout << "Edge case test failed!" << endl;
            }
        }
        
        {
            SatNet networkThree;
            
            for(int i = 0; i < teamSize; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkThree.insert(satellite);
            }
            
            Sat satellite(MAXID + 10, MI215, I53);
            networkThree.insert(satellite);
            
            if (tester.testInsertError(networkThree)) {
                cout << "Error case test passed!" << endl;
                
            } else {
                cout << "Error case test failed!" << endl;
            }
        }
        
        {
            SatNet networkFour;
            for(int i = 0; i < 300; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkFour.insert(satellite);
            }
            
            if (tester.testTreeBalance(networkFour)) {
                cout << "Balance test passed!" << endl;
                
            } else {
                cout << "Balance test failed!" << endl;
            }
        }
        
        {
            SatNet networkFive;
            for(int i = 0; i < 300; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkFive.insert(satellite);
            }
            
            if (tester.testBSTProperty(networkFive)) {
                cout << "BST property is preserved!" << endl;
                
            } else {
                cout << "BST property is NOT preserved!" << endl;
            }
        }
        
        {
            SatNet networkSix;
            for(int i = 0; i < teamSize; i++){
                ID = idGen.getRandNum();
                if (i == teamSize / 2) tempID = ID;//we store this ID for later use
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkSix.insert(satellite);
            }

            if (tester.testRemoveNormal(networkSix, tempID)) {
                cout << "Normal remove case is passed!" << endl;
                
            } else {
                cout << "Normal remove case is failed!" << endl;
            }
        }
        
        {
            SatNet networkSeven;
            for(int i = 0; i < teamSize; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkSeven.insert(satellite);
            }
            
            if (tester.testRemoveEdge(networkSeven)) {
                cout << "Edge remove case is passed!" << endl;
                
            } else {
                cout << "Edge remove case is failed!" << endl;
            }
        }
        
        {
            SatNet networkEight;
            vector<int> idsToRemove;
            for(int i = 0; i < 300; i++){
                ID = idGen.getRandNum();
                Sat satellite(ID,
                              static_cast<ALT>(altGen.getRandNum()),
                              static_cast<INCLIN>(inclinGen.getRandNum()));
                networkEight.insert(satellite);
                if (i < 150) {
                    idsToRemove.push_back(ID);
                }
            }
            
            for (int startId : idsToRemove) {
                networkEight.remove(startId);
            }

            if (tester.testTreeBalance(networkEight)) {
                cout << "Balance test passed!" << endl;
                
            } else {
                cout << "Balance test failed!" << endl;
            }
            
            if (tester.testBSTProperty(networkEight)) {
                cout << "BST property is preserved!" << endl;
                
            } else {
                cout << "BST property is NOT preserved!" << endl;
            }
        }
        
        {
            SatNet networkNine;
            networkNine.insert(Sat(10001, MI208, I48, DEORBITED));
            networkNine.insert(Sat(10002, MI215, I53, ACTIVE));
            networkNine.insert(Sat(10003, MI340, I70, ACTIVE));
            networkNine.insert(Sat(10004, MI350, I97, DEORBITED));
            networkNine.insert(Sat(10011, MI340, I70, ACTIVE));
            networkNine.insert(Sat(10008, MI350, I97, DEORBITED));
            networkNine.insert(Sat(10010, MI340, I70, ACTIVE));
            networkNine.insert(Sat(10006, MI350, I97, DEORBITED));
            networkNine.removeDeorbited();
            
            if (!tester.testRemoveDeorbited(networkNine)) {
                cout << "Remove Deorbited test is passed!" << endl;
                
            } else {
                cout << "Remove Deorbited test failed!" << endl;
            }
        }
        
        {
            SatNet networkTen;
            
            if(tester.testCountSatellite(networkTen)) {
                cout << "Count Satellite test is passed!" << endl;
                
            } else {
                cout << "Count Satellite test is failed!" << endl;
            }
        }
        
        {
            SatNet networkEleven;
            networkEleven.insert(Sat(10011, MI208, I70));
            int desiredID = 10011;
            
            if(tester.testFindSatelliteNormal(networkEleven, desiredID)) {
                cout << "Find Satellite normal test is passed!" << endl;
                
            } else {
                cout << "Find Satellite normal test is failed!" << endl;
            }
            
            int anotherDesiredID = 10099;
            if(tester.testFindSatelliteError(networkEleven, anotherDesiredID)) {
                cout << "Find Satellite error test is passed! Desired ID is not found." << endl;
                
            } else {
                cout << "Find Satellite error test is failed!" << endl;
            }
        }
        
        {
            SatNet originalNet;
            Sat sat1(10001, MI208, I48);
            Sat sat2(10002, MI215, I53);
            Sat sat3(10003, MI340, I70);
            Sat sat4(10006, MI340, I70);

            originalNet.insert(sat1);
            originalNet.insert(sat2);
            originalNet.insert(sat3);
            
            SatNet copiedNet;
            copiedNet = originalNet;
            
            if (tester.testAssignmentOperatorNormal(originalNet, copiedNet)) {
                cout << "Normal assignment test is passed!" << endl;
                
            } else {
                cout << "Normal assignment test is failed!" << endl;
            }
        }
        
        {
            SatNet emptyOriginalNet;
            SatNet emptyCopiedNet;
            
            emptyCopiedNet = emptyOriginalNet;
            
            if (tester.testAssignmentOperatorError(emptyOriginalNet, emptyCopiedNet)) {
                cout << "Error assignment test is passed!" << endl;
                
            } else {
                cout << "Error assignment test is failed!" << endl;
            }
        }
        
        {
            if (tester.testRemovalTimeComplexity()) {
                cout << "The Removal time complexity test passed. The AVL tree removal performs in O(log n)." << endl;
            } else {
                cout << "The Removal time complexity test failed." << endl;
            }
        }
        
        {
            if (tester.testInsertionTimeComplexity()){
                cout << "The Insertion time complexity test passed. The AVL tree insertion performs in O(log n)." << endl;
            } else {
                cout << "The Insertion time complexity test failed." << endl;
            }
        }
    }
}
