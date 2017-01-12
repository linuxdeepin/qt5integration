#ifndef QDEEPINFILEDIALOGHELPER_H
#define QDEEPINFILEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h>

#include <QPointer>

QT_BEGIN_NAMESPACE

class QFileDialog;
class ComDeepinFilemanagerFiledialogInterface;
typedef ComDeepinFilemanagerFiledialogInterface DFileDialogHandle;
class ComDeepinFilemanagerFiledialogmanagerInterface;
typedef ComDeepinFilemanagerFiledialogmanagerInterface DFileDialogManager;
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

    static void initDBusFileDialogManager();

private:
    mutable QPointer<DFileDialogHandle> nativeDialog;
    mutable QPointer<QWindow> auxiliaryWindow;
    mutable QPointer<QFileDialog> qtDialog;
    QPointer<QWindow> activeWindow;
    static DFileDialogManager *manager;

    void ensureDialog() const;
    void applyOptions();

    friend class QDeepinTheme;
};

QT_END_NAMESPACE

#endif // QDEEPINFILEDIALOGHELPER_H
