#include "../pch.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace Hax;

class ConfigTest : public ::testing::Test 
{
public:
    void SetUp() override 
    {
        IniAddEntry(File, "Section1", "VariableInt", &FieldInt, IniFileWrite_Int, IniFileRead_Int);
        IniAddEntry(File, "Section1", "VariableFloat", &FieldFloat, IniFileWrite_Float, IniFileRead_Float);
        IniAddEntry(File, "Section2", "VariableBool", &FieldBool, IniFileWrite_Bool, IniFileRead_Bool);
    }

    void TearDown() override 
    {
        ::DeleteFileW(File.FileName);
    }

    IniFile File{L"test_config.ini"};
    int FieldInt = 90;
    float FieldFloat = 33.f;
    bool FieldBool = true;
};

TEST_F(ConfigTest, SaveValues) 
{
    IniSave(File);

    const char expectedContent[] =
        "[Section1]\r\n"
        "VariableInt=90\r\n"
        "VariableFloat=33.000\r\n"
        "[Section2]\r\n"
        "VariableBool=1\r\n";
    
    HANDLE hFile = ::CreateFileW(File.FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    EXPECT_NE(hFile, INVALID_HANDLE_VALUE);

    DWORD fileSize = ::GetFileSize(hFile, NULL);
    char* buffer = new char[fileSize]();
    DWORD bytesRead;

    EXPECT_TRUE(ReadFile(hFile, buffer, fileSize, &bytesRead, NULL) && bytesRead == fileSize);
    EXPECT_TRUE(memcmp(buffer, expectedContent, fileSize) == 0);

    ::CloseHandle(hFile);
    delete[] buffer;
}

TEST_F(ConfigTest, LoadValues) 
{
    const char content[] =
        "[Section1]\r\n"
        "VariableInt=10\r\n"
        "VariableFloat=99.000\r\n"
        "[Section2]\r\n"
        "VariableBool=0\r\n";

    HANDLE hFile = ::CreateFileW(File.FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    ::WriteFile(hFile, content, sizeof(content), nullptr, NULL);
    ::CloseHandle(hFile);

    IniLoad(File);
    EXPECT_EQ(FieldInt, 10);
    EXPECT_FLOAT_EQ(FieldFloat, 99.f);
    EXPECT_EQ(FieldBool, 0);
}