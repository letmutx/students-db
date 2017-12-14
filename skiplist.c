#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_LEVEL 20

// TODO: Display List of Students whose Data is missing ,and provide an option to Enter the subject marks again .

struct student {
	int marks_len;			/* number of subjects */
	char *name;			/* name of the student */
	int deleted;			/* if this student is deleted */
	int marks[];			/* flexible array to contain subject marks */
};

struct node {
	struct student *data;	/* data pointer */
	struct node *next[];	/* flexible array to contain variable number of next pointers */
};

struct list {
	struct node *head;		/* head pointer */
	int level;			/* the current maximum height of any node in list */
	int key_index;			/* if key is in an array, its index in array */
	char type;			/* the type of the key */
};

struct list *init(int key_index, char type) {
	struct list *header;
	header = malloc(sizeof(struct list));
	assert (header != NULL);
	header->head = malloc(sizeof(struct node) + MAX_LEVEL * sizeof(struct node *));
	assert (header->head != NULL);
	for (int i = 0; i < MAX_LEVEL; i++)
		header->head->next[i] = header->head;
	header->level = 0;
	header->key_index = key_index;
	header->type = type;
	return header;
}

int compare_students(struct student *s1, struct student *s2, int key_index, char type) {
	if (type == 'a') {
		return s1->marks[key_index] - s2->marks[key_index];
	}
	else if (type == 's') {
		// sort string in alphabetical order
		return -strcmp(s1->name, s2->name);
	}
	return 0;
}

int compare_key(struct student *s, void *key, int key_index) {
	if (key_index == -1)
		// strings are sorted in alphabetic order
		return -strcmp(s->name, (char *)key);
	else
		return s->marks[key_index] - *(int *)key;
}

int insert(struct list *header, struct student *data) {
	int new_level;
	struct node *cur;
	struct node *update[MAX_LEVEL + 1];
	cur = header->head;
	for (int i = header->level; i >= 0; i--) {
		while (cur->next[i] != header->head &&
				// reverse sorted list except for names
				compare_students(cur->next[i]->data, data, header->key_index, header->type) > 0)
			cur = cur->next[i];
		update[i] = cur;
	}
	for (new_level = 0; rand() % 2 && new_level < MAX_LEVEL; new_level++)
		;
	cur = malloc(sizeof(struct node) + (new_level + 1) * sizeof(struct node *));
	assert (cur != NULL);
	cur->data = data;
	if (new_level > header->level) {
		for (int i = new_level; i > header->level; i--)
			update[i] = header->head;
		header->level = new_level;
	}
	for (int i = 0; i <= new_level; i++) {
		cur->next[i] = update[i]->next[i];
		update[i]->next[i] = cur;
	}
	return 1;
}

struct student *find(struct list *header, void *key) {
	struct node *cur;
	cur = header->head;
	for (int i = header->level; i >= 0; i--) {
		while (cur->next[i] != header->head &&
				compare_key(cur->next[i]->data, key, header->key_index) > 0)
			cur = cur->next[i];
	}
	cur = cur->next[0];
	if (cur == header->head)
		return NULL;
	else if (compare_key(cur->data, key, header->key_index) != 0)
		return NULL;
	else if (cur->data->deleted) {
		while (cur->next[0] != header->head &&
			compare_key(cur->next[0]->data, key, header->key_index) == 0 &&
			cur->next[0]->data->deleted) {

			cur = cur->next[0];
		}
		if (cur->next[0] == header->head)
			return NULL;
		else if (compare_key(cur->next[0]->data, key, header->key_index) != 0)
			return NULL;
		else return cur->next[0]->data;
	}
	return cur->data;
}

void print_student(struct student *s);

int find_r(struct list *header, void *low, void *high) {
	struct node *cur, *start, *end;
	void *temp;
	cur = header->head;
	if (header->type == 's') {
		if (-strcmp((char *)low, (char *)high) > 0) {
			temp = high;
			high = low;
			low = temp;
		}
	}
	else if (header->type == 'a') {
		if (*(int *)low > *(int *)high) {
			temp = high;
			high = low;
			low = temp;
		}
	}
	for (int i = header->level; i >= 0; i--) {
		while (cur->next[i] != header->head &&
				compare_key(cur->next[i]->data, high, header->key_index) > 0)
			cur = cur->next[i];
	}
	start = cur->next[0];
	while (start->data->deleted)
		start = start->next[0];
	cur = header->head;
	for (int i = header->level; i >= 0; i--) {
		while (cur->next[i] != header->head &&
				compare_key(cur->next[i]->data, low, header->key_index) >= 0)
			cur = cur->next[i];
	}
	end = cur->next[0];
	while (end->data->deleted)
		end = end->next[0];
	if (start == header->head && end == header->head)
		return 0;
	while (start != end) {
		if (!start->data->deleted) {
			print_student(start->data);
			putchar('\n');
		}
		start = start->next[0];
	}
	return 1;
}

int delete(struct list *header, void *key) {
	struct node *cur = header->head;
	for (int i = header->level; i >= 0; i--) {
		while (cur->next[i] != header->head &&
				// find the least greatest value in list than key
				compare_key(cur->next[i]->data, key, header->key_index) > 0)
			cur = cur->next[i];
	}
	cur = cur->next[0];
	// key not found
	if (cur == header->head ||
			compare_key(cur->data, key, header->key_index) != 0)
		return 0;
	else if (cur->data->deleted) {
		while (cur->next[0] != header->head && compare_key(cur->next[0]->data, key,header->key_index) == 0
				&& cur->next[0]->data->deleted)
			cur = cur->next[0];
		cur = cur->next[0];
		if (cur == header->head || compare_key(cur->data, key, header->key_index) > 0)
			return 0;
		cur->data->deleted = 1;
	}
	else {
		cur->data->deleted = 1;
	}
	return 1;

}

void print_student(struct student *s) {
	printf("%25s", s->name);
	for (int i = 0; i < s->marks_len; i++)
		printf("%25d ", s->marks[i]);
	return;
}

void print_particular_subject(struct list *header, int index) {
	struct node *cur = header->head;
	while (cur->next[0] != header->head) {
		if (!cur->next[0]->data->deleted) {
			printf("%30s %30d", cur->next[0]->data->name, cur->next[0]->data->marks[index]);
			putchar('\n');
		}
		cur = cur->next[0];
	}
}

struct student *create_student(char *name, int *marks, int marks_count) {
	int i;
	struct student *s = malloc(sizeof(*s) + marks_count * sizeof(int));
	assert (s != NULL);
	s->marks_len = marks_count;
	s->name = strdup(name);
	s->deleted = 0;
	for (i = 0; i < marks_count; i++)
		s->marks[i] = marks[i];
	return s;
}

/* input helpers */
/* get student data from input file */
struct student **get_student_data(FILE *fp, int sub_count, int *stu_count) {
	int i = 0, size = 100, j, c;
	char name[1000];
	struct student **s = malloc(sizeof(*s) * size);
	int marks[sub_count];
	while (!feof(fp)) {
		j = 0;
		while ((c = fgetc(fp)) != EOF && c != '\n')
			name[j++] = c;
		name[j] = 0;
		for (j = 0; j < sub_count-1; j++)
			fscanf(fp, "%d,", marks + j);
		fscanf(fp, "%d\n", marks + j);
		if (i == size) {
			size += 100;
			s = realloc(s, sizeof(struct student *) * size);
		}
		s[i++] = create_student(name, marks, sub_count);
	}
	s = realloc(s, sizeof(struct student *) * i);

	*stu_count = i;
	return s;
}

/* get n subject names from file fp */
char **get_subject_names(FILE *ip, int sub_count) {
	char **s;
	int i, j, c;
	fscanf(ip, "%d\n", &sub_count);
	s = malloc(sizeof(char *) * sub_count);
	for (i = 0; i < sub_count; i++) {
		s[i] = malloc(sizeof(char) * 100);
		j = 0;
		while ((c = fgetc(ip)) != EOF && c != '\n' && c != ',')
			s[i][j++] = c;
		if (c == EOF)
			break;
		s[i][j] = 0;
		s[i] = realloc(s[i], sizeof(char) * (j + 1));
	}
	return s;
}

void get_data(struct student ***students, int *stu_count, char ***sub_names, int *sub_count, char *filename) {
	FILE *fp = fopen(filename, "r");
	assert (fp != NULL);
	fscanf(fp, "%d\n", sub_count);
	*sub_names = get_subject_names(fp, *sub_count);
	*students = get_student_data(fp, *sub_count, stu_count);
	fclose(fp);
}

void display(struct list *header, char **sub_names, int sub_count) {
	struct node *cur = header->head->next[0];
	printf("%25s", "Names");
	for (int i = 0; i < sub_count; i++)
		printf("%25s", sub_names[i]);
	putchar('\n');
	while (cur != header->head) {
		if (!cur->data->deleted) {
			print_student(cur->data);
			putchar('\n');
		}
		cur = cur->next[0];
	}
	return;
}

void insert_data_to_list(struct list *lists[], char **sub_names, int sub_count) {
	int choice;
	char name[1000];
	int marks[sub_count];
	struct student *s;
	while (1) {
		printf("1) Insert 2) Exit\n");
		do {
			printf("Enter your choice: ");
			scanf("%d", &choice);
		} while (choice < 1 && choice > 2);
		if (choice == 2)
			return;
		int j = 0, c;
		// skip new line character above
		while ((c = getchar()) != '\n');
		printf("Enter name: ");
		while ((c = getchar()) != EOF && c != '\n')
			name[j++] = c;
		name[j] = 0;
		for (int i = 0; i < sub_count; i++) {
			printf("Enter marks in %s: ", sub_names[i]);
			scanf("%d", marks + i);
		}
		if ((s = find(lists[sub_count], name)) != NULL) {
			printf("Student with name %s already found. Do you want to replace? [y/N]: ", name);
			char c;
			scanf("%c", &c);
			if (c == 'y') {
				s->deleted = 1;
				s = create_student(name, marks, sub_count);
				for (int i = 0; i < sub_count; i++)
					insert (lists[i], s);
				insert(lists[sub_count], s);
			}
			else
				continue;
		}
		else {
			s = create_student(name, marks, sub_count);
			for (int i = 0; i < sub_count; i++)
				insert (lists[i], s);
			insert(lists[sub_count], s);
		}
	}
	return;
}

void search_data(struct list *lists[], char **sub_names, int sub_count) {
	char name[1000];
	int marks, choice;
	struct student *ans;
	while (1) {
		printf("Enter the number you want to search with\n");
		for (int i = 0; i < sub_count; i++) {
			printf("%d) %s\n", i + 1, sub_names[i]);
		}
		printf("%d) Name\n", sub_count + 1);
		printf("%d) Exit\n",sub_count + 2);
		do {
			printf("Enter your choice: ");
			scanf("%d", &choice);
		} while (choice < 1 || choice > sub_count + 2);
		if (choice == sub_count + 1) {
			int j = 0, c;
			printf("1) Print average marks for this student 2) Print all marks 3) Maximum mark in all subject\n");
			do {
				printf("Enter your choice: ");
				scanf("%d", &choice);
			}while (choice < 1 && choice > 2);
			printf("Enter name: ");
			// skip new line char at the beginning
			while ((c = getchar()) != '\n');
			while ((c = getchar()) != EOF && c != '\n')
				name[j++] = c;
			name[j] = 0;
			ans = find(lists[sub_count], name);
			if (!ans)
				printf("%s not found!", name);
			else {
				if (choice == 1) {
					float sum = 0;
					for (int i = 0; i < ans->marks_len; i++)
						sum += ans->marks[i];
					printf("%30s %.3f", ans->name, sum / ans->marks_len);
				}
				else if (choice == 2) {
					int max_index = 0;
					for (int i = 1; i < ans->marks_len; i++)
						max_index = ans->marks[i] > ans->marks[max_index] ? i : max_index;
					printf("%s got maximum marks in %s: %d", name, sub_names[max_index], max_index);
				}
				else
					print_student(ans);
			}
			putchar('\n');
		}
		else if (choice == sub_count + 2)
			return;
		else {
			int c;
			printf("1) Query in a range 2) Query a particular mark 3) Print all marks for this subject\n");
			do {
				printf("Enter your choice: ");
				scanf("%d", &c);
			} while (c < 1 || c > 3);
			if (c == 1) {
				int low, high;
				printf("Enter low index: ");
				scanf("%d", &low);
				printf("Enter high index: ");
				scanf("%d", &high);
				if (!find_r(lists[choice - 1], &low, &high))
					printf("No students found within that range\n");
			}
			else if (c == 2) {
				printf("Enter marks: ");
				scanf("%d", &marks);
				// to get all students with same marks, using find_r with same marks
				if (!find_r(lists[choice - 1], &marks, &marks))
					printf("No student with marks %d in subject %s\n", marks, sub_names[choice - 1]);
				putchar('\n');
			}
			else {
				print_particular_subject(lists[sub_count], choice - 1);
			}
		}
	}
}

void delete_data(struct list *names_list) {
	char name[1000];
	int j = 0, c;
	// skip spaces
	while ((c = getchar()) != '\n');
	printf("Enter the name of student you want to delete: ");
	while ((c = getchar()) != EOF && c != '\n')
		name[j++] = c;
	name[j] = 0;
	if (delete(names_list, name))
		printf("%s deleted from list\n", name);
	else
		printf("%s not found!\n", name);
	return;
}

void modify_data(struct list *lists[], char **sub_names, int sub_count) {
	int choice, j, c;
	char name[1000];
	struct student *temp;
	for (;;) {
		j = 0;
		printf("1)Change name 2) Change particular subject marks 3)Exit\n");
		do {
			printf("Enter your choice: ");
			scanf("%d", &choice);
		} while (choice < 1 || choice > 3);
		if (choice == 1) {
			while ((c = getchar()) != '\n');
			printf("Enter old name: ");
			while ((c = getchar()) != EOF && c != '\n')
				name[j++] = c;
			name[j] = 0;
			if ((temp = find(lists[sub_count], name)) == NULL)
				printf("Student with name %s not found.\n", name);
			else {
				j = 0;
				delete(lists[sub_count], temp->name);
				while ((c = getchar()) != '\n');
				printf("Enter new name: ");
				while ((c = getchar()) != EOF && c != '\n')
					name[j++] = c;
				name[j] = 0;
				temp = create_student(name, temp->marks, temp->marks_len);
				for (int i = 0; i < sub_count; i++)
					insert(lists[i], temp);
				insert(lists[sub_count], temp);
				printf("Done!\n");
			}
		}
		else if (choice == 2) {
			while ((c = getchar()) != '\n');
			printf("Enter name of the student: ");
			while ((c = getchar()) != EOF && c != '\n')
				name[j++] = c;
			name[j] = 0;
			printf("1) Change all marks 2) Change particular subject marks\n");
			do {
				printf("Enter your choice: ");
				scanf("%d", &choice);
			} while (choice < 1 || choice > 2);
			if ((temp = find(lists[sub_count], name)) == NULL) {
				printf("Student with name %s not found.\n", name);
				continue;
			}
			temp->deleted = 1;
			struct student *t;
			int sub_marks[sub_count];
			if (choice == 1) {
				for (int i = 0; i < sub_count; i++) {
					printf("Enter marks for %s: ", sub_names[i]);
					scanf("%d", sub_marks + i);
				}
				t = create_student(temp->name, sub_marks, sub_count);
			}
			else if (choice == 2) {
				for (int i = 0; i < sub_count; i++) {
					printf("%d) %s\n", i + 1, sub_names[i]);
				}
				do {
					printf("Enter your choice: ");
					scanf("%d", &choice);
				} while (choice < 1 || choice > sub_count + 1);
				for (int i = 0; i < sub_count; i++)
					sub_marks[i] = temp->marks[i];
				printf("Enter marks: ");
				scanf("%d", sub_marks + choice - 1);
				t = create_student(temp->name, sub_marks, sub_count);
			}
			for (int i = 0; i < sub_count; i++)
				insert(lists[i], t);
			insert(lists[sub_count], t);
		}
		else
			return;
	}
}

void delete_list(struct list *header) {
	struct node *cur = header->head->next[0], *temp;
	while (cur != header->head) {
		temp = cur->next[0];
		free(cur);
		cur = temp;
	}
	free(header->head);
	free(header);
}

void delete_list_with_data(struct list *header) {
	struct node *cur = header->head->next[0], *temp;
	while (cur != header->head) {
		temp = cur->next[0];
		free(cur->data->name);
		free(cur->data);
		free(cur);
		cur = temp;
	}
	free(header->head);
	free(header);
}

void delete_lists(struct list *lists[], int size) {
	for (int i = 0; i < size - 1; i++)
		delete_list(lists[i]);
	delete_list_with_data(lists[size - 1]);
}

void save_data_to_file(char *filename, int sub_count, char **sub_names, struct list *header) {
	struct node *cur;
	FILE *fp = fopen(filename, "w");
	assert (fp != NULL);
	fprintf(fp, "%d\n", sub_count);
	for (int i = 0; i < sub_count - 1; i++)
		fprintf(fp, "%s,", sub_names[i]);
	fprintf(fp, "%s\n", sub_names[sub_count - 1]);
	cur = header->head->next[0];
	while (cur != header->head) {
		struct student *st = cur->data;
		if (!st->deleted) {
			fprintf(fp, "%s\n", st->name);
			for (int i = 0; i < sub_count - 1; i++) {
				fprintf(fp, "%d,", st->marks[i]);
			}
			fprintf(fp, "%d\n", st->marks[sub_count - 1]);
		}
		cur = cur->next[0];
	}
	fclose(fp);
	return;
}

int main(int argc, char *argv[]) {
	struct student **students;
	struct student *temp;
	int stu_count, sub_count;
	char *input = "input.txt", *out = "output.txt", **sub_names;
	int choice;

	get_data(&students, &stu_count, &sub_names, &sub_count, input);

	struct list *lists[sub_count + 1 + 1];
	for (int i = 0; i < sub_count; i++) {
		lists[i] = init(i, 'a');
	}
	lists[sub_count] = init(-1, 's'); // for string searches

	// insert students into all lists based on different keys
	for (int i = 0; i < stu_count; i++) {
		// name of student must be unique as no id is specified
		if ((temp = find(lists[sub_count], students[i]->name))) {
			char c;
			printf("Student with name %s already present. Do you want to override?[y/N]:", students[i]->name);
			fflush(stdout);
			scanf("%c", &c);
			if (c == 'y')
				temp->deleted = 1;
			else
				continue;
		}
		for (int j = 0; j < sub_count; j++)
			insert(lists[j], students[i]);
		insert (lists[sub_count], students[i]);
	}
	free(students);

	while (1) {
		printf("1)Display Entries, 2)Insert, 3)Search, 4)Delete, 5)Modify, 6)Exit\n");
		fflush(stdout);
		do {
			printf("Enter your choice: ");
			scanf("%d", &choice);
		} while (choice < 1 || choice > 6);
		switch(choice) {
		case 1:
			display(lists[sub_count], sub_names, sub_count);
			break;
		case 2:
			insert_data_to_list(lists, sub_names, sub_count);
			break;
		case 3:
			search_data(lists, sub_names, sub_count);
			break;
		case 4:
			delete_data(lists[sub_count]);
			break;
		case 5:
			modify_data(lists, sub_names,  sub_count);
			break;
		case 6:
			save_data_to_file(out, sub_count, sub_names, lists[sub_count]);
			delete_lists(lists, sub_count + 1);
			for (int i = 0; i < sub_count; i++)
				free(sub_names[i]);
			free(sub_names);
			return 0;
		}
	}
}
