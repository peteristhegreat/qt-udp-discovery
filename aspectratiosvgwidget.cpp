#include "aspectratiosvgwidget.h"
#include <QPainter>
#include <QSvgRenderer>
#include <QRectF>
#include <QDebug>

AspectRatioSvgWidget::AspectRatioSvgWidget(QString file, QWidget *parent) :
    QSvgWidget(file, parent)
{
}


void AspectRatioSvgWidget::paintEvent(QPaintEvent * )
{
    QPainter painter(this);
    if(this->size() != m_prevSize)
    {
        QSize sd = this->renderer()->defaultSize();
        QSize sw = this->size();
        qreal ratio_x = (qreal)sw.width() / sd.width();
        qreal ratio_y = (qreal)sw.height() / sd.height();
//        qDebug() << "ratios" << this->size();
        QRectF new_rect;
        if(ratio_x < ratio_y)
        {
            new_rect.setWidth(sw.width());
            new_rect.setHeight(sw.width() * (qreal) sd.height() /sd.width());
            new_rect.moveTopLeft(QPointF(0, (sw.height() - new_rect.height())/2));
        }
        else
        {
            new_rect.setWidth(sw.height() * (qreal)sd.width()/sd.height());
            new_rect.setHeight(sw.height());
            new_rect.moveTopLeft(QPointF((sw.width() - new_rect.width())/2, 0));
        }
        m_prevSize = this->size();
        m_cached_rect = new_rect;
    }
    this->renderer()->render(&painter, m_cached_rect);
}
