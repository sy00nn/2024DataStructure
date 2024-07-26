#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node {
    int element;
    struct Node* next;
} Node;

typedef struct {
    Node* top;
} Stack;

Node* newNode(int element) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;
    node->element = element;
    node->next = NULL;
    return node;
}

void initStack(Stack* stack) {
    stack->top = NULL;
}

void push(Stack* stack, int element, FILE* fout) {
    Node* node = newNode(element);
    if (node == NULL) {
        fprintf(fout, "Overflow\n");
        return;
    }
    node->next = stack->top;
    stack->top = node;
}

int pop(Stack* stack, FILE* fout) {
    if (stack->top == NULL) {
        fprintf(fout, "Underflow\n");
        return -1;
    }
    Node* temp = stack->top;
    int popped = temp->element;
    stack->top = temp->next;
    free(temp);
    return popped;
}

int top(Stack* stack) {
    if (stack->top == NULL) {
        return -1;
    }
    return stack->top->element;
}

void printStackBottomToTop(FILE* fout, Stack* stack) {
    char output[1024] = { 0 };  
    int offset = 0;
    Stack temp;
    initStack(&temp);
    Node* current = stack->top;
    while (current != NULL) {
        push(&temp, current->element, fout);
        current = current->next;
    }
    current = temp.top;
    while (current != NULL) {
        offset += sprintf(output + offset, "%d ", current->element);
        current = current->next;
    }
    if (offset > 0) {
        output[offset - 1] = '\0'; 
    }
    fprintf(fout, "%s\n", output);
    while (temp.top != NULL) {
        pop(&temp, fout);
    }
}


int isAlphanumeric(char c) {
    return isalpha(c) || isdigit(c);
}

int isPalindrome(char* str) {
    int left = 0, right = strlen(str) - 1;
    while (left < right) {
        if (!isAlphanumeric(str[left])) {
            left++;
            continue;
        }
        if (!isAlphanumeric(str[right])) {
            right--;
            continue;
        }
        if (tolower(str[left]) != tolower(str[right])) {
            return 0;
        }
        left++;
        right--;
    }
    return 1;
}


int isBalanced(char* str) {
    Stack s;
    initStack(&s);
    for (int i = 0; str[i]; i++) {
        char c = str[i];
        switch (c) {
        case '(': case '{': case '[':
            push(&s, c, NULL);
            break;
        case ')': case '}': case ']':
            if (s.top == NULL || (c == ')' && top(&s) != '(') ||
                (c == '}' && top(&s) != '{') ||
                (c == ']' && top(&s) != '[')) {
                while (s.top != NULL) pop(&s, NULL);
                return 0;
            }
            pop(&s, NULL);
            break;
        }
    }
    int balanced = s.top == NULL;
    while (s.top != NULL) pop(&s, NULL);
    return balanced;
}


int main() {
    FILE* fin = fopen("input.txt", "r");
    FILE* fout = fopen("output.txt", "w");

    if (fin == NULL || fout == NULL) {
        perror("Error opening files");
        return EXIT_FAILURE;
    }

    Stack stack;
    initStack(&stack);
    char operation;
    char buffer[1024];

    while (fscanf(fin, "%c", &operation) != EOF) {
        if (operation == 'H') {
            int number;
            fscanf(fin, "\t%d\n", &number);
            push(&stack, number, fout);
            printStackBottomToTop(fout, &stack);
        }
        else if (operation == 'O') {
            fscanf(fin, "\n");
            pop(&stack, fout);
            printStackBottomToTop(fout, &stack);
        }
        else if (operation == 'T') {
            fscanf(fin, "\n");
            int t = top(&stack);
            if (t != -1) {
                fprintf(fout, "%d\n", t);
            }
            else {
                fprintf(fout, "Underflow\n");
            }
        }
        else if (operation == 'P') {
            fscanf(fin, "\t%[^\n]\n", buffer);
            fprintf(fout, "%s\n", isPalindrome(buffer) ? "T" : "F");
        }
        else if (operation == 'B') {
            fscanf(fin, "\t%[^\n]\n", buffer);
            fprintf(fout, "%s\n", isBalanced(buffer) ? "T" : "F");
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}
