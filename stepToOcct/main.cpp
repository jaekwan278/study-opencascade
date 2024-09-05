#include <iostream>
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Standard_Mutex.hxx>
#include <XSControl_Reader.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>

using namespace std;

int main(int argc, const char* argv[]){
    const string fileDir = "/Users/jaekwan/WorkSpace/Cpp/opencascadeTest/study-opencascade/testFile/";

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

        if(!Interface_Static::SetIVal("read.step.product.context", 1)){
        cout << "Error : 데이터 읽기형태 오류" << endl;
        return 1;
        }

        if(!reader.TransferRoot() > 0 ){
            reader.PrintCheckTransfer(false, mode);
        return 1;
        }

        TopoDS_Shape shape = reader.OneShape();
        // Shape객체 내부 구성요소 탐색
        TopExp_Explorer faceFinder;

        // TopAbs_FACE 형태(면)의 개체를 shape에서 추출
        for(faceFinder.Init(shape, TopAbs_FACE); faceFinder.More(); faceFinder.Next()){
            // 조건에 맞는 개체를 TopoDS_FACE로 다운캐스트
            const TopoDS_Face& face = TopoDS::Face(faceFinder.Current());
            // Face객체 내부 구성요소 탐색
            TopExp_Explorer edgeFinder(face, TopAbs_EDGE);
            for(;edgeFinder.More();edgeFinder.Next()){

                const TopoDS_Edge& edge = TopoDS::Edge(edgeFinder.Current());

                // edge개체에서 곡선의 정보를 추출, first와 last에 매개변수의 범위를 반환 받음
                Standard_Real first, last;
                Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);

                cout << "first : " << first << ", last : " << last << endl;

                // 곡선이 null일 경우 패스
                if(curve.IsNull()){
                    continue;
                }
                // 타입이 Geom_Circle(원형)과 일치하는지 확인
                if(curve->IsKind(STANDARD_TYPE(Geom_Circle))){
                    // Geom_Curve 객체를 Geom_Circle로 다운캐스트
                    Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(curve);
                    // circle 객체에서 기하학적 정보만 추출(gp_Circ 클래스 : 중심점, 반지름, 원이 위치한 평면(방향?))
                    gp_Circ cirInfo = circle->Circ();

                    cout << "Circle's Radius : " << cirInfo.Radius() << endl;
                }

            }
        }

    }else{
        reader.PrintCheckLoad(false, mode);
        return 1;
    }

    return 0;
}

// 면의 기하학적 정보 추출
// Handle(Geom_Surface) surface = BRep_Tool::Surface(face);
// IsKind : 객체가 특정 클래스의 인스턴스인지 확인.
// Geom_Surface : 기하학적 표면을 표현 가능한 객체