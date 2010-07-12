project(ekaya_keyboards)

add_test(NAME mywin_test COMMAND $<TARGET_FILE:kmfltest> ${PROJECT_SOURCE_DIR}/kmfl/myWin.kmn ${PROJECT_SOURCE_DIR}/kmfl/tests/myWinTest.txt)
#add_test(NAME myanmar3_test COMMAND $<TARGET_FILE:kmfltest> ${PROJECT_SOURCE_DIR}/kmfl/myanmar3std.kmn ${PROJECT_SOURCE_DIR}/kmfl/tests/myanmar3Test.txt)
add_test(NAME sgawkaren_test COMMAND $<TARGET_FILE:kmfltest> ${PROJECT_SOURCE_DIR}/kmfl/SgawKaren.kmn ${PROJECT_SOURCE_DIR}/kmfl/tests/SgawKarenTest.txt)
add_test(NAME pao_test COMMAND $<TARGET_FILE:kmfltest> ${PROJECT_SOURCE_DIR}/kmfl/pa-oh.kmn ${PROJECT_SOURCE_DIR}/kmfl/tests/pa-ohTest.txt)
add_test(NAME anuhkongso_test COMMAND $<TARGET_FILE:kmfltest> ${PROJECT_SOURCE_DIR}/kmfl/aNuHkongso.kmn ${PROJECT_SOURCE_DIR}/kmfl/tests/anuhkongsuTest.txt)
