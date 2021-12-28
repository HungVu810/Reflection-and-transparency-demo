#include"../include/runtime_except.h"
#include<iostream>
#include<exception>

void runtimeException() noexcept{
    try{
        throw;
    }
    catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
