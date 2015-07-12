/*
Josh Bosley CSCI300 AI
Knapsack 1/0 GA solver
3/22/2012
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#define MAX_ITEMS 100
#define MAX_SOLUTIONS 1000
#define SHUFFLE_NO 25
using namespace std;

class Item
{
 public:
        int value;
        int weight;
} items[MAX_ITEMS];

class Solution
{
 public:
        int value;
        int weight;
        bool flags[MAX_ITEMS];
        void mutate_swap();
        void mutate_shuffle();
        void choose_random();
        void print_result();
        void find_value();
        void find_weight();
        int pop_sort();
} solutions[MAX_SOLUTIONS];

int numberItems = 0;
int knapsackCap = 0;
int bestValue;
int pop_size = 50;

void get_init_pop();
void run_ga(int numgen, int optimal);

int main(int argc, char *argv[])
{
    ifstream inf;
    inf.open("data.txt");

    inf >> numberItems;
    inf >> knapsackCap;

    cout << "Number of items: " << numberItems << endl;
    cout << "Knapsack Capacity: \n" << knapsackCap << endl;

    for (int i=0; i < numberItems; i++)
    {
     inf >> items[i].value >> items[i].weight;
     cout << "Number: " << i+1 << "  Value: " << items[i].value << "  Weight : " << items[i].weight << endl;
    }
    // This is where the fun begins. Populate -> GA 1000 Generations
    // Then we spit it out to the screen.
    bestValue = 0;
    get_init_pop();
    run_ga (1000, bestValue);
    solutions[0].print_result();
    cin.get();
    return 0;
}

void get_init_pop()
{
 int i;
 for (i=0; i<pop_size; i++)
 {
  solutions[i].choose_random();
  solutions[i].find_value();
  solutions[i].find_weight();
 }
}

void Solution::choose_random()
{
    for (int i=0; i<MAX_ITEMS; i++)
        flags[i] = false;
    for (int i=0; i<numberItems; i++)
        flags[i] = rand() % 2;
}

void Solution::find_value()
{
 value = 0;
 for (int i=0;i<numberItems;i++)
    {
        if(flags[i] == true)
        value += items[i].value;
    }
}

void Solution::find_weight()
{
 weight = 0;
 for (int i=0;i<numberItems;i++)
    {
        if(flags[i] == true)
        weight += items[i].weight;
    }
}

void  Solution::mutate_swap()
{
   int x1 = rand() % numberItems;
   int x2 = rand() % numberItems;
   while (x1 == x2)
      x2 = rand() % numberItems;
   bool temp = flags[x1];
   flags[x1] = flags[x2];
   flags[x2] = temp;
}

void  Solution::mutate_shuffle()
{
  for (int i=0; i < SHUFFLE_NO; i++)
   {
      int x1 = rand()%numberItems;
      int x2 = rand()%numberItems;
      while (x1 == x2)
         x2 = rand()%numberItems;
      bool temp = flags[x1];
      flags[x1] = flags[x2];
      flags[x2] = temp;
   }
}

int pop_sort()
{
  for (int i=0; i<2*pop_size; i++)
     if (solutions[i].weight > knapsackCap)
       solutions[i].value = 0;
  for (int i=0; i<2*pop_size-1; i++)
    for (int j=i+1; j<2*pop_size; j++)
    {
	if (solutions[i].value < solutions[j].value)
	   {
	     Solution temp = solutions[i];
	     solutions[i] = solutions[j];
	     solutions[j] = temp;
	    }
    }
  return (solutions[0].value);
}

void run_ga(int numgen, int optimal)
{

 for (int k=0; k < numgen; k++)
 {

    for (int i=0; i < pop_size; i++)
	   solutions[pop_size+i] = solutions[i];
     for (int num=pop_size; num < pop_size*2; num++)
	 {
       int RDM = rand()% 4; // Yeah, 1 in 4
	   if (RDM==1)
	   {
	      solutions[num].mutate_swap();
        }
       else
	   {
	      solutions[num].mutate_shuffle();
        }
       solutions[num].find_weight();
	   solutions[num].find_value();
	  }
     bestValue = pop_sort();
 }
}

void Solution::print_result()
{
    bool trigger = false;
    cout << "\n\n Items: \n";
    for (int T=0; T<numberItems; T++)
    {
      if(flags[T] == true)
      {
         if(trigger == true)
            cout << ", ";
         cout << T+1;
         trigger = true;
      }
    }
    cout << "\nHave been determined to be best options " << endl;
    cout << endl << "\nTotal Value:  " << value << endl;
    cout << "Total Weight: " << weight << endl;
}
