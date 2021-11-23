#include <omp.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;

const int MIN_SUPPORT = 2;
const float MIN_CONFIDENCE = 1.5;

vector< vector<string> > read_file(char file_name[]);
void find_itemsets(vector<string> matrix, vector<string> candidates, map<string,int> &temp_dictionary, int k, int item_idx, string itemset, int current);
void prune_itemsets(map<string,int> &temp_dictionary, vector<string> &candidates, int min_support);
void update_candidates(vector<string> &candidates, string itemset);
void combinations(int offset, int k, vector<string> &comb, vector<string> &items);


// ------------------------------------------------------------
// Main
// ------------------------------------------------------------

int main (){
    char file_name[] = "./prova.txt";
    vector< vector<string> > matrix;
    map<string,int> dictionary;
    map<string,int> temp_dictionary;
    vector<string> candidates;

    // read file into 2D vector matrix
    matrix = read_file(file_name);

    // read matrix and insert 1-itemsets in dictionary as key with their support as value
    for (int i = 0; i < matrix.size(); i++){
        for (int j = 0; j < matrix[i].size(); j++){
            string item = matrix[i][j];

            if(dictionary.count(item)){ // if key exists
                dictionary[item]++;
            }
            else{
                dictionary.insert(pair<string,int>(item, 1));
            }
        }
    }

    // prune from dictionary 1-itemsets with support < min_support and insert items in candidates vector
    prune_itemsets(dictionary, candidates, MIN_SUPPORT);

    // insert in dictionary all k-itemset
    int n = 2; // starting from 2-itemset
    do{
        temp_dictionary.clear();
        // read matrix and insert n-itemsets in temp_dictionary as key with their support as value
        for (int i = 0; i < matrix.size(); i++){
            find_itemsets(matrix[i], candidates, temp_dictionary, n, -1, "", 0);
        }
        // prune from temp_dictionary n-itemsets with support < min_support and insert items in candidates vector
        prune_itemsets(temp_dictionary, candidates, MIN_SUPPORT);
        // append new n-itemsets to main dictionary
        dictionary.insert(temp_dictionary.begin(), temp_dictionary.end());
        n++;
    }while(!temp_dictionary.empty());

    cout<<"KEY\tVALUE\n";
    for (map<string, int>::iterator itr = dictionary.begin(); itr != dictionary.end(); ++itr) {
        cout << itr->first << '\t' << itr->second << '\n';
    }

    
    // stringstream ss;
    // vector<string> items;
    // vector<string> comb;
    // string item;

    // for (map<string, int>::iterator itr = dictionary.begin(); itr != dictionary.end(); ++itr) {
    //     if(!(itr->first.find(' ') != string::npos)) continue; // if itemset has just 1 element continue
    //     cout<<"-------\n"<<itr->first<<endl;
    //     ss << itr->first;
    //     while(getline (ss, item, ' ')) {
    //         items.push_back(item);
    //     }
    //     for (int idx = items.size()-1; idx>0; idx--){
    //         combinations(0, idx, comb, items);
    //     }
        
    //     ss.clear();
    //     items.clear();
    // }

    return 0;
}

// ------------------------------------------------------------
// Functions
// ------------------------------------------------------------

vector< vector<string> > read_file(char file_name[]){
    ifstream myfile (file_name);

    vector< vector<string> > matrix;
    vector<string> row;  
    
    string line;
    stringstream ss;
    string item;

    while(getline (myfile, line)){
        ss << line;

        while(getline (ss, item, ' ')) {
            row.push_back(item);
        }

        sort(row.begin(), row.end());
        matrix.push_back(row);

        ss.clear();
        row.clear();
    }

    myfile.close();

    return matrix;
}

void find_itemsets(vector<string> matrix, vector<string> candidates, map<string,int> &temp_dictionary, int k, int item_idx, string itemset, int current){
    if(current == k){
        itemset = itemset.erase(0,1); // remove first space
        if(temp_dictionary.count(itemset)){ // if key exists
                temp_dictionary[itemset]++;
        }
        else{
            temp_dictionary.insert(pair<string,int>(itemset, 1));
        }
        return;
    }
    
    string item;
    for (int j = ++item_idx; j < matrix.size(); j++){
        item = matrix[j];

        if(find(candidates.begin(), candidates.end(), item) != candidates.end()){
            find_itemsets(matrix, candidates, temp_dictionary, k, j, itemset + " " + item, current+1);
        }
    }
}

void prune_itemsets(map<string,int> &temp_dictionary, vector<string> &candidates, int min_support){
    candidates.clear(); // empty candidates to then update it
    for (map<string, int>::iterator it = temp_dictionary.begin(); it != temp_dictionary.end(); ){
        if (it->second < MIN_SUPPORT){
            temp_dictionary.erase(it++);
        }
        else{
            update_candidates(candidates, it->first);
            ++it;
        }
    }
}

void update_candidates(vector<string> &candidates, string itemset){
    stringstream ss;
    string item;
    ss << itemset;
    while(getline (ss, item, ' ')) {
        if(!(find(candidates.begin(), candidates.end(), item) != candidates.end())){
            candidates.push_back(item);
        }
    }
}


void combinations(int offset, int k, vector<string> &comb, vector<string> &items) {
  if (k == 0){
        cout << "[ ";
        for (int i = 0; i < comb.size(); ++i) { cout << comb[i] << " "; }
        cout << "] " << endl;
        return;
  }
  for (int i = offset; i <= items.size() - k; ++i) {
    comb.push_back(items[i]);
    combinations(i+1, k-1, comb, items);
    comb.pop_back();
  }
}