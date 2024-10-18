#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <set>

// 스텝 파일 리더
TopoDS_Shape processStepFile(const std::string &inputFilePath, const std::string &outputFilePath)
{
    STEPControl_Reader reader;
    if (reader.ReadFile(inputFilePath.c_str()) != IFSelect_RetDone)
    {
        std::cerr << "STEP 파일을 읽을 수 없습니다: " << inputFilePath << std::endl;
        throw std::runtime_error("step 파일 읽기 실패");
    }

    reader.TransferRoot();
    TopoDS_Shape shape = reader.OneShape();

    return shape;
}

// 넓이 계산 함수
double calculateFaceArea(const TopoDS_Face &face)
{
    GProp_GProps props;
    BRepGProp::SurfaceProperties(face, props);
    return props.Mass();
}

// 변환된 형상을 STEP 파일로 저장하는 함수
void saveAsStepFile(const TopoDS_Shape &shape, const std::string &filePath)
{
    STEPControl_Writer writer;
    writer.Transfer(shape, STEPControl_AsIs);
    if (writer.Write(filePath.c_str()) != IFSelect_RetDone)
    {
        std::cerr << "STEP 파일 저장 실패: " << filePath << std::endl;
    }
    else
    {
        std::cout << "STEP 파일 저장 성공: " << filePath << std::endl;
    }
}

// 면의 모든 꼭짓점 좌표를 얻는 함수
// * 꼭짓점 좌표가 8개가 나옴.(테스트로 사용한 basic.step 파일에서는 각각 4개의 좌표에 대해서 같은 좌표가 한번 더 출력.)
// * 1. 로직 문제 (?...X)
// * 2. (사각형 예시)face객체가 면을 이루는 4개의 좌표 + 다른 면과 관계된 4개의 좌표를 가지고 있을 경우. (?)
std::vector<gp_Pnt> getFaceVertices(const TopoDS_Face &face)
{
    std::vector<gp_Pnt> vertices;
    for (TopExp_Explorer vertexExp(face, TopAbs_VERTEX); vertexExp.More(); vertexExp.Next())
    {
        TopoDS_Vertex vertex = TopoDS::Vertex(vertexExp.Current());
        vertices.push_back(BRep_Tool::Pnt(vertex));
    }
    return vertices;
}

TopoDS_Shape moveModelToBase(TopoDS_Shape shape, gp_Pnt &referencePnt)
{
    gp_Vec translationVec = gp_Vec(referencePnt.X(), referencePnt.Y(), referencePnt.Z());

    gp_Trsf transformation;
    transformation.SetTranslation(translationVec);

    BRepBuilderAPI_Transform transform(shape, transformation);
    shape = transform.Shape();
    return shape;
}

// TopoDS_Shape 객체에서 모든 gp_Pnt(점의 좌표)객체를 얻기 위한 함수
std::vector<gp_Pnt> GetVerticesFromShape(const TopoDS_Shape &shape)
{
    std::vector<gp_Pnt> points;

    for (TopExp_Explorer explorer(shape, TopAbs_VERTEX); explorer.More(); explorer.Next())
    {
        TopoDS_Vertex vertex = TopoDS::Vertex(explorer.Current());

        gp_Pnt pnt = BRep_Tool::Pnt(vertex);
        points.push_back(pnt);
    }
    return points;
}

// 변환 전후의 좌표를 출력하는 함수
void printVertices(const std::vector<gp_Pnt> &vertices, const std::string &label)
{
    std::cout << label << std::endl;
    for (const auto &point : vertices)
    {
        std::cout << std::fixed << std::setprecision(1)
                  << "Vertex: (" << point.X() << ", " << point.Y() << ", " << point.Z() << ")" << std::endl;
    }
}

// Step 파일로 변환
void saveObjectToStep(const TopoDS_Face &face, const std::string &filename)
{
    STEPControl_Writer writer;
    writer.Transfer(face, STEPControl_AsIs);
    if (writer.Write(filename.c_str()))
    {
        std::cout << "Save face to " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error saving face to " << filename << std::endl;
    }
}

// gp_Pnt 객체를 set에 담기위한 규칙
struct CompareGp_Pnt
{
    bool operator()(const gp_Pnt &a, const gp_Pnt &b) const
    {
        if (a.X() != b.X())
            return a.X() < b.X();
        if (a.Y() != b.Y())
            return a.Y() < b.Y();
        return a.Z() < b.Z();
    }
};

// 소숫점 두자리까지 허용하는 형식으로 변환.
double roundTwoDecimal(double value)
{
    return std::round(value * 100.0) / 100.0;
}

// 가장 큰 넓이를 가지고 있는 한 쌍의 면을 담은 배열 리턴
std::vector<TopoDS_Face> findLargestFaces(TopoDS_Shape &shape)
{
    // 가장 큰 넓이를 가지고 있는 복수의 면을 담을 배열
    std::vector<TopoDS_Face> largestFaces;
    double largestArea = 0.0;

    // 가장 큰 면 찾기
    for (TopExp_Explorer faceExplorer(shape, TopAbs_FACE); faceExplorer.More(); faceExplorer.Next())
    {
        TopoDS_Face face = TopoDS::Face(faceExplorer.Current());

        double area = roundTwoDecimal(calculateFaceArea(face));
        largestArea = roundTwoDecimal(largestArea);

        if (area > largestArea)
        {
            largestArea = area;           // 순차적 비교
            largestFaces.clear();         // 기존 면 삭제
            largestFaces.push_back(face); // 새로운 면 추가
        }
        else if (area == largestArea)
        { // 반올림하여 소숫점 둘째자리까지 표현한 값이 일치하는지 확인.
            // 동일한경우 vector 배열에 추가 삽입
            largestFaces.push_back(face);
        }
    }

    return largestFaces;
}

gp_Pnt findBaseFace(const gp_Pnt &pointA, const gp_Pnt &pointB)
{
    double aX = roundTwoDecimal(pointA.X());
    double aY = roundTwoDecimal(pointA.Y());
    double aZ = roundTwoDecimal(pointA.Z());

    double bX = roundTwoDecimal(pointB.X());
    double bY = roundTwoDecimal(pointB.Y());
    double bZ = roundTwoDecimal(pointB.Z());

    if (aX != bX && aY == bY && aZ == bZ)
    {
        std::cout << "USED X" << std::endl;
        if (aX > bX)
        {
            return pointB;
        }
        else
        {
            return pointA;
        }
    }
    else if (aX == bX && aY != bY && aZ == bZ)
    {
        std::cout << "USED Y" << std::endl;
        if (aY > bY)
        {
            return pointB;
        }
        else
        {
            return pointA;
        }
    }
    else if (aX == bX && aY == bY && aZ != bZ)
    {
        std::cout << "USED Z" << std::endl;
        if (aZ > bZ)
        {
            return pointB;
        }
        else
        {
            return pointA;
        }
    }
    else
    {
        std::cerr << "Get Thick : 판단할 수 없는 구조." << std::endl;
        return gp_Pnt(0, 0, 0);
    }
}

// 두께 구하기, 두개의 대칭되는 면에대한 좌표에서 불일치하는 하나의 좌표를 찾고 그 차이를 리턴.
// (x,y,z 중 임의의 2개 좌표는 일치할거라고 예상.)
double getThickFromTwoFaces(const gp_Pnt &pointA, const gp_Pnt &pointB)
{

    double aX = roundTwoDecimal(pointA.X());
    double aY = roundTwoDecimal(pointA.Y());
    double aZ = roundTwoDecimal(pointA.Z());

    double bX = roundTwoDecimal(pointB.X());
    double bY = roundTwoDecimal(pointB.Y());
    double bZ = roundTwoDecimal(pointB.Z());

    if (aX != bX && aY == bY && aZ == bZ)
    {
        std::cout << "USED X" << std::endl;
        if (aX > bX)
        {
            return aX - bX;
        }
        else
        {
            return bX - aX;
        }
    }
    else if (aX == bX && aY != bY && aZ == bZ)
    {
        std::cout << "USED Y" << std::endl;
        if (aY > bY)
        {
            return aY - bY;
        }
        else
        {
            return bY - aY;
        }
    }
    else if (aX == bX && aY == bY && aZ != bZ)
    {
        std::cout << "USED Z" << std::endl;
        if (aZ > bZ)
        {
            return aZ - bZ;
        }
        else
        {
            return bZ - aZ;
        }
    }
    else
    {
        std::cerr << "Get Thick : 판단할 수 없는 구조." << std::endl;
        return 0.0;
    }
}

gp_Pnt toBaseCalc(gp_Pnt &point)
{
    double x = 0.0 - point.X();
    double y = 0.0 - point.Y();
    double z = 0.0 - point.Z();

    return gp_Pnt(x, y, z);
}

int main()
{
    std::string inputFilePath = "/Users/jaekwan/WorkSpace/Project/project-Cadify-Cpp/testFile/1.5T.step";
    std::string outputFilePath = "result.step";
    // 파일 읽기 & 쓰기
    // return : shape 객체
    TopoDS_Shape shape = processStepFile(inputFilePath, outputFilePath);

    std::vector<TopoDS_Face> largestFaces = findLargestFaces(shape);

    // * 한 쌍 이상의 면이 동일한 면적을 가질 경우(하나 이상의 절곡이 있고 평평한 면 두개 이상이 동일한 크기일 경우.)
    // * 가장 앞에 저장된 임의의 한 쌍의 면에 대해서 로직 수행.
    // * set 배열을 사용해 중복된 값을 제거하고,
    // * set 배열을 정렬할 기준을 생성하여 한 쌍을 기준으로 순서대로 저장되도록 설계. (하나의 면을 기준으로 대칭되는 면, 두개의 좌표가 같고 하나의 좌표만이 다른 쌍)

    for (int i = 0; i < largestFaces.size(); i++)
    {
        TopoDS_Face &face = largestFaces[i];
        std::cout << calculateFaceArea(face) << std::endl;
    }

    // 면의 꼭짓점 좌표를
    std::vector<std::set<gp_Pnt, CompareGp_Pnt>> faceVertexSets;

    for (const TopoDS_Face &face : largestFaces)
    {
        // 면의 꼭짓점 좌표를 가져옴
        std::vector<gp_Pnt> vertices = getFaceVertices(face);
        std::set<gp_Pnt, CompareGp_Pnt> singleVerticesSet(vertices.begin(), vertices.end());

        faceVertexSets.push_back(singleVerticesSet);

        // 각각의 면
        std::cout << "Face Vertices: " << std::endl;
        for (const gp_Pnt &point : singleVerticesSet)
        {
            std::cout << std::fixed << std::setprecision(2)
                      << "Vertex: (" << point.X() << ", " << point.Y() << ", " << point.Z() << ")" << std::endl;
        }
    }

    // 한 쌍의 면의 좌표중 첫번째 좌표를 가져와서 비교(4개의 꼭짓점 중 하나)
    const gp_Pnt &pointA = *faceVertexSets[0].begin();
    const gp_Pnt &pointB = *faceVertexSets[1].begin();

    double thick = getThickFromTwoFaces(pointA, pointB);

    gp_Pnt basePoint = findBaseFace(pointA, pointB);

    gp_Pnt referecePnt = toBaseCalc(basePoint);

    TopoDS_Shape resultShape = moveModelToBase(shape, referecePnt);

    largestFaces = findLargestFaces(resultShape);

    for (int i = 0; i < largestFaces.size(); i++)
    {
        TopoDS_Face &face = largestFaces[i];
        std::cout << calculateFaceArea(face) << std::endl;
    }

    for (const TopoDS_Face &face : largestFaces)
    {
        // 면의 꼭짓점 좌표를 가져옴
        std::vector<gp_Pnt> vertices = getFaceVertices(face);
        std::set<gp_Pnt, CompareGp_Pnt> singleVerticesSet(vertices.begin(), vertices.end());

        faceVertexSets.push_back(singleVerticesSet);

        // 각각의 면
        std::cout << "Face Vertices: " << std::endl;
        for (const gp_Pnt &point : singleVerticesSet)
        {
            std::cout << std::fixed << std::setprecision(2)
                      << "Vertex: (" << point.X() << ", " << point.Y() << ", " << point.Z() << ")" << std::endl;
        }
    }

    saveAsStepFile(resultShape, outputFilePath);
}

// TopoDS_Face 객체에 해당하는 모든 2D로 구성된 면(3D모델의 2D구성요소)를 step파일로 내보내기
// int objectIndex = 1;
// std::string filename = "part" + std::to_string(objectIndex) + ".step";
//         saveObjectToStep(face, filename);
//         objectIndex++;

// vector 배열에 저장된 복수의 면적 출력
// for(int i = 0; i < largestFaces.size(); i++){
//         TopoDS_Face& face = largestFaces[i];
//         std::cout << calculateFaceArea(face) << std::endl;
//     }