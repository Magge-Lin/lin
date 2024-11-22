#include "interface/IDatabase.h"
#include "CDatabase.h"

#include <memory>


IDatabasePtr IDatabase::instance() 
{
    return std::dynamic_pointer_cast<IDatabase>(CDatabase::instance());
}