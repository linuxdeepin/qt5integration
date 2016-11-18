#include <qpa/qplatformthemeplugin.h>
#include "qdeepintheme.h"

QT_BEGIN_NAMESPACE

class QDeepinThemePlugin : public QPlatformThemePlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "deepin.json")

public:
    QPlatformTheme *create(const QString &key, const QStringList &params) Q_DECL_OVERRIDE;
};

QPlatformTheme *QDeepinThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    if (!key.compare(QLatin1String(QDeepinTheme::name), Qt::CaseInsensitive))
        return new QDeepinTheme;

    return 0;
}

QT_END_NAMESPACE

#include "main.moc"
