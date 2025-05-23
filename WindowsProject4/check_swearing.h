#ifndef SWEAR_DETECTION_H
#define SWEAR_DETECTION_H

/**
 * 检测文本中是否包含脏话
 * @param text 待检测的文本
 * @return 包含脏话返回1，不包含返回0
 */
int detect_swear_words(const char* text);

#endif    #pragma once
