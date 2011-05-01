/*****************************************************************************
FILE:               random_stat.cc
LANGUAGE:           c++
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
    XXX
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    1999-05-27 <PJB> Created.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1999 - 2011
    
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
    
extern "C"{
#include <stdio.h>
#include <values.h>
}
#define BIT_PER_BYTE    (CHARBITS)

// typedef char bool;
#define true ((bool)(0==0))
#define false ((bool)(0!=0))

class BitFile
{
public:
    BitFile(void);
    virtual ~BitFile(void);
    
    virtual bool isOpen(void);
    virtual bool isAtEof(void);
    virtual void open(const char* fname,const char* mode);
    virtual void assign(FILE* file);
    virtual short getBit(void);
    virtual void close(void);

    /*
      virtual long read(void* bitbuf,short bitoffset,long bitcount);
      virtual long write(const void* bitbuf,short bitoffset,long bitcount);
    */

protected:
    FILE*           _file;
    unsigned long   _buffer;
    unsigned long   _bitMask;
    unsigned long   _currentBit;
    bool            _fileIsMine;
    bool            _isOpen;
};//BitFile;


BitFile::BitFile(void)
{
    _file=NULL;
    _buffer=0;
    _bitMask=0;
    _currentBit=0;
    _fileIsMine=false;
    _isOpen=false;
}//BitFile;
    
    
BitFile::~BitFile(void)
{
    close();
}//~BitFile;
    
    
    
bool BitFile::isOpen(void)
{
    return(_isOpen);
}//isOpen;
    
    
bool BitFile::isAtEof(void)
{
    return((!_isOpen)||(feof(_file)&&(_currentBit==0)));
}//isAtEof;
    
    
void BitFile::open(const char* fname,const char* mode)
{
    close();
    _file=fopen(fname,mode);
    _fileIsMine=(_file!=NULL);
    _isOpen=(_file!=NULL);
    _bitMask=0;
    _currentBit=0;
}//open;
    
    
void BitFile::assign(FILE* file)
{
    close();
    _file=file;
    _fileIsMine=false;
    _isOpen=true;
    _bitMask=0;
    _currentBit=0;
}//assign;
    
    
short BitFile::getBit(void)
{
    if(_isOpen){
        if(_currentBit==0){
            if(feof(_file)){
                return(-1);
            }else{
                // 111111111111 32
                // 111111111110 31
                // 111111111100 30
                // 111111111000 29
                // ...
                // 110000000000  2
                // 100000000000  1
                // 000000000000  0
                _bitMask=(~0)<<(BIT_PER_BYTE*(sizeof(_buffer)-fread(&_buffer,1,sizeof(_buffer),_file)));
                if(_bitMask==0){
                    // DEBUG printf("\n_bitMask   =%0lx\n",_bitMask);
                    // DEBUG printf("\n_currentBit=%0lx\n",_currentBit);
                    _currentBit=0;
                    return(-1);
                }else{
                    _currentBit=((unsigned long)1)<<(BIT_PER_BYTE*sizeof(_buffer)-1);
                }
            }
        }
        // DEBUG printf("\n");
        // DEBUG printf("_bitMask   =%08lx\n",_bitMask);
        // DEBUG printf("_currentBit=%08lx\n",_currentBit);
        // DEBUG printf("_buffer    =%08lx\n",_buffer);
        if(_currentBit&_buffer){
            _currentBit=(_currentBit>>1)&_bitMask;
            return(1);
        }else{
            _currentBit=(_currentBit>>1)&_bitMask;
            return(0);
        }
    }else{
        return(-1);
    }
}//getBit;
    
    
void BitFile::close(void)
{
    if(_isOpen){
        fclose(_file);
        _isOpen=false;
        _fileIsMine=false;
    }
}//close;
    

/*
  Compter:
  Les sequences de 1 classees par taille
  Les sequences de 0 classees par taille      
*/
class SequenceCounter
{
public:
    SequenceCounter(void);
    virtual ~SequenceCounter(void);
    virtual void reset(long maxlength);
    virtual void process(short bit);
    virtual void print(FILE* out);
protected:
    short   _lastBit;
    long    _currentLength;
    long    _maxLength;
    long*   _lengthCounts[2];
};//SequenceCounter;

    
SequenceCounter::SequenceCounter(void)
{
    _lastBit=-1;
    _currentLength=0;
    _maxLength=0;
    _lengthCounts[0]=NULL;
    _lengthCounts[1]=NULL;
}//SequenceCounter;
    
    
SequenceCounter::~SequenceCounter(void)
{
    if(_lengthCounts[0]!=NULL){
        delete _lengthCounts[0];
        delete _lengthCounts[1];
    }
}//~SequenceCounter;
    
    
void SequenceCounter::reset(long maxlength)
{
    int i,k;
    _lastBit=-1;
    _currentLength=0;
    _maxLength=maxlength;
    for(k=0;k<2;k++){
        if(_lengthCounts[k]!=NULL){
            delete _lengthCounts[k];
        }
        _lengthCounts[k]=new long[_maxLength];
        for(i=0;i<_maxLength;i++){
            _lengthCounts[k][i]=0;
        }
    }
}//reset;
    
    
void SequenceCounter::process(short bit)
{
    if(bit==_lastBit){
        _currentLength++;
    }else{
        if(_lastBit>=0){
            if(_currentLength>=_maxLength){
                long*   newCounts;
                int     i,k;
                for(k=0;k<2;k++){
                    newCounts=new long[_currentLength];
                    for(i=0;i<_maxLength;i++){
                        newCounts[i]=_lengthCounts[k][i];
                    }
                    for(;i<_currentLength;i++){
                        newCounts[i]=0;
                    }
                    delete _lengthCounts[k];
                    _lengthCounts[k]=newCounts;
                }
            }
            (_lengthCounts[_lastBit][_currentLength])++;
        }
        _lastBit=(bit&1);
        _currentLength=1;
    }
}//process;
    
    
void SequenceCounter::print(FILE* out)
{
    int i,m;
    m=(int)(_maxLength-1);
    while(_lengthCounts[0][m]+_lengthCounts[1][m]==0){
        m--;
    }
    fprintf(out,":-------:--------:--------:\n");
    fprintf(out,": size  : 0-seq  : 1-seq  :\n");
    fprintf(out,":-------:--------:--------:\n");
    for(i=1;i<=m;i++){
        fprintf(out,":%6d : %6ld : %6ld :\n",
                i,_lengthCounts[0][i],_lengthCounts[1][i]);
    }
    fprintf(out,":-------:--------:--------:\n");
}//print;
    
    

int main(void)
{
    BitFile*            file;
    SequenceCounter*    counter;
    
    file=new BitFile;
    file->assign(stdin);
    counter=new SequenceCounter;
    counter->reset(128);
    
    while(!(file->isAtEof())){
        counter->process(file->getBit());
    }
    
    counter->print(stdout);
    
    return(0);
}/*main*/


/**** THE END ****/
