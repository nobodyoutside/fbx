#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtGui/qmatrix4x4.h>
#include "FbxLoader.h"

// Qt5OpenGL 라이브러리가 없는 환경에서 컴파일을 허용하기 위한 매크로 정의
#ifndef QT_OPENGL_LIB
#define QT_OPENGL_LIB
#endif

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLTexture;

/// @brief 3D 렌더링 뷰 모드를 나타내는 열거형
enum class ViewMode {
    Normal,       // 기본 셰이딩 모드
    VertexColor,  // 버텍스 컬러 표시 모드
    VertexNormal, // 법선 벡터 시각화 모드
    Texture       // 텍스처 매핑 표시 모드
};

/// @brief FBX 메시 데이터를 렌더링하는 위젯.
/// 현재는 Qt5OpenGL이 없어 플레이스홀더(검정 배경 + 텍스트)로 동작한다.
class ViewerWidget : public QWidget {
    Q_OBJECT
public:
    /// @brief 뷰어 위젯을 초기화한다. (기본 모드: Normal, 셰이더 프로그램: nullptr)
    ViewerWidget(QWidget* parent = nullptr);

    /// @brief 소멸자. OpenGL 리소스가 있으면 해제한다. (현재 플레이스홀더이므로 빈 구현)
    ~ViewerWidget();

    /// @brief 외부에서 로드된 메시 데이터를 받아 저장하고 화면을 갱신한다.
    /// @param meshes 표시할 메시 데이터 목록
    void setMeshes(const std::vector<MeshData>& meshes);

    /// @brief 렌더링 뷰 모드를 변경하고 화면을 갱신한다.
    /// @param mode 새로 적용할 뷰 모드
    void setViewMode(ViewMode mode);

protected:
    /// @brief 위젯의 페인트 이벤트 핸들러.
    /// 현재 플레이스홀더 구현: 검정 배경에 "OpenGL Missing" 메시지를 그린다.
    void paintEvent(QPaintEvent* event) override;

    /// @brief 위젯 크기 변경 시 호출되어 화면을 다시 그린다.
    void resizeEvent(QResizeEvent* event) override;

private:
    std::vector<MeshData> mMeshes;      // 표시할 메시 데이터 목록
    ViewMode mMode;                     // 현재 렌더링 뷰 모드
    QOpenGLShaderProgram* mProgram;     // OpenGL 셰이더 프로그램 (현재 미사용)

    /// @brief GPU에 업로드된 메시별 OpenGL 리소스를 보관하는 구조체 (현재 플레이스홀더)
    struct MeshGL {
        void* vbo_ptr;              // VBO 포인터 (플레이스홀더)
        void* ibo_ptr;              // IBO 포인터 (플레이스홀더)
        QOpenGLTexture* texture;    // 텍스처 객체 포인터
        int indexCount;             // 인덱스 개수
    };
    std::vector<MeshGL> mMeshGLs;       // 메시별 GPU 리소스 목록
};

#endif // VIEWERWIDGET_H
