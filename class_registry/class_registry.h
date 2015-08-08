#ifndef CLASS_REGISTRY_H_
#define CLASS_REGISTRY_H_

#include <cstdlib>
#include <cassert>

#include <map>
#include <vector>
#include <string>

#include <iostream>

// 之所以分成两步实现MACRO_CONS，是因为A,B本身可能也是宏，
// 例如此代码中用到的__LINE__，如果用一步实现，__LINE__将不会执行
#define MACRO_CONS(A, B) MACRO_DO_CONS(A, B)
#define MACRO_DO_CONS(A, B) A##B

namespace util
{

// ClassRegistry的基类
// ClassRegistry是模板类，将功能实现放在基类中，可以防止代码膨胀
class ClassRegistryBase
{
protected:
    typedef void* (*FuncPtr)();

protected:
    ClassRegistryBase() {}
    ~ClassRegistryBase() {}

    void DoRegisterClass(const std::string& entry_name, FuncPtr fptr);
    void* DoCreateObject(const std::string& entry_name) const;

public:
    size_t RegisterCount() const;
    const std::string& ClassName(size_t i) const;

private:
    typedef std::map<std::string, FuncPtr> ClassEntryMap; // 子类名到构造函数的映射
    ClassEntryMap class_entry_map_;
    std::vector<std::string> entry_names_;
};

// ClassRegistry<BaseClass>是BaseClass的注册表
// 它提供了BaseClass子类名到构造函数的映射
template <typename BaseClass>
class ClassRegistry : public ClassRegistryBase
{
public:
    typedef BaseClass* (*CtorPtr)();

public:
    ClassRegistry() {}
    ~ClassRegistry() {}

    void RegisterClass(const std::string& entry_name, CtorPtr ctor)
    {
        DoRegisterClass(entry_name, reinterpret_cast<FuncPtr>(ctor));
    }

    BaseClass* CreateObject(const std::string entry_name) const
    {
        return static_cast<BaseClass*>(DoCreateObject(entry_name));
    }
};

// 一个BaseClass可以有多个注册表，为方便管理每个注册表需要一个
// 注册表标签（名称），注册表标签记录了注册表及其对应的BaseClass
//
// RegistryTagBase<BaseClass>就是BaseClass的注册表标签的基类，它
// 的每个子类都是一个注册表标签；如果有多个子类，那么就可以有多个
// 注册表标签，每个标签都对应一个注册表。
//
// 使用时，宏CLASS_REGISTRY_DEFINE根据registry_name定义（拼接）
// 一个RegistryTag类；后面对该RegistryTag的所有访问都可以通过
// registry_name实现。
//
// 因此，一个BaseClass可以有多个注册表，可以用不同的
// registry_name区分
template <typename BaseClassName>
struct RegistryTagBase 
{
    typedef BaseClassName BaseClass;
    typedef ClassRegistry<BaseClassName> RegistryType;
};

// RegistryTag是一个注册表标签，函数ClassRegistryInstance根据
// 给定的RegistyTag创建及访问对应的注册表实例
// 下面的方法使用了单例模式(effective cpp 条款47)
// static对象会在首次函数调用的时候被初始化
template <typename RegistryTag>
typename RegistryTag::RegistryType& ClassRegistryInstance()
{
    static typename RegistryTag::RegistryType registry;
    return registry;
};

// 注册一个类到指定的注册表中
template <typename RegistryTag>
class ClassRegister
{
    typedef typename RegistryTag::BaseClass BaseClass;
public:
    ClassRegister(const std::string& entry_name,
        typename ClassRegistry<BaseClass>::CtorPtr ctor)
    {
        ClassRegistryInstance<RegistryTag>().RegisterClass(entry_name, ctor);
    }

    ~ClassRegister() {}
};

// 创建一个SubClass对象，并返回BaseClass指针
template <typename BaseClass, typename SubClass>
BaseClass* ClassRegistry_NewObject()
{
    return new SubClass();
}

} // namespace util

// 为一个base_class定义一个注册类
//
// 第一个参数registry_name必须是全局惟一的
// 一个base_class可以有多个注册类，使用registry_name定义
//
// 这个宏应该和base_class在同一个命名空间中
#define CLASS_REGISTRY_DEFINE(registry_name, base_class) \
    struct registry_name##RegistryTag : \
        public ::util::RegistryTagBase<base_class> {};

#define CLASS_REGISTRY_REGISTER_CLASS(registry_name, base_class, \
                                      entry_name, class_name) \
    static ::util::ClassRegister<registry_name##RegistryTag> \
        MACRO_CONS(g_register_##class_name, __LINE__) ( \
            entry_name, \
            &::util::ClassRegistry_NewObject<base_class, class_name>)

#define CLASS_REGISTRY_CREATE_OBJECT(registry_name, entry_name) \
    ::util::ClassRegistryInstance<registry_name##RegistryTag>().CreateObject(entry_name)

#define CLASS_REGISTRY_CLASS_COUNT(registry_name) \
    ::util::ClassRegistryInstance<registry_name##RegistryTag>().RegisterCount()

#define CLASS_REGISTRY_CLASS_NAME(registry_name, i) \
    ::util::ClassRegistryInstance<registry_name##RegistryTag>().ClassName(i)

#endif
