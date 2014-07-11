/*******************************************************************************

crc32.c by elektron (Modified for internal use by Thomas König)

  Compile:

    # gcc -lz -o crc32 crc32.c

  Usage:
    # ./crc32 -h

    crc32 by elektron
      crc32 [-h] file1 [fileN...]


  Notes:
    This is a hack from someone elses code :
     http://www.linuxquestions.org/questions/showthread.php?s=&forumid=1&threadid=199640

    Find the most current version of this application at:
         http://kremlor.net/projects/crc32


*******************************************************************************/

#define VERSION "v.0.1.1"

#include <errno.h>
#include <stdio.h>
#include <zlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>


#define _GNU_SOURCE
#include <getopt.h>

int usage( char *progname )
{
  printf( "\n%s %s by elektron\n", progname, VERSION);
  printf( "  %s [-h] file1 [fileN...]\n\n", progname );
}

int getFileSize( char *filename, unsigned long *size )
{
  FILE *input_file;
  unsigned long file_size;

  input_file = fopen( filename, "r" );
  if( input_file == NULL )
  {
    printf( "Could not open %s for reading.\n", filename );
    return -1;
  }

  fseek( input_file, 0, SEEK_END );
  file_size = ftell( input_file );
  rewind( input_file);

  fclose( input_file );

  memcpy(size, &file_size, sizeof(file_size));

  return 0;

}

int computeCRC32( char *filename, unsigned long *crcaddr )
{
  int input_fd;
  char *file_buffer;
  unsigned long file_size;
  unsigned long chunk_size=1024;
  unsigned long bytes_read;
  unsigned long crc=0L;

  if ( getFileSize( filename, &file_size ) != 0)
  {
    printf( "error determining file size\n" );
    return -1;
  }

  if( file_size == 0 )
  {
    printf( "File size is calculated to be 0. Cannot calculate CRC\n" );
    return -1;
  }

  //printf( "size:0x%08X\n", file_size );

  file_buffer = (char *)malloc( chunk_size );
  if( file_buffer == NULL )
  {
    printf( "Unable to reserve enough memory for file buffer.\n" );
    return -2;
  }
  crc = crc32( 0L, Z_NULL, 0 );

  input_fd = open( filename, O_RDONLY );
  if( input_fd == -1 )
  {
    printf( "Could not open %s for reading.\n", filename );
    free( file_buffer );
    return -3;
  }
  while( 1 ){

   bytes_read = read( input_fd, file_buffer, chunk_size );

   if (bytes_read == 0 ) {
 	break;
   }
   if (bytes_read == -1 ) {
	perror("read");
	free( file_buffer );
	return -4;
   }

   crc = crc32( crc, file_buffer, bytes_read );

  }

  free( file_buffer );

  close( input_fd );

  memcpy(crcaddr, &crc, sizeof(crc));

  return 0;
}

int main( int argc, char *argv[] )
{
  unsigned long crc=0L;
  extern char *optarg;
  extern int optind, opterr, optopt;
  int i,e=0;

  while ( getopt( argc, argv, "h" ) != -1 ) {
  //	printf("optarg = %s, optind = %d, opterr = %d, optopt = %d\n",
  //		optarg, optind, opterr, optopt);
  	if ( strncmp( argv[optind-1], "-h", 2 ) == 0 ) {
		usage( basename(argv[0]) );
		return 0;
	}
  }
 //printf("optarg = %s, optind = %d, opterr = %d, optopt = %d, argc = %d\n",
 //	optarg, optind, opterr, optopt, argc);


  if( argc < optind+1 )
  {
    usage( basename(argv[0]) );
    return 2;
  }
  for ( i=optind; i<argc; i++)
  {
    if ( computeCRC32(argv[i], &crc ) != 0 )
    {
      e=1;
    }
    else
    {
      printf("%08X", crc, argv[i] );
    }
  }
  return e;
}
