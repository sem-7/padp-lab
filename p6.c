#include <stdio.h>
#include <omp.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#define COUNT 10

char search_words[20][COUNT] = {"The","around","graphics","from","by","be","any",
	"which","various","mount"};
long counts[COUNT];
int line_c = 0;

int is_equal(char* a,const char* key, int ignore_case) {
	if(strlen(a) !=  strlen(key)) {
		return 0;
	}
	if(ignore_case != 0) return (strcasecmp(a, key) == 0);
	return (strcmp(a, key) ==  0);
}
void read_word(char *temp, FILE *fp) {
	int i = 0;
	char ch;

	while((ch = fgetc(fp)) !=  EOF && isalpha(ch) ==  0);

	while(ch !=  EOF && isalpha(ch) !=  0) {
		temp[i++] = ch;
		ch = fgetc(fp);
	}

	temp[i] = '\0';
}

long determine_count(const char *file_name, const char *key, int ignore_case) {
	int key_index = 0,key_len = strlen(key);
	long word_count = 0;
	char ch;
	FILE *fp = fopen(file_name,"r");
	char temp[40];
	int i = 0;
	while(feof(fp) ==  0) {
		read_word(temp,fp);
		if(is_equal(temp,key,ignore_case))
			word_count++;
	}
	return word_count;
}

int main(int argc, char** argv) {
	int i;

	if (argc != 2) {
		printf("Expected argument for input file.");
		exit(-1);
	}

	int nt = 0;
	printf("Enter number of threads: ");
	scanf("%d", &nt);

	for(i = 0; i < COUNT; i++) counts[i] = 0;

	double t = omp_get_wtime();
	#pragma omp parallel for shared(counts, search_words) private(i) num_threads(nt)
	for(i = 0; i < COUNT; i++)
		counts[i] = determine_count(argv[1], search_words[i], 1);

	t = omp_get_wtime() - t;

	for(i = 0; i < COUNT; i++)
		printf("\n%s: %ld",search_words[i],counts[i]);

	printf("\nTime Taken: %lf\n",t);
}
