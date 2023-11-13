/*
 * open-read.c
 *
 * Simple example of opening and reading to a file.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[])
{
   /*
    *
    * the attributes are:
    * - O_RDONLY: open the file for reading
    *
    */
   FILE *fd = fopen("testeFixed.txt", "r");
   FILE *fd1 =fopen("test-out.txt","w+");
   if (fd == NULL){
      fprintf(stderr, "open error: %s\n", strerror(errno));
      return -1;
   }

   char buffer[128];
   memset(buffer,0,sizeof(buffer));

   /* read the contents of the file */
   int bytes_read;
   int bytes_written;
   while((bytes_read=fread(buffer,sizeof(char),127,fd))>0) {
      if (bytes_read < 0){
         fprintf(stderr, "read error: %s\n", strerror(errno));
         return -1;
      }
      bytes_written = fwrite(buffer, sizeof(char),bytes_read,fd1);
      if (bytes_written < 0){
         fprintf(stderr, "write error: %s\n", strerror(errno));
         return -1;
      }
      memset(buffer,0,sizeof(buffer));
   }
   /* close the file */
   fclose(fd);
   fclose(fd1);
   return 0;
}