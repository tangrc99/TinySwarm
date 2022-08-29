//
// Created by 唐仁初 on 2021/11/3.
//
#include "HTTPData.h"
#include "Logging.h"
#include "HTTPParser.h"

HTTPData::HTTPData(const std::string &in_str, const HTTP_TYPE &data_type) : message_type(data_type) {

    if (data_type == REQUEST)
        HTTPParser::parserReqMessage(in_str, requests, params, body);
    if (data_type == RESPONSE)
        HTTPParser::parserResMessage(in_str, requests, params, body);
}


std::string &HTTPData::getHTTPBody() {
    if (message_type == REQUEST) {
        if (requests[0] != "POST")
            return error;
    }
    return body;
}

std::string &HTTPData::getParamValue(const std::string &param) {
    return params.find(param)->second;
}

ParamMap &HTTPData::getParams() {
    return params;
}

std::string &HTTPData::getMethod() {
    if (message_type != REQUEST) {
        LOG_ERROR("%s", "Not a request http data")
        return error;
    }
    return requests[0];
}

std::string &HTTPData::getHTTPURL() {
    if (message_type != REQUEST) {
        LOG_ERROR("%s", "Not a request http data")
        return error;
    }
    return requests[1];
}

std::string &HTTPData::getHTTPVersion() {
    return (message_type == HTTP_TYPE::REQUEST) ? requests[2] : requests[0];
}

std::string &HTTPData::getStatus() {
    if (message_type != REQUEST) {
        LOG_ERROR("%s", "Not a response http data")
        return error;
    }
    return requests[1];
}

std::string &HTTPData::getPhrase() {
    if (message_type != REQUEST) {
        LOG_ERROR("%s", "Not a response http data")
        return error;
    }
    return requests[2];
}

bool HTTPData::isParamExists(const std::string &param) {
    return params.find(param) != params.end();
}

bool HTTPData::makeResponseData(std::string &out_str, const std::string &in_str,const std::string &body) {
    return HTTPParser::getResMessage(out_str, {"1.1", "200", "OK"}, {{"RPCRESPONSE", in_str}},body);
}
