#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* MARK NAME Leandro Marques Venceslau de Souza */
/* MARK NAME Nome */

/****************************************************************
 * Shell xv6 simplificado
 *
 * Este codigo foi adaptado do codigo do UNIX xv6 e do material do
 * curso de sistemas operacionais do MIT (6.828).
 ***************************************************************/

#define MAXARGS 10

/* Todos comandos tem um tipo.  Depois de olhar para o tipo do
 * comando, o código converte um *cmd para o tipo específico de
 * comando. */
struct cmd {
  int type; /* ' ' (exec)
               '|' (pipe)
               '<' or '>' (redirection) */
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // argumentos do comando a ser executado
};

struct redircmd {
  int type;          // < ou > 
  struct cmd *cmd;   // o comando a rodar (ex.: um execcmd)
  char *file;        // o arquivo de entrada ou saída
  int mode;          // o modo no qual o arquivo deve ser aberto
  int fd;            // o número de descritor de arquivo que deve ser usado
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // lado esquerdo do pipe
  struct cmd *right; // lado direito do pipe
};

int fork1(void);  // Fork mas fechar se ocorrer erro.
struct cmd *parsecmd(char*); // Processar o linha de comando.

/* Executar comando cmd.  Nunca retorna. */
void
runcmd(struct cmd *cmd)
{
  int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "tipo de comando desconhecido\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(0);
    /////////////////////////////////////////////////////////
    // * Task 2                                           //
    // * Implemente abaixo um código capaz de            //
    // * executar comandos simples.                     //
    /////////////////////////////////////////////////////

    // We can execute simple commands using execvp()
    // int execvp (
    //   const char *file,
    //   char *const argv[]
    // );

    // i.e. the following code is equivalent to the ls -l command

    // char* arg[] = {"ls", "-l", NULL};
    // execvp(arg[0],arg);

    // More info on : 
    // https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/e/execvp.html

    /* MARK START task2 */

    // Create a child process
    r = fork1();
    // Exit indicating an error, in case the fork failed
    if(r < 0) {
      // The perror() function produces a message on standard error 
      // describing the last error encountered during a call to a 
      // system or library function.
      perror("Fork error on EXEC..");
      exit(1);
    }
    // Test to see if the child process was created
    else if (r == 0){
      // Execute the current command entered in the bash shell
      int exe;
      exe = execvp(ecmd->argv[0], ecmd->argv);

      if (exe < 0) {
        perror("Exec error..");
        exit(1);
      }
    }
    // Otherwise, suspend the calling process until the 
    // child process ends or is stopped
    else {
      int exitStatus;
      waitpid(r, &exitStatus, 0);
    }

    /* MARK END task2 */
    break;

  case '>':
  case '<':
    rcmd = (struct redircmd*)cmd;
    /////////////////////////////////////////////////////////
    // * Task 3                                           //
    // * Implemente codigo abaixo para executar          //
    // * comando com redirecionamento.                  //
    /////////////////////////////////////////////////////

    /* MARK START task3 */

    close(rcmd->fd);

    int fileReadWrite;
    fileReadWrite = open(rcmd->file,rcmd->mode, 0666);

    if(fileReadWrite < 0){
      perror("Redirect error..");
      exit(1);
    }

    /* MARK END task3 */
    runcmd(rcmd->cmd);
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    /////////////////////////////////////////////////////////
    // * Task 4                                           //
    // * Implemente codigo abaixo para executar          //
    // * comando com pipes.                             //
    /////////////////////////////////////////////////////

    /* MARK START task4 */

    // TO DO !!!

    /* MARK END task4 */
    break;
  }    
  exit(0);
}

/* 
 * Function responsible for reading what was entered in the shell
 */
int
getcmd(char *buf, int nbuf)
{
  // isatty() tests wheter a file descriptor refers to a terminal.
  // Returns 1 if fd is an open file descriptor referring to a terminal;
  // otherwise 0 is returned, and errno is set to indicate the error.

  // fileno() returns the integer value of the file descriptor associated
  // with stream. Otherwise, the value -1 is returned and errno is set to
  // indicate the error.

  // IF there is a file descriptor associated what was entered in the stdin
  // AND IF said file descriptor refers to a terminal :
  if (isatty(fileno(stdin))) {
    // Print "$ ", indicatitng that a new command can be entered in the shell
    fprintf(stdout, "$ ");
  }

  // REMINDER : A buffer is the region of the memory that stores data that was read.

  // For a buffer of size nbuf, set all of its contents to 0.
  memset(buf, 0, nbuf);

  // fgets() reads the content of a file :

  // char* fgets (
  //   char *str,    => string that will be read
  //   int size,     => how many characters must be read
  //   FILE *fp      => where to read the string from
  // );
  
  // Put the content that was entered in stdin inside the buffer :
  fgets(buf, nbuf, stdin);

  if(buf[0] == 0) { // EOF
    return -1;
  }

  return 0;
}

int
main(void)
{
  static char buf[100];
  int r;

  // Ler e rodar comandos.
  while(getcmd(buf, sizeof(buf)) >= 0){
    /////////////////////////////////////////////////////////////////////
    // * Task 1                                                       //
    // * O que faz o if abaixo e por que ele é necessário?           //
    // * Insira sua resposta no código e modifique o fprintf abaixo //
    // * para reportar o erro corretamente                         //
    ////////////////////////////////////////////////////////////////

    // O if abaixo verifica se no diretório atual existe uma pasta
    // com o nome passado como parâmetro do comando cd. Caso não
    // seja o caso, o shell imprime um erro e continua executando.

    /* MARK START task1 */
    
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      buf[strlen(buf)-1] = 0;
      if(chdir(buf+3) < 0)
        fprintf(stderr, "Não existe uma pasta com esse nome no diretório atual.\n");
      continue;
    }

    /* MARK END task1 */

    if(fork1() == 0) {
      runcmd(parsecmd(buf));
    }

    wait(&r);
  }
  exit(0);
}

/*
 * The fork() function purpose is to create a new process, which becomes the child 
 * process of the caller .Both processes will execute the next instruction 
 * following the fork() system call. Two identical copies of the computer's address
 * space,code, and stack are created one for parent and child. Thinking of the fork
 * as it was a person; Forking causes a clone of your program (process), that is 
 * running the code it copied.
 */
int
fork1(void)
{
  int pid;
  pid = fork();

  if(pid == -1) {
    perror("fork");
  }

  return pid;
}

/****************************************************************
 * Funções auxiliares para criar estruturas de comando
 ***************************************************************/

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

/****************************************************************
 * Processamento da linha de comando
 ***************************************************************/

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

/*
 * Tokens are the different parts that constitute a command.
 * i.e. ps -ael
 *      args[0] = 'ps'
 *      args[1] = '-ael'
 *      args[2] = NULL
 */
int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s; // This is a form of declaring a string
  int ret;
  s = *ps;

  while(s < es && strchr(whitespace, *s)) {
    s++;
  }
  if(q) {
    *q = s;
  }

  ret = *s;

  switch(*s){
  case 0:
    break;
  case '|':
  case '<':
    s++;
    break;
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq) {
    *eq = s;
  }
  while(s < es && strchr(whitespace, *s)) {
    s++;
  }

  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;
  s = *ps;

  while(s < es && strchr(whitespace, *s)) {
    s++;
  }

  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

/* Copiar os caracteres no buffer de entrada, começando de s ate es.
 * Colocar terminador zero no final para obter um string valido. */
char 
*mkcopy(char *s, char *es)
{
  int n = es - s;
  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;
  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;
  
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}

// vim: expandtab:ts=2:sw=2:sts=2

