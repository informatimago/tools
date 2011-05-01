extern "C"{
#include <stdio.h>
}
#include <BcTypes.h>


class File
{
public:
    File(void);
    virtual ~File(void);
    virtual void open(const char* fname,const char* mode);
    virtual void assign(FILE* file);
    virtual long int read(void* buffer,long int size);
    virtual void close(void);
    virtual short int isOpen(void);
    virtual short int isAtEnd(void);
protected:
    FILE*           _file;
    short int       _weOpened;
};//file;


    File::File(void)
    {
        _file=NULL;
        _weOpened=0;
    }//File;
    
    
    File::~File(void)
    {
        if(_weOpened){
            close();
        }
    }//~File;
    
    
    void File::open(const char* fname,const char* mode)
    {
        if(_weOpened){
            close();
        }
        _file=fopen(fname,mode);
        if(_file!=NULL){
            _weOpened=1;
        }
    }//open;
    
    
    void File::assign(FILE* file)
    {
        if(_weOpened){
            close();
        }
        _file=file;
        _weOpened=0;
    }//assign;
    
    
    long int File::read(void* buffer,long int size)
    {
        if(isOpen()){
            return(fread(buffer,1,size,_file));
        }else{
            return(0);
        }
    }//read;
    
    
    void File::close(void)
    {
        if(isOpen()){
            fclose(_file);
            _file=NULL;
            _weOpened=0;
        }
    }//close;
    
    
    short int File::isOpen(void)
    {
        return(_file!=NULL);
    }//isOpen;
    
    
    short int File::isAtEnd(void)
    {
        return(isOpen()&&feof(_file));
    }//isAtEnd;
    
    

class CharCount
{
public:
    CharCount(void);
    virtual ~CharCount(void);
    virtual void reset(void);
    virtual void countFile(File* file);
    virtual long int total(void);
    virtual long int countOf(unsigned char c);
protected:
    long int    _count[256];
};//CharCount;


    CharCount::CharCount(void)
    {
        reset();
    }//CharCount;

    
    CharCount::~CharCount(void)
    {
        // NOP.
    }//~CharCount;
    

    void CharCount::reset(void)
    {
            int c;
        for(c=0;c<256;c++){
            _count[c]=0;
        }
    }//reset;
    

#define buffer_size (4096)

    void CharCount::countFile(File* file)
    {
            int             size;
            unsigned char   buffer[buffer_size];
            int             i;

        if(file->isOpen()){     
            while(!(file->isAtEnd())){
                size=(int)(file->read(buffer,buffer_size));
                for(i=0;i<size;i++){
                    _count[buffer[i]]++;
                }
            }
        }
    }//countFile;
    
    
    long int CharCount::total(void)
    {
            int c;
            long int t=0;
        for(c=0;c<256;c++){
            t+=_count[c];
        }
        return(t);
    }//total;
    
    
    long int CharCount::countOf(unsigned char c)
    {
        return(_count[c]);
    }//countOf;
    
    

    const char* charName(unsigned char c)
    {
            static const char*  asciinames[]={
                    "NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL",
                    "BS ","HT ","LF ","VT ","FF ","CR ","SO ","SI ",
                    "DLE","DC1","DC2","DC3","DC4","NAK","SYN","ETB",
                    "CAN","EM ","SUB","ESC","FS ","GS ","RS ","US ",
                    "SP ","DEL",
                    NULL};
            static char     buffer[8];
        if(c<33){
            return(asciinames[c]);
        }else if(c==127){
            return(asciinames[33]);
        }else if((128<=c)&&(c<160)){
            sprintf(buffer,"M-%s",asciinames[c-128]);
            return(buffer);
        }else{
            sprintf(buffer,"'%c'",c);
            return(buffer);
        }
    }//charName;
    

int main(int argc,const char** argv)
{
        CharCount*      count;
        File*           file;
        int             i;
        
    count=new CharCount;
    file=new File;
    if(argc<2){
        file->assign(stdin);
        count->countFile(file);
    }else{
        for(i=1;i<argc;i++){
            file->open(argv[i],"r");
            count->countFile(file);
            file->close();
        }
    }
    delete file;
    for(i=0;i<256;i++){
        fprintf(stdout,"%02x %-8s %10ld\n",i,charName((unsigned char)i),count->countOf((unsigned char)i));
    }
    return(0);  
}/*main*/

/**** THE END ****/
