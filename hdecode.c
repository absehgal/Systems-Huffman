#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "structure.h"
#define ASCII 256 
#define FIRST_NODE 0 
#define SIZE 1024
#define BYTE 8
#define INNOUT 3
#define FOURBYTE 4

int main(int argc, char *argv[]){
	struct node *top;
	struct node *new;
	struct node *treetop;
	struct node *temp_location;
	uint32_t countbuf[1];
	uint8_t charbuf[1];
	uint8_t codebuf[SIZE];
	char outbuf[1];
	uint8_t byte;                  /* character byte from header */
	uint8_t one_bit;               /* bit masking for tree traversal */
	int character = 0;
	int num_chars = 0;             /* num of total chars from header */
	int character_count = 0;       /* num of each char from header */
	uint32_t freq[ASCII] = {0};    /* character freq for tree */
	uint32_t outfreq[ASCII] = {0};
	int first = 0;
	int sub_count = 0;
	int output;
	int c, i;                      /* iterators in for loops */
	int fin, fout;                 /* file names */
	int total_count = 0;
	int outputted = 0;
	int num;                       /* successfully read bytes */

	/* Check and open necessary input file */
	if (*argv[1] == '-' || argc == 1){
		fin = STDIN_FILENO;
	} else {
		fin = open(argv[1], O_RDONLY);
		if (fin == -1){
			perror(argv[1]);
			exit(EXIT_FAILURE);
		}
	}
	
	/* Check and open necessary output file */
	if (argc == INNOUT){
		fout = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR);
		if (fout == -1){
			perror(argv[2]);
			exit(EXIT_FAILURE);
		}
	} else {
		fout = STDOUT_FILENO;
	}

	/* Get total number of characters from header */
	if ((num = read(fin, countbuf, FOURBYTE)) > 0){
		num_chars = countbuf[0];
	}


	/* Add character frequencies to array from header */
	while (sub_count != num_chars){
		if ((num = read(fin, charbuf, 1)) > 0){
			character = charbuf[0];
		}
		if ((num = read(fin, countbuf, FOURBYTE)) > 0){
			character_count = countbuf[0];
		}
		freq[character] = character_count;
		total_count += character_count; 
		sub_count++;
	}
	if (-1 == num){
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	
	/* Create space for necessary nodes */
	top = (struct node *)malloc(sizeof(struct node));
	if (NULL == top){
		exit(EXIT_FAILURE);
	}

	new = (struct node *)malloc(sizeof(struct node));
	if (NULL == new){
		exit(EXIT_FAILURE);
	}

	treetop = (struct node *)malloc(sizeof(struct node));
	if (NULL == treetop){
		exit(EXIT_FAILURE);
	}

	/* Create nodes for ascii values with present frequencies */
	for (c = 0; c < ASCII; c++){
		if (freq[c] != 0){
			if (FIRST_NODE == first){
				top = create_node(c, freq[c]);
				first++;
			} else {
				new = create_node(c, freq[c]);
				top = sorted_insert_node_freq(top, new); 
			}
		}
	}	
	/* Sort nodes by frequency then ascii */

	/* Create Huffman Tree */
	treetop = create_tree(top);

	/* Read in one byte at a time */
	temp_location = treetop;
	while ((num = read(fin, codebuf, SIZE)) > 0){
		for (c = 0; c < num; c++){
			byte = codebuf[c];
			for (i = 0; i < BYTE; i++){
				/* bit mask for tree traversal */
				one_bit = ((byte & (1 << 7)) >> 7);
				if (1 == one_bit){
					temp_location = temp_location -> right;
				} else {
					temp_location = temp_location -> left;
				}
				byte = byte << 1;
				/* if a child node, output character */
				if ((temp_location -> right) == NULL && 
					(temp_location -> left) == NULL){
					output = (temp_location -> character);
					temp_location = treetop;
					outfreq[output]++;
					/* check for padding */
					if (total_count == outputted){
						break;
					}
					outbuf[0] = (char) output;
					write(fout, outbuf, 1); 
					outputted++;
				}
			}
		}
	}

	
	/* if one character, just output it the proper number of times */
	if (1 == num_chars){
		for (i = 0; i < character_count; i++){
			outbuf[0] = (char) character;
			write(fout, outbuf, 1);
		}
	}


	free(new);
	free(treetop);
	
	close(fin);
	close(fout);

	return 0;
}
