//
// Created by Administrator on 2023/2/26.
//

#ifndef LINUX_BASE_MYSQL_CONNECTION_POLL_NONCOPYABLE_H_
#define LINUX_BASE_MYSQL_CONNECTION_POLL_NONCOPYABLE_H_
class NonCopyAble
{
public:
    NonCopyAble () = default;
    ~NonCopyAble () = default;
    NonCopyAble (const NonCopyAble &) = delete;
    NonCopyAble &operator= (const NonCopyAble &) = delete;
};
#endif //LINUX_BASE_MYSQL_CONNECTION_POLL_NONCOPYABLE_H_
