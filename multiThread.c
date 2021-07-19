#include "rsa/config.h"
#include "rsa/aes.h"
#include "rsa/bignum.h"
#include "rsa/rsa.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rsa/memory_buffer_alloc.h"
#include "rsa/memory.h"
#include "rsa/platform.h"
#include "rsa/threading.h"


#define NUM_OF_THREAD 2  
#define KEY_LEN 256
# define KEY_BUFFER_SIZE 2368

threading_mutex_t lock;


void decryption(){
    unsigned char alloc_buf[10000];
    memory_buffer_alloc_init( &alloc_buf, sizeof(alloc_buf) );

    unsigned char private_encrypt[KEY_BUFFER_SIZE];
    int total_dec=5;
    unsigned char * buffer = 0;
    long length;
    unsigned char msg_decrypted[KEY_LEN];


    FILE * fp2 = fopen ("msg.enc", "rb");
    int size1=KEY_BUFFER_SIZE;
    if(fp2){
        while(size1>0){
            fread(private_encrypt,1,sizeof (private_encrypt),fp2);
            size1=size1-1;
        }
    }
    fclose(fp2);

    FILE * fp = fopen ("rsa_priv.txt", "rb");

    if (fp){
        fseek (fp, 0, SEEK_END);
        length = ftell (fp);
        fseek (fp, 0, SEEK_SET);
        buffer = calloc (1,length+1);
        if (buffer){
            fread (buffer, 1, length, fp);
        }
        fclose (fp);
    }

    // initialize rsaContext
    rsa_context rsaContext;
    rsa_init(&rsaContext,RSA_PKCS_V15, 0);
    rsaContext.len=KEY_LEN;

    // spliting keys and load into rsa context
    const char s[3] = "= ";
    char *token;
    int k=0, size;
    char *rest=buffer;

    // get the first token
    token = strtok_r(rest,s,&rest);

    // walk through other tokens
    while( token != NULL ) {
        size = strlen(token);

        switch (k) {
            case 1:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.N, 16, token);
                break;

            case 3:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.E, 16, token);
                break;

            case 5:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.D, 16, token);
                break;

            case 7:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.P, 16, token);
                break;

            case 9:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.Q, 16, token);
                break;

            case 11:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.DP, 16, token);
                break;

            case 13:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.DQ, 16, token);
                break;

            case 15:
                token[size-1]='\0';
                mpi_read_string(&rsaContext.QP, 16, token);
                break;
        }
        k=k+1;
        //token = strtok(NULL, "= \n");
        token = strtok_r(rest, "= \n", &rest);
    }

    
    if( rsa_private(&rsaContext,private_encrypt, msg_decrypted) != 0 ) {
	printf( "Decryption failed! %d\n", rsa_private(&rsaContext,private_encrypt, msg_decrypted));
    }else{
        printf("%d Decrypted plaintext-----> %s\n",total_dec, msg_decrypted );
    }

    memory_buffer_alloc_free();

}


void thread_function(void * input){

	printf("Test thread\n");
    //unsigned char alloc_buf[10000];
    //memory_buffer_alloc_init( &alloc_buf, sizeof(alloc_buf) );
    
    
	int total_loop=5;

    
	while(total_loop>0){
        //pthread_mutex_lock(&lock);
		decryption();
        //pthread_mutex_unlock(&lock);
		total_loop--;
        			
	}

    //memory_buffer_alloc_free();
    
    
	
}


int main(){ 
    int i;
    clock_t t;

    // total number of thread
    pthread_t ths[NUM_OF_THREAD];

    //pthread_mutex_init(&lock,NULL);
    //polarssl_mutex_init(&lock);


    // start time count
    time_t start = time(NULL);

    for (i = 0; i < NUM_OF_THREAD; i++) {
        pthread_create(&ths[i], NULL, thread_function, NULL);
    }

    for (i = 0; i < NUM_OF_THREAD; i++) {
        void* res;
        pthread_join(ths[i], &res);
    }

    //pthread_mutex_destroy(&lock);
    //polarssl_mutex_free(&lock);

    // end time count
    printf("Time %.2f\n", (double)(time(NULL) - start));
    return 0;
}
