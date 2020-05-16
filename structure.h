#ifndef NODE
#define NODE 

struct node *sorted_insert_node_freq(struct node *head, struct node *insert); 
struct node *sorted_insert_node_ascii(struct node *head, struct node *insert); 
struct node *create_node(int newchar, int newfreq);
void printout(struct node *output);
struct node *remove_top(struct node *head);
struct node *create_tree(struct node *sorted);
void build_codes(char *codes[], struct node *tree, char *temp);

struct node{
	int character;
	int frequency;
	struct node *left;
	struct node *right;
	struct node *next;
};

#endif
