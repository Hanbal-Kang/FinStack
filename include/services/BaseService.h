#pragma once
#include <vector>

template <typename T>
class BaseService {
public:
    virtual ~BaseService() = default;

    virtual T create(const T& entity) = 0;
    virtual T getById(int id) = 0;
    virtual std::vector<T> getAll() = 0;
    virtual bool update(int id, const T& entity) = 0;
    virtual bool remove(int id) = 0;
};