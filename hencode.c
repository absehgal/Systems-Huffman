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
	char *temp_code;
	char buf[SIZE];
	char codebuf[SIZE];
	uint8_t bytebuf[1];
	uint32_t countbuf[1];
	char *codes[ASCII] = {NULL};
	char *temp = NULL;
	uint8_t bit_code;
	uint32_t num_chars = 0;       /* num of chars for header */
	uint32_t freq[ASCII] = {0};   /* character freq for header */
	int first = 0;
	int c, i;                     /* iterators in for loops */
	int fin, fout;                /* file names */
	int num;                      /* successfully read bytes */
	int bit_count = 0;            /* keep track of bits until size byte */
	int codeindex = 0;
	

	/* Open input file */
	fin = open(argv[1], O_RDONLY);
	if (fin == -1){
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	
	/* Open output file */
	if (argc == INNOUT){
		fout = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR);
		if (fout == -1){
			perror(argv[2]);
			exit(EXIT_FAILURE);
		}
	} else {
		fout = STDOUT_FILENO;
	}


	/* Add frequency counts to array */
	while ((num = read(fin, buf, SIZE)) > 0){
		for (c = 0; c < num; c++){
			freq[(uint8_t) buf[c]]++;	
		}
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
			num_chars++;
			if (FIRST_NODE == first){
				top = create_node(c, freq[c]);
				first++;
			} else {
				new = create_node(c, freq[c]);
				top = sorted_insert_node_freq(top, new);
				/* Sort nodes by frequency then ascii */
			}
		}
	}	


	/* Create Huffman Tree */
	treetop = create_tree(top);

	/* Write codes for each ascii value by traversing tree */
	build_codes(codes, treetop, temp);

	/* Write out number of characters first for header */
	countbuf[0] = num_chars;
	write(fout, countbuf, FOURBYTE);

	/* Exit if empty file */
	if (0 == num_chars){
		exit(0);
	}

	/* Write out character and frequency for header */
	for (c = 0; c < ASCII; c++){
		if (freq[c] != 0){
			bytebuf[0] = c;
			write(fout, bytebuf, 1);
			countbuf[0] = freq[c];
			write(fout, countbuf, FOURBYTE);
		}
	}
	
	/* Set reading location to start of file for rereading */
	if (lseek(fin, 0, SEEK_SET) < 0){
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Read file and output corresponding binary code after header */
	while ((num = read(fin, codebuf, SIZE)) > 0){
		for (c = 0; c < num; c++){
			codeindex = (uint8_t) codebuf[c];
			if((temp_code = codes[codeindex]) != NULL){
				while (*temp_code != '\0'){
					bit_code = bit_code << 1;
					if (*temp_code == '1'){
						bit_code |= 1;
					}
						bit_count++;
						temp_code++;
					if (bit_count == BYTE){
						bytebuf[0] = bit_code;
						write(fout, bytebuf, 1);
						bit_code = 0;
						bit_count = 0;
					}
				}
			}
		}
	}

	/* Pad end of file with zeros if not a full byte */
	if ((bit_count = bit_count % BYTE) != 0){
		for (i = 0; i < (BYTE - bit_count); i++){
			bit_code = bit_code << 1;
		}
		bytebuf[0] = bit_code;
		write(fout, bytebuf, 1);
	}

	free(new);
	free(treetop);
	
	close(fin);
	close(fout);

	return 0;
}
