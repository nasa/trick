TEST_WCHAR.c = alloc(2);
TEST_WCHAR.w = alloc(2);
int i, j;
for(i = 0; i < 2; i++) {
    TEST_WCHAR.w[i] = alloc(2);
    for (j = 0; j < 2; j++) {
        TEST_WCHAR.w[i][j] = alloc(2);
    }
}

TEST_WCHAR.c[0] = "Normal", "String";
//TEST_WCHAR.w[0][0][0] = "Wide", "String";
TEST_WCHAR.w[0][0][0] = 'W', 'S';
