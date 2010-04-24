/******************************************************************************
FILE:               BinHex.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This module can encode and decode a binhex file.
AUTHOR
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992/05/13 <PJB> Creation.
    1993/04/28 <PJB> Updated.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1993
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <strings.h>
#include <BcInterface.h>
#include <BcTypes.h>
#include <BinHex.h>
#include <BcImplementation.h>

//
// Seven-bit stream
//
    
    typedef enum {Seven_OpNone,Seven_OpRead,Seven_OpWrite} Seven_OperationT;
    typedef struct {
        Seven_OperationT        operation;
        BinHex_GetPr            get;        // operation=Seven_OpRead
        BinHex_PutPr            put;        // operation=Seven_OpWrite
        void*                   stream;
        INT16                   lastok;     // index of last byte ok    
        INT16                   index;      // of next 8-bit byte to read/write
            // operation=Seven_OpWrite =>  index=2 => put will be called.
            // operation=Seven_OpRead  =>  index=0 => get will be called.
        CHAR                    buffer[4];
    }                       Seven_StreamT;
    typedef Seven_StreamT*  Seven_StreamP;

/*
    the syntax of Seven use is:
        csrun       :   writerun | readrun ;
        writerun    :   Seven_Rewrite(csi,get,s) 
                            { Seven_get(csi,byte) } Seven_Close(csi) ;
        readrun:    :   Seven_Reset(csi,get,s)
                            { Seven_Get(csi,&byte) } Seven_Close(csi) ;
*/
    
    PROCEDURE(Seven_Reset,
            (Seven_StreamP csi,BinHex_GetPr get,void* stream),void)
        /*
            POST:       csi is a valid stream for use with Seven_Get.
        */
    {
        csi->operation=Seven_OpRead;
        csi->get=get;
        csi->stream=stream;
        csi->index=0;
        csi->lastok=-1;
    }//Seven_Reset
    
    CHAR    Seven_Table[65]=
        "!\"#$%&\'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";

    PROCEDURE(CharTo6bit,(CHAR c),CARD8)
        /*
            RETURN:     64 if c is not a valid encoded value,
                        [0..63] the value encoded in c.
        */
    {
            CARD16      index;
        index=0;
        while((index<64)&&(Seven_Table[index]!=c)){
            INC(index);
        }
        return((CARD8)index);
    }//CharTo6bit
    
    PROCEDURE(CharFrom6bit,(CARD8 bits),CHAR)
        /*
            PRE:        bits:[0..63];
            RETURN:     the encoded CHAR representing bits.
        */
    {
        return(Seven_Table[bits]);
    }//CharFrom6bit
    
    PROCEDURE(Seven_Get,(void* stream,CHAR* byte),BOOLEAN)
        /*
            DO:         read a byte from stream->stream with stream->get, 
                        update the stream->checksum and return the read byte.
            RETURN:     same result as stream->get.
        */
    {
            BOOLEAN                 ok;
            Seven_StreamP       csi;
        
        csi=(Seven_StreamP)stream;
        if(csi->operation!=Seven_OpRead){
            return(FALSE);
        }
        if(csi->index==0){
            do{
                    CHAR        c;
                    
                if(!csi->get(csi->stream,&c)){ csi->lastok=-1; continue; }
                csi->buffer[0]=CharTo6bit(c);
                if(!csi->get(csi->stream,&c)){ csi->lastok=0; continue; }
                csi->buffer[1]=CharTo6bit(c);
                if(!csi->get(csi->stream,&c)){ csi->lastok=1; continue; }
                csi->buffer[2]=CharTo6bit(c);
                if(!csi->get(csi->stream,&c)){ csi->lastok=2; continue; }
                csi->buffer[3]=CharTo6bit(c);
                csi->lastok=3;
            }while(FALSE);//once
        }
/*
    4 6-bit bytes   ->      3 8-bit bytes
    -------------           --------------
    lastok          ->      index : [0..2]
        -1                      < -1    none
         0                      <  0    none
         1                      <  1    [0..0]
         2                      <  2    [0..1]
         3                      <  3    [0..2]
*/
        if(csi->index<csi->lastok){
            switch(csi->index){
            case 0:
                (*byte)=((csi->buffer[0]&0x3f)<<2)
                            |((csi->buffer[1]&0x30)>>4);
                break;
            case 1:
                (*byte)=((csi->buffer[1]&0x0f)<<4)
                            |((csi->buffer[2]&0x3c)>>2);
                break;
            case 2:
                (*byte)=((csi->buffer[2]&0x03)<<6)
                            |(csi->buffer[3]&0x3f);
                break;
            default:
                break;
            }
            INC(csi->index);
            if(csi->index>2){
                csi->index=0;
            }
            return(TRUE);
        }else{
            return(FALSE);
        }
    }//Seven_Get
        
    PROCEDURE(Seven_Rewrite,
            (Seven_StreamP csi,BinHex_PutPr put,void* stream),void)
        /*
            POST:       csi is a valid stream for use with Seven_Put.
        */
    {
        csi->operation=Seven_OpWrite;
        csi->put=put;
        csi->stream=stream;
        csi->index=0;
        csi->lastok=-1;
    }//Seven_Rewrite
    
    PROCEDURE(Seven_Put,(void* stream,CHAR byte),BOOLEAN)
        /*
            DO:         write the byte to stream->stream with stream->put, 
                        update the stream->checksum.
            RETURN:     same result as stream->put.
        */
    {
            Seven_StreamP       csi;
            INT16               index;
            CHAR                c;
            
        csi=(Seven_StreamP)stream;
        if(csi->operation!=Seven_OpWrite){
            return(FALSE);
        }
        csi->buffer[index]=byte;
        INC(csi->index);
        if(csi->index>=3){
            csi->index=0;
            c=CharFrom6bit((csi->buffer[0]&0xfc)>>2);
            if(!csi->put(stream,c)){ return(FALSE); }
            c=CharFrom6bit(((csi->buffer[0]&0x03)<<4)
                            |((csi->buffer[1]&0xf0)>>4));
            if(!csi->put(stream,c)){ return(FALSE); }
            c=CharFrom6bit(((csi->buffer[1]&0x0f)<<2)
                            |((csi->buffer[2]&0xc0)>>6));
            if(!csi->put(stream,c)){ return(FALSE); }
            c=CharFrom6bit(csi->buffer[2]&0x3f);
            if(!csi->put(stream,c)){ return(FALSE); }
        }
        return(TRUE);       
    }//Seven_Put
    
    PROCEDURE(Seven_Close,(void* stream),BOOLEAN)
        /*
            DO:         closes the Seven stream; if the stream was open 
                        for writting and the output buffer is not empty
                        then fill the buffer with 0s and put it.
            RETURN:     if the stream was open for reading then
                            the input buffer is empty,
                        else
                            the result of put, or TRUE.
                        end
        */
    {
            Seven_StreamP       csi;
            CHAR                c;
            
        csi=(Seven_StreamP)stream;
        switch(csi->operation){
        case Seven_OpWrite:
            csi->operation=Seven_OpNone;
            if(csi->index>0){
                while(csi->index<3){
                    csi->buffer[csi->index]=0;
                    INC(csi->index);
                }
                csi->index=0;
                c=CharFrom6bit((csi->buffer[0]&0xfc)>>2);
                if(!csi->put(stream,c)){ return(FALSE); }
                c=CharFrom6bit(((csi->buffer[0]&0x03)<<4)
                                |((csi->buffer[1]&0xf0)>>4));
                if(!csi->put(stream,c)){ return(FALSE); }
                c=CharFrom6bit(((csi->buffer[1]&0x0f)<<2)
                                |((csi->buffer[2]&0xc0)>>6));
                if(!csi->put(stream,c)){ return(FALSE); }
                c=CharFrom6bit(csi->buffer[2]&0x3f);
                if(!csi->put(stream,c)){ return(FALSE); }
            }
            return(TRUE);
        case Seven_OpRead:
            csi->operation=Seven_OpNone;
            return(csi->buffer==0);
        default:
            csi->operation=Seven_OpNone;
            return(FALSE);
        }
    }//Seven_Close
    
//
// RunLength encoding
//

    typedef enum {RunLength_OpNone,RunLength_OpRead,RunLength_OpWrite} 
                RunLength_OperationT;
                
    typedef struct {
        RunLength_OperationT    operation;
        BinHex_GetPr            get;    // operation=RunLength_OpRead
        BinHex_PutPr            put;    // operation=RunLength_OpWrite
        void*                   stream;
        CARD8                   length; // current length of run    
        CHAR                    byte;   // current CHAR
    }                       RunLength_StreamT;
    typedef RunLength_StreamT*  RunLength_StreamP;

/*
    the syntax of RunLength use is:
        csrun       :   writerun | readrun ;
        writerun    :   RunLength_Rewrite(csi,get,s) 
                            { RunLength_get(csi,byte) } RunLength_Close(csi) ;
        readrun:    :   RunLength_Reset(csi,get,s)
                            { RunLength_Get(csi,&byte) } RunLength_Close(csi) ;
*/
    
    PROCEDURE(RunLength_Reset,
            (RunLength_StreamP csi,BinHex_GetPr get,void* stream),void)
        /*
            POST:       csi is a valid stream for use with RunLength_Get.
        */
    {
        csi->operation=RunLength_OpRead;
        csi->get=get;
        csi->stream=stream;
        csi->length=0;
        csi->byte=0;
    }//RunLength_Reset
        
    PROCEDURE(RunLength_Get,(void* stream,CHAR* byte),BOOLEAN)
        /*
            DO:         read a byte from stream->stream with stream->get, 
                        update the stream->checksum and return the read byte.
            RETURN:     same result as stream->get.
        */
    {
            BOOLEAN                 ok;
            RunLength_StreamP       csi;
            CHAR                    code;
            
        csi=(RunLength_StreamP)stream;
        if(csi->operation!=RunLength_OpRead){
            return(FALSE);
        }
        if(csi->length>0){
            DEC(csi->length);
            (*byte)=csi->byte;
            return(TRUE);
        }else{
            if(!csi->get(csi->stream,&code)){ return(FALSE); }
            if(code=(CHAR)0x90){
                if(!csi->get(csi->stream,&(csi->length))){ return(FALSE); }
                if(csi->length==0){
                    csi->byte=code;
                    (*byte)=code;
                    return(TRUE);
                }else{
                    DEC(csi->length);
                    (*byte)=csi->byte;
                    return(TRUE);
                }
            }else{
                csi->byte=code;
                (*byte)=code;
                return(TRUE);
            }
        }
    }//RunLength_Get
        
    PROCEDURE(RunLength_Rewrite,
            (RunLength_StreamP csi,BinHex_PutPr put,void* stream),void)
        /*
            POST:       csi is a valid stream for use with RunLength_Put.
        */
    {
        csi->operation=RunLength_OpWrite;
        csi->put=put;
        csi->stream=stream;
        csi->length=0;
        csi->byte=0;
    }//RunLength_Rewrite
    
    PROCEDURE(RunLength_Put,(void* stream,CHAR byte),BOOLEAN)
        /*
            DO:         write the byte to stream->stream with stream->put, 
                        update the stream->checksum.
            RETURN:     same result as stream->put.
        */
    {
            RunLength_StreamP       csi;
        
        csi=(RunLength_StreamP)stream;
        if(csi->operation!=RunLength_OpWrite){
            return(FALSE);
        }
        switch(csi->length){
        case 0:
            if(!csi->put(csi->stream,byte)){ return(FALSE); }
            if(byte==(CHAR)0x90){
                if(!csi->put(csi->stream,(CHAR)0x00)){ return(FALSE); }
            }
            /* going to 1 */
            csi->length=1;
            csi->byte=byte;
            break;
        case 1:
            if(byte==csi->byte){
                INC(csi->length);
                /* going to >1 */
            }else{
                /* put the new byte */
                if(!csi->put(csi->stream,byte)){ return(FALSE); }
                if(byte==(CHAR)0x90){
                    if(!csi->put(csi->stream,(CHAR)0x00)){ return(FALSE); }
                }
                /* staying in 1 */
            }
            break;
        default:
            if(byte==csi->byte){
                if(csi->length==254){
                    if(!csi->put(csi->stream,(CHAR)0x90)){ return(FALSE); }
                    if(!csi->put(csi->stream,(CHAR)255)){ return(FALSE); }
                    /* going to 0 */
                    csi->length=0;
                }else{
                    INC(csi->length);
                    /* staying in >1 */
                }
            }else{
                if((csi->byte==(CHAR)0x90)||(csi->length>3)){
                    if(!csi->put(csi->stream,(CHAR)0x90)){ return(FALSE); }
                    if(!csi->put(csi->stream,(CHAR)(csi->length))){
                        return(FALSE);
                    }
                }else{
                    do{
                        if(!csi->put(csi->stream,csi->byte)){ return(FALSE); }
                        DEC(csi->length);
                    }while(csi->length>1);
                }
                /* put the new byte */
                if(!csi->put(csi->stream,byte)){ return(FALSE); }
                if(byte==(CHAR)0x90){
                    if(!csi->put(csi->stream,(CHAR)0x00)){ return(FALSE); }
                }
                /* going to 1 */
                csi->length=1;
                csi->byte=byte;
            }
            break;
        }
        return(TRUE);
    }//RunLength_Put
/*
    9090909090
    909005
    9090
    90009000
    909002
*/
    
    PROCEDURE(RunLength_Close,(void* stream),BOOLEAN)
        /*
            DO:         closes the RunLength stream; if the stream was 
                        open for writting and the output buffer is not
                        empty then fill the buffer with 0s and put it.
            RETURN:     if the stream was open for reading then
                            the input buffer is empty,
                        else
                            the result of put, or TRUE.
                        end
        */
    {
            RunLength_StreamP       csi;
        
        csi=(RunLength_StreamP)stream;
        switch(csi->operation){
        case RunLength_OpWrite:
            csi->operation=RunLength_OpNone;
            if(csi->length>1){
                if((csi->byte==(CHAR)0x90)||(csi->length>3)){
                    if(!csi->put(csi->stream,(CHAR)0x90)){ return(FALSE); }
                    if(!csi->put(csi->stream,(CHAR)(csi->length))){
                        return(FALSE);
                    }
                }else{
                    do{
                        if(!csi->put(csi->stream,csi->byte)){ return(FALSE); }
                        DEC(csi->length);
                    }while(csi->length>1);
                }
            }
            return(TRUE);
        case RunLength_OpRead:
            csi->operation=RunLength_OpNone;
            return(csi->length==0);
        default:
            csi->operation=RunLength_OpNone;
            return(FALSE);
        }
    }//RunLength_Close


//
// CheckSum-ing stream
//


    typedef enum {CheckSum_OpNone,CheckSum_OpRead,CheckSum_OpWrite} 
                    CheckSum_OperationT;
    
    typedef struct {
        CheckSum_OperationT     operation;
        BinHex_GetPr            get;    // operation=CheckSum_OpRead
        BinHex_PutPr            put;    // operation=CheckSum_OpWrite
        void*                   stream;
        CARD16                  checksum;
    }                       CheckSum_StreamT;
    typedef CheckSum_StreamT*   CheckSum_StreamP;

/*
    the syntax of CheckSum use is:
        csrun       :   writerun | readrun ;
        writerun    :   CheckSum_Rewrite(csi,get,s) 
                            { CheckSum_get(csi,byte) } CheckSum_Write(csi) ;
        readrun:    :   CheckSum_Reset(csi,get,s)
                            { CheckSum_Get(csi,&byte) } CheckSum_Check(csi) ;
*/
    
    PROCEDURE(CheckSum_Reset,
            (CheckSum_StreamP csi,BinHex_GetPr get,void* stream),void)
        /*
            POST:       csi is a valid stream for use with CheckSum_Get.
        */
    {
        csi->operation=CheckSum_OpRead;
        csi->get=get;
        csi->stream=stream;
        csi->checksum=0;
    }//CheckSum_Reset
    
    PROCEDURE(CheckSum_Get,(void* stream,CHAR* byte),BOOLEAN)
        /*
            DO:         read a byte from stream->stream with stream->get, 
                        update the stream->checksum and return the read byte.
            RETURN:     same result as stream->get.
        */
    {
            BOOLEAN                 ok;
            CheckSum_StreamP        csi;
        
        csi=(CheckSum_StreamP)stream;
        if(csi->operation!=CheckSum_OpRead){
            return(FALSE);
        }
        ok=csi->get(csi->stream,byte);
        if(ok){
            csi->checksum=(CARD16)(((CARD32)(csi->checksum)
                                    +(CARD32)(*byte))%65536);
        }
        return(ok);
    }//CheckSum_Get
    
    PROCEDURE(CheckSum_Check,(void* stream),BOOLEAN)
        /*
            DO:         get two more bytes from the stream, and compare
                        them to the check sum
            RETURN:     CheckSum_ is ok.
        */
    {
            CARD16                  sum;
            CHAR*                   ptr;
            CheckSum_StreamP        csi;
        
        csi=(CheckSum_StreamP)stream;
        if(csi->operation!=CheckSum_OpRead){
            return(FALSE);
        }
        csi->operation=CheckSum_OpNone;
        ptr=(CHAR*)&sum;
        if(!csi->get(stream,&(ptr[0]))){ return(FALSE); }
        if(!csi->get(stream,&(ptr[1]))){ return(FALSE); }
        return(sum==csi->checksum);
    }//CheckSum_Check
    
    PROCEDURE(CheckSum_Rewrite,
            (CheckSum_StreamP csi,BinHex_PutPr put,void* stream),void)
        /*
            POST:       csi is a valid stream for use with CheckSum_Put.
        */
    {
        csi->operation=CheckSum_OpWrite;
        csi->put=put;
        csi->stream=stream;
        csi->checksum=0;
    }//CheckSum_Rewrite
    
    PROCEDURE(CheckSum_Put,(void* stream,CHAR byte),BOOLEAN)
        /*
            DO:         write the byte to stream->stream with stream->put, 
                        update the stream->checksum.
            RETURN:     same result as stream->put.
        */
    {
            CheckSum_StreamP        csi;
        
        csi=(CheckSum_StreamP)stream;
        if(csi->operation!=CheckSum_OpWrite){
            return(FALSE);
        }
        csi->checksum=(CARD16)(((CARD32)(csi->checksum)+(CARD32)byte)%65536);
        return(csi->put(csi->stream,byte));
    }//CheckSum_Put
    
    PROCEDURE(CheckSum_Write,(void* stream),BOOLEAN)
        /*
            DO:         write the two check sum bytes to the stream.
            RETURN:     same result as stream->put.
        */
    {
            CHAR*                   ptr;
            CheckSum_StreamP        csi;
        
        csi=(CheckSum_StreamP)stream;
        if(csi->operation!=CheckSum_OpWrite){
            return(FALSE);
        }
        csi->operation=CheckSum_OpNone;
        ptr=(CHAR*)&(csi->checksum);
        if(!csi->get(stream,&(ptr[0]))){ return(FALSE); }
        if(!csi->get(stream,&(ptr[1]))){ return(FALSE); }
        return(TRUE);
    }//CheckSum_Write
    

//
// BinHex info and fork encoding/decoding
//

    PROCEDURE(InfoEncode,
            (BinHex_PutPr put,void* stream,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info contains information about the Macintosh file.
            DO:         encode the write info INTo the binhex stream with put.
            RETURN:     all is ok.
        */
    {
            INT32       length;
            CHAR*       ptr;
            
        length=strlen(info->name);
        if((length<=0)||(length>63)){
            return(FALSE);
        }
        if(!put(stream,(CHAR)length)){ return(FALSE); }
        ptr=&(info->name[0]);
        while(length>0){
            if(!put(stream,(*ptr))){ return(FALSE); }
            INC(ptr);
            DEC(length);
        }
        if(!put(stream,info->version)){ return(FALSE); }
        ptr=(CHAR*)&(info->type);
        if(!put(stream,ptr[0])){ return(FALSE); }
        if(!put(stream,ptr[1])){ return(FALSE); }
        if(!put(stream,ptr[2])){ return(FALSE); }
        if(!put(stream,ptr[3])){ return(FALSE); }
        ptr=(CHAR*)&(info->creator);
        if(!put(stream,ptr[0])){ return(FALSE); }
        if(!put(stream,ptr[1])){ return(FALSE); }
        if(!put(stream,ptr[2])){ return(FALSE); }
        if(!put(stream,ptr[3])){ return(FALSE); }
        ptr=(CHAR*)&(info->flags);
        if(!put(stream,ptr[0])){ return(FALSE); }
        if(!put(stream,ptr[1])){ return(FALSE); }
        ptr=(CHAR*)&(info->dataLength);
        if(!put(stream,ptr[0])){ return(FALSE); }
        if(!put(stream,ptr[1])){ return(FALSE); }
        if(!put(stream,ptr[2])){ return(FALSE); }
        if(!put(stream,ptr[3])){ return(FALSE); }
        ptr=(CHAR*)&(info->rsrcLength);
        if(!put(stream,ptr[0])){ return(FALSE); }
        if(!put(stream,ptr[1])){ return(FALSE); }
        if(!put(stream,ptr[2])){ return(FALSE); }
        if(!put(stream,ptr[3])){ return(FALSE); }
        return(TRUE);
    }//InfoEncode
    
    PROCEDURE(InfoDecode,
            (BinHex_GetPr get,void* stream,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info poINTs to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                        Macintosh file.
            DO:         decode the info read from the binhex stream by get.
            RETURN:     all is ok.
        */
    {
            CHAR        lenbyte;
            CARD32      length;
            CHAR*       ptr;
            
        if(!get(stream,&lenbyte)){ return(FALSE); }
        length=(CARD32)lenbyte;
        if(length>63){
            return(FALSE);
        }
        ptr=&(info->name[0]);
        while(length>0){
            if(!get(stream,ptr)){ return(FALSE); }
            INC(ptr);
            DEC(length);
        }
        if(!get(stream,&(info->version))){ return(FALSE); }
        ptr=(CHAR*)(&(info->type));
        if(!get(stream,&(ptr[0]))){ return(FALSE); }
        if(!get(stream,&(ptr[1]))){ return(FALSE); }
        if(!get(stream,&(ptr[2]))){ return(FALSE); }
        if(!get(stream,&(ptr[3]))){ return(FALSE); }
        ptr=(CHAR*)(&(info->creator));
        if(!get(stream,&(ptr[0]))){ return(FALSE); }
        if(!get(stream,&(ptr[1]))){ return(FALSE); }
        if(!get(stream,&(ptr[2]))){ return(FALSE); }
        if(!get(stream,&(ptr[3]))){ return(FALSE); }
        ptr=(CHAR*)(&(info->flags));
        if(!get(stream,&(ptr[0]))){ return(FALSE); }
        if(!get(stream,&(ptr[1]))){ return(FALSE); }
        ptr=(CHAR*)(&(info->dataLength));
        if(!get(stream,&(ptr[0]))){ return(FALSE); }
        if(!get(stream,&(ptr[1]))){ return(FALSE); }
        if(!get(stream,&(ptr[2]))){ return(FALSE); }
        if(!get(stream,&(ptr[3]))){ return(FALSE); }
        ptr=(CHAR*)(&(info->rsrcLength));
        if(!get(stream,&(ptr[0]))){ return(FALSE); }
        if(!get(stream,&(ptr[1]))){ return(FALSE); }
        if(!get(stream,&(ptr[2]))){ return(FALSE); }
        if(!get(stream,&(ptr[3]))){ return(FALSE); }
        return(TRUE);
    }//InfoDecode
    
    PROCEDURE(CopyFork,(BinHex_GetPr get,BinHex_PutPr put,
                    void* src,void* dst,CARD32 maxcount),CARD32)
        /*
            DO:         copies at most maxcount bytes from the src stream
                        to the dst stream.
            RETURN:     the number of byte copied witout error.
        */
    {
            CHAR    byte;
            CARD32  count;
    
        count=0;
        while(get(src,&byte)&&(maxcount>0)){
            DEC(maxcount);
            if(put(dst,byte)){
                INC(count);
            }
        }
        return(count);
    }//CopyFork

//
// BinHex formats encoding/decoding
//

    PROCEDURE(BinHex_Hqx8Encode,(BinHex_GetPr get,BinHex_PutPr put,
                    void* data,void* rsrc,void* binhex,
                    BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info contains information about the Macintosh file.
            DO:         encode the data and rsrc streams read by get 
                        and write the binhex encoded stream with get.
            RETURN:     TRUE if all is ok
                        FALSE if an error occured.
        */
    {
            RunLength_StreamT       rli;
            CheckSum_StreamT        csi;
            CARD32                  count;
        
        RunLength_Rewrite(&rli,put,binhex);
        
        CheckSum_Rewrite(&csi,RunLength_Put,&rli);
        if(!InfoEncode(CheckSum_Put,&csi,info)){ return(FALSE); }
        if(!CheckSum_Write(&csi)){ return(FALSE); }     

        CheckSum_Rewrite(&csi,RunLength_Put,&rli);
        count=CopyFork(get,CheckSum_Put,data,&csi,info->dataLength);
        if(count!=info->dataLength){ return(FALSE); }
        if(!CheckSum_Write(&csi)){ return(FALSE); }     
        
        CheckSum_Rewrite(&csi,RunLength_Put,&rli);
        count=CopyFork(get,CheckSum_Put,rsrc,&csi,info->dataLength);
        if(count!=info->rsrcLength){ return(FALSE); }
        if(!CheckSum_Write(&csi)){ return(FALSE); }     
        
        if(!RunLength_Close(&rli)){ return(FALSE); }        
        return(TRUE);
    }//BinHex_Hqx8Encode
    
        
    PROCEDURE(BinHex_Hqx8Decode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info poINTs to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                         Macintosh file.
            DO:         decode the binhex stream read by get
                         and write data and rsrc streams with get.
            RETURN:     TRUE if all is ok
                        FALSE if an error occured.
        */
    {
            RunLength_StreamT       rli;
            CheckSum_StreamT        csi;
            CARD32                  count;
        
        RunLength_Reset(&rli,get,binhex);
        
        CheckSum_Reset(&csi,RunLength_Get,&rli);
        if(!InfoDecode(CheckSum_Get,&csi,info)){ return(FALSE); }
        if(!CheckSum_Check(&csi)){ return(FALSE); }     

        CheckSum_Reset(&csi,RunLength_Get,&rli);
        count=CopyFork(CheckSum_Get,put,&csi,data,info->dataLength);
        if(count!=info->dataLength){ return(FALSE); }
        if(!CheckSum_Check(&csi)){ return(FALSE); }     
        
        CheckSum_Reset(&csi,RunLength_Get,&rli);
        count=CopyFork(CheckSum_Get,put,&csi,rsrc,info->dataLength);
        if(count!=info->rsrcLength){ return(FALSE); }
        if(!CheckSum_Check(&csi)){ return(FALSE); }     

        if(!RunLength_Close(&rli)){ return(FALSE); }                
        return(TRUE);
    }//BinHex_Hqx8Decode
    

    PROCEDURE(BinHex_Hqx7Encode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info contains information about the Macintosh file.
            DO:         encode the data and rsrc streams read by get 
                         and write the binhex encoded stream with get.
            RETURN:     TRUE if all is ok
                        FALSE if an error occured.
        */
    {
            Seven_StreamT       csi;
        
        Seven_Rewrite(&csi,put,binhex);
        if(!BinHex_Hqx8Encode(get,Seven_Put,data,rsrc,&csi,info)){
            return(FALSE);
        }
        if(!Seven_Close(&csi)){ return(FALSE); }
        return(TRUE);
    }//BinHex_Hqx7Encode
    
    
    PROCEDURE(BinHex_Hqx7Decode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info poINTs to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                        Macintosh file.
            DO:         decode the binhex stream read by get
                         and write data and rsrc streams with get.
            RETURN:     TRUE if all is ok
                        FALSE if an error occured.
        */
    {
            Seven_StreamT       csi;
        
        Seven_Reset(&csi,get,binhex);
        if(!BinHex_Hqx8Decode(Seven_Get,put,data,rsrc,&csi,info)){
            return(FALSE);
        }
        if(!Seven_Close(&csi)){ return(FALSE); }
        return(TRUE);
    }//BinHex_Hqx7Decode

    PROCEDURE(BinHex_HqxInfo,
            (BinHex_GetPr get,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info points to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                        Macintosh file.
            DO:         decode the binhex stream read by get
                         and fills the info record.
            RETURN:     true if all is ok
                        false if an error occured.
        */
    {
        
    }//BinHex_HqxInfo;
    


//end binhex