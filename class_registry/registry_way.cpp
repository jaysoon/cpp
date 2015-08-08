#include "class_registry.h"

#include <iostream>
using namespace std;

class PlaqueProcessor
{
public: 
    PlaqueProcessor(const string& n) : name_(n)
    { }

    virtual ~PlaqueProcessor()
    { }

    virtual string Process(const string& text) = 0;

private:
    string name_;
};

CLASS_REGISTRY_DEFINE(plaque_processor_registry, PlaqueProcessor);

#define REGISTER_PROCESSOR(processor_name) \
    CLASS_REGISTRY_REGISTER_CLASS( \
        plaque_processor_registry, PlaqueProcessor, #processor_name, processor_name)

#define CREATE_PROCESSOR(processor_name_string) \
    CLASS_REGISTRY_CREATE_OBJECT(plaque_processor_registry, processor_name_string)


class Capitalize : public PlaqueProcessor
{
public:
    Capitalize() : PlaqueProcessor("capitalize")
    { 
        cout << "Create Capitalize Processor" << endl;
    }

    string Process(const string& text)
    {
        if (islower(text[0])) {
            return string(1, char(toupper(text[0]))) + text.substr(1);
        }
        return text;
    }
};

REGISTER_PROCESSOR(Capitalize);


class AppendDollar : public PlaqueProcessor
{
public:
    AppendDollar() : PlaqueProcessor("append_dollar")
    {
        cout << "Create AppendDollar Processor" << endl;
    }

    string Process(const string& text)
    {
        return text + "$";
    }
};
REGISTER_PROCESSOR(AppendDollar);

class DashString : public PlaqueProcessor
{
public:
    DashString() : PlaqueProcessor("dash_string")
    {
        cout << "Create DashString Processor" << endl;
    }

    string Process(const string& text) 
    {
        string dash_text = "";
        for (int i=0; i < text.length(); ++i) {
            dash_text += string("-") + text[i];
        }
        return dash_text.substr(1);
    }
};
REGISTER_PROCESSOR(DashString);

int main(int argc, char* argv[])
{

    string text = "helloworld";
    const int processor_count = 3;
    string processor_names[] = {"Capitalize", "DashString", "AppendDollar"};
    for (int i=0; i < processor_count; ++i) 
    {
        string processor_name = processor_names[i];
        PlaqueProcessor* processor = CREATE_PROCESSOR(processor_name);
        text = processor->Process(text);
    }
    cout << "Plaque Sketch:" << text << endl;

    // size_t count = CLASS_REGISTRY_CLASS_COUNT(plaque_processor_registry);
    // for (int i=0; i < count; ++i)
    // {
    //     cout << CLASS_REGISTRY_CLASS_NAME(plaque_processor_registry, i) << endl;
    // }
    return 0;
}
