#include <array>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <algorithm>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <climits>
#include <omp.h>

#define SIZE 31

int MUTATION_RATE;
int mc;

/**
* The tree class is a representation of a
* binary search tree.
**/
class Tree {
    private:
    std::array<int,SIZE> values;
    public:
    
    /**
    * Create start value. Fill array with values and shuffle.
    **/
    void init() {
        for(int i=0; i<SIZE; i++)
        values[i] = i;
        std::random_shuffle(values.begin(), values.end());
    }
    
    /**
    * Check if a value is contained in the tree.
    * @param a value to check for
    * @return true or false
    **/
    bool contains(int a) const {
        for(int element : values)
        if(element == a)
        return true;
        return false;
    }
    
    /**
    * Returns element at position n
    * @param pos element position
    * @return value at position
    **/
    int get(int pos) const {
        return values[pos];
    }
    
    /**
    * Set a value at given position
    * @param pos position
    * @param value value to set
    **/
    void set(int pos, int value) {
        values[pos] = value;
    }
    
    /**
    * Mutates the tree
    **/
    void mutate() {
        if((rand() % 100) <= MUTATION_RATE) {
            double f0 = fitness(0);
            mc++;
            int i = rand() % SIZE;
            int j = rand() % SIZE;
            int tmp=values[i];
            values[i]=values[j];
            values[j]=tmp;
        }
    }
    
    /**
    * Combines two trees and returns the combination
    * @param other tree that should be combined with this
    * @return new tree
    **/
    Tree combine(const Tree& other) const {
        Tree ret;
        for(int i=0; i<SIZE; i++)
        ret.set(i,-1);
        for(int i=0; i<SIZE; i++) {
            int valueA = this->get(i);
            int valueB = other.get(i);
            bool setValue = false;
            //check if both values exist and
            //try to find a new value that doesn't
            //exist in the ret element
            //if(ret.contains(valueA) && ret.contains(valueB)) {
                //    while(!setValue) {
                    //        int needle = rand() % SIZE;
                    //        if(!ret.contains(needle)) {
                        //            ret.set(i, needle);
                        //            setValue = true;
                    //        }
                    //
                //}
                if(ret.contains(valueA) && ret.contains(valueB)) {
                    setValue = true;
                }
                //check if first exists
                if(!setValue && ret.contains(valueA)) {
                    ret.set(i, valueB);
                    setValue = true;
                }
                //check if second exists
                if(!setValue && ret.contains(valueB)) {
                    ret.set(i, valueA);
                    setValue = true;
                }
                //if no element exists yet lets
                //choose one by random
                if(!setValue) {
                    if(rand()%10 < 5)
                    ret.set(i, valueA);
                    else
                    ret.set(i, valueB);
                }
            }
            std::vector<int> missing;
            for(int i=0; i<SIZE; i++) {
                if(!ret.contains(i))
                missing.push_back(i);
            }
            for(int i=0; i<SIZE; i++) {
                if(ret.get(i)==-1) {
                    bool set=false;
                    while(!set) {
                        int n=rand() % missing.size();
                        if(!ret.contains(missing[n])) {
                            ret.set(i, missing[n]);
                            set=true;
                        }
                    }
                    //                ret.set(i, missing.back());
                    //                missing.pop_back();
                }
            }
            return ret;
        }
        
        /**
        * Check fitness of this tree
        * @param pos position to check
        * @param min minimum value
        * @param max maximum value
        * @param deepness deepness level
        * @return fitness of the tree
        **/
        double fitness(int pos, int min=INT_MIN, int max=INT_MAX, double deepness=1) {
            int lpos = 2*pos+1;
            int rpos = 2*pos+2;
            double fit=0;
            if(values[pos] > max)
            fit=(fit+1)*deepness;
            if(values[pos] < min)
            fit=(fit+1)*deepness;
            if(lpos < SIZE)
            fit+=fitness(lpos, min, values[pos], deepness/2);
            if(rpos < SIZE)
            fit+=fitness(rpos, values[pos], max, deepness/2);
            return fit;
        }
        
        /**
        * Print the tree
        **/
        void print() const {
            std::cout << "[ " << values[0];
            for(int i=1; i<SIZE; i++)
            std::cout << ", " << values[i];
            std::cout << " ]" << std::endl;
        }
    };
    
    /**
    * This method is needed to sort a map of int,double pairs
    **/
    struct sort_pred {
        bool operator()(const std::pair<int,double> &left, const std::pair<int,double> &right) {
            return left.second > right.second;
        }
    };
    
    /**
    * Main function
    **/
    int main() {
        srand (time(NULL));
        /*
        Tree x;
        x.set(0,4); x.set(1,1); x.set(2,9); x.set(3,2); x.set(4,14);
        x.set(5,8); x.set(6,13); x.set(7,0); x.set(8,3); x.set(9,12);
        x.set(10,10); x.set(11,5); x.set(12,7); x.set(13,6); x.set(14,11);
        //getMax at 1 -> 14
        std::cout << x.getMax(0) << std::endl;
        std::cout << x.getMin(0) << std::endl;
        std::cout << x.fitness(0) << std::endl;
        std::cout << x.fitness(3) << std::endl;
        */
        mc=0;
        std::array<Tree, 200> population;
        
        //init population
        for(int i=0; i<100; i++) {
            Tree x;
            x.init();
            population[i] = x;
        }
        
        int count = 0;
        MUTATION_RATE=5;
        
        //alter population
        double start = omp_get_wtime();
        while(population[0].fitness(0) > 0.000) {
            count++;
            std::list<std::pair<int,double>> fitnesses;
            
            //create offspring
            for(int j=0; j<100; j++) {
                int p1 = rand() % 100;
                int p2 = rand() % 100;
                Tree kid = population[p1].combine(population[p2]);
                kid.mutate();
                population[100+j] = kid;
            }
            
            //calc fitness
            for(int k=0; k<200; k++) {
                auto x = std::make_pair(k, population[k].fitness(0));
                fitnesses.push_back(x);
            }
            
            //sort by value
            fitnesses.sort(sort_pred());
            
            //remove old population
            std::array<Tree, 100> newpop;
            for(int j=0; j<100; j++) {
                int newPos = fitnesses.back().first;
                fitnesses.pop_back();
                newpop[j] = population[newPos];
            }
            
            //clear old population and take the new ones
            if(count%10000==0) {
                double end = omp_get_wtime();
                std::cout << "Iteration count: " << count << " in " << (end-start) <<  std::endl;
                std::cout << "Best fitness: " << population[0].fitness(0) << std::endl;
                std::cout << "Weakest fitness: " << population[199].fitness(0) << std::endl;
                std::cout << "Mutation rate: " << MUTATION_RATE << std::endl;
                std::cout << "Mutation count: " << mc << std::endl;
                population[0].print();
                std::cout << std::endl;
                start = omp_get_wtime();
            }
            for(int i=0; i<100; i++) {
                population[i] = newpop[i];
            }
        }
        
        std::cout << "found solution with " << count << " iterations.n";
        population[0].print();
        return 0;
    }
