#ifndef RUNTIME_EXCEPT_H
#define RUNTIME_EXCEPT_H

void runtimeException() noexcept{
    try{
        throw;
    }
    catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

#endif // RUNTIME_EXCEPT_H
