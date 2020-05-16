#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "structure.h"
#define parent_char -1
#define ascii 256

/* Sort nodes by frequency */
struct node *sorted_insert_node_freq(struct node *head, struct node *insert){
	if ((insert -> frequency) < (head -> frequency)){
		(insert -> next) = head;
		return insert;
	} else if ((insert -> frequency) > (head -> frequency)){
		if (NULL == (head -> next)){
			(head -> next) = insert;
			return head;
		} else {
	(head -> next) = sorted_insert_node_freq((head -> next), insert);
			return head;
		}
	} else {
		return sorted_insert_node_ascii(head, insert);
	}
}


/* Sort nodes by ascii */
struct node *sorted_insert_node_ascii(struct node *head, struct node *insert){
	if (insert -> character == -1){
		(insert -> next) = head;
		return insert;
	}
	if ((insert -> character) < (head -> character)){
		(insert -> next) = head;
		return insert;
	} else {
		if (NULL == (head -> next)){
			(head -> next) = insert;
			return head;
		} else {
	(head -> next) = sorted_insert_node_freq((head -> next), insert);
			return head;
		}
	}
}

/* Create nodes from inputted information to create list */
struct node *create_node(int newchar, int newfreq){
	struct node *temp;	
	temp = (struct node *)malloc(sizeof(struct node));
	(temp -> frequency) = newfreq;
	(temp -> character) = newchar;	
	(temp -> left) = NULL;
	(temp -> right) = NULL;
	(temp -> next) = NULL;
	return temp;
}

/* Print out frequency and ascii values, assisted with testing throughout */
void printout(struct node *output){
printf("freq: %i, ascii: %i\n", (output -> frequency), (output -> character));
}

/* Remove head of linked list when added to tree */
struct node *remove_top(struct node *head){
	head = (head -> next);
	return head; 
}

/* Create Huffman Tree */
struct node *create_tree(struct node *sorted){
	int i;
	struct node *newtop;
	struct node *topcopy;
	struct node *topcopy2;
	newtop = (struct node *)malloc(sizeof(struct node));
	if (sorted -> next == NULL){
		newtop = sorted;
		return newtop;
	}
	topcopy = (struct node *)malloc(sizeof(struct node));
	topcopy2 = (struct node *)malloc(sizeof(struct node));
	while (0 == 0){
		newtop = create_node(parent_char, 
		((sorted -> frequency) + (sorted -> next -> frequency)));
		(newtop -> left) = sorted;
		(newtop -> right) = (sorted -> next);
		sorted = sorted_insert_node_freq(sorted, newtop);
		if ((sorted -> next -> next -> next) == NULL){
			sorted = remove_top(sorted);
			sorted = remove_top(sorted);
			return sorted;	
		} else {
			sorted = remove_top(sorted);
			sorted = remove_top(sorted);
		}
	}
}


/* Build codes for printing based on tree traversal */
void build_codes(char *codes[], struct node *tree, char *temp){
	char *other;
	char *one = "1";
	char *zero = "0";
	if (tree -> left != NULL){
		if (temp == NULL){
			other = (char *) malloc(strlen(zero) + 1);
		} else {	
		other = (char *) malloc(strlen(temp) + strlen(zero) + 1);
			strcpy(other, temp);
		}
		strcat(other, zero);
		build_codes(codes, tree -> left, other);
	}
	if (tree -> right != NULL){
		if (temp == NULL){
			other = (char *) malloc(strlen(one) + 1);
		} else {	
			other = (char *) malloc(strlen(temp) + strlen(one) + 1);
			strcpy(other, temp);
		}
 		strcat(other, one);
		build_codes(codes, tree -> right, other);
	} 
	if (((tree -> left) == NULL) && ((tree -> right) == NULL)){
		codes[tree -> character] = temp;
		temp = NULL;
	}
}
