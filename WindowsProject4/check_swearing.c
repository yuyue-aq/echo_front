#include <string.h>
#include <stdlib.h>

// 定义脏话数组
static const char* swear_words[] = {
"装逼", "操你妈","卧槽","他妈的", "你妈的", "傻逼", "艹","肏", "狗日的", "日你", "杂种", "混球", "二逼", "傻叉","妈逼", "贱货", "白痴", "脑残", "死妈","滚犊子", "去你妈", "草泥马", "尼玛", "妈了个巴子","老逼", "骚货", "臭傻逼", "狗逼", "穷逼", "装逼犯","傻吊","愣逼", "逼样", "我日", "我艹", "卧艹", "握草","cnm", "nmb", "sb", "nc", "djb", "nt",
"fuck", "shit", "asshole", "bitch", "dick", "cock", "pussy","cunt", "motherfucker", "son of a bitch", "whore", "slut","douchebag", "prick", "bastard", "twat", "arsehole", "nigger","faggot", "retard", "stupid", "idiot", "moron"
};
static const int WORD_COUNT = 7;

// KMP算法的next数组计算
static void compute_next(const char* pattern, int* next, int len) {
    int j = 0;
    next[0] = 0;

    for (int i = 1; i < len; i++) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = next[j - 1];
        }
        if (pattern[i] == pattern[j]) {
            j++;
        }
        next[i] = j;
    }
}

// KMP字符串匹配
static int kmp_search(const char* text, const char* pattern) {
    int n = strlen(text);
    int m = strlen(pattern);

    if (m == 0) return 0;

    int* next = (int*)malloc(m * sizeof(int));
    if (!next) return -1;

    compute_next(pattern, next, m);

    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern[j]) {
            j = next[j - 1];
        }
        if (text[i] == pattern[j]) {
            j++;
        }
        if (j == m) {
            free(next);
            return i - j + 1; // 找到匹配
        }
    }

    free(next);
    return -1; // 未找到匹配
}

int detect_swear_words(const char* text) {
    if (!text) return 0;

    for (int i = 0; i < WORD_COUNT; i++) {
        if (kmp_search(text, swear_words[i]) >= 0) {
            return 1; // 包含脏话
        }
    }

    return 0; // 不包含脏话
}