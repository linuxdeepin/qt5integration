#include "qdeepinfiledialoghelper.h"
#include "qdeepintheme.h"

#include <QDialog>
#include <QEvent>
#include <QWindow>
#include <QApplication>
#include <QDebug>

#include <private/qwidgetwindow_p.h>

#include <dfiledialoghandle.h>

QT_BEGIN_NAMESPACE

QDeepinFileDialogHelper::QDeepinFileDialogHelper()
{

}

QDeepinFileDialogHelper::~QDeepinFileDialogHelper()
{
    if (dialog)
        dialog->deleteLater();
}

bool QDeepinFileDialogHelper::show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent)
{
    qDebug() << flags << modality << parent;

    ensureDialog();
    applyOptions();

    if (parent && parent->metaObject()->className() == QStringLiteral("QWidgetWindow")) {
        qApp->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
        dialog->setParent(static_cast<QWidgetWindow*>(parent)->widget());
    }

    dialog->widget()->setWindowModality(modality);
    dialog->widget()->setWindowFlags(flags | dialog->widget()->windowFlags());
    dialog->show();

    return true;
}

void QDeepinFileDialogHelper::exec()
{
    qDebug() << "exec";

    ensureDialog();
    applyOptions();

    if (dialog->widget()->isVisible())
        dialog->hide();

    dialog->exec();
}

void QDeepinFileDialogHelper::hide()
{
    qDebug() << "hide";

    ensureDialog();

    dialog->hide();
}

bool QDeepinFileDialogHelper::defaultNameFilterDisables() const
{
    qDebug() << __FUNCTION__;

    return true;
}

void QDeepinFileDialogHelper::setDirectory(const QUrl &directory)
{
    qDebug() << __FUNCTION__ << directory;

    ensureDialog();

    dialog->setDirectoryUrl(directory);
}

QUrl QDeepinFileDialogHelper::directory() const
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    return dialog->directoryUrl();
}

void QDeepinFileDialogHelper::selectFile(const QUrl &filename)
{
    qDebug() << __FUNCTION__ << filename;

    ensureDialog();

    dialog->selectUrl(filename);
}

QList<QUrl> QDeepinFileDialogHelper::selectedFiles() const
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    return dialog->selectedUrls();
}

void QDeepinFileDialogHelper::setFilter()
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    dialog->setFilter(options()->filter());
}

void QDeepinFileDialogHelper::selectNameFilter(const QString &filter)
{
    qDebug() << __FUNCTION__ << filter;

    ensureDialog();

    dialog->selectNameFilter(filter);
}

QString QDeepinFileDialogHelper::selectedNameFilter() const
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    return dialog->selectedNameFilter();
}

void QDeepinFileDialogHelper::ensureDialog() const
{
    if (dialog)
        return;

    QDeepinTheme::m_usePlatformNativeDialog = false;

    dialog = new DFileDialogHandle();

    connect(dialog, &DFileDialogHandle::accepted, this, &QDeepinFileDialogHelper::accept);
    connect(dialog, &DFileDialogHandle::rejected, this, &QDeepinFileDialogHelper::reject);

    QDeepinTheme::m_usePlatformNativeDialog = true;
}

void QDeepinFileDialogHelper::applyOptions()
{
    QFileDialogOptions *options = this->options().data();

    for (int i = 0; i < QFileDialogOptions::DialogLabelCount; ++i) {
        if (options->isLabelExplicitlySet((QFileDialogOptions::DialogLabel)i)) {
            dialog->setLabelText((QFileDialog::DialogLabel)i, options->labelText((QFileDialogOptions::DialogLabel)i));
        }
    }

    dialog->setFilter(options->filter());
    dialog->widget()->setWindowTitle(options->windowTitle());
    dialog->setViewMode((QFileDialog::ViewMode)options->viewMode());
    dialog->setFileMode((QFileDialog::FileMode)options->fileMode());
    dialog->setAcceptMode((QFileDialog::AcceptMode)options->acceptMode());
    dialog->setNameFilters(options->nameFilters());
    dialog->setOptions((QFileDialog::Options)(int)options->options());

    setDirectory(options->initialDirectory());

    foreach (const QUrl &filename, options->initiallySelectedFiles())
        selectFile(filename);

    selectNameFilter(options->initiallySelectedNameFilter());
}

QT_END_NAMESPACE
