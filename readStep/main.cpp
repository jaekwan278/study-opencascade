#include <iostream>
#include <STEPControl_Reader.hxx>
#include <fstream>

/*
1. Step 파일 읽기 (AP203, AP214 우선 구현.)
    = Step >> OCCT SHAPE >> OCAF?
*/
using namespace std;
int main(){
    STEPControl_Reader reader;
    
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;

    IFSelect_ReturnStatus status = reader.ReadFile("basicSquare.step");
    
    reader.PrintCheckLoad(false, mode);
}
