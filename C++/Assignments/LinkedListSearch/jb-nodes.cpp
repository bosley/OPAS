/*
	Josh Bosley
	CSCI201 Assignment # 2
	Linked list searching.

	Using transform and move-to-front methods to see the outcome of associated data.
	Input file is " input.jb ."
*/

#include "jb-nodes.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <ctime>

#define NUM_WORDS_SEARCH 20
#define INPUT_FILE "input.jb"
#define NOT_ALLOWED 40
#define COLUMN_NUM 9

using namespace std;

/*	Initiate the pointers.	*/
JNodes::JNodes(){

	head  = NULL;
	curr  = NULL;
	temp  = NULL;
    }

/*	Go through each node until data matches. Return the hops. */
int JNodes::GetJumpNum(char *chkData)
{
	int ck = 0;						// Jump counter. Start at 0 in case head is node.
	curr = head;
	while(curr->next != NULL) 				// Go through nodes.
	{
		if (!strcmp(curr->data, chkData))		// If its the data we're looking for break.
		{
			break;
		}
		else
		{
			ck++;					// Jump count.
			curr = curr->next;			// Move down a node.
		}
	}

	if(strcmp(curr->data, chkData) && curr->next == NULL)   // Not in list, return -1
	{
		return -1;
	}

	return ck;						// If break, return
}

/*	Create a new node, and loop until end is found. Add to end.	*/
void JNodes::AddNode(char *addData){

        nodePtr n = new node;					// Create the node and set data.
        n->next = NULL;
	strcpy(n->data, addData);

        if(head != NULL){					// If head insnt null, assign and find the last node.
            curr = head;
            while(curr->next != NULL){
                curr = curr->next;
                }
                curr->next = n;					// Once tail node is found, add new node to tail->next

            } else {

                head = n;					// If there is nothing, add to head.

                }
    }

/*	Find Node with data, and delete. Mend pointers	*/
void JNodes::DeleteNode(char *delData){

    nodePtr delPtr = NULL;					// New temp pointer for deletion, and temps for
    temp = head;						// travelling nodes.
    curr = head;

//    cout << "data to delete: " << delData << endl; //		data to delete

    while(curr != NULL && strcmp(curr->data, delData))		// Find the node to delete.
    {

        temp = curr;						// Assign temp for fixing list
        curr = curr->next;

        }
// Being safe. :)
    if(curr == NULL)
    {

        delete delPtr;						// If node not found, delete the temp pointer.

    }
	else
    {

//	cout << "Current data: " << curr->data << endl;

      delPtr = curr;						//	Set delete pointer to the current node.
      curr = curr->next;
      temp->next = curr;					//	Ammend pointers.

      if (delPtr == head)
      {
      	 head = head->next;					// 	If is head node, reassign head.
      	 temp = NULL;
      }
            delete delPtr;					// 	Delete the node.
     }

}

// If exist, return True
bool JNodes::CheckExist(char *chkData)
{
    curr = head;
    while(curr != NULL)						//	Go through list
    {
	if (!strcmp(curr->data,chkData))			// 	Data found .. return true
	{
		return true;
	}

        curr = curr->next;					// 	node++

        }

	return false;						//	Not in list, return false.
   }



void JNodes::MoveToFront(char *data)
{
	DeleteNode(data);					// 	Delete the existing node with *data
	curr = head;						// 	Set the current pointer to head
	nodePtr n = new node;
	strcpy(n->data, data);					// 	Create new node with n->data = *data
	n->next = curr;						// 	Set the second item in the list to the old head
	head = n;						// 	Set new node as head of list.
}

void JNodes::TransposePos(int index)
{
	curr = head;
	temp = head;

	int cnt = 0;
	char gtemp[MAX_DATA];					//	Temp for data

	if(index > 0)						//	If not head
	{
		for(cnt; cnt < index; cnt++)			// 	Cycle through to get to node at index (jumpnum)
		{
			curr = curr->next;

			if ( cnt != (index-1) )
			{
				temp = temp->next;		//	Set temp to node-1 (spot to switch to
			}
		}
		strcpy(gtemp, temp->data);			//	Switch temp and curr data
		strcpy(temp->data, curr->data);
		strcpy(curr->data, gtemp);
	}
}


/*	Go through each element int list od nodes and print data.	*/
void JNodes::PrintList()
{
    int a = 0;
    int count = 1;
    curr = head;
    while(curr != NULL)						//	Loop through list
    {
	if(a<COLUMN_NUM)
	{
		cout << left
		<< setw(15)  << curr->data;
		a++;
	} else {
		cout << "\n";
		a = 0;
	}
	 curr = curr->next;					//	node++
	 count++;
    }
	cout << endl;
}

int main()
{
	JNodes jist;						// 	Make the list
 	ifstream in;
 	in.open(INPUT_FILE);					//	open input file
	srand( time(NULL) );

	char s[32];						//	Illegal character array
    	int a, b, c, j, k;
	const char notAllowed[NOT_ALLOWED] = ",.<\">;':{}[]-=_+@#$%^&*()~?!`1234567890";


	vector<string> filtered_words;	// Filtered words. For keeping the initial list state
	vector<string> speech;		// The un-filtered text.
	vector<int> wsearch;		// The random numbers used to generate a search.
	vector<int> pre_comp_nums;		// The number of compairisons for each word before.
	vector<int> post_comp_nums;		// The number of compairisons for each word after.

	cout << "\033[2J";					//	Clear the terminal

    	while(!in.eof())
    	{
		in >> s;					//	Get a word, loop through it and filter characters
		for (j=0; s[j] != '\0'; j++)
		{
			for(k=0; k<NOT_ALLOWED;k++)
				if (s[j] == notAllowed[k])
					s[j] = ' ';
			s[j] = toupper(s[j]);			//	After filter, uppercase the character
		}

		speech.push_back(s);

		if(!jist.CheckExist(s))				//	Check to see if imported word is in the list
		{
			jist.AddNode(s);			//	If it isnt, add it.
			filtered_words.push_back(s);		//	Add to filtered vec.
		}

	    } //EOF

	in.close();						//	close the file

// Linked list now populated with uppercase alphnumeric unique nodes, and there are two vectors with useful data.
// query_word which is filled with formatted words of the speech. This is for generating seatch terms.
// filtered_words the second vector, that contains the inital state of the node list. For repairing post-search.
// The int vector (not yet full) will be populated with the location of words in filtered_words what will be used
// for automating searches.

	for (k = 0; k < NUM_WORDS_SEARCH; k++)	//	Fill an int vector with random ints for word search
	{							//	I left out duplicate checking on purpose.
		wsearch.push_back(rand() % filtered_words.size());
	}

	cout 	<< "Words from file [" << speech.size() << "] | Words in list [" << filtered_words.size()
	 	<< "]" << "\n\nContents of linked list :" << endl;

	jist.PrintList();

	// --------------------------------------------------------------------------------------------------------//

	// THIS IS WHERE MOVE TO FRONT BEGINS

	a, b, c, k = 0;	// RESET REQ INTS

	for (j = 0; j < NUM_WORDS_SEARCH; j++)							//
	{
		a = wsearch[j];
		k = jist.GetJumpNum( (char *)filtered_words[a].c_str());
		pre_comp_nums.push_back(k);
		b = b + k;
	}
	c = (b / NUM_WORDS_SEARCH);

	cout << "\n\nWords to search for : \n " << endl;

	for (j = 0; j < NUM_WORDS_SEARCH; j++)							// Move to front searching
	{
		// Apply Move to front.
		a = wsearch[j];
		jist.MoveToFront( (char *) filtered_words[a].c_str() );
		cout << filtered_words[a] << " ";
	}

	cout << "\n\nBefore Move-To-Front \nTotal comparisons : " << b << " | Average comparisons : " << c << endl;

	a, b, c, k = 0;		// RESET REQ INTS

	for (j = 0; j < NUM_WORDS_SEARCH; j++)							//
	{
		a = wsearch[j];
		k = jist.GetJumpNum( (char *)filtered_words[a].c_str());
		pre_comp_nums.push_back(k);
		b = b + k;
	}
	c = (b / NUM_WORDS_SEARCH);

	cout << "\n\nAfter Move-To-Front \nTotal comparisons : " << b << " | Average comparisons : " << c << endl;
	cout << "\nLinked list after Move-to-front:  \n" << endl;
	jist.PrintList();

	// THIS IS WHERE MOVE TO FRONT ENDS


	// --------------------------------------------------------------------------------------------------------//

												// Reset linked list
	//		---	RESET THE Linked List.	---
	for(vector<string>::size_type i = 0; i < filtered_words.size(); i++)
	{
		char *temp = (char *)filtered_words[i].c_str();
		jist.DeleteNode(temp);
	}
	for(vector<string>::size_type i = 0; i < filtered_words.size(); i++)
	{
		char *temp = (char *)filtered_words[i].c_str();
		jist.AddNode(temp);
	}
	//		---	/RESET	---


	// --------------------------------------------------------------------------------------------------------//

	cout << "\n\n Contents of linked list have been reset to initial entry. \n\n";

	// --------------------------------------------------------------------------------------------------------//
	//	THIS IS WHERE TRANSPOSE BEGINS

	cout << "\n\nWords to search for : \n " << endl;

	a, b = 0;
	for (j = 0; j < NUM_WORDS_SEARCH; j++)
	{
		// Transpose.
		b = jist.GetJumpNum( (char *)filtered_words[a].c_str());
		jist.TransposePos( wsearch[j] );
		cout << filtered_words[ wsearch[j]  ] << " ";
	}

	a, b, c, k = 0;		// RESET REQ INTS

	for (j = 0; j < NUM_WORDS_SEARCH; j++)							//
	{
		a = wsearch[j];
		k = jist.GetJumpNum( (char *)filtered_words[a].c_str());
		pre_comp_nums.push_back(k);
		b = b + k;
	}
	c = (b / NUM_WORDS_SEARCH);

	cout << "\n\nAfter Transpose \nTotal compairisons : " << b << " | Average compairisons : " << c << endl;
	cout << "\nLinked list after Transpose:  \n" << endl;
	jist.PrintList();

} //   END MAIN











