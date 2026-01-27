#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtGui/qmatrix4x4.h>
#include "FbxLoader.h"

// Fallback types to allow compilation if Qt5OpenGL is missing
#ifndef QT_OPENGL_LIB
#define QT_OPENGL_LIB
#endif

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLTexture;

enum class ViewMode {
    Normal,
    VertexColor,
    VertexNormal,
    Texture
};

class ViewerWidget : public QWidget {
    Q_OBJECT
public:
    ViewerWidget(QWidget* parent = nullptr);
    ~ViewerWidget();

    void setMeshes(const std::vector<MeshData>& meshes);
    void setViewMode(ViewMode mode);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    std::vector<MeshData> mMeshes;
    ViewMode mMode;
    QOpenGLShaderProgram* mProgram;
    
    struct MeshGL {
        // Use dummy pointer types if we can't link
        void* vbo_ptr;
        void* ibo_ptr;
        QOpenGLTexture* texture;
        int indexCount;
    };
    std::vector<MeshGL> mMeshGLs;
};

#endif // VIEWERWIDGET_H
