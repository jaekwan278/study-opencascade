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

// 면적 계산 함수
double calculateFaceArea(const TopoDS_Face& face) {
    GProp_GProps props;
    BRepGProp::SurfaceProperties(face, props);
    return props.Mass();
}

// 법선 벡터 계산 함수
gp_Vec calculateFaceNormal(const TopoDS_Face& face) {
    BRepAdaptor_Surface surface(face);
    gp_Pnt p1, p2, p3;
    surface.D0(surface.FirstUParameter(), surface.FirstVParameter(), p1);
    surface.D0(surface.LastUParameter(), surface.FirstVParameter(), p2);
    surface.D0(surface.FirstUParameter(), surface.LastVParameter(), p3);

    gp_Vec v1(p1, p2);
    gp_Vec v2(p1, p3);
    return v1.Crossed(v2).Normalized();
}

// 면의 모든 꼭짓점 좌표를 얻는 함수
std::vector<gp_Pnt> getFaceVertices(const TopoDS_Face& face) {
    std::vector<gp_Pnt> vertices;
    for (TopExp_Explorer vertexExp(face, TopAbs_VERTEX); vertexExp.More(); vertexExp.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(vertexExp.Current());
        vertices.push_back(BRep_Tool::Pnt(vertex));
    }
    return vertices;
}

// 변환 전후의 좌표를 출력하는 함수
void printVertices(const std::vector<gp_Pnt>& vertices, const std::string& label) {
    std::cout << label << std::endl;
    for (const auto& point : vertices) {
        std::cout << std::fixed << std::setprecision(1)
            << "Vertex: (" << point.X() << ", " << point.Y() << ", " << point.Z() << ")" << std::endl;
    }
}

// XY 평면에 맞추는 변환 생성 함수
gp_Trsf alignToXYPlane(const gp_Vec& faceNormal) {
    gp_Vec targetNormal(0, 0, 1);
    gp_Trsf transform;

    if (!faceNormal.IsParallel(targetNormal, Precision::Angular())) {
        gp_Vec rotationAxis = faceNormal.Crossed(targetNormal);
        rotationAxis.Normalize();
        double rotationAngle = faceNormal.AngleWithRef(targetNormal, rotationAxis);
        transform.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(rotationAxis)), rotationAngle);
    }
    return transform;
}

// 가장 긴 변을 x축에 맞추는 회전 변환 생성 함수
gp_Trsf alignLongestEdgeToX(const std::vector<gp_Pnt>& vertices) {
    double maxLength = 0.0;
    gp_Vec longestEdgeDir;

    for (size_t i = 0; i < vertices.size(); ++i) {
        gp_Vec edge(vertices[i], vertices[(i + 1) % vertices.size()]);
        double length = edge.Magnitude();
        if (length > maxLength) {
            maxLength = length;
            longestEdgeDir = edge.Normalized();
        }
    }

    gp_Vec xAxis(1, 0, 0);
    gp_Trsf transform;

    if (!longestEdgeDir.IsParallel(xAxis, Precision::Angular())) {
        gp_Vec rotationAxis = longestEdgeDir.Crossed(xAxis);
        rotationAxis.Normalize();
        double rotationAngle = longestEdgeDir.AngleWithRef(xAxis, rotationAxis);
        transform.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(rotationAxis)), rotationAngle);
    }
    return transform;
}

// 변환된 형상을 STEP 파일로 저장하는 함수
void saveAsStepFile(const TopoDS_Shape& shape, const std::string& filePath) {
    STEPControl_Writer writer;
    writer.Transfer(shape, STEPControl_AsIs);
    if (writer.Write(filePath.c_str()) != IFSelect_RetDone) {
        std::cerr << "STEP 파일 저장 실패: " << filePath << std::endl;
    }
    else {
        std::cout << "STEP 파일 저장 성공: " << filePath << std::endl;
    }
}

// STEP 파일 처리
void processStepFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    STEPControl_Reader reader;
    if (reader.ReadFile(inputFilePath.c_str()) != IFSelect_RetDone) {
        std::cerr << "STEP 파일을 읽을 수 없습니다: " << inputFilePath << std::endl;
        return;
    }

    reader.TransferRoot();
    TopoDS_Shape shape = reader.OneShape();
    TopoDS_Face largestFace;
    double largestArea = 0.0;

    // 가장 큰 면 찾기
    for (TopExp_Explorer faceExplorer(shape, TopAbs_FACE); faceExplorer.More(); faceExplorer.Next()) {
        TopoDS_Face face = TopoDS::Face(faceExplorer.Current());
        double area = calculateFaceArea(face);
        if (area > largestArea) {
            largestArea = area;
            largestFace = face;
        }
    }

    // 변환 전 법선 벡터 출력
    gp_Vec initialNormal = calculateFaceNormal(largestFace);
    std::cout << "변환 전 법선 벡터: (" << initialNormal.X() << ", " << initialNormal.Y() << ", " << initialNormal.Z() << ")" << std::endl;

    // 변환 전 꼭짓점 좌표 출력
    std::vector<gp_Pnt> originalVertices = getFaceVertices(largestFace);
    printVertices(originalVertices, "변환 전 꼭짓점 좌표:");

    // XY 평면에 맞추는 변환 적용
    gp_Trsf xyAlignTransform = alignToXYPlane(initialNormal);
    BRepBuilderAPI_Transform xyAlignTransformer(shape, xyAlignTransform);
    TopoDS_Shape xyAlignedShape = xyAlignTransformer.Shape();

    // XY 평면에 맞춰진 후 꼭짓점 좌표 가져오기
    std::vector<gp_Pnt> xyAlignedVertices;
    for (const auto& vertex : originalVertices) {
        xyAlignedVertices.push_back(vertex.Transformed(xyAlignTransform));
    }

    // X축에 맞추는 변환 적용
    gp_Trsf xAlignTransform = alignLongestEdgeToX(xyAlignedVertices);
    BRepBuilderAPI_Transform xAlignTransformer(xyAlignedShape, xAlignTransform);
    TopoDS_Shape finalAlignedShape = xAlignTransformer.Shape();

    // 최종 변환 후 STEP 파일로 저장
    saveAsStepFile(finalAlignedShape, outputFilePath);

    // 변환 후 꼭짓점 좌표
    std::vector<gp_Pnt> finalVertices;
    for (const auto& vertex : xyAlignedVertices) {
        finalVertices.push_back(vertex.Transformed(xAlignTransform));
    }
    printVertices(finalVertices, "변환 후 꼭짓점 좌표:");

    // 최종 법선 벡터 계산 및 출력
    gp_Vec finalNormal = calculateFaceNormal(largestFace);
    std::cout << "변환 후 법선 벡터: (" << finalNormal.X() << ", " << finalNormal.Y() << ", " << finalNormal.Z() << ")" << std::endl;
}

int main() {
    std::string inputFilePath = "/Users/jaekwan/WorkSpace/Study/OCCT/study-opencascade/testFile/basic.step";
    std::string outputFilePath = "result.step";
    processStepFile(inputFilePath, outputFilePath);
    return 0;
}
