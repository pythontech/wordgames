/***************************************************************************
 *      Find sub-words from dictionary                                     *
 ***************************************************************************/
#include "d2.h"
#include <stdlib.h>
#include <stdio.h>                     
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

Dict dict;

/*-------------------------------------------------------------------------*
 *      types used                                                         *
 *-------------------------------------------------------------------------*/
typedef struct {unsigned char count[26];} Pool;
#define Pool_contains(p,c) ((p).count[(c)-'A']>0)
#define Pool_add(p,c) ((p).count[(c)-'A']++)
#define Pool_remove(p,c) ((p).count[(c)-'A']--)
#define Pool_count(p,c) ((p).count[(c)-'A'])
#define Pool_clear(p) {int i; for (i=0; i<26; i++) (p).count[i]=0;}

/*-------------------------------------------------------------------------*
 *      Make a pool out of a word                                          *
 *-------------------------------------------------------------------------*/
int make_pool(const char *word, Pool *p)
{
  int c, n=0;

  Pool_clear(*p);			/* clear the counts */

  while ((c = *word++) != 0) {
    if (isalpha(c)) {
      Pool_add(*p, toupper(c));
      n++;
    }
  }
  return n;
}

/*-------------------------------------------------------------------------*
 *      Process all sub-words of the given length made from the pool       *
 *-------------------------------------------------------------------------*/
void find_subwords(Pool *wordpool, int sublen,
                   void process(const char *sw, Pool *rem))
{
  if (sublen >= 2 && sublen <= MAXLEN) {
    Dscan ds = Dscan_open(dict, sublen);

    /* scan dictionary for each initial pair of letters */
    const char *subword;

    while ((subword = Dscan_read(ds)) != NULL) {
      Pool remd = *wordpool;
      int i, c, ok=TRUE;

      for (i=0; i<sublen; i++) {
      	c = subword[i];
      	if (Pool_contains(remd, c)) {
      	  Pool_remove(remd, c);
      	}else{
      	  ok = FALSE;
      	  Dscan_skip(ds, i);
      	  break;
      	}
      }
      if (ok) {
        process(subword, &remd);
      }
    }
    Dscan_close(ds);
  }
}

char word1[MAXLEN+1];
int len2;

void print_both(const char *word2, Pool *remd)
{
  printf("%s %s\n", word1, word2);
}

void find_other_and_print(const char *word, Pool *remd)
{
  strcpy(word1, word);
  find_subwords(remd, len2, print_both);
}

int main(int argc, char **argv)
{
  char word[100];
  Pool wordpool;
  int len, i;

  dict = Dict_open(NULL);

  if (argc > 1) {
    len = make_pool(argv[1], &wordpool);
    for (i=len; i>=(len+1)/2; i--) {
      len2 = len-i;
      find_subwords(&wordpool, i, find_other_and_print);
    }
  } else {
    while (printf("word> "), fflush(stdout),
	   fgets(word, sizeof(word), stdin),
	   (len = make_pool(word, &wordpool)) != 0) {
      for (i=len; i>=(len+1)/2; i--) {
	len2 = len-i;
	find_subwords(&wordpool, i, find_other_and_print);
      }
    }
  }

  Dict_close(dict);
  return 0;
}
