//
// Created by 唐仁初 on 2021/11/3.
//

#ifndef TCPTEST_HTTPDATA_H
#define TCPTEST_HTTPDATA_H

#include <unordered_map>
#include <vector>
#include <string>

typedef std::unordered_map<std::string, std::string> ParamMap;


enum HTTP_TYPE {
    REQUEST, RESPONSE
};

class HTTPData {
private:
    HTTP_TYPE message_type;

    /// method url hhtp_v
    /// http_v status reason_phrase
    std::vector<std::string> requests;

    ParamMap params;
    std::string body;

    std::string error = "error";
public:

    explicit HTTPData(const std::string &in_str, const HTTP_TYPE &data_type);

    explicit HTTPData() = default;

    std::string &getHTTPBody();

    std::string &getParamValue(const std::string &param);

    ParamMap &getParams();

    bool isParamExists(const std::string &param);

    std::string &getMethod();

    std::string &getHTTPVersion();

    std::string &getHTTPURL();

    std::string &getStatus();

    std::string &getPhrase();

    static bool makeResponseData(std::string &out_str,const std::string &in_str,const std::string &body = {});

    static bool makeRequestData(std::string &out_str,const std::string &in_str);
};


#endif //TCPTEST_HTTPDATA_H
