
/*
 * This crypt(3) validation program shipped with UFC-crypt
 * is derived from one distributed with Phil Karns PD DES package.
 *
 * @(#)cert.c   1.8 11 Aug 1996
 */

#include "crypt.h"
#include "crypt-obsolete.h"
#include "test-des-cases.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

symver_ref(encrypt, encrypt, SYMVER_FLOOR);
symver_ref(setkey, setkey, SYMVER_FLOOR);

static void
expand (unsigned char ex[64], const unsigned char pk[8])
{
  int i, j;
  unsigned int t;

  for (i = 0; i < 8; i++)
    {
      t = pk[i];
      for (j = 0; j < 8; j++)
        ex[i*8 + j] = (t & (0x01u << (7 - j))) != 0;
    }
}

static void
ex_print (const unsigned char ex[64])
{
  int i, j;
  unsigned int t;

  for (i = 0; i < 8; i++)
    {
      t = 0;
      for (j = 0; j < 8; j++)
        t = (t << 1) | ex[i*8 + j];
      printf ("%02x", t);
    }
}

static void
pk_print (const unsigned char pk[8])
{
  for (int i = 0; i < 8; i++)
    printf ("%02x", (unsigned int)pk[i]);
}

static void
report_failure (size_t n, bool decrypt,
                const struct des_testcase *tc, const unsigned char got[64])
{
  printf ("FAIL: %zu/%s: k=", n, decrypt ? "de" : "en");
  pk_print (tc->key);
  fputs ("  exp ", stdout);
  if (decrypt)
      pk_print (tc->plain);
  else
      pk_print (tc->answer);
  fputs ("  got ", stdout);
  ex_print (got);
  putchar ('\n');
}

int
main (void)
{
  unsigned char key[64], plain[64], cipher[64], answer[64];
  const struct des_testcase *tc;
  size_t t;
  int status = 0;

  for (t = 0; t < N_DES_TESTCASES; t++)
    {
      tc = &des_testcases[t];
      expand (key, tc->key);
      expand (plain, tc->plain);
      expand (answer, tc->answer);

      setkey ((char *)key);
      memcpy (cipher, plain, 64);
      encrypt ((char *)cipher, 0);

      if (memcmp (cipher, answer, 64) != 0)
        {
          status = 1;
          report_failure (t, false, tc, cipher);
        }

      memcpy (cipher, answer, 64);
      encrypt ((char *)cipher, 1);
      if (memcmp (cipher, plain, 64) != 0)
        {
          status = 1;
          report_failure (t, true, tc, cipher);
        }
    }

  return status;
}
