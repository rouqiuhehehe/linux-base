//
// Created by Administrator on 2023/2/26.
//

#ifndef LINUX_BASE_MYSQL_CONNECTION_POLL_ERROR_H_
#define LINUX_BASE_MYSQL_CONNECTION_POLL_ERROR_H_
#include <exception>

class Error : public std::exception
{
public:
    Error () = default;
    void setError (std::string err)
    {
        error = std::move(err);
    }

    [[nodiscard]] const char *what () const noexcept override
    {
        return error.c_str();
    }
private:
    std::string error{};
};
#endif //LINUX_BASE_MYSQL_CONNECTION_POLL_ERROR_H_
