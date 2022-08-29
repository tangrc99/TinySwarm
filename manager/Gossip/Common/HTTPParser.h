//
// Created by trc on 2021/10/25.
//

#ifndef TCPTEST_HTTPPARSER_H
#define TCPTEST_HTTPPARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

using std::string;

typedef std::unordered_map<std::string, std::string> ParamMap;

class HTTPParser {

public:

    static size_t moveNext(const string &in_str, size_t &pos, const char &symbol) {
        while (in_str[pos++] != symbol) {}
        return pos;
    }

    static bool parseHeaderURI(const string &in_str, std::vector<std::string> &requests) {
        size_t current_pos = 0;
        size_t GET_pos, POST_pos;
        GET_pos = in_str.find("GET");
        if (GET_pos != std::string::npos) {
            current_pos += 3;
        } else {
            POST_pos = in_str.find("POST");
            current_pos += 4;
        }
        string request = in_str.substr(0, current_pos);
        requests.push_back(request);

        moveNext(in_str, current_pos, '/');
        size_t url_begin = current_pos;
        moveNext(in_str, current_pos, ' ');
        size_t url_end = current_pos - 1;

        std::string url = in_str.substr(url_begin, url_end - url_begin);
        requests.push_back(url);


        moveNext(in_str, current_pos, '/');
        size_t http_v_begin = current_pos;
        std::string http_v = in_str.substr(http_v_begin, in_str.size() - http_v_begin);
        requests.push_back(http_v);


        return true;
    }


    static void splitString(const string &in_str, ParamMap &out_pairs) {
        size_t n = 0, nn = 0;
        while (nn != (std::string::npos + 2)) {
            n = in_str.find(':', nn);
            string key = in_str.substr(nn, n - nn);
            nn = in_str.find("\r\n", n + 1);
            string value = in_str.substr(n + 1, nn - n - 1);
            out_pairs.emplace(key, value);
            nn += 2;
        }
    }

    static bool
    parserReqMessage(const string &in_str, std::vector<std::string> &requests, ParamMap &head, std::string &body) {
        size_t n = in_str.find("\r\n");
        string request_line = in_str.substr(0, n);
        parseHeaderURI(request_line, requests);

        std::vector<string> ss;
        size_t nn = in_str.find("\r\n\r\n", n + 1);
        string request_head = in_str.substr(n + 2, nn - n - 2);

        splitString(request_head, head);

        if (requests[0] == "POST") {
            size_t nnn = in_str.find("\r\n\r\n", nn + 4);
            body = in_str.substr(nn + 4, nnn - nn - 4);
        }

        return true;
    }

    static bool parseResHeadline(const string &in_str, std::vector<std::string> &requests) {
        size_t n = in_str.find('/', 0);
        if (n == std::string::npos)
            return false;

        size_t nn = in_str.find(' ', n + 1);

        string http_v = in_str.substr(n + 1, nn - n - 1);
        requests.push_back(http_v);

        size_t nnn = in_str.find(' ', nn + 1);

        string status = in_str.substr(nn + 1, nnn - nn - 1);
        requests.push_back(status);

        string end = in_str.substr(nnn + 1, in_str.size() - nnn - 1);
        requests.push_back(end);

        return true;
    }

    static bool
    parserResMessage(const string &in_str, std::vector<std::string> &requests, ParamMap &head, std::string &body) {

        size_t n = in_str.find("\r\n\r\n");
        string request_line = in_str.substr(0, n);
        parseResHeadline(request_line, requests);

        std::vector<string> ss;
        size_t nn = in_str.find("\r\n\r\n", n + 1);
        string request_head = in_str.substr(n + 4, nn - n - 4);

        splitString(request_head, head);

        size_t nnn = in_str.find("\r\n\r\n", nn + 4);
        body = in_str.substr(nn + 4, nnn - nn - 4);

        return true;
    }

    static bool getReqMessage(string &out_str, const std::vector<std::string> &requests, const ParamMap &head,
                              const std::string &body = {}) {
        if (requests.size() != 3) {
            return false;
        }
        out_str += (requests[0] + " /" + requests[1] + " HTTP/" + requests[2]);

        out_str += "\r\n";
        for (const auto &i: head) {
            out_str += (i.first + ":" + i.second + "\r\n");
        }

        out_str += "\r\n";

        if(!body.empty()){
            out_str += body;
            out_str += "\r\n\r\n";
        }
        return true;
    }

    static bool getResMessage(string &out_str, const std::vector<std::string> &requests, const ParamMap &head,
                              const std::string &body = {}) {
        if (requests.size() != 3) {
            return false;
        }

        out_str += ("HTTP/" + requests[0] + " " + requests[1] + " " + requests[2]);
        out_str += "\r\n\r\n";
        for (const auto &i: head) {
            out_str += (i.first + ":" + i.second + "\r\n");
        }
        out_str += "\r\n";
        if(!body.empty()){
            out_str+=body;
            out_str+="\r\n\r\n";
        }
        return true;
    }

};


#endif //TCPTEST_HTTPPARSER_H
