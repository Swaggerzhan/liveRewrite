# RTSP server


#### learn:
    
```C++
// std::search
// 一种比较好的寻找CRLF的方式
const char* crlf = "\r\n";
char* buf = new char[1024]; // data
const char* find_crlf = std::search(buf, buf+1024, crlf, crlf+2);
// 如果找到crlf，则返回指针首部，否则返回buf+1024
// std::search(迭代器头，迭代器尾，迭代器头，迭代器尾);
// 配合buffer的设计，0 <= readIndex <= writeIndex < size
// readIndex即解析到的索引，writeIndex即收到的索引，那么就可以写出
const char* find_crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
```