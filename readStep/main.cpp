#include <iostream>
#include <STEPControl_Reader.hxx>

// 1. Step 파일 읽기

using namespace std;
int main(){
    // 파일 경로
    Standard_CString filename = "";
    // 파일읽기 관련 에러 메시지 mode : STEP 엔티티당 모든 메시지의 순차적 목록 제공.
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;

    // 읽기
    STEPControl_Reader reader;
    IFSelect_ReturnStatus status = reader.ReadFile(filename);

    // 파일 읽기 관련 에러 메시지, ()
    reader.PrintCheckLoad(true, mode);

    return 0;
}
