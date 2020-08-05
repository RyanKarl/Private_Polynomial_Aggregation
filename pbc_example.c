#include <pbc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
  pairing_t pairing;
  element_t g;
  element_t public_key, sig;
  element_t sk;
  element_t h;
  element_t sig2;
  element_t h2;
  element_t public_key2;
  element_t temp1, temp2;
  int valid;

  // Set up
  char s[16384];
  FILE *fp = fopen("param/a.param", "r");
  if(!fp) pbc_die("error opening param/a.param");
  size_t count = fread(s, 1, 16384, fp);
  if(!count) pbc_die("input error");
  fclose(fp);

  if(pairing_init_set_buf(pairing, s, count)) pbc_die("pairing init failed");

  element_init_G1(g, pairing);
  element_init_G1(public_key, pairing);
  element_init_G1(h, pairing);
  element_init_G1(sig, pairing);
  element_init_GT(temp1, pairing);
  element_init_GT(temp2, pairing);
  element_init_Zr(sk, pairing);
  element_init_G1(sig2, pairing);
  element_init_G1(h2, pairing);
  element_init_G1(public_key2, pairing);

  element_set_str(g, argv[1], 10);
  //element_printf("g = %B\n", g);

  // Key generatation

  //printf("secret key %s", argv[3]);
  element_set_str(sk, argv[3], 10);
  //element_printf("sk = %B\n", sk);

  element_pow_zn(public_key, g, sk);
  //element_printf("public key = %B\n", public_key);

  // Sign
  //printf("Message: %s\n", argv[2]);

  element_from_hash(h, argv[2], strlen(argv[2]));
  //element_printf("message hash = %B\n", h);

  element_pow_zn(sig, h, sk);
  //element_printf("sig = %B\n", sig);

  // Verify
  element_from_hash(h2, argv[4], strlen(argv[4]));
  //element_printf("message hash2 = %B\n", h2);
  element_set_str(sig2, argv[5], 10);
  element_set_str(public_key2, argv[6], 10);


  pairing_apply(temp1, sig2, g, pairing);
  pairing_apply(temp2, h2, public_key2, pairing);
  if(!element_cmp(temp1, temp2)) {
    valid = 1;
  }
  else {
    valid = 0;
  }

element_printf("%B,%B\n", sig, g);
printf("%d\n", valid);
element_clear(g);
element_clear(public_key);
element_clear(sig);
element_clear(sk);
element_clear(h);
element_clear(temp1);
element_clear(temp2);
element_clear(h2);
element_clear(sig2);
element_clear(public_key2);
pairing_clear(pairing);
return 0;
}
