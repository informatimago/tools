/*****************************************************************************
FILE:               unsparse.c
LANGUAGE:           C
SYSTEM:             UNIX
USER-INTERFACE:     CLI
DESCRIPTION
    
    This tool unsparses a file. That is it writes all blocks in the file
    that are full of 0 bytes so as to allocate them.

    Usage:
        unsparse file...
    
AUTHORS
    <PJB> Pascal Bourguignon
MODIFICATIONS
    2003-03-02 <PJB> 
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 2003 - 2011
    mailto:pjb@informatimago.com
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    2 of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public
    License along with this program; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA 02111-1307 USA
*****************************************************************************/

/* #define _LARGEFILE64_SOURCE 1 */
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#ifdef Linux
#include <sys/vfs.h>
#endif
#include <unistd.h>


    static const char* pname="unsparse";


    static int bread(int fd,off_t position,unsigned char* buffer,size_t* size)
    {
        int    error_count=0;
        size_t total_read_size;
        ssize_t read_size;
        off_t  new_position;
        size_t remain_to_read;
        unsigned char* remain_buffer;
    again:
        total_read_size=0;
        remain_to_read=(*size);
        remain_buffer=buffer;
        new_position=lseek(fd,position,SEEK_SET);
        if(((off_t)-1)==new_position){
            perror("lseek");
            goto abort;
        }
    rest:
        read_size=read(fd,remain_buffer,remain_to_read);
        if(read_size==0){
            /* Should be EOF */
            (*size)=total_read_size;
            return(1);
        }else if(read_size<0){
            switch(errno){
            case EIO:
            case EAGAIN:
            case EINTR:
                if(error_count>10){
                    perror("read");
                    goto abort;
                }
                error_count++;
                goto again;
            default:
                perror("read");
                goto abort;
            }
        }else{
            total_read_size=(size_t)((ssize_t)total_read_size+read_size);
            remain_to_read=(size_t)((ssize_t)remain_to_read-read_size);
            remain_buffer+=read_size;
            if(remain_to_read<=0){
                (*size)=total_read_size;
                return(1);
            }
            goto rest;
        }
    abort:
        (*size)=0;
        return(0);
    }/*bread*/



    static int bwrite(int fd,off_t position,unsigned char* buffer,size_t* size)
    {
        int    error_count=0;
        size_t total_write_size;
        ssize_t write_size;
        off_t  new_position;
        size_t remain_to_write;
        unsigned char* remain_buffer;
    again:
        total_write_size=0;
        remain_to_write=(*size);
        remain_buffer=buffer;
        new_position=lseek(fd,position,SEEK_SET);
        if(((off_t)-1)==new_position){
            perror("lseek");
            goto abort;
        }
    rest:
        write_size=write(fd,remain_buffer,remain_to_write);
        if(write_size==0){
            /* Should not occur. */
            error_count++;
            if(error_count>10){
                perror("write");
                goto abort;
            }else{
                goto again;
            }
        }else if(write_size<0){
            switch(errno){
            case EIO:
            case EAGAIN:
            case EINTR:
                error_count++;
                if(error_count>10){
                    perror("write");
                    goto abort;
                }else{
                    goto again;
                }
            default:
                perror("write");
                goto abort;
            }
        }else{
            total_write_size=(size_t)((ssize_t)total_write_size+write_size);
            remain_to_write=(size_t)((ssize_t)remain_to_write-write_size);
            remain_buffer+=write_size;
            if(remain_to_write<=0){
                (*size)=total_write_size;
                return(1);
            }
            goto rest;
        }
    abort:
        (*size)=0;
        return(0);
    }/*bwrite*/


    static void unsparse(char* fname)
    {
        char* fdir=dirname(fname);
        struct statfs s;
        int res=statfs(fdir,&s);
        size_t size;
        off_t  position;
        off_t  eof;
        int    fd;
        unsigned char* buffer;

        printf("Processing %s...\n",fname);
        if(res<0){
            perror("statfs");
            size=512;
        }else{
            size=(size_t)s.f_bsize;
        }

        fd=open(fname,O_RDWR/*|O_LARGEFILE*/,0);
        if(fd<0){
            perror("open");
            return;
        }
        eof=lseek(fd,(off_t)0,SEEK_END);
        if(((off_t)-1)==eof){
            close(fd);
            perror("lseek eof");
            return;
        }

        buffer=(unsigned char*)malloc(size);
        if(buffer==0){
            perror("malloc");
            return;
        }

        position=0;
        while(position<eof){
            unsigned char or=0;
            size_t i;
            size_t read_size=size;
            int res2=bread(fd,position,buffer,&read_size);
            if(res2==0){
                goto abort;
            }
            for(i=0;i<read_size;i++){
                or|=buffer[i];
            }
            if(!or){
                size_t write_size=read_size;
                res2=bwrite(fd,position,buffer,&write_size);
                if(res2==0){
                    goto abort;
                }
            }
            position=(off_t)((size_t)position+size);
        }
    abort:
        free(buffer);
        close(fd);
    }/*unsparse*/



    int main(int argc,char** argv)
    {
        pname=argv[0];
        argv++;
        while(*argv){
            unsparse(*argv);
            argv++;
        }
        return(0);
    }/*main*/


/*** unsparse.c                       -- 2003-12-02 12:06:45 -- pascal   ***/
