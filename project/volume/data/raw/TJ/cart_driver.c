////////////////////////////////////////////////////////////////////////////////
//
//  File           : cart_driver.c
//  Description    : This is the implementation of the standardized IO functions
//                   for used to access the CART storage system.
//
//  Author         : [Jing Tang] 
//  PSU email      : [jjt5556@psu.edu] 
//

// Includes
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Project Includes
#include "cart_driver.h"
#include "cart_controller.h"

//initialize the stucture of file
struct openfile{
  int file_size;
  uint32_t file_pos;
  char *file_name;
  int frame_id[100];
  int cart_id[100];
  int status; 
};


//initialize counter for file, frame and cart
struct openfile cartlab[1024];
int fileCount = 0;
uint16_t  current_frame =-1;
uint16_t current_cart = 0;

//set up an opcode of 64 bits with data from ky1,ky2,rt1,ct1,fm1
CartXferRegister opcode(uint64_t ky1, uint64_t ky2, uint64_t rt1, uint64_t ct1, uint64_t fm1){
  CartXferRegister opcode;
  ky1 = ky1<<56;
  ky2 = ky2<<48;
  rt1 = rt1<<47;
  ct1 = ct1<<31;
  fm1 = fm1<<15;
  opcode = ky1|ky2|rt1|ct1|fm1;
  return opcode;
}

//check rt1 sent back by kernel to make sure the process is successful
int issuccessful(CartXferRegister respond){
  int rt1;
  rt1 =(respond >> 47)&0x1;
  if(rt1 == 0){
    return 0;
  }
  else{
    return -1;
  }
} 


//
// Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_poweron
// Description  : Startup up the CART interface, initialize filesystem
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int32_t cart_poweron(void) {
  int result;
  int i;
  CartXferRegister initms,bzero,ldcart;
  CartXferRegister respond1,respond2,respond3;
  
  //cart_op_initms
  initms = opcode(CART_OP_INITMS,0,0,0,0);
  respond1 = cart_io_bus(initms,NULL);
  if((result = issuccessful(respond1))==-1){
    return -1;
  }

  for(i=0;i<CART_MAX_CARTRIDGES;i++){
    //cart_op_ldcart
    ldcart = opcode(CART_OP_LDCART,0,0,i,0); 
    respond3 = cart_io_bus(ldcart,NULL);
    if((result = issuccessful(respond3))==-1){
      return -1;
    }
    //cart_op_bzero
    bzero = opcode(CART_OP_BZERO,0,0,0,0);
    respond2 = cart_io_bus(bzero,NULL);
    if((result = issuccessful(respond2))==-1){
      return -1;
    }
  }    
  // Return successfully
  return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_poweroff
// Description  : Shut down the CART interface, close all files
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int32_t cart_poweroff(void) {
  int result;
  int i;
  CartXferRegister powoff;
  CartXferRegister respond;
  
  //clean file
  for(i=0;i<fileCount;i++){
    if(cartlab[i].status == 1){
      cart_close(i);
    }
  }

  //power off
  powoff = opcode(CART_OP_POWOFF,0,0,0,0);
  respond = cart_io_bus(powoff,NULL);
  if((result = issuccessful(respond))==-1){
    return -1;  
 }
  // Return successfully
  return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_open
// Description  : This function opens the file and returns a file handle
//
// Inputs       : path - filename of the file to open
// Outputs      : file handle if successful, -1 if failure

int16_t cart_open(char *path) {
  //check the length of name
  if(strlen(path)>128){
    return -1;
  }
  int i;
  
  //if the file exist and the file is close
  //set status to open and postion to 0
  if(fileCount!=0){
    for(i=0;i<fileCount;i++){
      if(strcmp(cartlab[i].file_name,path)==0){
        if(cartlab[i].status == 1){
          return -1;
        }
        else{
          cartlab[i].status = 1;
          cartlab[i].file_pos =0;
          return i;
        }
      }
    }
  } 
  
  //if the file does not exist
  //initialize every varible
  //file counter plus 1 and set it to a new frame
  //if the frane counter is 1024, cart go to the next one
  cartlab[fileCount].file_name = path;
  cartlab[fileCount].file_size = 0;
  cartlab[fileCount].status = 1;
  cartlab[fileCount].file_pos = 0;

  fileCount++;
  current_frame++;
  if(current_frame == 1024){
    current_cart++;
    current_frame=0;
  }  
  // THIS SHOULD RETURN A FILE HANDLE
  return fileCount-1;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_close
// Description  : This function closes the file
//
// Inputs       : fd - the file descriptor
// Outputs      : 0 if successful, -1 if failure

int16_t cart_close(int16_t fd) {
  //check fd and current status
  if(fd < 0){
    return -1;
  }
  if(fd > fileCount){
    return -1;
  }
  if(cartlab[fd].status == 0){
    return -1;
  }
  //change status to close
  cartlab[fd].status = 0;
  // Return successfully
  return (0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_read
// Description  : Reads "count" bytes from the file handle "fh" into the 
//                buffer "buf"
//
// Inputs       : fd - filename of the file to read from
//                buf - pointer to buffer to read into
//                count - number of bytes to read
// Outputs      : bytes read if successful, -1 if failure

int32_t cart_read(int16_t fd, void *buf, int32_t count) {
  int cid;
  int fid;
  char buf1[1024];
  int ipos;
  int i;
  int c;
  CartXferRegister ldcart,rdfrme;
  CartXferRegister respond1,respond2;
  int fcount;
  int result;
  int cpos = 0;
  int bpos = 0;
  int iframe;
  int32_t remain = count;

  //get the start frame and state position
  ipos = cartlab[fd].file_pos;
  iframe = ipos/1024;
 
  //check fd and status
  if(fd<0){
    return -1;
  }
  if(fd>fileCount){
    return -1;
  }
  if(cartlab[fd].status == 0){
    return -1;
  }
 
  //check whether the count the user want to read is over the size of file
  //if it is larger than the size, only read to the end of the file
  if(ipos+count > cartlab[fd].file_size){
    remain = cartlab[fd].file_size - cartlab[fd].file_pos;
  }
 
  //calculate how many frames the user want to read
  fcount = ((ipos%1024)+remain)/1024;
 
  //if there's only one frame,simply copy the frame to the buf1 
  //select the part user want to buf
  //move position to current postion
  if(fcount==0){
    cpos = ipos%1024;
    cid = cartlab[fd].cart_id[iframe];
    fid = cartlab[fd].frame_id[iframe];
   
    ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
    respond1 = cart_io_bus(ldcart,NULL);
    if((result = issuccessful(respond1))==-1){
      return -1;
    }
    rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
    respond2 = cart_io_bus(rdfrme,buf1);
    if((result = issuccessful(respond2))==-1){
      return -1;
    }
      
    memcpy(buf+0,buf1+cpos,remain);
    cartlab[fd].file_pos += remain;
    return remain; 
  }
 
  //if there are more than one file
  else{
    
    //treat the first frame almost the same as only one frame
    //just read to the end of this frame 
    //cpos is the postion of start position in current frame
    cpos = ipos%1024;
    c = 1024-cpos;

    cid = cartlab[fd].cart_id[iframe];
    fid = cartlab[fd].frame_id[iframe];

    ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
    respond1 = cart_io_bus(ldcart,NULL);
    if((result = issuccessful(respond1))==-1){
      return -1;
    }
    rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
    respond2 = cart_io_bus(rdfrme,buf1);
    if((result = issuccessful(respond2))==-1){
      return -1;
    }

    //copy the frame out to bu1 and select from state position to copy to buf
    //adjust the postion
    //adjust the postion in buf
    //reduce the left bytes
    //move to next frame used by this file
    memcpy(buf,buf1+cpos,c);
    cartlab[fd].file_pos += c;
    bpos += c;
    remain -= c;
    iframe += 1;


    for(i=1;i<fcount+1;i++){
    //cart_op_ldcart
      cid = cartlab[fd].cart_id[iframe];
      fid = cartlab[fd].frame_id[iframe];
    
      //if the left bytes is less than 1024,which means it is the last frame
      //copy from the start of this frame and end at the remain count
      //adjust the position
      if(remain<=1024){
        ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
        respond1 = cart_io_bus(ldcart,NULL);
        if((result = issuccessful(respond1))==-1){
          return -1;
        }
        rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
        respond2 = cart_io_bus(rdfrme,buf1);
        if((result = issuccessful(respond2))==-1){
          return -1;
        }
      
        memcpy(buf+bpos,buf1,remain);

        cartlab[fd].file_pos += remain;
        return count;
      }
      //if the frame is either the first one or the last one
      //read all 1024 bytes in this frame
      //adjust current position
      //adjust postion in buf
      //move to next frame used by this file
      else{
        ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
        respond1 = cart_io_bus(ldcart,NULL);
        if((result = issuccessful(respond1))==-1){  
          return -1;
        }
        rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
        respond2 = cart_io_bus(rdfrme,buf1);  
        if((result = issuccessful(respond2))==-1){
          return -1;  
        }
        memcpy(buf+bpos,buf1,1024);
        bpos += 1024;
        remain -= 1024;
        cartlab[fd].file_pos += 1024;
        iframe+=1;
      }
    }   
  }
  // Return successfully
  return (count);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_write
// Description  : Writes "count" bytes to the file handle "fh" from the 
//                buffer  "buf"
//
// Inputs       : fd - filename of the file to write to
//                buf - pointer to buffer to write from
//                count - number of bytes to write
// Outputs      : bytes written if successful, -1 if failure

int32_t cart_write(int16_t fd, void *buf, int32_t count) {
 // int cid;
 // int fid;
  char buf1[1024];
  int cid;
  int fid;
  int bpos=0;
  int ipos=0;
  int i;
  int c;
  CartXferRegister ldcart,rdfrme,wrfrme;
  CartXferRegister respond1,respond2,respond3;
  int fcount=0;
  int result;
  int cpos = 0;
  int iframe;
  int32_t remain = count;

  //get the current position and current frame
  //calculate how many frames may use 
  ipos = cartlab[fd].file_pos;
  fcount = ((ipos%1024)+count)/1024;
  iframe = ipos/1024;

  //check fd and status 
  if(fd<0){
    return -1;
  }
  if(fd>fileCount){
    return -1;
  }
  if(cartlab[fd].status ==0){
    return -1;
  }
 
  //if the file haven't been written yet
  //give it a new frame and the corresponding cart
  if(cartlab[fd].file_size == 0 && cartlab[fd].file_pos == 0){
    cartlab[fd].frame_id[0] = current_frame;
    cartlab[fd].cart_id[0] = current_cart;
  }

  //if it only use one frame
  //get the frame id and cart id
  //read the corresponding frame and write new staff into it
  //put it back to the frame
  //adjust the size of the file
  if(fcount==0){
    cpos = ipos%1024;
    cid = cartlab[fd].cart_id[iframe];
    fid = cartlab[fd].frame_id[iframe];

    ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
    respond1 = cart_io_bus(ldcart,NULL);
    if((result = issuccessful(respond1))==-1){
      return -1;
    }
    rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
    respond2 = cart_io_bus(rdfrme,buf1);
    if((result = issuccessful(respond2))==-1){
      return -1;
    }
    memcpy(buf1+cpos,buf,count);

    wrfrme = opcode(CART_OP_WRFRME,0,0,0,fid);
    respond3 = cart_io_bus(wrfrme,buf1);
    if((result = issuccessful(respond3))==-1){
      return -1;
    }
    
    cartlab[fd].file_pos = cartlab[fd].file_pos + count;
    if(cartlab[fd].file_size < cartlab[fd].file_pos){
      cartlab[fd].file_size = cartlab[fd].file_pos;
    }
    else{
      cartlab[fd].file_size = cartlab[fd].file_size;
    }   
  }

  //if there's more than one frame
  //treat the first frame almost the same as only one frame
  //write to the end of the frame
  else{
    cpos = cartlab[fd].file_pos%1024;
    c = 1024-cpos;
    bpos = 0;

    cid = cartlab[fd].cart_id[iframe];
    fid = cartlab[fd].frame_id[iframe];

    ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
    respond1 = cart_io_bus(ldcart,NULL);
    if((result = issuccessful(respond1))==-1){
      return -1;
    }
    rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
    respond2 = cart_io_bus(rdfrme,buf1);
    if((result = issuccessful(respond2))==-1){
      return -1;
    }
    memcpy(buf1+cpos,buf+0,c);

    wrfrme = opcode(CART_OP_WRFRME,0,0,0,fid);
    respond3 = cart_io_bus(wrfrme,buf1);
    if((result = issuccessful(respond3))==-1){
      return -1;
    }

    cartlab[fd].file_pos +=c;
    bpos = c+bpos;
    remain = remain - c;
    iframe += 1;
  
    for(i=1;i<fcount+1;i++){
      //check if there's a next frame exist for this file
      //if there's not, give it a new frame and frame counter plus 1
      //check it each time it go to the next frame
      if(cartlab[fd].file_pos >= cartlab[fd].file_size){
        current_frame ++;
        if(current_frame == 1024){
          current_frame = 0;
          current_cart++;
        }
        cartlab[fd].cart_id[iframe] = current_cart;
        cartlab[fd].frame_id[iframe] = current_frame;
      }

      //get the cid and fid
      //get the postion in current frame     
      cid = cartlab[fd].cart_id[iframe];
      fid = cartlab[fd].frame_id[iframe];
      cpos = cartlab[fd].file_pos%1024;

      //if remian bytre is less than 1024
      //which means it is the last frame the user need to use
      //adjust the size of file and the position
      if(remain<=1024){
        ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
        respond1 = cart_io_bus(ldcart,NULL);
        if((result = issuccessful(respond1))==-1){
          return -1;
        }
        rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
        respond2 = cart_io_bus(rdfrme,buf1);
        if((result = issuccessful(respond2))==-1){
          return -1;
        }
        memcpy(buf1,buf+bpos,remain);
        
        wrfrme = opcode(CART_OP_WRFRME,0,0,0,fid);
        respond3 = cart_io_bus(wrfrme,buf1);
        if((result = issuccessful(respond3))==-1){
          return -1;
        }  
        
        cartlab[fd].file_pos += remain;
        if(cartlab[fd].file_pos >= cartlab[fd].file_size){
          cartlab[fd].file_size = cartlab[fd].file_pos;
        }
        else{
          cartlab[fd].file_size = cartlab[fd].file_size;
        }
      }
      //if the frame is neither the first one nor the last one
      //write this frame completely   
      //adjust position of file and position of buf
      //move to next frame
      else{
        ldcart = opcode(CART_OP_LDCART,0,0,cid,0);
        respond1 = cart_io_bus(ldcart,NULL);
        if((result = issuccessful(respond1))==-1){
          return -1;
        }
        rdfrme = opcode(CART_OP_RDFRME,0,0,0,fid);
        respond2 = cart_io_bus(rdfrme,buf1);
        if((result = issuccessful(respond2))==-1){
          return -1;
        }
        memcpy(buf1,buf+bpos,1024);

        wrfrme = opcode(CART_OP_WRFRME,0,0,0,fid);
        respond3 = cart_io_bus(wrfrme,buf1);
        if((result = issuccessful(respond3))==-1){
          return -1;
        }

        bpos = bpos+1024;
        cpos = cpos+1024;
        remain = remain-1024;
        cartlab[fd].file_pos += 1024;
        iframe = iframe + 1;
      }
    }
  }
    
   
  // Return successfully
  return (count);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cart_read
// Description  : Seek to specific point in the file
//
// Inputs       : fd - filename of the file to write to
//                loc - offfset of file in relation to beginning of file
// Outputs      : 0 if successful, -1 if failure

int32_t cart_seek(int16_t fd, uint32_t loc) {
  //check fd and status
  //check whether loc is larger than the file size
  //move the postion of file to loc
  if(fd < 0){
    return -1;
  }
  if(fd > fileCount){
    return -1;
  }
  if(loc>cartlab[fd].file_size){
    return -1;
  }
  if(cartlab[fd].status == 0){
    return -1;
  }
  cartlab[fd].file_pos = loc;
  // Return successfully
  return (0);
}
