#include "view.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

View::View(QWidget *parent)
    : QWidget(parent)
    , m_img(nullptr)
    , m_roi(10, 10, 100, 100)
    , m_isPressed(false)
    , m_status(MOUSE_STATUS::invalid)
{
    installEventFilter(parent);
}

View::~View()
{
    if (m_img) delete m_img;
}

void View::addImage(QImage *image)
{
    if (m_img) delete m_img;
    m_img = new QImage(*image);

    update();
}

void View::setROI(const QRect &roi)
{
    m_roi = roi;
    update();
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    QPoint p = e->pos();
    if (!m_isPressed)
    {
        // left
        if (abs(p.x() - m_roi.left()) <= 5)
        {
            //topleft
            if (abs(p.y() - m_roi.top()) <= 5)
            {
                setCursor(Qt::SizeFDiagCursor);
                m_status = leftTop;
            }
            //bottomleft
            else if (abs(p.y() - m_roi.bottom()) <= 5)
            {
                setCursor(Qt::SizeBDiagCursor);
                m_status = leftBottom;
            }
            // only left
            else
            {
                setCursor(Qt::SizeHorCursor);
                m_status = left;
            }
        }
        // right
        else if (abs(p.x() - m_roi.right()) <= 5)
        {
            //topright
            if (abs(p.y() - m_roi.top()) <= 5)
            {
                setCursor(Qt::SizeBDiagCursor);
                m_status = rightTop;
            }
            //bottomright
            else if (abs(p.y() - m_roi.bottom()) <= 5)
            {
                setCursor(Qt::SizeFDiagCursor);
                m_status = rightBottom;
            }
            //only right
            else
            {
                setCursor(Qt::SizeHorCursor);
                m_status = right;
            }
        }
        // only top
        else if (abs(p.y() - m_roi.top()) <= 5)
        {
            setCursor(Qt::SizeVerCursor);
            m_status = top;
        }
        // only bottom
        else if (abs(p.y() - m_roi.bottom()) <= 5)
        {
            setCursor(Qt::SizeVerCursor);
            m_status = bottom;
        }
        // in
        else if (m_roi.contains(p))
        {
            setCursor(Qt::OpenHandCursor);
            m_status = move;
        }
        // out
        else
        {
            setCursor(Qt::ArrowCursor);
            m_status = move;
        }
    }
    else
    {
        QRect rect_temp(m_roi);
        int x, y;
        
        switch (m_status)
        {
        case move:
            m_roi = QRect(p - m_point + m_roi.topLeft(), rect_temp.size());
            break;
        case left:
            x = qMin(p.x(), rect_temp.right() - 1);
            m_roi = QRect(x, rect_temp.y(), rect_temp.right() - x + 1, rect_temp.height());
            break;
        case right:
            x = qMax(p.x(), rect_temp.left() + 1);
            m_roi = QRect(rect_temp.x(), rect_temp.y(), x - rect_temp.x() + 1, rect_temp.height());
            break;
        case top:
            y = qMin(p.y(), rect_temp.bottom() - 1);
            m_roi = QRect(rect_temp.x(), y, rect_temp.width(), rect_temp.bottom() - y + 1);
            break;
        case bottom:
            y = qMax(p.y(), rect_temp.top() + 1);
            m_roi = QRect(rect_temp.x(), rect_temp.y(), rect_temp.width(), y - rect_temp.top() + 1);
            break;
        case leftTop:
            x = qMin(p.x(), rect_temp.right() - 1);
            y = qMin(p.y(), rect_temp.bottom() - 1);
            m_roi = QRect(x, y, rect_temp.right() - x + 1, rect_temp.bottom() - y + 1);
            break;
        case rightTop:
            x = qMax(p.x(), rect_temp.left() + 1);
            y = qMin(p.y(), rect_temp.bottom() - 1);
            m_roi = QRect(rect_temp.x(), y, x - rect_temp.x() + 1, rect_temp.bottom() - y + 1);
            break;
        case leftBottom:
            x = qMin(p.x(), rect_temp.right() - 1);
            y = qMax(p.y(), rect_temp.top() + 1);
            m_roi = QRect(x, rect_temp.y(), rect_temp.right() - x + 1, y - rect_temp.top() + 1);
            break;
        case rightBottom:
            x = qMax(p.x(), rect_temp.left() + 1);
            y = qMax(p.y(), rect_temp.top() + 1);
            m_roi = QRect(rect_temp.x(), rect_temp.y(), x - rect_temp.x() + 1, y - rect_temp.top() + 1);
            break;
        default:
            break;
        }
        emit roiValueChanged(m_roi);
        update();
    }

    m_point = e->pos();
}

void View::mousePressEvent(QMouseEvent *e)
{
    m_isPressed = true;
    if (cursor() == Qt::OpenHandCursor)
        setCursor(Qt::ClosedHandCursor);
    m_point = e->pos();
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    (void)e;
    m_isPressed = false;
    if (cursor() == Qt::ClosedHandCursor)
        setCursor(Qt::OpenHandCursor);
}

void View::paintEvent(QPaintEvent *e)
{
    (void)e;

    QPainter painter(this);
    painter.setPen(Qt::red);

    if (m_img)
        painter.drawImage(QPoint(0, 0), *m_img);

    painter.drawRect(m_roi);
}
