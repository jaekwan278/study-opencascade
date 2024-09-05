#include <iostream>
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Standard_Mutex.hxx>
#include <XSControl_Reader.hxx>

using namespace std;

int main(int argc, const char* argv[]){
    const string fileDir = "/Users/jaekwan/WorkSpace/Cpp/opencascadeTest/study-opencascade/readStep/testFile/";

    if(argc < 2){
        cerr << "Step File Not Foud" << endl;
        return 1;
    }
    
    string fileName = fileDir + argv[1];
    
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;

    STEPControl_Reader reader;
    IFSelect_ReturnStatus status = reader.ReadFile(fileName.c_str());

    if ( status == IFSelect_RetDone){
        cout << "@ read success" << endl;
    }
    reader.PrintCheckLoad(false, mode);

    if(!Interface_Static::SetIVal("read.step.product.context", 4)){
        cout << "Error : 데이터 읽기형태 오류" << endl;
        return 1;
    }

    Standard_Integer rootOK = reader.TransferRoots();

    Standard_Integer shapeCnt = reader.NbShapes();
    TopoDS_Shape shape = reader.OneShape();

    cout << "Root 개수 : " << rootOK << endl;
    cout << "모양 개수 : " << shapeCnt << endl;
}
