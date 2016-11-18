#ifndef QDEEPINFILEDIALOGHELPER_H
#define QDEEPINFILEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h>

#include <QPointer>

QT_BEGIN_NAMESPACE

class DFileDialogHandle;

class QDeepinFileDialogHelper : public QPlatformFileDialogHelper
{
public:
    QDeepinFileDialogHelper();
    ~QDeepinFileDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) Q_DECL_OVERRIDE;
    void exec() Q_DECL_OVERRIDE;
    void hide() Q_DECL_OVERRIDE;

    bool defaultNameFilterDisables() const Q_DECL_OVERRIDE;
    void setDirectory(const QUrl &directory) Q_DECL_OVERRIDE;
    QUrl directory() const Q_DECL_OVERRIDE;
    void selectFile(const QUrl &filename) Q_DECL_OVERRIDE;
    QList<QUrl> selectedFiles() const Q_DECL_OVERRIDE;
    void setFilter() Q_DECL_OVERRIDE;
    void selectNameFilter(const QString &filter) Q_DECL_OVERRIDE;
    QString selectedNameFilter() const Q_DECL_OVERRIDE;

private:
    mutable QPointer<DFileDialogHandle> dialog;

    void ensureDialog() const;
    void applyOptions();
};

QT_END_NAMESPACE

#endif // QDEEPINFILEDIALOGHELPER_H
