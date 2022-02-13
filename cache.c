#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include<math.h>
struct cache_addr;

int S, E, B, m, h, p, n = 1;
int offset_bits, set_bits, line_bits, tag_bits;
char policy[4];
char** addr;
//char addr[][3] = {"20", "22", "24","26","28","2a","30","32","34","36","38","3a","3c","3e","40","20","22","24","12","10","0e","0c"};
//char addr[][3] = {"20", "22","40","80","24","82"};
struct cache_addr* addrs;


struct cache_addr {
    char* tag, * set, * offset;
};

void calcBits() {
    offset_bits = (int)(ceil(log(B) / log(2)));
    set_bits = (int)(ceil(log(S / E) / log(2)));
    line_bits = (int)(ceil(log(E) / log(2)));
    tag_bits = m - set_bits - offset_bits;
}

char* toBinary(char* h) {
    char* res = (char*)malloc(9 * sizeof(char));
    int i = 0;
    while (h[i]) {
        switch (h[i]) {
        case '0':
            strcat(res, "0000");
            break;
        case '1':
            strcat(res, "0001");
            break;
        case '2':
            strcat(res, "0010");
            break;
        case '3':
            strcat(res, "0011");
            break;
        case '4':
            strcat(res, "0100");
            break;
        case '5':
            strcat(res, "0101");
            break;
        case '6':
            strcat(res, "0110");
            break;
        case '7':
            strcat(res, "0111");
            break;
        case '8':
            strcat(res, "1000");
            break;
        case '9':
            strcat(res, "1001");
            break;
        case 'A':
        case 'a':
            strcat(res, "1010");
            break;
        case 'B':
        case 'b':
            strcat(res, "1011");
            break;
        case 'C':
        case 'c':
            strcat(res, "1100");
            break;
        case 'D':
        case 'd':
            strcat(res, "1101");
            break;
        case 'E':
        case 'e':
            strcat(res, "1110");
            break;
        case 'F':
        case 'f':
            strcat(res, "1111");
            break;
        }
        i++;
    }
    res[8] = '\0';
    return res;
}

int toDecimal(char* b) {
    int p = strlen(b) - 1;
    int bin = 0;
    for (int i = 0; i < strlen(b); i++) {
        if (b[i] == '1')
            bin += pow(2, p);
        p--;
    }
    return bin;
}

char* substring(char* s, int start, int end) {
    char* sub = (char*)malloc(10 * sizeof(char));
    int j = 0;
    for (int i = start; i < end; i++) {
        sub[j++] = s[i];
    }
    sub[j] = '\0';
    return sub;
}

struct cache_addr getSingleAddr(char* s) {
    struct cache_addr ca;
    // for tag
    ca.tag = (char*)malloc((tag_bits + 1) * sizeof(char));
    strcpy(ca.tag, substring(s, 0, tag_bits));
    // for set
    ca.set = (char*)malloc((set_bits + 1) * sizeof(char));
    strcpy(ca.set, substring(s, tag_bits, tag_bits + set_bits));
    // for offset
    ca.offset = (char*)malloc((offset_bits + 1) * sizeof(char));
    strcpy(ca.offset, substring(s, tag_bits + set_bits, tag_bits + set_bits + offset_bits));
    return ca;
}

void breakAddresses() {
    char s[9];
    addrs = (struct cache_addr*)malloc(n * sizeof(struct cache_addr));
    for (int i = 0; i < n; i++) {
        strcpy(s, toBinary(addr[i]));
        addrs[i] = getSingleAddr(s);
    }
}

void lfu() {
    char out;
    float misses = 0, hits = 0;
    int idx, found, cycles = 0, miss_rate, r_idx;
    int rows = S / E, cols = E;
    int** table = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < S; i++)
        table[i] = (int*)malloc(cols * sizeof(int));
    for (int i = 0; i < rows;i++)
        for (int j = 0; j < cols; j++)
            table[i][j] = -1;
    breakAddresses();
    for (int i = 0; i < n; i++) {
        idx = toDecimal(addrs[i].set);
        found = 0;
        // if exists in cache
        for (int j = 0; j < cols; j++) {
            if (table[idx][j] == toDecimal(addrs[i].tag)) {
                found = 1;
                out = 'H';
                hits++;
                cycles += h;
            }
        }
        if (!found) {
            r_idx = 0;
            out = 'M';
            for (int j = 0; j < cols; j++) {
                if (table[idx][j] == -1) r_idx = j;
            }
            table[idx][r_idx] = toDecimal(addrs[i].tag);
            misses++;
            cycles += h + p;
        }
        printf("%s %c\n", addr[i], out);
    }
    miss_rate = round((misses / (misses + hits)) * 100);
    printf("%d %d\n", miss_rate, cycles);
    for (int i = 0; i < rows; i++)
        free(table[i]);
    free(table);
}

void lru() {
    char out;
    float misses = 0, hits = 0;
    int idx, found, cycles = 0, miss_rate, r_idx;
    int rows = S / E, cols = E + 1;
    int** table = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < S; i++)
        table[i] = (int*)malloc(cols * sizeof(int));
    for (int i = 0; i < rows;i++)
        for (int j = 0; j < cols; j++)
            table[i][j] = -1;
    breakAddresses();
    for (int i = 0; i < n; i++) {
        idx = toDecimal(addrs[i].set);
        found = 0;
        // if exists in cache
        for (int j = 1; j < cols; j++) {
            if (table[idx][j] == toDecimal(addrs[i].tag)) {
                found = 1;
                out = 'H';
                hits++;
                cycles += h;
                table[idx][0] = j == 1 ? 2 : 1;
            }
        }
        if (!found) {
            r_idx = -1;
            out = 'M';
            for (int j = cols - 1; j > 0; j--) {
                if (table[idx][j] == -1) {
                    r_idx = j;
                    break;
                }
            }
            if (r_idx == -1) r_idx = table[idx][0];
            table[idx][r_idx] = toDecimal(addrs[i].tag);
            table[idx][0] = r_idx == 1 ? 2 : 1;
            misses++;
            cycles += h + p;
        }
        printf("%s %c\n", addr[i], out);
    }
    miss_rate = round((misses / (misses + hits)) * 100);
    printf("%d %d\n", miss_rate, cycles);
    for (int i = 0; i < rows; i++)
        free(table[i]);
    free(table);
}

int main() {
    printf("lol\n");
    unsigned int a;
    
    scanf("%x", &a);
    printf("%X\n", a);
    /*unsigned int a;
    unsigned int* adds = (unsigned int*)malloc(sizeof(unsigned int));
    scanf("%d", &S);
    scanf("%d", &E);
    scanf("%d", &B);
    scanf("%d", &m);
    scanf("%s", policy);
    scanf("%d", &h);
    scanf("%d", &p);
    do {
        scanf("%x", &a);
        if (a != 0xff) {
            adds[n - 1] = a;
            n++;
            adds = (unsigned int*)realloc(adds, n * sizeof(unsigned int));
            if (adds == NULL) {
                printf("Memory could not be allocated.\n");
                return -1;
            }
        }
        else break;
    } while (1);
    n--;
    calcBits();*/
    /*addr = (char**)malloc(n * sizeof(char*));
    for (int i = 0; i < n;i++) {
        addr[i] = (char*)malloc(3 * sizeof(char));
        sprintf(addr[i], "%x", adds[i]);
        if (strlen(addr[i]) == 1) {
            addr[i][1] = addr[i][0];
            addr[i][0] = '0';
            addr[i][2] = '\0';
        }
    }
    if (strcmp(policy, "LFU") == 0)
        lfu();
    else lru();*/
    return 0;
}