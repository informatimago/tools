/******************************************************************************
FILE:               MfFile.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the MfFile class.
    This class represents plain files.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:39:06  pascal
   Initial revision
   
    Revision 1.6  95/05/08  02:22:12  pascal
    0003: Ajout d'une methode donnant le nombre de ligne dans le fichier.
    
    Revision 1.5  95/04/18  03:08:30  pascal
    Freezing version WORKING_1.
    
    Revision 1.4  95/04/17  05:03:48  pascal
    Removed StatusInquire (merged it with size()).
    
    Revision 1.3  95/04/17  03:05:36  pascal
    Version optimized for space. Added ident keyword.
    
    Revision 1.2  95/04/17  02:13:17  pascal
    Version with fPath cache in MfNode.
    
    Revision 1.1  95/02/25  22:22:42  pascal
    Initial revision
    
    1994-12-25 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1994 - 2011

    GPL

    This file is part of the dosname tool.

    This  program is  free software;  you can  redistribute  it and/or
    modify it  under the  terms of the  GNU General Public  License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program  is distributed in the  hope that it  will be useful,
    but  WITHOUT ANY WARRANTY;  without even  the implied  warranty of
    MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a  copy of the GNU General Public License
    along with  this program; see the  file COPYING; if  not, write to
    the Free  Software Foundation, Inc.,  59 Temple Place,  Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/
#ifndef __MfFile__
#define __MfFile__
#include <BcInterface.h>
#include <BcTypes.h>
#include <MfNode.hh>

class MfDirectory;

#define MfFile_SUPER MfNode
class MfFile:public MfFile_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(MfFile)
    DESTRUCTOR(MfFile)

        
// override of BpObject methods:

    METHOD(MfFile,makeBrother,(void),BpObject*)
    METHOD(MfFile,printOnLevel,(FILE* file,CARD32 level),void)


// override of MfNode methods:

    METHOD(MfFile,isDirectory,(void),BOOLEAN)
        /*
            RETURN:     FALSE
        */


    METHOD(MfFile,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfFile,copyToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfFile,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfFile,symLinkToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfFile,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfFile,hardLinkToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)



// MfFile methods:

    METHOD(MfFile,size,(void),CARD32)
        /*
            RETURN:     the number of byte in the file,
                        or MAX_CARD32 if the size cannot be obtained.
        */
        
    METHOD(MfFile,lineCount,(void),CARD32)
        /*
            RETURN:     the number of line in the file,
                        or MAX_CARD32 if the lineCount cannot be obtained.
        */
        
    METHOD(MfFile,compare,(MfFile* otherFile),CARD32)
        /*
            RETURN:     0 <=> this file and otherFile are identical,
                        a value proportionnal to the number of differences.
        */

    METHOD(MfFile,compareFirstBlock,(MfFile* otherFile),CARD32)
        /*
            RETURN:     0 <=> this file and otherFile are identical,
                        1 <=> this file and otherFile have same first block.
                        2 <=> this file and otherFile have different 1st block.
        */
        
    METHOD(MfFile,copyFileFrom,(MfFile* source),void)

    METHOD(MfFile,textContents,(void),BpString*)
        /*
            RETURN:     A string initialized with the data read from this file.
                        It is expected to be a text file, since nothing is 
                        done about null characters inside the file.
        */

protected:
    CARD32          fSize;
    CARD32          fLineCount;
    CARD32*         fBlock;
    
    METHOD(MfFile,loadFirstBlock,(void),void)
    
};

#endif // MfFile.
