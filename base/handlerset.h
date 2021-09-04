//
// Created by Swagger on 2021/9/4.
//

#ifndef LIVEREWRITE_HANDLERSET_H
#define LIVEREWRITE_HANDLERSET_H

class HandlerSet{
public:
    HandlerSet();
    ~HandlerSet();

private:

};


class HandlerDescriptor{
public:
    HandlerDescriptor();
    ~HandlerDescriptor();

private:
    int sockNum_;
    HandlerDescriptor* pre_;
    HandlerDescriptor* next_;
    void* clientData_;
    friend HandlerSet;
};





#endif //LIVEREWRITE_HANDLERSET_H
