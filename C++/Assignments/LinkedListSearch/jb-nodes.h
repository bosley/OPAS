/*

	Author:			Josh Bosley
	Description: 	A linked list of nodes, with delete / add / exist? fucntionality.
	Purpose:		Provide a linked list of nodes.
				-> This version has been modified from my original code to fit
					the requirements of the assignment.
*/

#ifndef JBNODE_H
#define JBNODE_H
#define MAX_DATA 32	// Max word length

class JNodes
{
    private:

	// Structure for the actual nodes.
        typedef struct node
	{

        	char data[MAX_DATA];
		node* next;
		// Icky SLL

	}* nodePtr;

	// Pointers for the traversing the nodes
        nodePtr  head;
        nodePtr  curr;
        nodePtr  temp;

    public:

        JNodes();				//  Constructor
	int  GetJumpNum(  char *chkData);	//  Return position of data in list
        void AddNode   (  char *addData);	//  Append a node to the end of the list
        void DeleteNode(  char *delData);	//  Delete a node from the list, and append pointers
	bool CheckExist(  char *chkData);	//  See if the data exists in the list already or not.
	void PrintList();			//  cout data from all nodes.
	void MoveToFront( char *data);		//  Look for node with data, move to front.
	void TransposePos(int index );		//  Swap positions of words

};
#endif
