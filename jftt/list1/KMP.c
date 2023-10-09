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
int check_if_diffrent(int lenk, int lenq, int k, int q, char* template, int* CS_CLT) {
  if (lenq != lenk) {
    return 1;//they're diffrent
  } else {
    for (int i = 0; i < lenk; i++) {
      if (template[CS_CLT[k] + i] != template[CS_CLT[q] + i]) {
        return 1;
      } 
    }
    return 0;//they're the same
  }
}
int *PI(char* template, int m, int *CS_CLT){
  int *pi = (int*)malloc(sizeof(int) * m);
  int len = 0; // length of the previous longest prefix suffix
  pi[0] = 0; // pi[0] is always 0
  int i = 1;
  while (i < m) {  // the loop calculates pi[i] for i = 1 to m-1
    int leni =  CS_CLT[i + 1] - CS_CLT[i];
    int lenl =  CS_CLT[len + 1] - CS_CLT[len];
    //if (template[i] == template[len]) {
    if (!check_if_diffrent(leni, lenl, i, len, template, CS_CLT)) {
      len++;
      pi[i] = len;
      i++;
    } else { // (template[i] != template[len])
      if (len != 0) {
        len = pi[len - 1];
      } else {// if (len == 0)
        pi[i] = 0;
        i++;
      }
    }
  }
  return pi;
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
  printf("CS_CLT: 0, ");
  for (int i = 1; i <= input_len; i++) {
    CS_CLT[i] = CS_CLT[i-1] + char_len_table[i-1];
    printf("%d, ", CS_CLT[i]);

  }

  int *pi = PI(argv[1], input_len, CS_CLT);
  printf("\n"); 

  for (int i = 0; i < input_len; i++) {
    printf("pi[%d] = %d\n",i, pi[i]);
  }
  printf("\n"); 

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
    for (; q <= input_len;) {
        if (dif == 0) {
          if (argv[1][CS_CLT[q]] == c1) {
            if (len == 1) {
              counter2++;
              break;
            }
            if ((c2 = fgetc(in)) != EOF) {
              counter++;
              if (argv[1][CS_CLT[q] + 1] == c2) {
                if (len == 2) {
                  counter2++;
                  break;
                }
                if ((c3 = fgetc(in)) != EOF) {
                  counter++;
                  if (argv[1][CS_CLT[q] + 2] == c3) {
                    if (len == 3) {
                      counter2++;
                      break;
                    }
                    if ((c4 = fgetc(in)) != EOF) {
                      counter++;
                      if (argv[1][CS_CLT[q] + 3] == c4) {
                        counter2++;
                        break;
                      } else {
                        dif = 4;
                        if (q != 0) {
                          q = pi[q - 1];
                          //dif = 0;
                        } else {
                          break;
                        }
                      }
                    } else {
                      exit_r = 1;
                    }
                  } else {
                    dif = 3;
                    if (q != 0) {
                      q = pi[q - 1];
                      //dif = 0;
                    } else {
                      break;
                    }
                  }
                } else {
                  exit_r = 1;
                }
              } else {
                dif = 2;
                if (q != 0) {
                  q = pi[q - 1];
                  //dif = 0;
                } else {
                  break;
                }
              }
            } else {
              exit_r = 1;
            }
          } else {
            dif = 1;
            if (q != 0) {
              q = pi[q - 1];
              //dif = 0;
            } else {
              break;
            }
          }
        } else if (dif >= 1) {
          if (argv[1][CS_CLT[q]] == c1) {
            if (len == 1) {
              dif = 0;
              counter2++;
              break;
            }
            if (dif >= 2) {
              if (argv[1][CS_CLT[q] + 1] == c2) {
                if (len == 2) {
                  dif = 0;
                  counter2++;
                  break;
                }
                if (dif >= 3) {
                  if (argv[1][CS_CLT[q] + 2] == c3) {
                    if (len == 3) {
                      dif = 0;
                      counter2++;
                      break;
                    }
                    if (dif >= 4) {
                      if (argv[1][CS_CLT[q] + 3] == c4) {
                        dif = 0;
                        counter2++;
                        break;
                      } else {
                        dif = 4;
                        if (q != 0) {
                          q = pi[q - 1];
                          //dif = 0;
                        } else {
                          break;
                        }
                      }
                    } else {
                      if ((c4 = fgetc(in)) != EOF) {
                        counter++;
                        if (argv[1][CS_CLT[q] + 3] == c4) {
                          dif = 0;
                          counter2++;
                          break;
                        } else {
                          dif = 4;
                          if (q != 0) {
                            q = pi[q - 1];
                            //dif = 0;
                          } else {
                            break;
                          }
                        }
                      } else {
                        exit_r = 1;
                      }
            
                    }       

                  } else {
                    dif = max(3, dif);
                    if (q != 0) {
                      q = pi[q - 1];
                      //dif = 0;
                    } else {
                      break;
                    } 
                  }
                } else {
                  if ((c3 = fgetc(in)) != EOF) {
                    counter++;
                    if (argv[1][CS_CLT[q] + 2] == c3) {
                      if (len == 3) {
                        dif = 0;
                        counter2++;
                        break;
                      }
                      if ((c4 = fgetc(in)) != EOF) {
                        counter++;
                        if (argv[1][CS_CLT[q] + 3] == c4) {
                          counter2++;
                          dif = 0;
                          break;
                        } else {
                          dif = 4;
                          if (q != 0) {
                            q = pi[q - 1];
                            //dif = 0;
                          } else {
                            break;
                          }
                        }
                      } else {
                        exit_r = 1;
                      }
                    } else {
                      dif = max(3, dif);
                      if (q != 0) {
                        q = pi[q - 1];
                        //dif = 0;
                      } else {
                        break;
                      }
                    }
                  } else {
                    exit_r = 1;
                  }
                }

              } else {
                dif = max(2, dif);
                if (q != 0) {
                  q = pi[q - 1];
                  //dif = 0;
                } else {
                  break;
                }
              }
            } else {
              if ((c2 = fgetc(in)) != EOF) {
                counter++;
                if (argv[1][CS_CLT[q] + 1] == c2) {
                  if (len == 2) {
                    counter2++;
                    dif = 0;
                    break;
                  }
                  if ((c3 = fgetc(in)) != EOF) {
                    counter++;
                    if (argv[1][CS_CLT[q] + 2] == c3) {
                      if (len == 3) {
                        counter2++;
                        dif = 0;
                        break;
                      }
                      if ((c4 = fgetc(in)) != EOF) {
                        counter++;
                        if (argv[1][CS_CLT[q] + 3] == c4) {
                          counter2++;
                          dif = 0;
                          break;
                        } else {
                          dif = 4;
                          if (q != 0) {
                            q = pi[q - 1];
                            //dif = 0;
                          } else {
                            break;
                          }
                        }
                      } else {
                        exit_r = 1;
                      }
                    } else {
                      dif = max(3, dif);
                      if (q != 0) {
                        q = pi[q - 1];
                        //dif = 0;
                      } else {
                        break;
                      }
                    }
                  } else {
                    exit_r = 1;
                  }
                } else {
                  dif = max(2, dif);
                  if (q != 0) {
                    q = pi[q - 1];
                    //dif = 0;
                  } else {
                    break;
                  }
                }
              } else {
                exit_r = 1;
              }
            
            }
          } else {
            dif = max(1, dif);
            if (q != 0) {
              q = pi[q - 1];
              //dif = 0;
            } else {
              break;
            }
          }

        } else {
          printf("WTF?\n");
        }
      if (exit_r == 1)
        break;
    }
    if (dif == 0 && exit_r == 0) {// nasza litera zaczyna sie na indexie CS_CLT[i] i ma dlugosc len
      q++;
      if (q == input_len) {
        position = counter - input_len;
        printf("[%d], ", position);
        ///////
        q = pi[q - 1];
      }
    }
    if (dif >= 1) {
      if (len - dif > 0)
          for (int j = 0; j < len - dif; j++) {
            counter++;
            if ((c1 = fgetc(in)) == EOF) {
              exit_r = 1;
            }
            if (exit_r == 1)
              break;
          }
      dif = 0;
      //q = pi[q - 1];
    }
    dif = 0;

    if (exit_r == 1)
      break;
  }
  fclose(in);
  printf("\n");
  printf("[]\n");


  free(CS_CLT);
  free(char_len_table);
  free(pi);
  return EXIT_SUCCESS;
}

