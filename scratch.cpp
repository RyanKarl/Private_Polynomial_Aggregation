static struct Point result;

#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

//TODO init ecall

size_t size_in_bytes(const mpz_t * arg){
  size_t size_base2 = mpz_sizeinbase(*arg, 2);
  size_t numb = sizeof(char)*CHAR_BIT; //Need to modify this on a system with nails
  return (size_base2 + (numb-1))/numb;
}

void write_out(const mpz_t * arg, char ** buf, size_t * size){
  *buf = mpz_export(NULL, size, -1, sizeof(char), 0, 0, *arg);
  return;
}

void write_out(const Point * p, char ** buf_x, size_t * size_x,
               char ** buf_y, size_t * size_y){
  write_out(&(p->x), buf_x, size_x);
  write_out(&(p->y), buf_y, size_y);  
  return;       
}

//Untrusted stubs to make ecalls
void point_double(const Point & p){
  size_t res_x, res_y;
  char * p_x;
  char * p_y;
  size_t x_size, y_size;
  write_out(&p, &p_x, &x_size, &p_y, &y_size);
  call_doubling(p_x, x_size, p_y, y_size, &x_size, &y_size);
}

void point_add(const Point & p, const Point & q){
  size_t res_x, res_y;
  char * p_x;
  char * p_y;
  size_t p_x_size, p_y_size;
  char * q_x;
  char * q_y;
  size_t q_x_size, q_y_size;
  write_out(&p, &p_x, &p_x_size, &p_y, &p_y_size);
  write_out(&q, &q_x, &q_x_size, &q_y, &q_y_size);
  call_add(p_x, p_x_size, p_y, p_y_size, q_x, q_x_size, q_y, q_y_size);
}

void point_mult(const Point & p, const mpz_t & m){
  size_t res_x, res_y;
  char * p_x;
  char * p_y;
  size_t x_size, y_size;
  write_out(&p, &p_x, &x_size, &p_y, &y_size);
  char * m_buf;
  size_t m_size;
  write_out(&m, &m_buf, &m_size);
  call_mult(p_x, x_size, p_y, y_size, m_buf, m_size);
}

void call_doubling(const char * p_x, const size_t p_x_size, 
                   const char * p_y, const size_t p_y_size, 
                   size_t * retsize_x, size_t * retsize_y){
  //Initialize operands                 
  Point P;
  mpz_init(P.x);
  mpz_init(P.y);
  mpz_import(P.x, p_x_size, -1, sizeof(char), 0, 0, p_x);
  mpz_import(P.y, p_y_size, -1, sizeof(char), 0, 0, p_y);
  
  //Actually call function
  Point_Doubling(P, &result);
  //Find sizes of results
  *retsize_x = size_in_bytes(&(result.x));
  *retsize_y = size_in_bytes(&(result.y));
  //Clear space
  mpz_clear(P.x);
  mpz_clear(P.y);
  return;
}            

void call_add(const char * p_x, const size_t p_x_size, 
             const char * p_y, const size_t p_y_size,
             const char * q_x, const size_t q_x_size,
             const char * q_y, const size_t q_y_size, 
             size_t * retsize_x, size_t * retsize_y){
  //Initialize operands                 
  Point P;
  mpz_init(P.x);
  mpz_init(P.y);
  mpz_import(P.x, p_x_size, -1, sizeof(char), 0, 0, p_x);
  mpz_import(P.y, p_y_size, -1, sizeof(char), 0, 0, p_y);
  Point Q;
  mpz_init(Q.x);
  mpz_init(Q.y);
  mpz_import(Q.x, q_x_size, -1, sizeof(char), 0, 0, q_x);
  mpz_import(Q.y, q_y_size, -1, sizeof(char), 0, 0, q_y);
  
  //Actually call function
  Point_Addition(P, Q, &result);
  //Find sizes of results
  *retsize_x = size_in_bytes(&(result.x));
  *retsize_y = size_in_bytes(&(result.y));
  //Clear used space
  mpz_clear(P.x);
  mpz_clear(P.y);
  mpz_clear(Q.x);
  mpz_clear(Q.y);
  return;
}               


void call_mult(const char * p_x, const size_t p_x_size, 
               const char * p_y, const size_t p_y_size,
               const char * m_data, const size_t m_size,
               size_t * retsize_x, size_t * retsize_y){
  //Initialize operands             
  Point P;
  mpz_init(P.x);
  mpz_init(P.y);
  mpz_import(P.x, p_x_size, -1, sizeof(char), 0, 0, p_x);
  mpz_import(P.y, p_y_size, -1, sizeof(char), 0, 0, p_y);
  
  mpz_t m;
  mpz_init(m);
  mpz_import(m, m_size, -1, sizeof(char), 0, 0, m_data);
  
  //Actually call function
  Point_Addition(P, &result, m);
  //Find sizes of results
  *retsize_x = size_in_bytes(&(result.x));
  *retsize_y = size_in_bytes(&(result.y));
  //Free used space
  mpz_clear(P.x);
  mpz_clear(P.y);
  mpz_clear(m);
  return;
}             


