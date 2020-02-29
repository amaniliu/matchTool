#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QImage>

class View : public QWidget
{
    Q_OBJECT

    enum MOUSE_STATUS
    {
        invalid,
        move,
        left,
        right,
        top,
        bottom,
        leftTop,
        rightTop,
        leftBottom,
        rightBottom
    };

public:
    explicit View(QWidget *parent = nullptr);

    ~View();

    void addImage(QImage* image);

    inline QRect getROI() const { return m_roi; }

    void setROI(const QRect& roi);

signals:
    void roiValueChanged(const QRect& rect);

protected:
    void mouseMoveEvent(QMouseEvent* e);

    void mousePressEvent(QMouseEvent* e);

    void mouseReleaseEvent(QMouseEvent* e);

    void paintEvent(QPaintEvent* e);

private:

    QImage* m_img;
    QRect m_roi;

    bool m_isPressed;
    MOUSE_STATUS m_status;

    QPoint m_point;
};

#endif // VIEW_H
