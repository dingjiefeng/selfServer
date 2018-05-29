//
// Created by jeff on 18-5-29.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../server/base/FileUtil.h"


using testing::Eq;

class FileUtilTest : public testing::Test{
public:
protected:
    FileUtilTest()
            : mp_appFile(new AppendFile("/home/jeff/Desktop/selfServer/Tests/outputFile.txt"))
    {

    };

    void SetUp() override {
        std::cout << "test of FileUtil begin ..." << std::endl;
    }

    std::unique_ptr<AppendFile> mp_appFile;
};


TEST_F(FileUtilTest, example){
    std::string str("this a string text of the demo\n");
    mp_appFile->append(str.c_str(), str.size());
    mp_appFile->append(str.c_str(), str.size());
    mp_appFile->append(str.c_str(), str.size());
    mp_appFile->flush();

}