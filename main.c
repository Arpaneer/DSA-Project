#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100
#define HASH 127
#define QSIZE 100
#define TITLE 100
#define CAT 30
#define PREF 10
typedef struct {
    int id, likes, views, valid;
    char title[TITLE], category[CAT];
    float age, score, pop, rec, pref;
} News;
typedef struct Node {
    int id;
    char title[TITLE];
    struct Node *next;
} Node;
News news[MAX];
int n = 0, table[HASH];
int heap[MAX], hs = 0;
int queue[QSIZE], front = 0, rear = 0, qc = 0;
char preferences[PREF][CAT];
int pc = 0;
Node *history = NULL;
void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
int same(char a[], char b[]) {
    int i = 0;
    while (a[i] && b[i]) {
        char x = a[i], y = b[i];
        if (x >= 'A' && x <= 'Z') x += 32;
        if (y >= 'A' && y <= 'Z') y += 32;
        if (x != y) return 0;
        i++;
    }
    return a[i] == b[i];
}
int findID(int id) {
    for (int i = 0; i < n; i++)
        if (news[i].valid && news[i].id == id) return i;
    return -1;
}
int preferred(char cat[]) {
    for (int i = 0; i < pc; i++)
        if (same(preferences[i], cat)) return 1;
    return 0;
}
void initHash(void) {
    for (int i = 0; i < HASH; i++) table[i] = -1;
}
void hashInsert(int index) {
    int p = news[index].id % HASH;
    while (table[p] != -1) p = (p + 1) % HASH;
    table[p] = index;
}
int hashSearch(int id) {
    int p = id % HASH;
    for (int i = 0; i < HASH; i++) {
        if (table[p] == -1) return -1;
        if (news[table[p]].valid && news[table[p]].id == id)
            return table[p];
        p = (p + 1) % HASH;
    }
    return -1;
}
void rebuildHash(void) {
    initHash();
    for (int i = 0; i < n; i++)
        if (news[i].valid) hashInsert(i);
}
void calculateScores(void) {
    int maxLikes = 1;
    float minAge = 999999;
    for (int i = 0; i < n; i++) {
        if (!news[i].valid) continue;
        if (news[i].likes > maxLikes) maxLikes = news[i].likes;
        if (news[i].age < minAge) minAge = news[i].age;
    }
    if (minAge == 999999) return;
    for (int i = 0; i < n; i++) {
        if (!news[i].valid) continue;
        news[i].pop = 40.0f * news[i].likes / maxLikes;
        news[i].rec = 40.0f * minAge / news[i].age;
        news[i].pref = preferred(news[i].category) ? 20.0f : 0.0f;
        news[i].score = news[i].pop + news[i].rec + news[i].pref;
    }
}
void swap(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}
void heapUp(int p) {
    while (p > 0) {
        int parent = (p - 1) / 2;
        if (news[heap[p]].score <= news[heap[parent]].score) break;
        swap(&heap[p], &heap[parent]);
        p = parent;
    }
}
void heapDown(int p) {
    int big = p, l = 2 * p + 1, r = 2 * p + 2;
    if (l < hs && news[heap[l]].score > news[heap[big]].score) big = l;
    if (r < hs && news[heap[r]].score > news[heap[big]].score) big = r;
    if (big != p) {
        swap(&heap[p], &heap[big]);
        heapDown(big);
    }
}
void buildHeap(void) {
    hs = 0;
    for (int i = 0; i < n; i++) {
        if (news[i].valid) {
            heap[hs] = i;
            heapUp(hs++);
        }
    }
}
int popHeap(void) {
    if (hs == 0) return -1;
    int top = heap[0];
    heap[0] = heap[--hs];
    if (hs) heapDown(0);
    return top;
}
void enqueue(int id) {
    if (qc == QSIZE) {
        printf("Queue is full.\n");
        return;
    }
    queue[rear] = id;
    rear = (rear + 1) % QSIZE;
    qc++;
}
int dequeue(void) {
    if (!qc) return -1;
    int id = queue[front];
    front = (front + 1) % QSIZE;
    qc--;
    return id;
}
void addHistory(int id) {
    int i = hashSearch(id);
    if (i == -1) {
        printf("Article not found.\n");
        return;
    }
    Node *p = malloc(sizeof(Node));
    if (!p) return;
    p->id = id;
    strcpy(p->title, news[i].title);
    p->next = history;
    history = p;
}
void showHistory(void) {
    Node *p = history;
    if (!p) printf("History is empty.\n");
    while (p) {
        printf("%d - %s\n", p->id, p->title);
        p = p->next;
    }
    int id;
    printf("Enter article ID to add to history (0 to skip): ");
    scanf("%d", &id);
    if (id) addHistory(id);
}
void loadSamples(void) {
    News sample[] = {
        {101,950,12000,1,"AI Breakthrough Announced","Technology",2,0,0,0,0},
        {102,870,9500,1,"India Wins Cricket Match","Sports",5,0,0,0,0},
        {103,620,7800,1,"New Smartphone Launched","Technology",8,0,0,0,0},
        {104,410,5000,1,"New Movie Released","Entertainment",12,0,0,0,0},
        {105,530,6200,1,"Stock Market Hits Record High","Finance",6,0,0,0,0},
        {106,340,4100,1,"Health Tips for Monsoon","Health",14,0,0,0,0},
        {107,780,8900,1,"SpaceX Launches Satellite","Science",3,0,0,0,0},
        {108,990,15000,1,"Election Results Declared","Politics",1,0,0,0,0},
        {109,660,8100,1,"Best Budget Laptops","Technology",7,0,0,0,0},
        {110,720,9100,1,"Cricket World Cup Schedule","Sports",4,0,0,0,0}
    };
    n = sizeof(sample) / sizeof(sample[0]);
    for (int i = 0; i < n; i++) news[i] = sample[i];
    for (int i = 0; i < n; i++) hashInsert(i);
}
void addNews(void) {
    if (n == MAX) { printf("Storage full.\n"); return; }
    News x = {0};
    printf("ID: "); scanf("%d", &x.id); clearInput();
    if (hashSearch(x.id) != -1) { printf("ID already exists.\n"); return; }
    printf("Title: "); fgets(x.title, TITLE, stdin);
    x.title[strcspn(x.title, "\n")] = 0;
    printf("Category: "); fgets(x.category, CAT, stdin);
    x.category[strcspn(x.category, "\n")] = 0;
    printf("Likes: "); scanf("%d", &x.likes);
    printf("Views: "); scanf("%d", &x.views);
    printf("Age in hours: "); scanf("%f", &x.age); clearInput();
    if (x.likes < 0 || x.views < 0 || x.age <= 0 ||
        !x.title[0] || !x.category[0]) {
        printf("Invalid details.\n"); return;
    }
    x.valid = 1;
    news[n] = x;
    hashInsert(n);
    enqueue(x.id);
    n++;
    printf("Article added.\n");
}
void deleteNews(void) {
    int id;
    printf("Enter ID to delete: "); scanf("%d", &id); clearInput();
    int i = hashSearch(id);
    if (i == -1) { printf("Article not found.\n"); return; }
    news[i].valid = 0;
    rebuildHash();
    printf("Article deleted.\n");
}
void searchNews(void) {
    int ch;
    printf("1. Search by ID\n2. Search by category\nChoice: ");
    scanf("%d", &ch); clearInput();
    if (ch == 1) {
        int id;
        printf("Enter ID: "); scanf("%d", &id); clearInput();
        int i = hashSearch(id);
        if (i == -1) printf("Not found.\n");
        else {
            printf("%d | %s | %s | Likes:%d | Score:%.2f\n",
                news[i].id, news[i].title, news[i].category,
                news[i].likes, news[i].score);
            addHistory(id);
        }
    } else if (ch == 2) {
        char cat[CAT];
        printf("Category: "); fgets(cat, CAT, stdin);
        cat[strcspn(cat, "\n")] = 0;
        for (int i = 0; i < n; i++)
            if (news[i].valid && same(news[i].category, cat))
                printf("%d | %s | Score: %.2f\n",
                    news[i].id, news[i].title, news[i].score);
    }
}
void displayNews(void) {
    calculateScores();
    for (int i = 0; i < n; i++)
        if (news[i].valid)
            printf("%d | %-32s | %-14s | Likes:%d | Score:%.2f\n",
                news[i].id, news[i].title, news[i].category,
                news[i].likes, news[i].score);
}
void setPreferences(void) {
    printf("Number of preferred categories (1-%d): ", PREF);
    scanf("%d", &pc); clearInput();
    if (pc < 1 || pc > PREF) { pc = 0; printf("Invalid number.\n"); return; }
    for (int i = 0; i < pc; i++) {
        printf("Category %d: ", i + 1);
        fgets(preferences[i], CAT, stdin);
        preferences[i][strcspn(preferences[i], "\n")] = 0;
    }
    calculateScores();
    printf("Preferences saved.\n");
}
void topK(void) {
    int k, count = 0;
    for (int i = 0; i < n; i++) if (news[i].valid) count++;
    if (!count) { printf("No news available.\n"); return; }
    printf("Enter K: "); scanf("%d", &k); clearInput();
    if (k < 1) return;
    if (k > count) k = count;
    calculateScores();
    buildHeap();
    printf("\nTOP %d NEWS FEED\n", k);
    for (int i = 0; i < k; i++) {
        int x = popHeap();
        printf("%d. %s (%s) Score: %.2f%s\n", i + 1,
            news[x].title, news[x].category, news[x].score,
            preferred(news[x].category) ? " *" : "");
    }
}
void trending(void) {
    int ids[MAX], count = 0;
    for (int i = 0; i < n; i++)
        if (news[i].valid) ids[count++] = i;
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++)
            if (news[ids[j]].likes / news[ids[j]].age <
                news[ids[j+1]].likes / news[ids[j+1]].age)
                swap(&ids[j], &ids[j+1]);
    printf("TRENDING NEWS (Likes / Age)\n");
    for (int i = 0; i < count && i < 10; i++) {
        int x = ids[i];
        printf("%s | Trend: %.2f\n", news[x].title,
            news[x].likes / news[x].age);
    }
}
void sortByScore(void) {
    int ids[MAX], count = 0;
    calculateScores();
    for (int i = 0; i < n; i++) if (news[i].valid) ids[count++] = i;
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++)
            if (news[ids[j]].score < news[ids[j+1]].score)
                swap(&ids[j], &ids[j+1]);
    for (int i = 0; i < count; i++)
        printf("%d. %s - %.2f\n", i + 1, news[ids[i]].title,
            news[ids[i]].score);
}
void processQueue(void) {
    calculateScores();
    if (!qc) printf("Queue is empty.\n");
    while (qc) {
        int id = dequeue(), i = hashSearch(id);
        if (i != -1)
            printf("Processed: %d - %s\n", id, news[i].title);
        else printf("Skipped deleted article %d\n", id);
    }
}
int main(void) {
    int choice;
    initHash();
    loadSamples();
    do {
        printf("\n===== SMART NEWS FEED =====\n");
        printf("1.Add  2.Delete  3.Search  4.Display\n");
        printf("5.Preferences  6.Top-K Feed  7.Trending\n");
        printf("8.Reading History  9.Process Queue\n");
        printf("10.Sort by Score  0.Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            clearInput(); choice = -1; continue;
        }
        clearInput();
        switch (choice) {
            case 1: addNews(); break;
            case 2: deleteNews(); break;
            case 3: searchNews(); break;
            case 4: displayNews(); break;
            case 5: setPreferences(); break;
            case 6: topK(); break;
            case 7: trending(); break;
            case 8: showHistory(); break;
            case 9: processQueue(); break;
            case 10: sortByScore(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
    while (history) {
        Node *temp = history;
        history = history->next;
        free(temp);
    }
    return 0;
}
