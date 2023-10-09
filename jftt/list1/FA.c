#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <stdint.h>
#include <string.h>

#define min(a,b) \
	({__typeof__ (a) _a = (a); \
	  __typeof__ (b) _b = (b); \
	  _a < _b ? _a : _b; })
#define max(a,b) \
	({__typeof__ (a) _a = (a); \
	  __typeof__ (b) _b = (b); \
	  _a > _b ? _a : _b; })

int utf8_char_length(uint8_t first_byte) {
  if ((first_byte & 0x80) == 0x00) {
    return 1;
  } else if ((first_byte & 0xE0) == 0xC0) {
    return 2;
  } else if ((first_byte & 0xF0) == 0xE0) {
    return 3;
  } else if ((first_byte & 0xF8) == 0xF0) {
    return 4;
  } else {
    // Invalid UTF-8 character
    return -1;
  }
}


//my mod - comment some stuff inside, return number of utf-8 chars so void -> int
// Function to split a UTF-8 encoded character array into separate characters
int split_utf8_chars(const char *input, int *char_len_table) {
  int index = 0;
  int number_of_chars = 0;
  while (input[index] != '\0') {
    int char_length = utf8_char_length((uint8_t)input[index]);
    if (char_length == -1) {
      printf("Invalid UTF-8 character at index %d\n", index);
      break; 
    }

    printf("Character at index %d: ", index);
    for (int i = 0; i < char_length; i++) {
      printf("%c", input[index + i]);
    }
    printf("\n");

    char_len_table[number_of_chars] = char_length;
    index += char_length;
    number_of_chars++;
  }
  return number_of_chars;
}

int str_len(char *input_str) {
  int i = 0;
  for (; input_str[i] != '\0'; i++);
  return i;
}

int compare_strings(char* template, int q, int k, int i, int *CS_CLT) {
  //check if letter is contained in pattern
  k = k - 2;
  q = q - 1;
  for (; k >= 0; k--) {
    int diffrence = 0;
    int szi = 0;
    if (i == 0) {
      szi = CS_CLT[i + 1];
    } else {
      szi = CS_CLT[i + 1] - CS_CLT[i];
    }

    int szk1 = CS_CLT[k + 1] - CS_CLT[k];
    if (szk1 == szi) { // if size of k and a (pattern of i) are diffrent -> skip
      for (int j = 0; j < szi; j++) {
        if (template[CS_CLT[k] + j] != template[CS_CLT[i] + j]) {//CS_CLT dla 3 znaku gdzie kazdy znak mial size 2 to 6, musimy sprawdzic 6 i 5
          diffrence = 1;
          break;
        } 
      }
    } else {
      diffrence = 1;
    } 
    if (q == -1 && diffrence == 0) {
      break;
    }
    
    if (diffrence == 0) { //porownujemy z symbolem a
      if (k == 0) {
        break;
      }
      int tmp_k = k - 1;
      int tmp_q = q;
      int szk = CS_CLT[tmp_k + 1] - CS_CLT[tmp_k];
      int szq = CS_CLT[tmp_q + 1] - CS_CLT[tmp_q];
      if (szk != szq) { // if charcters have diff sizes, then they must be diffrent
        diffrence = 1;
      }
      int offset = 0;
      while (diffrence == 0 && (CS_CLT[tmp_k + 1] - 1 - offset) >= 0 && (CS_CLT[tmp_q + 1] - 1 - offset) >= 0 && tmp_k >= 0 && tmp_q >= 0) {
        if (template[CS_CLT[tmp_k + 1] - 1 - offset] == template[CS_CLT[tmp_q + 1] - 1 - offset]) {
          offset++;
        } else { //roznia sie w jakims miejscu czyli wychodzimy z while **
          offset = 0;
          break;
        }
      }
      if (offset != 0) {
        return k + 1;
      }
    }
  }
  return k + 1;
}
//sprwadz czy lit nalezy do patternu jezeli tak to stan, jak nie to stan na 0
int** CTF(char* template, int m, int *CS_CLT) {
  //ALFABET BIORE TYLKO Z TEMPLATE, JAK POZA TEMPLATE TO SIEMA
  //NOTE: w pseudokodzie mamy od P od 1 do m
  int **delta = (int**)malloc(sizeof(int*) * (m + 1));
  for (int i = 0; i <= m; i++)
    delta[i] = (int*)malloc(sizeof(int) * (m + 1));
  for (int q = 0; q <= m; q++) {
    for (int i = 0; i < m; i++) {
      int k = min(m + 1, q + 2);
      delta[q][i] = compare_strings(template, q, k, i, CS_CLT);//i instead of a to read a from template
    }
    delta[q][m] = 0;//diffrent characters than those in pattern 
  }
  return delta;
}

int main(int argc, char **argv) {
  if (argc > 3)
    return -1;

  char *locale = setlocale(LC_ALL, "");
  
  int m = str_len(argv[1]);//upper bound for char_len_table size
  printf("m: %d\n", m);

  int *tmp_char_len_table = (int*)malloc(sizeof(int) * m);
  int input_len = split_utf8_chars(argv[1], tmp_char_len_table);
  printf("\ninput_len: %d\n", input_len);
  int *char_len_table = (int*)malloc(sizeof(int) * input_len);
  printf("char_len_table: ");
  for (int i = 0; i < input_len; i++) {
    char_len_table[i] = tmp_char_len_table[i];
    printf("%d, ", char_len_table[i]);
  }
  printf("\n");
  free(tmp_char_len_table);

  //it'll be usefull to chave char_len_table in cumsum
  int *CS_CLT = (int*)malloc(sizeof(int) * (input_len + 1));
  CS_CLT[0] = 0;
  for (int i = 1; i <= input_len; i++) {
    CS_CLT[i] = CS_CLT[i-1] + char_len_table[i-1];
  }
  int **delta = CTF(argv[1], input_len, CS_CLT);

  printf("\nDELTA TABLE:\n");
  printf("columns - letters of pattern //hopefully\nrows - states\n");
  //print d to check if its right
  for (int q = 0; q <= input_len; q++) {
    printf("state %d: ", q);
    for (int i = 0; i <= input_len; i++) {
      printf("%d,", delta[q][i]);
    }
    printf("\n"); 
  }

  int q = 0;
  FILE *in = fopen(argv[2], "r");
  char c1;
  char c2;
  char c3;
  char c4;
  int i = 0;
  int exit_r = 0;
  int dif = 0;
  int counter = 0;
  int counter2 = 0;
  int position = 0;
  //kinda powinny byc 4 chary bo tyle jest max size utf8
  // no a jak wejdziemy do srodka i odczytamy z 3 chary to nam zgina poprzednie
  // wiec trzeba 4 trzymac just in case
  while ((c1 = fgetc(in)) != EOF) {
    counter++;
    int len = utf8_char_length((uint8_t)c1);
    counter = counter - len + 1;
    for (i = 0; i <= input_len; i++) {//patrzymy jaki to symbol
      if (i != input_len) {
        if (dif == 0) {
          if (argv[1][CS_CLT[i]] == c1) {
            if (len == 1) {
              counter2++;
              break;
            }
            if ((c2 = fgetc(in)) != EOF) {
              counter++;
              if (argv[1][CS_CLT[i] + 1] == c2) {
                if (len == 2) {
                  counter2++;
                  break;
                }
                if ((c3 = fgetc(in)) != EOF) {
                  counter++;
                  if (argv[1][CS_CLT[i] + 2] == c3) {
                    if (len == 3) {
                      counter2++;
                      break;
                    }
                    if ((c4 = fgetc(in)) != EOF) {
                      counter++;
                      if (argv[1][CS_CLT[i] + 3] == c4) {
                        counter2++;
                        break;
                      } else {
                        dif = 4;
                      }
                    } else {
                      exit_r = 1;
                    }
                  } else {
                    dif = 3;
                  }
                } else {
                  exit_r = 1;
                }
              } else {
                dif = 2;
              }
            } else {
              exit_r = 1;
            }
          } else {
            dif = 1;
          }
        } else if (dif >= 1) {
          if (argv[1][CS_CLT[i]] == c1) {
            if (len == 1) {
              dif = 0;
              counter2++;
              break;
            }
            if (dif >= 2) {
              if (argv[1][CS_CLT[i] + 1] == c2) {
                if (len == 2) {
                  dif = 0;
                  counter2++;
                  break;
                }
                if (dif >= 3) {
                  if (argv[1][CS_CLT[i] + 2] == c3) {
                    if (len == 3) {
                      dif = 0;
                      counter2++;
                      break;
                    }
                    if (dif >= 4) {
                      if (argv[1][CS_CLT[i] + 3] == c4) {
                        dif = 0;
                        counter2++;
                        break;
                      } else {
                        dif = 4;
                      }
                    } else {
                      if ((c4 = fgetc(in)) != EOF) {
                        counter++;
                        if (argv[1][CS_CLT[i] + 3] == c4) {
                          dif = 0;
                          counter2++;
                          break;
                        } else {
                          dif = 4;
                        }
                      } else {
                        exit_r = 1;
                      }
            
                    }       

                  } else {
                    dif = max(3, dif);
                  }
                } else {
                  if ((c3 = fgetc(in)) != EOF) {
                    counter++;
                    if (argv[1][CS_CLT[i] + 2] == c3) {
                      if (len == 3) {
                        dif = 0;
                        counter2++;
                        break;
                      }
                      if ((c4 = fgetc(in)) != EOF) {
                        counter++;
                        if (argv[1][CS_CLT[i] + 3] == c4) {
                          counter2++;
                          dif = 0;
                          break;
                        } else {
                          dif = 4;
                        }
                      } else {
                        exit_r = 1;
                      }
                    } else {
                      dif = max(3, dif);
                    }
                  } else {
                    exit_r = 1;
                  }
                }

              } else {
                dif = max(2, dif);
              }
            } else {
              if ((c2 = fgetc(in)) != EOF) {
                counter++;
                if (argv[1][CS_CLT[i] + 1] == c2) {
                  if (len == 2) {
                    counter2++;
                    dif = 0;
                    break;
                  }
                  if ((c3 = fgetc(in)) != EOF) {
                    counter++;
                    if (argv[1][CS_CLT[i] + 2] == c3) {
                      if (len == 3) {
                        counter2++;
                        dif = 0;
                        break;
                      }
                      if ((c4 = fgetc(in)) != EOF) {
                        counter++;
                        if (argv[1][CS_CLT[i] + 3] == c4) {
                          counter2++;
                          dif = 0;
                          break;
                        } else {
                          dif = 4;
                        }
                      } else {
                        exit_r = 1;
                      }
                    } else {
                      dif = max(3, dif);
                    }
                  } else {
                    exit_r = 1;
                  }
                } else {
                  dif = max(2, dif);
                }
              } else {
                exit_r = 1;
              }
            
            }
          } else {
            dif = max(1, dif);
          }

        } else {
          printf("WTF?\n");
        }
      } else {
        if (len - dif > 0)
          for (int j = 0; j < len - dif; j++) {
            counter++;
            if ((c1 = fgetc(in)) == EOF) {
              exit_r = 1;
            }
            if (exit_r == 1)
              break;
          }
        q = delta[q][i];
      }
      if (exit_r == 1)
        break;
    }
    if (dif == 0 && exit_r == 0) {// nasza litera zaczyna sie na indexie CS_CLT[i] i ma dlugosc len
      q = delta[q][i];
      if (q == input_len) {
        position = counter - input_len;
        printf("[%d], ", position);
      }
    }
    dif = 0;

    if (exit_r == 1)
      break;
  }
  fclose(in);
  printf("\n");
  printf("[]\n");
  for (int i = 0; i <= input_len; i++)
    free(delta[i]);
  free(delta);

  free(CS_CLT);
  free(char_len_table);
  return EXIT_SUCCESS;
}

