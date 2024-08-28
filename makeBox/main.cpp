#include <iostream>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <STEPControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Interface_Static.hxx>

int main(){
    // 시작
    std::cout << "프로그램 시작" << std::endl;

    double length = 10.0, width = 10.0, height = 10.0;
    // 박스 생성
    std::cout << "박스 생성 : " << length << "x" << width << "x" << height << std::endl;  
    TopoDS_Shape box = BRepPrimAPI_MakeBox(length, width, height).Shape();

    // step 파일로 저장
    STEPControl_Writer writer;
    std::cout << "step writer 설정" << std::endl;
    Interface_Static::SetCVal("write.step.scheme", "AP214");

    writer.Transfer(box, STEPControl_AsIs);
    std::cout << "변환 성공" << std::endl;

    // write 호출 및 확인
    std::cout << "basicSquare.step 생성" << std::endl;
    IFSelect_ReturnStatus status = writer.Write("basicSquare.step");

    // 상태에 따른 결과 반환
    if(status == IFSelect_RetDone){
        std::cout << "CREATE SUCCESS !@!@" << std::endl;
    }else {
        std::cerr << "ERROR @!@! code : " << status << std::endl;
    }

    std::cout << "프로그램 종료" << std::endl;
    return 0;
}

