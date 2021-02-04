#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <ctype.h>
#define COUNT 10

char search_words[20][COUNT] = {"The","around","graphics","from","by","be","any",
	"which","various","mount"};
long counts[COUNT];
int line_c = 0;

int is_equal(char *a, const char *key, int ignore_case) {
	char b[20];
	strcpy(b, key);
	int len_a = strlen(a), len_b = strlen(b);

	if(len_a != len_b)
		return 0;
	else if (ignore_case) {
		for(int i = 0; i < len_a; i++) {
			a[i] = tolower(a[i]);
			b[i] = tolower(b[i]);
		}
	}
	return (strcmp(a, b) == 0);
}

void read_word(char *temp, FILE *fp) {
	int i = 0;
	char ch;

	while((ch = fgetc(fp)) != EOF && isalpha(ch) == 0);

	while(ch != EOF && isalpha(ch) != 0) {
		temp[i++] = ch;
		ch = fgetc(fp);
	}
	temp[i] = '\0';
}

long determine_count(const char *file_name, const char *key, int ignore_case) {
	int key_index = 0, key_len = strlen(key);
	long word_count = 0;
	char ch, temp[40];
	FILE *fp = fopen(file_name, "r");
	while(feof(fp) == 0) {
		read_word(temp, fp);
		if(is_equal(temp, key, ignore_case)) word_count++;
	}
	return word_count;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Error: Run as: ./a.out input.txt");
		exit(-1);
	}
	int nt = 1;
	printf("Enter number of threads: ");
	scanf("%d", &nt);

	for(int i = 0; i < COUNT; i++) counts[i] = 0;

	double t = omp_get_wtime();
	for(int i = 0; i < COUNT; i++)
		determine_count(argv[1], search_words[i], 1);

	t = omp_get_wtime() - t;

	for(int i = 0; i < COUNT; i++)
		printf("\n%s: %ld", search_words[i], counts[i]);

	printf("Time: %lf",  t);
}
