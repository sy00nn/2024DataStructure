#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define INF INT_MAX

// MinHeapNode 구조체 정의
typedef struct {
    int vertex; 
    int dist;  
} MinHeapNode;

// MinHeap 구조체 정의
typedef struct {
    int size;      // 현재 힙의 크기
    int capacity;  // 힙의 최대 용량
    int *pos;      // vertex의 위치를 추적하는 배열
    MinHeapNode **array; // 힙 노드 배열
} MinHeap;

// 새로운 MinHeapNode 생성
MinHeapNode* newMinHeapNode(int v, int dist) {
    MinHeapNode* minHeapNode = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    minHeapNode->vertex = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// 새로운 MinHeap 생성
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->pos = (int*) malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// 두 MinHeapNode를 교체
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// 힙을 재구성하는 함수
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx) {
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->vertex] = idx;
        minHeap->pos[idxNode->vertex] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// 힙이 비어 있는지 확인
int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

// 최소값을 추출하는 함수
MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root->vertex] = minHeap->size - 1;
    minHeap->pos[lastNode->vertex] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// 힙의 키 값을 감소시키는 함수
void decreaseKey(MinHeap* minHeap, int v, int dist) {
    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;

    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {
        minHeap->pos[minHeap->array[i]->vertex] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->vertex] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// vertex가 힙에 있는지 확인하는 함수
bool isInMinHeap(MinHeap *minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

// 결과 배열을 출력하는 함수
void printArr(int dist[], int n, int parent[]) {
    for (int i = 0; i < n; ++i) {
        if (dist[i] == INF) {
            printf("%d\tINF\tNIL\n", i);
        } else {
            printf("%d\t%d\t%d\n", i, dist[i], parent[i]);
        }
    }
}

// 다익스트라 알고리즘 함수
void dijkstra(int **graph, int src, int V) {
    int *dist = (int *)malloc(V * sizeof(int)); // 거리 배열
    int *parent = (int *)malloc(V * sizeof(int)); // 부모 배열
    MinHeap* minHeap = createMinHeap(V); // 최소 힙 생성

    // 초기화
    for (int v = 0; v < V; ++v) {
        dist[v] = INF;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
        parent[v] = -1;
    }

    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    minHeap->size = V;

    // 다익스트라 알고리즘 수행
    while (!isEmpty(minHeap)) {
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->vertex;

        for (int v = 0; v < V; ++v) {
            if (graph[u][v] && isInMinHeap(minHeap, v) && dist[u] != INF && 
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                decreaseKey(minHeap, v, dist[v]);
                parent[v] = u;
            }
        }
    }

    printArr(dist, V, parent);

    // 메모리 해제
    free(dist);
    free(parent);
    free(minHeap->pos);
    free(minHeap->array);
    free(minHeap);
}

// 메인 함수
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    FILE *output = freopen(argv[2], "w", stdout);

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Error opening file\n");
        return EXIT_FAILURE;
    }

    int V, E, src;
    fscanf(input, "%d %d %d", &V, &E, &src);

    // 그래프 초기화
    int **graph = (int **)malloc(V * sizeof(int *));
    for (int i = 0; i < V; i++) {
        graph[i] = (int *)malloc(V * sizeof(int));
        for (int j = 0; j < V; j++)
            graph[i][j] = 0;
    }

    // 그래프 edge 정보 입력
    for (int i = 0; i < E; i++) {
        int u, v, w;
        fscanf(input, "%d %d %d", &u, &v, &w);
        graph[u][v] = w;
    }

    // 다익스트라 알고리즘 호출
    dijkstra(graph, src, V);

    // 메모리 해제
    for (int i = 0; i < V; i++)
        free(graph[i]);
    free(graph);
    fclose(input);
    fclose(output);

    return 0;
}
