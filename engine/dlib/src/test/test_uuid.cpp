#include <stdint.h>
#include <stdio.h>
#include <string>
#include <map>
#include <gtest/gtest.h>
#include "../dlib/uuid.h"

TEST(dmUUID, Linkage)
{
    dmUUID::UUID uuid;
    memset(&uuid, 0, sizeof(uuid));
    dmUUID::Generate(&uuid);

    printf("{");
    for (int i = 0; i < 16; ++i)
    {
        printf("%x", uuid.m_UUID[i]);
        if (i < 15)
            printf(", ");
    }
    printf("}\n");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
