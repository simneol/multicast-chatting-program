# Simple multicast chatting program
---
### Usage example
#### execute
```bash
g++ *.cpp -o chatting
./chatting 224.0.0.0 12345 User1
```
* ** IP must be class D (224.0.0.0 to 239.255.255.255) **

#### file transfer (after execute)
```bash
/sendfile myfile.txt 192.168.1.100
```
* It use port number 20000 to transfer file (TCP connection)
* It use POSIX library. So, it doesn't guarantee that it will run correctly on Windows.
