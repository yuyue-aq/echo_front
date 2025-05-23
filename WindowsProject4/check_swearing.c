#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 定义脏话数组
static const char* swear_words[] = {
    // 中文脏话
    "装逼", "操你妈", "卧槽", "他妈的", "你妈的", "傻逼", "艹",
    "肏", "狗日的", "日你", "杂种", "混球", "二逼", "傻叉",
    "妈逼", "贱货", "白痴", "脑残", "死妈", "滚犊子", "去你妈",
    "草泥马", "尼玛", "妈了个巴子", "老逼", "骚货", "臭傻逼",
    "狗逼", "穷逼", "装逼犯", "傻吊", "愣逼", "逼样", "我日",
    "我艹", "卧艹", "握草", "cnm", "nmb", "sb", "nc", "djb", "nt",

    // 英文脏话（按长度降序排列提高匹配效率）
    "motherfucker", "son of a bitch", "asshole", "douchebag",
    "sonofabitch", "fuck", "shit", "bitch", "dick", "cock",
    "pussy", "cunt", "whore", "slut", "prick", "bastard",
    "twat", "arsehole", "nigger", "faggot", "retard",
    "stupid", "idiot", "moron"
};

// 自动计算数组长度
static const int WORD_COUNT = sizeof(swear_words) / sizeof(swear_words[0]);

// 转换为小写（用于英文脏话不区分大小写匹配）
static char* to_lowercase(char* str) {
    if (!str) return NULL;
    char* p = str;
    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }
    return str;
}

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

// KMP字符串匹配（增强版，支持大小写不敏感匹配）
static int kmp_search(const char* text, const char* pattern, int case_sensitive) {
    int n = strlen(text);
    int m = strlen(pattern);

    if (m == 0) return 0;

    // 创建模式串的本地副本用于大小写转换
    char* pattern_local = strdup(pattern);
    if (!pattern_local) return -1;

    // 如果不区分大小写，转换为小写
    if (!case_sensitive) {
        to_lowercase(pattern_local);

        // 创建文本的本地副本用于大小写转换
        char* text_local = strdup(text);
        if (text_local) {
            to_lowercase(text_local);
            text = text_local; // 使用转换后的文本进行匹配
        }
    }

    int* next = (int*)malloc(m * sizeof(int));
    if (!next) {
        free(pattern_local);
        return -1;
    }

    compute_next(pattern_local, next, m);

    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern_local[j]) {
            j = next[j - 1];
        }
        if (text[i] == pattern_local[j]) {
            j++;
        }
        if (j == m) {
            free(next);
            free(pattern_local);
            if (!case_sensitive) free((void*)text);
            return i - j + 1; // 找到匹配
        }
    }

    free(next);
    free(pattern_local);
    if (!case_sensitive) free((void*)text);
    return -1; // 未找到匹配
}

// 检测文本中是否包含脏话
int detect_swear_words(const char* text) {
    if (!text) return 0;

    // 遍历所有脏话进行匹配
    for (int i = 0; i < WORD_COUNT; i++) {
        // 判断是否为英文脏话（简单通过首字符判断）
        int is_english = (unsigned char)swear_words[i][0] < 128;

        // 英文脏话不区分大小写，中文脏话区分
        if (kmp_search(text, swear_words[i], !is_english) >= 0) {
            return 1; // 包含脏话
        }
    }

    return 0; // 不包含脏话
}