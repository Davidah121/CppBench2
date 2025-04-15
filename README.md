# C++Bench2.0
An improved version of my previous attempt [cppBenchmark](https://github.com/Davidah121/cppBenchmark).

# Outline
This approach takes advantage of the compiler flag -finstrument-functions and the attributes [[no_instrument_function]].
This method uses these functions as apposed to the windows verision:
```C
void __cyg_profile_func_enter(void *func, void *callsite);
void __cyg_profile_func_exit(void *func, void *callsite);
```

If you need to use _penter and _pexit instead, you must add an implementation for those directly and retrieve the correct start address.

# Usage
For each type, a serialization and deserialization function must be defined otherwise a default approach will be used.
The default approach is just to write out the raw values in memory which may work fine for many data types.

For some, not all, classes in the standard library that are commonly used, a default serialization function is provided.
These are separated by headers allowing the user to omit these if they have a custom implementation of it.

For classes, either a serialization function can be provided or it can be extended from a SerializableObject. If defined as a
Serializable object, the variables can be passed through to be serialized in a variadic manner.

To ease the extra code need to be written, Preprocessor Macros are used to eliminate extra work for most cases outside of needing
a special method to serialize/deserialize due to pointers or just wanting to keep certain variables private and not passing them through
the getSerializedVariables() function.

# Usage Examples
A clear example is provided of how serialization works, compiles, and runs with the file TestSerialization.cpp along with
build files for convience.

For defining new Serialization functions:
```C++

//Note consists of a pointer. Not known where its set from
//Skipping storing pointer
struct SomeData
{
    long x;
    int y;
    short z;
    char* string;
};

void staticSerializationHandler(Streamable& output, DataFormatter& formatter, const std::string& varName, SomeData& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<SomeData>(), varName, 1);
    staticSerialization(output, formatter, var.x);
    staticSerialization(output, formatter, var.y);
    staticSerialization(output, formatter, var.z);
}

void staticDeserializationHandler(Streamable& input, DataFormatter& formatter, const std::string& varName, SomeData& var)
{
    int64_t elements = formatter.readStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<SomeData>(), varName);
    if(elements != 1)
        return;
    staticDeserialization(output, formatter, var.x);
    staticDeserialization(output, formatter, var.y);
    staticDeserialization(output, formatter, var.z);
}
```

If the data type uses templates:
```C++
//Defining one for std::pair
template<typename T, typename T2>
void staticSerializationHandler(Streamable& output, DataFormatter& formatter, const std::string& varName, std::pair<T, T2>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<std::pair<T, T2>>(), varName, 1);
    staticSerialization(output, formatter, var.first);
    staticSerialization(output, formatter, var.second);
}

template<typename T, typename T2>
void staticDeserializationHandler(Streamable& input, DataFormatter& formatter, const std::string& varName, std::pair<T, T2>& var)
{
    int64_t elements = formatter.readStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<std::pair<T, T2>>(), varName);
    if(elements != 1)
        return;
    staticDeserialization(input, formatter, var.first);
    staticDeserialization(input, formatter, var.second);
}
```

If its a class:
```C++

//Note consists of a pointer. Not known where its set from
//Writing pointer out in this case
class SomeClass : public SerializedObject
{
public:
    SomeClass(){}
    SomeClass(long v1, int v2, short v3, char* v4)
    {
        x = v1;
        y = v2;
        z = v3;
        str = v4;
    }
    ~SomeClass(){}
private:
    long x;
    int y;
    short z;
    char* str = nullptr;

SERIALIZE_CLASS(x, y, z, string)
};

//NO ADDITIONAL WORK NEEDED FOR CLASSES IF THIS IS DONE
```

If its a class with a parent that extends from SerializedObject:
```C++

class SomeClass2 : public SomeClass
{
public:
    SomeClass2(){}
    SomeClass2(SomeData extraStuff)
    {
        myProtectedStuff = extraStuff;
    }
    ~SomeClass2(){}
protected:
    SomeData myProtectedStuff;

SERIALIZE_SUPER_CLASS(SomeClass)
SERIALIZE_CLASS(myProtectedStuff)
};

//NO ADDITIONAL WORK NEEDED FOR CLASSES IF THIS IS DONE
```


If its a class with a parent that extends from something else that isn't a SerializedObject:
```C++

class SomeClass3 : public SerializedObject, public SomeData
{
public:
    SomeClass3(){}
    ~SomeClass3(){}

SERIALIZE_SUPER_CLASS(SomeData)
SERIALIZE_CLASS()
};

//NO ADDITIONAL WORK NEEDED FOR CLASSES IF THIS IS DONE
```

Accessing those variables and its type_info from another function:
```C++
void testFunction(SerializedObject* obj)
{
    if(obj == nullptr)
        return;
    auto vars = obj->getSerializedVariables();
    for(std::pair<std::string, SerializedVariable>& nameVariablePair : vars)
    {
        if(nameVariablePair.second.type == TypeInfo::get<int>())
        {
            *(int*)nameVariablePair.second.data = 32;
        }
    }
}
```

Actual Serialization and Deserialization of data:
```C++
SomeData globalSomeData;
SomeClass globalSomeClass;
std::vector<int> globalIntList;
void initTestingData()
{
    globalSomeData = {12, 32, -2, "Hello Pls"};
    globalSomeClass = SomeClass(100, 1200, 3002, "don't delete me");
    globalIntList = {10000, 20000, 30000};
}
void save()
{
    initTestingData();
    StreamableFile file = StreamableFile("someSerializedDataFile", StreamableFile::TYPE_WRITE);
    RawDataFormatter formatter = RawDataFormatter();
    staticSerialization(file, formatter, globalSomeData);
    staticSerialization(file, formatter, globalSomeClass);
    staticSerialization(file, formatter, globalIntList);
}
void load()
{
    StreamableFile file = StreamableFile("someSerializedDataFile", StreamableFile::TYPE_READ);
    RawDataFormatter formatter = RawDataFormatter();
    staticDeserialization(file, formatter, globalSomeData);
    staticDeserialization(file, formatter, globalSomeClass);
    staticDeserialization(file, formatter, globalIntList);
}
```

# Problems
As you may have noticed, this particular implementation suffers from a few issues that aren't necessarily easy to solve.

Currently, it writes out all data as raw memory values. This has 2 issues.

- Moving from 32 bit systems to 64 bit systems
- Endianness of values between systems
- Pointers

The size of a particular datatype is not guaranteed to be the same across systems which can cause problems when loading data. Using int32_t and int64_t datatypes and similar 
help avoid these issues along with implementing a custom DataFormatter class.

Endianness of data is a potenial problem as well. This can be fixed by implementing a custom DataFormatter class but a programmer must be aware of this.

Pointers are written out as they are. The data that they store is not written out and restoring them has them pointing to junk that is potentially unallocated. Even if it is
there is no guarantee that it was allocated for that purpose. It can easily belong to some other class or function for some other purpose.

As it stands, there isn't a way that fixes all issues for pointers but there is a potential extension that can be added for them.

Looking at the load function provided above. globalSomeData will have a nullptr for its string since we didn't serialize the pointer and globalSomeClass will have a bad pointer.
It will map to some invalid location likely so dereferencing it is highly unsafe.

# Additional Notes
In order to prevent each class from using significantly more memory than it would without using Serialization and Reflection, SerializedObject is a pure virtual function
that passes an unordered_map back with the name of the variables, their memory references, and some type information. This only requires an additional pointer for any class
to bind the proper virtual functions.

Q: Why not use std::any for SerializedVariable instead of a custom structure?

A: Simple. To target eariler version of C++. std::any does provide nice guarantees when casting but it isn't avaiable on versions of C++ earlier than C++17. 

For that same reason, __VA_OPT__ is not the default method for the preprocessor methods FOR_EACH. You'd need C++20 to guarantee that it exists.

Q: Why is USE_PREPROCESSOR_TRICKS there?

A: Intellisense doesn't particularly like the preprocessor FOR_EACH due to how it expands. It will always show an error which can be confusing when looking through existing code
for errors.

For that reason, it can be turned off by defining USE_PREPROCESSOR_TRICKS to 0 and later in compilation defining it as 1.

Q: Is there a solution for pointers?

A: Yes. Check below for a potential solution and why I chose to not implement it.

Q: How does the FOR_EACH macros work?

A: The references I used to implement them are listed in PreprocessorTricks.h but here they are again: [Link1](https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms) and [Link2](https://www.scs.stanford.edu/~dm/blog/va-opt.html)

TLDR; The C preprocessor is almost turing complete (its still a PDA) outside of not having infinite memory and infinite numbers of scans. With some careful definitions, you can create while loops and such.

Note that only 65 variables are allowed to be serialized using the preprocessor which should be more than enough. It can be expanded by changing EVAL.

Just add more to EVAL but the more expansions, the slower intellisense is when trying to expand it out.

Q: Does a class have to extend from SerializedObject?

A: No. The example given for the structure is an example of how you would implement it for a class without extending from SerializedObject.

Note that private variables can't be serialized without some modification to the class. A friend function must be defined to access those.

Q: Why use fopen instead of the C++ approach using fstream and why not fopen_s for security?

Cross platform is why I don't use fopen_s. For not using fstream, I found that the speed of reading is hampered by fstream while the C version doesn't have these issues.

This could easily be a bug in the compiler I used or some other things need to be set to get the full speed. This can easily be changed if desired.

Q: Can this be used to write out and read JSON / XML data formats?

Absolutely. Though not implemented here, the framework for adding those now exists. Using the DataFormatter class, you can write out data however you wish and format it however you wish.

This also allows a programmer to search for data by name and load that way. This is advantageous for managing versions of software that can have variable name changes or additional variables that didn't exist before and not breaking software.

# Solution for storing pointers
There does exist a solution to pointers that can be utilized though it still has issues that can't be easily solved.

- Take a pointer that should be serialized. That pointer, assuming it isn't nullptr, can be deferred meaning that the data it points to isn't stored in a file yet.

- From there, store the pointer like normal. The pointer once deferred goes into a class that keeps a list of all the deferred data. It stores the pointer as a key
and stores the data it points to.

- Once all normal data has been serialized, write out the deferred data. These are written out as a table consisting of the pointer, size, and data.

- When deserializing, read all of the deferred data first. This requires knowledge of where the pointer table would start so that must be saved in a header for the file.

- Then when serializing all of the variables, anytime a pointer is come across, look for it in the pointer table, deserialize it after creating new memory, and store that pointer
in the table so other variables pointing to the same address don't create new memory.


Simple right? Well.... this still has a few issues.

- What if the memory wasn't created by the user but instead created by a different library? Probably shouldn't be
created without going through the proper procedure.

- What if the memory was pointing to an object that hasn't been deserialized yet? Potentially referencing global memory or stack memory.

- This method requires a header which isn't bad but that header points to a pointer table which has to be created later. That makes things complicated if this data is sent across a network.
It would not be able to deserialize till all data has been sent over.

- What if the pointer wasn't meant to be serialized at all? The way the preprocessor SERIALIZE_CLASS works doesn't separate getSerializedVariables() from serialize() and deserialize() meaning
that if you wish to allow other functions to see the pointer, you'd have to implement the functions separately.
  - Note that a separate preprocessor exist for each function so this isn't too much of an issue.

This highlights why I've chosen to avoid any default automatic serialization pointers. In the event that you are serializing pointers, for instance smart pointers, that can be safer and cause no issues
but I'll leave this up to extension for specific use cases.

Leaving pointers as undefined behavior causes developers to assume that pointers will not be stored correctly and to create a method that best suits their needs instead of relying on a one size
fits all approach.
