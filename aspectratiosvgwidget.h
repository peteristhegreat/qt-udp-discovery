#ifndef ASPECTRATIOSVGWIDGET_H
#define ASPECTRATIOSVGWIDGET_H

#include <QSvgWidget>
#include <QPaintEvent>

class AspectRatioSvgWidget : public QSvgWidget
{
    Q_OBJECT
public:
    AspectRatioSvgWidget(QString file, QWidget *parent = 0);

signals:

public slots:
    void paintEvent(QPaintEvent * e);
private:
    QSize m_prevSize;
    QRectF m_cached_rect;
};

#endif // ASPECTRATIOSVGWIDGET_H
