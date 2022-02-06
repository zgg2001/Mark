/*************************************************************************
	> File Name: m_md5.h
	> Author: ZHJ
	> Remarks: 用于计算md5值
	> Created Time: Sun 06 Feb 2022 10:38:35 AM CST
 ************************************************************************/

#ifndef _M_MD5_H_
#define _M_MD5_H_

#include<string.h>
#include<string>
#include<openssl/md5.h>

class m_md5
{
public:
    /*
    * 计算md5值 
    * src: 需要计算md5值的字符串
    * 返回计算后的结果
    */
    static std::string md5sum(const std::string& src)
    {
        MD5_CTX ctx;
        std::string md5_string;
        unsigned char md[16] = { 0 };
        char tmp[33] = { 0 };
        
        MD5_Init(&ctx);
        MD5_Update(&ctx, src.c_str(), src.size());
        MD5_Final(md, &ctx);
        
        for(int i = 0; i < 16; ++i)
        {
            memset(tmp, 0x00, sizeof(tmp));
            sprintf(tmp, "%02x", md[i]);
            md5_string += tmp;
        }
        return md5_string;
    }
};

#endif
