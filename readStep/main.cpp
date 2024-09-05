#include <iostream>
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>


using namespace std;
int main(int argc, const char* argv[]){

    const string fileDir = "/Users/jaekwan/WorkSpace/Cpp/opencascadeTest/study-opencascade/testFile/";

    if(argc < 2){
        cerr << "Step File Not Foud" << endl;
        return 1;
    }
    
    string fileName = fileDir + argv[1];
    
    // 파일읽기 관련 에러 메시지 mode : STEP 엔티티당 모든 메시지의 순차적 목록 제공.
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;

    STEPControl_Reader reader;
    // 파일 읽기와 데이터 전송 작업의 상태를 나타내는 열거형.   
    IFSelect_ReturnStatus status = reader.ReadFile(fileName.c_str());

    if ( status == IFSelect_RetDone){
        cout << "@ read success" << endl;
    }
    // }else if(status == IFSelect_RetError){
    //     cout << "@ read fail" << endl;
    // }else if(status == IFSelect_RetVoid){
    //     cout << "empty file" << endl;
    // }

    // 파일 읽기 관련 에러 메시지, (true = 실패 메시지만, false = 모든 메시지)
    reader.PrintCheckLoad(false, mode);

    // OCCT 매핑에 사용할 데이터 추출
    // 1. 정밀도 값. 
    // STEP 의 UNCERTAINTY_MEASURE_WITH_UNI 내부 LENGTH_MEASURE 값.
    // 수정 필요시 >> 
    //      1. read.percision.mode 값을 1로 설정 (기본값 : 0, STEP 파일에서 가져옴.)
    //      2. read.precision.val 에 원하는 값 설정.
    // read.percision.val : 0.0001
    Standard_Integer precisionMode = Interface_Static::IVal("read.precision.mode");
    Standard_Real precisionVal = Interface_Static::RVal("read.percision.val");

    //2. 모양의 최대 허용 오차.
    // 기본 허용 오차가 적용되지 못하는 상황에서 사용.
    // 모양의 유효성을 유지하기 위해 오차를 조금 더 허용할 수 있는 최대 값을 설정.
    // read.percision.val(기본 허용 오차)보다 작은 값이 될 수 없음.
    // 1
    Standard_Real maxPrecisionVal = Interface_Static::RVal("read.maxprecision.val");
    
    //3. 모양의 유효성 보장.
    // 0 : 최대 허용 오차를 제한으로 사용하지만, 모양의 유효성을 보장하기 위해 최대 허용 오차를 초과할 수 있음. (곡선 같은 경우)
    // 1 : 어떠한 경우에도 최대 허용 오차를 초과할 수 없음.
    // (0)
    Standard_Integer maxPrecisionMode = Interface_Static::IVal("read.maxprecision.mode");

    // cout << "정밀도 모드 :" << rpm << endl;
    // cout << "최대허용 오차 :" << rpm << endl;
    // cout << "모양 유효성 모드 :" << rpm << endl;

    //4. 3D곡선과 pcurve ( 둘다 동일한 물리적 엣지를 나타냄 )이 동일한 파라미터 값을 가지게 함.(혹은 그에대한 허용오차 지정.)
    // BRepLib::SameParameter
    //3D곡선 : 3D 환경에서의 면의 경계(엣지)를 표현.
    //pcurve : 3D도형을 평면에서 보았을때의 엣지를 표현.
    // (0) : 호출되지 않음
    Standard_Integer stdSamePmMode = Interface_Static::IVal("read.stdsameparameter.mode");

    //5. OCCT개체가 참조할 표면의 3D곡선 및 2D곡선에 대한 정의
    // 0 : 두 곡선 모두 사용.
    // 3 : 3D 곡선을 2D 곡선을 다시 작성하는데 사용.
    // (0)
    Standard_Integer surfaceCurveMode = Interface_Static::IVal("read.surfacecurve.mode");

    //6. 하나의 모서리가 두 면에서 공유될 때 모서리의 규칙 ( 두면이 연결된 각도 )
    // 각도의 정밀도 값 0.01 : Ex) 45.067 >> 45.07 로 조정.
    // (0.01)
    Standard_Real regularityAngle = Interface_Static::RVal("read.encoderegularity.angle");

    //7. read.step.resource.name & read.step.sequence
    
    //8. cascade로 변환할 떄 사용할 단위
    // xstep.cascade.unit
    // (mm)

    //9. 번역을 위한 최상위 엔티티 선택 및 어셈블리 구조
    // 1 : PRODUCT_DEFINITION 엔티티는 최상위 엔티티로 인식, 어셈블리 구조 NEXT_ASSEMBLY_USAGE_OCCURRENCE 엔티티로 인식. 
    //     (AP203, AP214, AP209)
    // 0 : 최상위 : SHAPE_DEFINITION_REPRESENTATION, 어셈블리 : CONTEXT_DEPENDENT_SHAPE_REPRESENTATION
    // (1)
    Standard_Integer stepProductMode = Interface_Static::IVal("read.step.product.mode");

    //10. AP209 파일 읽기 관련 설정.
    // 1(all), 2(desing), 3(analysis)
    // AP203, AP214 사용시 desing 반드시 포함 ( 1 or 2 )
    // (1)
    Standard_Integer stepProductContext = Interface_Static::IVal("read.step.product.context");

    //11. step파일 내에 단일 제품에 대한 두 개 이상의 표현(여러개의 PRODUCT_DEFINITION_SHAPE)이 있는경우 모양에 대한 선호 표현 유형 지정
    // 1(전체) – 모든 표현을 번역합니다(두 개 이상인 경우 합성어로 넣습니다).
    // 2(ABSR) - ADVANCED_BREP_SHAPE_REPRESENTATION을 선호합니다. ( BREP : 3D모델을 경계로 정의, 표면과 경계를 설명하여 그 형태를 정의. )
    // 3(MSSR) – MANIFOLD_SURFACE_SHAPE_REPRESENTATION을 선호합니다. ( 연속적이고 봉합된 표면 : 매끄럽고 끊김이 없는 표면 )
    // 4(GBSSR) – 기하학적으로 경계가 있는 표면 모양 표현을 선호합니다.
    // 5(FBSR) – FACETTED_BREP_SHAPE_REPRESENTATION을 선호합니다. ( 형상을 다면체로 나눔, 형상을 삼각형 또는 사각형의 면으로 나누어 표현 )
    // 6(EBWSR) – EDGE_BASED_WIREFRAME_SHAPE_REPRESENTATION을 선호합니다. ( 엣지와 선으로만 표현 )
    // 7(GBWSR) – 기하학적으로 경계된 와이어프레임 모양 표현을 선호합니다.
    // (1)
    Standard_Integer shapeRepr = Interface_Static::IVal("read.step.shape.repr");

    //12. step파일에서 찾은 제품에 대해 어떤 데이터를 읽어야 하는지 지정.
    // 1(전체) - 모양과 하위 어셈블리가 모두 동일한 제품과 연관된 경우, 모두 읽어서 단일 합성으로 넣음. ( 에러 발생 확률 매우 높음 - 이러한 구성은 step표준에서 명확하게 정의되지 않음. )
    // 2(어셈블리) - 부품과 관련된 어셈블리 구조 및 모양만 반환(하위 어셈블리 변환하지 않음.)
    // 3(구조) - 모양이 없는 어셈블리 구조만 반환
    // 4(모양) - 조립 구조를 무시하고 제품과 연관된 모양만 번역 ( * )
    // (1)
    Standard_Integer assemblyLevel = Interface_Static::IVal("read.step.assembly.level");

    //13. 보조 데이터의 변환. (SHAPE_REPRESENTATION_RELATIONSHIP 개체의 정보)
    // 하이브리드 모델:
    //    제품의 형상 데이터가 여러 가지 표현(예: 3D 모델, 2D 도면 등)으로 구성되어 있는 경우, read.step.shape.relationship 매개변수를 통해 특정 표현만 번역하거나 모든 표현을 번역할지 결정할 수 있습니다.
    // 보조 데이터:
    //    STEP 파일에 제품의 주요 형상 정의 외에 추가적인 보조 데이터(예: 물질 정보, 품질 데이터 등)가 포함된 경우, 이 보조 데이터의 번역을 피할 수 있습니다.
    // (1) : 켜짐
    Standard_Integer shapeRelationship = Interface_Static::IVal("read.step.shape.relationship");

    //14. 13과 동일 (SHAPE_ASPECT 개체의 정보) 
    // SHAPE_REPRESENTATION_RELATIONSHIP : 
    //      3D모델과 2D 도면 간의 관계 설정 가능.
    //      형상 표현 유형(면, 곡선, 점 등)을 명시하고 이들 간의 관계를 정의.
    // SHAPE_ASPECT : 
    //      형상 데이터가 여러가지 관점(정면, 측면, 평면 등)에서 제공될 수 있음. ( 3D모델의 특정 측면이 2D도면의 특정 측면에 해당한다고 명시 가능. )
    //      형상 데이터와 관련된 속성(재료속성, 색상, 텍스처 등)을 정의.
    //      추가적인 형상 정보나 설명을 포함.
    // (1) : 켜짐
    Standard_Integer shapeAspect = Interface_Static::IVal("read.step.shape.aspect");

    //15. read.step.constructivegeom.relationship
    // (0) 변환 x

    //16. 테셀레이션 관련 설정 / 테셀레이션 : 복잡한 기하학적 형상을 삼각형 또는 다각형으로 변환하는 기술.
    // (0) 켜짐

    // 변환

    // 변환 가능 루트 개체
    // PRODUCT_DEFINITION
    // NEXT_ASSEMBLY_USAGE_OCCURRENCE
    // SHAPE_DEFINITION_REPRESENTATION
    // SUBTYPES_OF_SHAPE_REPRESENTATION (ONLY_IF_REFERRED_REPRESENTATION_IS_TRANSFERABLE)
    // MANIFOLD_SOLID_BREP
    // BREP_WITH_VOIDS
    // FACETED_BREP
    // FACETED_BREP_AND_BREP_WITH_VOIDS
    // SHELL_BASED_SURFACE_MODEL
    // GEOMETRIC_SET_AND_GEOMETRIC_CURVE_SET
    // MAPPED_ITEM
    // SUBTYPES_OF_FACE_SURFACE (INCLUDING_ADVANCED_FACE)
    // SUBTYPES_OF_SHAPE_REPRESENTATION_RELATIONSHIP
    // CONTEXT_DEPENDENT_SHAPE_REPRESENTATION
    // TESSELLATED_SHAPE_REPRESENTATION
    // TESSELLATED_SHELL
    // TESSELLATED_SOLID
    // TRIANGULATED_FACE
    // COMPLEX_TRIANGULATED_FACE

    // 변환 방법
    // Standard_Boolean ok = reader.TransferRoot(rank) – 순위로 식별된 루트 엔터티를 반환, 루트 엔티티만 처리.
    // Standard_Boolean ok = reader.TransferOne(rank) – 순위로 식별된 엔터티를 반환, 루트가 아닌 다른 엔티티 처리 가능.
    // Standard_Integer num = reader.TransferList(list) – list에 포함된 엔티티 목록을 반환
    // Standard_Integer NbRoots = reader.NbRootsForTransfer() - 모든 전송 가능한 루트를 반환.
    // Standard_Integer num = reader.TransferRoots() – 위와 동일.

    // 변환 결과
    // Standard_Integer num = reader.NbShapes() – 결과에 기록된 모양의 수를 가져옴.
    // TopoDS_Shape shape = reader.Shape(rank) – 1과 NbShapes 사이의 정수인 순위로 식별된 결과를 가져옴.
    // TopoDS_Shape shape = reader.Shape() – 번역의 첫 번째 결과를 가져옴.
    // TopoDS_Shape shape = reader.OneShape() – 모든 결과를 단일 모양으로 가져옵니다.
    // 결과가 없으면 null 모양.
    // 단일 결과의 경우 해당 결과에 특정한 모양.
    // 결과가 여러 개 있는 경우 결과를 나열하는 화합물.
    
    return 0;
}
