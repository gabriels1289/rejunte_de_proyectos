#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <unistd.h>
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/wait.h>       /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_RX 8000
#define PORT_TX 8001
#define TIME_TOKEN  4500
#define TIMEOUT 500

int main()
{
  pid_t pid;
  int status, i;
  sem_t * sem;
  time_t t0, t1;
  double seg;
  int *flag_rx;

  int sock, len;
  struct sockaddr_in name;
  char *buf;  // Buffer compartido entre los procesos para manejo del Token

  flag_rx = mmap(NULL, sizeof(*flag_rx), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  buf = mmap(NULL, 1024*sizeof(*buf), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  *flag_rx = 0;
  *buf = 0;

  sem = mmap(NULL, sizeof(*sem), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_init(sem,1,1);

  if( fork() )
  {
    // Padre -> Proceso(RX)
    printf("Proceso RX: PID %d - sem: %x\n", getpid(), sem);
    //creo el socket
    sock = socket(AF_INET, SOCK_DGRAM,0);
    if( sock < 0 )
    {
      perror("socket RX");
      exit(1);
    }
    // el socket pertenecerá al dominio internet
    name.sin_family = AF_INET;
    // el socket recibirá mensajes de cualquier dirección de origen (INADDR_ANY)
    name.sin_addr.s_addr = INADDR_ANY;
    // el socket tomará un número de port disponible
    name.sin_port = htons(PORT_RX);
    // se asocia el socket creado con la dirección y port dados en la estructura name
    if( bind( sock, &name, sizeof(name) ) == –1)
    {
      perror("bind RX");
      exit(1);
    }
    // se averigua el número de port asignado al socket, y se lo imprime
    len = sizeof( name );
    if( getsockname( sock, &name, &len) == –1)
    {
      perror("getsockname RX");
      exit(1);
    }
    printf("Port RX# %d \r\n", ntohs(name.sin_port));

    while(1)
    {
      if( recvfrom(sock, buf, sizeof(buf), 0, &name, &len) != -1 )
      {
        printf( "msg: %s from %lx ; %hd \r\n", buf, ntohl(n.sin_addr.s_addr), ntohs(n.sin_port) );
      }
      sem_wait(sem);
      *flag_rx = 1;
      sem_post(sem);
    }

    if( (pid = wait(&status)) > 0 )
    {
      printf("Finalizó proceso %d\n",pid);
    }
    close( sock );
  }else
  {
    // Hijo -> Proceso(TX)
    int i=0; // Contador con el numero de token actual a enviar
    struct hostent *hp;
    //creo el socket
    sock = socket(AF_INET, SOCK_DGRAM,0);
    if( sock < 0 )
    {
      perror("socket TX");
      exit(1);
    }
    // el socket pertenecerá al dominio internet
    name.sin_family = AF_INET;
    // Se lee la dirección IP y el Port dados en la línea de comandos
    // El host de destino viene indicado por su nombre, por lo que debe estar declarado en /etc/hosts
    hp = gethostbyname( argv[1] );
    if( hp == 0 )
      exit(1);
    // Copia la dirección IP la estructura name
    memcpy( &name.sin_addr, hp->h_addr, hp->h_length );
    name.sin_family = AF_INET;
    // el socket tomará un número de port disponible
    name.sin_port = htons(PORT_TX);
    // se asocia el socket creado con la dirección y port dados en la estructura name
    if( bind( sock, &name, sizeof(name) ) == –1)
    {
      perror("bind TX");
      exit(1);
    }
    // se averigua el número de port asignado al socket, y se lo imprime
    len = sizeof( name );
    if( getsockname( sock, &name, &len) == –1)
    {
      perror("getsockname TX");
      exit(1);
    }
    printf("Port TX# %d \r\n", ntohs(name.sin_port));

    printf("Proceso TX: PID %d - sem: %x\n", getpid(), sem);

    while(1)
    {
      Sleep(TIME_TOKEN);
      i++;
      sem_wait(sem);
      //*p = *p - 1;
      if(*flag_rx == 1)
      {
        sprintf(buf,"TOKEN Num.: %d", i);
        // Envía un mensaje
        if( sendto( sock, buf, strlen(buf), 0, &name, sizeof(name)) == -1 )
        {
          perror("sendtoTX");
        }
        *flag_rx = 0;
      }
      else
        printf("\t\tError: Flag_RX == 0, el token se perdio\n");
      sem_post(sem);
      Sleep(TIMEOUT);
      printf("\t\tHIJO CICLO %d - Seccion NO CRITICA (Despues SEM_POST)\n", i);
    }

    printf("Presione Enter para finalizar proceso %d: \n", getpid());
    getchar();
    close( sock );
  }

  munmap(sem, sizeof(*sem));
  munmap(p, sizeof(*p));

  return 0;
}
