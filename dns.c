//
// Created by Administrator on 2023/2/28.
//

struct DnsHeader
{
    unsigned short transactionId;
    unsigned short flags;
    // 查询问题区域节的数量
    unsigned short questions;
    // 回答区域的数量
    unsigned short answer;
    // 授权区域的数量
    unsigned short authority;
    // 附加区域的数量
    unsigned short additional;
};

struct DnsBody
{
    int length;
    unsigned short qType;
    unsigned short qClass;
    char *qName;
};