//
// Created by 唐仁初 on 2021/10/22.
//

#ifndef TCPTEST_BUFFER_H
#define TCPTEST_BUFFER_H

#include <vector>
#include <unistd.h>
#include <iostream>

#define BUFFER_SIZE 6400000 //65536

class Buffer {
private:
    std::vector<char> buffer;
    int http_end = -1;
    size_t start = 0;
    size_t end;
    size_t buff_len = BUFFER_SIZE;

    size_t last_pos = 0;
    int find_num = 0;

    bool catchEOF;  ///判断是输出还是输入

public:

    Buffer() : end(0), catchEOF(false) {
        buffer.reserve(buff_len);
    }

    size_t CurrentSize(){
        return buffer.size();
    }

    //FIXME
    size_t readToBuffer(const int &fd) {
        if ((buffer.capacity() - end) <= 2000) {
            buffer.reserve(buff_len * 2);
            buff_len = buffer.capacity();
            std::cout << buff_len;
            std::cout << "buffer increase.\n";
        }

        if(start == end){
            start = 0;
            end = 0;
        }

        //这样修改会导致 std::string 的 size()没有改变
        size_t n = ::read(fd, &buffer[end], buff_len - end);

        // 丢弃心跳(写入位置不增长)
        if( n==1 && buffer[end]==' '){
            return 1;
        }

        if (n == -1){
            std::cout<<"errno: "<<errno<<std::endl;
            return -1;
        }

        end += n;
        return n;
    }

    void writeToBuffer(const char *str, size_t size) {
        if ((buffer.capacity() - end) <= size) {
            buffer.reserve(buff_len * 2);
            buff_len = buffer.capacity();
            std::cout << buff_len;
            std::cout << "buffer increase.\n";
        }
        for (int i = 0; i < size; i++) {
            buffer[end++] = str[i];
        }
    }

    size_t BufferToFd(const int &fd) {
        return ::write(fd, &buffer, buff_len);
    }

    bool isCatchEOF() {
        if (buffer[end - 1] == 'F') {
            if (buffer[end - 2] == 'O') {
                if (buffer[end - 3] == 'E') {
                    catchEOF = true;
                }
            }
        }
        return catchEOF;
    }

    bool isCatchHTTPEnd() {
        size_t i = std::max(last_pos,start);

        for (; i < end; i++) {
            if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n') {
                http_end = i + 3;
                find_num++;
                break;
            }
        }
        if(find_num == 0){
            last_pos = end;
            return false;
        }

        if(this->first() == 'P'){

            for (i++; i < end; i++) {
                if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n') {
                    http_end = i + 3;
                    find_num++;
                    break;
                }
            }
            if(find_num == 1){
                last_pos = end;
                return false;
            }
            return true;
        }

        return true;
    }

    void clearHttpEnd(){
        start = http_end +1 ;
        std::cout<<end<<" "<<start<<std::endl;
        http_end = -1 ;
        find_num = 0;
//        if(start >= end){
//            start = end = 0;
//        }
    }

    std::string str() {
//        if (catchEOF)
//            buffer[end - 3] = '\0';    ///截去 EOF

        if (http_end < 0) {
            return {};
        }

        return &buffer[start];
    }

    [[nodiscard]] char first() const {

        int i=0;
        while(buffer[i]==' '){i++;}

        return buffer[i];
    }

    size_t size() const {
        return catchEOF ? end - 4 : end;
    }
};


#endif //TCPTEST_BUFFER_H
