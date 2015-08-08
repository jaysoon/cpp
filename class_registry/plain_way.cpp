#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class PlaqueProcessor
{
public: 
    PlaqueProcessor(const string& n) : name_(n)
    { }

    virtual PlaqueProcessor()
    { }

    virtual string Process(const string& text) = 0;

private:
    string name_;
};

class Capitalize : public PlaqueProcessor
{
public:
    Capitalize() : PlaqueProcessor("capitalize")
    { 
        cout << "Create Capitalize Processor" << endl;
    }

    string Process(const string& text)
    {
        if (islower(text[0])) 
        {
            return string(1, char(toupper(text[0]))) + text.substr(1);
        }
        return text;
    }
};

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
        for (int i=0; i < text.length(); ++i) 
        {
            dash_text += string("-") + text[i];
        }
        return dash_text.substr(1);
    }
};


typedef map<string, PlaqueProcessor*> NameProcessorMap;
const NameProcessorMap& CreateProcessorManager()
{
    static NameProcessorMap processor_manager;
    processor_manager["Capitalize"] = new Capitalize();
    processor_manager["AppendDollar"] = new AppendDollar();
    processor_manager["DashString"] = new DashString();
    return processor_manager;
}

int main(int argc, char* argv)
{
    const NameProcessorMap& processor_manager = CreateProcessorManager();

    string text = "helloworld";
    const int processor_count = 3;
    string processor_names[] = {"Capitalize", "DashString", "AppendDollar"};
    for (int i=0; i < processor_count; ++i)
    {
        string processor_name = processor_names[i];
        NameProcessorMap::const_iterator citer = processor_manager.find(processor_name);
        if (citer == processor_manager.end())
            continue;

        PlaqueProcessor* processor = citer->second;
        text = processor->Process(text);
    }
    cout << "Plaque Sketch:" << text << endl;
    return 0;
}
