#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
    char readbinfile[1000];
    char filecontent[1000];
    char buf[1000];
    int binaryNum[7] ={ 0 };
    char SYNC[16]={'0','0','0','1','0','1','1','0','0','0','0','1','0','1','1','0'};
    char num4[8]={'0','0','0','0','0','1','0','0'};
    char num3[8]={'0','0','0','0','0','0','1','1'};
    char num2[8]={'0','0','0','0','0','0','1','0'};
    char num1[8]={'0','0','0','0','0','0','0','1'};
    int errorbit=0;
    
    char param[100]="encodedconsumer.chck";
    FILE * filePointer; //declare  file pointers
    FILE * fp;
    FILE * fptr;
    FILE * fileptr;
    int readfile[1000] ;
    void checkSum(char data[], int block_size,int n);
    int checksum(int b[4],int k);
    int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s write read\n", argv[0]);
        return 1;
    }
    int ofd = atoi(argv[1]);//sendpipe
    int ifd = atoi(argv[2]);//receivepipe
    char c_param[1000];
    int nbytes = read(ifd, c_param, sizeof(c_param));//read from the producer
    if (nbytes <= 0)
    {
        fprintf(stderr, "Error: failed to read any data (%d)\n", errno);
        return 1;
    }
    //printf("Child: [%.*s]\n", nbytes, c_param);
    filePointer = fopen(c_param, "r");

    if (NULL == filePointer) {
        printf("file can't be opened \n");
        return 0;
    }
 
     fp = fopen("modifiedData.outf", "w");//opening file in write mode

     if(fp==NULL)
        printf("Error Occurred while Opening/Creating the File!");
    
     fileptr = fopen("modifiedData.outf", "r");//opening file in read mode

     if(fileptr==NULL)
        printf("Error Occurred while Opening/Creating the File!");

//reading the file and decoding
char line[100];
while (fgets(line, sizeof(line), filePointer)) {
//printf("%s", line); 
char numofchars[8];
int num=0;
int indx=0;
//seperating control bit
for(int i=16;i<24;i++){
    numofchars[indx]=line[i];
    indx++;
}
//converting control bit into decimal
int j=0;
 for(int i=7;i>=0;i--)
  {
    int k=numofchars[i]-'0';
    num= num+ k*pow(2,j);
    j++;
  }
char datastream [32];
int intstream [4]={0,0,0,0};
char binary [7];
//seperating the data stream
int indx2=0;
for(int i=24;i<56;i++){
  datastream[indx2]=line[i];
    indx2++;
}
//seperating data stream into blocks
int index4=0;
for(int s=0;s<num;s++){
for(int i=0;i<8;i++){
  binary[i]=datastream[index4];
  index4++;
}
//convert binary array to int
int chartoint= atoi(binary);
intstream[s]=chartoint;//save in integer array
//converting binary to correspondig ascii values
int t=0;int num1=0;
 for(int i=6;i>=0;i--)
  {
    int k=binary[i]-'0';
    num1= num1+ k*pow(2,t);
    t++;
  }
 fprintf(fp,"%c",toupper(num1)); //write ascii chars in a file by modyfying lowercase into uppercase letters
}
//calculating checksum to check transmission errors
int retunval=checksum(intstream,4);
if(retunval!=0)
    errorbit++;

}
if(errorbit>0){
    printf("---------------------TRANSMISSION ERROR--------------------\n");
}
else if(errorbit==0){
    printf("---------------------NO TRANSMISSION ERROR-------------------\n");
}
fclose(filePointer);
 fclose(fp);

//////////////////////////////////////////////////////////////////////encode///////////////////////////////

fptr = fopen("encodedconsumer.chck", "w");

    if (NULL == fptr) {
        printf("file can't be opened \n");
         return 0;
    }
//reading and count the chars in file
int chcount=0;
    while((buf[chcount]=fgetc(fileptr))!=EOF){
        filecontent[chcount]=buf[chcount];
    	chcount++;
    	if(chcount+1>=1000){
    		break;
    	}    
    }

    int modval=chcount%4;
    int iterations=chcount/4;
    int s=0;
    
  //for loop to create data frames
    for(int i=0;i<iterations;i++){
          int c[32];
          int index=0;
        for(int p=0;p<4;p++){
        int paritybit;
        int asciiInput=toascii(filecontent[s]);
 //convert ascii character to 7 bit binary value
        int q = 0;int count=0;
       while (asciiInput > 0 || q<7) {
        // storing remainder in binary array
        binaryNum[q] = asciiInput % 2;
        if(binaryNum[q]==1){
            count++;//variable to count number of ones in binary number
        }
        asciiInput = asciiInput / 2;
        q++;
      }
    int k=0;
    for (int j = q - 1; j >= 0; j--){
//store the binary number in readfile array
    readfile[k]=binaryNum[j];
     k++;
    }
    //calculating the parity bit
    if(count % 2 == 0)
    {
     paritybit=1;
    }
    else{
    paritybit=0;
    }
     //appending the parity bit
     readfile[k]=paritybit;
     // storing encoded 4 characters into one array
    for(int r = 0; r < 8 ;r++) {
        c[index] = readfile[r]; 
        index++;
        
        }
      s++;   
        }
    index=0;
 //writing data frames into a file 
  
     for(int t = 0; t < 16; t++) 
        fprintf(fptr,"%c",SYNC[t]); 
       for(int t = 0; t < 8; t++) 
        fprintf(fptr,"%c",num4[t]); 
       for(int t = 0; t < 32; t++) 
        fprintf(fptr,"%d",c[t]); 
       fprintf(fptr,"\n");
}
// encoding data of last frame
  if(modval !=0){
      int val=8*modval;
        int c[val];
        int index=0;
        for(int p=0;p<modval;p++){
        int paritybit;
        int asciiInput=toascii(filecontent[s]);

        int q = 0;int count=0;
       while (asciiInput > 0 || q<7 ) {
       
        // storing remainder in binary array
        binaryNum[q] = asciiInput % 2;
        if(binaryNum[q]==1){
            count++;
        }
        asciiInput = asciiInput / 2;
        q++;
      }
     //store the binary number in readfile array
       int k=0;
    for (int j = q - 1; j >= 0; j--){

        readfile[k]=binaryNum[j];
 
       k++;
    }
 // calculating parity bit
    if(count % 2 == 0)
    {
     paritybit=1;
    }
    else{
    paritybit=0;
    }
     readfile[k]=paritybit;
    
    for(int r = 0; r < 8 ;r++) {
        c[index] = readfile[r]; 
        index++;
        
        }
      s++;   
        }
    index=0;

//writing the last frame into a file 
     for(int t = 0; t < 16; t++) 
        fprintf(fptr,"%c",SYNC[t]); 
    //if conditions for  different control bits
        if(modval==1){
       for(int t = 0; t < 8; t++) 
        fprintf(fptr,"%c",num1[t]); 
        }else if(modval==2){
             for(int t = 0; t < 8; t++) 
             fprintf(fptr,"%c",num2[t]); 
        }
        else if(modval==3){
             for(int t = 0; t < 8; t++) 
             fprintf(fptr,"%c",num3[t]); 
        }
       for(int t = 0; t < (sizeof(c)/sizeof(int)); t++) 
        fprintf(fptr,"%d",c[t]); 
       fprintf(fptr,"\n");

   }
 fclose(fileptr);
   fclose(fptr);


//////////////////////////////////////////////////////////////////////////////////////////////

    if (write(ofd, param, nbytes) != nbytes)// write file name into pipe
    {
        fprintf(stderr, "Error: failed to write all the data (%d)\n", errno);
        return 1;
    }

    return 0;
}
//function to calculate checksum 
int checksum(int b[4],int k)
 {
 	int checksum,sum=0,i;
     
for(i=0;i<k;i++)
      		sum+=b[i];
                     
    	checksum=~sum;
    	return ~(checksum+sum);
}

