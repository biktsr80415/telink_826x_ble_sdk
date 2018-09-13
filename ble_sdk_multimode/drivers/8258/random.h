
#pragma once





void random_generator_pre_init(void);
void random_generator_init(void);

unsigned int rand(void);

/*********************************************************************
 * @fn          generateRandomNum
 *
 * @brief       generate random number
 *
 * @param       len - len
 *
 * @param       data -  buffer
 *
 * @return      None
 */
void generateRandomNum(int len, unsigned char *data);

