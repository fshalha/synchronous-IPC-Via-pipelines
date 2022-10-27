# synchronous-IPC-Via-pipelines
A program to enable inter-process communication between a producer and a consumer via pipes. Here the producer sends encoded data as frames via a pipe after adding parity bits and sync bits. After receiving data frames the consumer decodes the data and checks for transmission errors using  checksum. Finally, the consumer modifies data, encodes it, and sends it back to the producer.


Follow the following steps to execute the programs

producer.c,consumer.c and Originaldata.inpf file should be in one folder.

1.  gcc producer.c -o  producer -lm 
2.  gcc consumer.c -o  consumer -lm
3.  ./producer

After executing encodedData.binf,modifiedData.outf,encodedconsumer.chck,and decodedproducer.done files will be created.