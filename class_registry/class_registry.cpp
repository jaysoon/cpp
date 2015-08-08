#include "class_registry.h"

namespace util {

void ClassRegistryBase::DoRegisterClass(const std::string& entry_name, 
                                        FuncPtr fptr)
{
    ClassEntryMap::const_iterator citer = class_entry_map_.find(entry_name);
    if (citer != class_entry_map_.end())
    {
        std::cerr << "ClassRegiser: class " << entry_name
            << " already registered. " << std::endl;
        std::abort();
    }

    class_entry_map_[entry_name] = fptr;
    entry_names_.push_back(entry_name);
}

void* ClassRegistryBase::DoCreateObject(const std::string& entry_name) const
{
    ClassEntryMap::const_iterator citer = class_entry_map_.find(entry_name);
    if (citer == class_entry_map_.end())
        return NULL;
    return (*(citer->second))();
}

size_t ClassRegistryBase::RegisterCount() const
{
    return entry_names_.size();
}

const std::string& ClassRegistryBase::ClassName(size_t i) const
{
    assert(i < entry_names_.size());
    return entry_names_[i];
}

}
