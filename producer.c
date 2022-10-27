#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

void decToBinary(int n);
int main(void)
{
    int to_Child[2];
    int to_Parent[2];
    int id;
    char param[100]="encodedData.binf";
    int readfile[1000] ;
    char sendPipe[1000];
    char recPipe[1000];
    char filecontent[1000];
    char buf[1000];
    int binaryNum[7] ={ 0 };
    char SYNC[16]={'0','0','0','1','0','1','1','0','0','0','0','1','0','1','1','0'};
    char num4[8]={'0','0','0','0','0','1','0','0'};
    char num3[8]={'0','0','0','0','0','0','1','1'};
    char num2[8]={'0','0','0','0','0','0','1','0'};
    char num1[8]={'0','0','0','0','0','0','0','1'};
    FILE * filePointer; //declare  file pointers
    FILE * fp;
    FILE * fptr;
    FILE * fileptr;

    if (pipe(to_Child) == -1 || pipe(to_Parent) == -1)
    {
        printf("Error on pipe creation: %d", errno);
        exit(1);
    }
 
//child Process
    id = fork();
    if (id == 0)
    {
        close(to_Child[1]);     // Child does not write to itself
        close(to_Parent[0]);    // Child does not read what it writes
        snprintf(sendPipe, sizeof(sendPipe), "%d", to_Parent[1]);//send to parent
        snprintf(recPipe, sizeof(recPipe), "%d", to_Child[0]);//received from parent
        execl("./consumer", "consumer", sendPipe, recPipe, (char *)0);
        fprintf(stderr, "Error on execl: %d\n", errno);
        exit(2);
    }//parent Process
    else if (id > 0)
    {
        close(to_Child[0]);     // Parent does not read childs input
        close(to_Parent[1]);    // Parent does not

    filePointer = fopen("Originaldata.inpf", "r");
    if (NULL == filePointer) {
        printf("file can't be opened \n");
        return 0;
    }
     fp = fopen("encodedData.binf", "w");//opening file in write mode

     if(fp==NULL)
        printf("Error Occurred while Opening/Creating the File!");

//reading and count the chars in file
    int chcount=0;
    while((buf[chcount]=fgetc(filePointer))!=EOF){
        filecontent[chcount]=buf[chcount];
    	chcount++;
    	if(chcount+1>=1000){
    		break;
    	}    
    }

    int modval=chcount%4;
    int iterations=chcount/4;

    int s=0;
/////////////////////////////////////encoding///////////////////////////////////////////////////////////   
   //for loop to create data frames
    for(int i=0;i<iterations;i++){
          int c[32];
          int index=0;
        //for loop to encode 4 characters
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
   //store the binary number in readfile array
    for (int j = q - 1; j >= 0; j--){
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
        fprintf(fp,"%c",SYNC[t]); 
       for(int t = 0; t < 8; t++) 
        fprintf(fp,"%c",num4[t]); 
       for(int t = 0; t < 32; t++) 
        fprintf(fp,"%d",c[t]); 
       fprintf(fp,"\n");
}// encoding data of last frame
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
    int k=0;
   //store the binary number in readfile array
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
     //appending parity bit
     readfile[k]=paritybit;
    //storing encoded chars of last frame in an array
    for(int r = 0; r < 8 ;r++) {
        c[index] = readfile[r]; 
        index++;
        
        }
      s++;   
        }
    index=0;
//writing the last frame into a file

     for(int t = 0; t < 16; t++) 
        fprintf(fp,"%c",SYNC[t]); 
//if conditions for  different control bits
        if(modval==1){
       for(int t = 0; t < 8; t++) 
        fprintf(fp,"%c",num1[t]); 
        }else if(modval==2){
             for(int t = 0; t < 8; t++) 
             fprintf(fp,"%c",num2[t]); 
        }
        else if(modval==3){
             for(int t = 0; t < 8; t++) 
             fprintf(fp,"%c",num3[t]); 
        }
       for(int t = 0; t < (sizeof(c)/sizeof(int)); t++) 
        fprintf(fp,"%d",c[t]); 
       fprintf(fp,"\n");

   }
 fclose(fp);
 fclose(filePointer);
    
        int nbytes = write(to_Child[1], param, 1000);
        if (nbytes == -1)
        {
            fprintf(stderr, "Error on write to pipe: %d\n", errno);
            exit(3);
        }
        close(to_Child[1]);
        if ((nbytes = read(to_Parent[0], param, 1000)) <= 0)
        {
            fprintf(stderr, "Error on read from pipe: %d\n", errno);
            exit(5);
            }

else{
       // printf("Data from pipe: [%.*s]\n", nbytes, param);
        
/////////////////////////////////////decoding///////////////////////////////////////////////////////////
//open the file to read which is sent by consumer after encoding
fptr = fopen(param, "r");

    if (NULL == fptr) {
        printf("file can't be opened \n");
        return 0;
    }
//open the file to write decoded data
fileptr = fopen("decodedproducer.done", "w");

    if (NULL == fileptr) {
        printf("file can't be opened \n");
        return 0;
    }

//read the file
char line[100];
while (fgets(line, sizeof(line), fptr)) {
//printf("%s", line); 
char numofchars[8];
int num=0;
int indx=0;
//seperating the control bit 
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
char binary [7];
//seperating data stream of characters
int indx2=0;
for(int i=24;i<56;i++){
  datastream[indx2]=line[i];
    indx2++;
}
// seperating data stream into characters
int index4=0;
for(int s=0;s<num;s++){
for(int i=0;i<8;i++){
  binary[i]=datastream[index4];
  index4++;
}
// converting every encoded char into ascii char
int t=0;int num1=0;
 for(int i=6;i>=0;i--)
  {
    int k=binary[i]-'0';
    num1= num1+ k*pow(2,t);
    t++;
  }
//write the decoded chars into a file
 fprintf(fileptr,"%c",num1); 
}

}
//exit and close the file pointers
exit(0);
fclose(fptr);
 fclose( fileptr );
}     
    }
    else
    {
        perror("fork failed");
        exit(4);
    }
    
}

