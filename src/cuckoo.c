// Cuckoo Cycle, a memory-hard proof-of-work
// Copyright (c) 2013-2016 John Tromp

#include "cuckoo.h"
#include <inttypes.h> // for SCNx64 macro
#include <stdio.h>    // printf/scanf
#include <stdlib.h>   // exit
#include <unistd.h>   // getopt
#include <assert.h>   // d'uh

int main(int argc, char **argv) {
  const char *header = "";
  int nonce = 0;
  int c;
  while ((c = getopt (argc, argv, "h:n:")) != -1) {
    switch (c) {
      case 'h':
        header = optarg;
        break;
      case 'n':
        nonce = atoi(optarg);
        break;
    }
  }
  printf("Verifying size %d proof for cuckoo%d(\"%s\",%d)\n",
               PROOFSIZE, SIZESHIFT, header, nonce);
  char headernonce[HEADERLEN];
  u32 hdrlen = strlen(header);
  memcpy(headernonce, header, hdrlen);
  memset(headernonce+hdrlen, 0, sizeof(headernonce)-hdrlen);
  ((u32 *)headernonce)[HEADERLEN/sizeof(u32)-1] = htole32(nonce);
  for (int nsols=0; scanf(" Solution") == 0; nsols++) {
    u64 nonces[PROOFSIZE];
    for (int n = 0; n < PROOFSIZE; n++) {
      int nscan = scanf(" %" SCNx64, &nonces[n]);
      assert(nscan == 1);
    }
    int pow_rc = verify(nonces, headernonce, sizeof(headernonce));
    if (pow_rc == POW_OK) {
      printf("Verified with cyclehash ");
      unsigned char cyclehash[32];
      SHA256((unsigned char *)nonces, sizeof(nonces), cyclehash);
      for (int i=0; i<32; i++)
        printf("%02x", cyclehash[i]);
      printf("\n");
    } else {
      printf("FAILED due to %s\n", errstr[pow_rc]);
    }
  }
  return 0;
}
