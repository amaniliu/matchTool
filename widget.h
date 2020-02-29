#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_view_roiValueChanged(const QRect& rect);

    void on_spinBox_x_editingFinished();

    void on_spinBox_y_editingFinished();

    void on_spinBox_w_editingFinished();

    void on_spinBox_h_editingFinished();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::Widget *ui;
    int m_num_mark;
    QString m_tempfilename;
    QStringList m_filenames;
    QString m_savePath;
};

#endif // WIDGET_H
