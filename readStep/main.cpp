#include <iostream>
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>


using namespace std;
int main(){

    Standard_CString filename = "";

    // 파일읽기 관련 에러 메시지 mode : STEP 엔티티당 모든 메시지의 순차적 목록 제공.
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;

    STEPControl_Reader reader;
    // 파일 읽기와 데이터 전송 작업의 상태를 나타내는 열거형.
    IFSelect_ReturnStatus status = reader.ReadFile(filename);

    if ( status == IFSelect_RetDone){
        cout << "@ read success" << endl;
    }else if(status == IFSelect_RetError){
        cout << "@ read fail" << endl;
    }else if(status == IFSelect_RetVoid){
        cout << "empty file" << endl;
    }

    // 파일 읽기 관련 에러 메시지, (true = 실패 메시지만, false = 모든 메시지)
    reader.PrintCheckLoad(false, mode);

    // OCCT 매핑에 사용할 데이터 추출
    // 1. 정밀도 값. ( Interfacd_Static )
    Standard_Integer ic = Interface_Static::IVal("read.precision.mode");

    return 0;
}
