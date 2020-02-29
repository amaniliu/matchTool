#include "widget.h"
#include "ui_widget.h"
#include <opencv2/opencv.hpp>

#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_num_mark(4)
{
    ui->setupUi(this);

    connect(ui->view, SIGNAL(roiValueChanged(const QRect&)), this, SLOT(on_view_roiValueChanged(const QRect&)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_view_roiValueChanged(const QRect& rect)
{
    ui->spinBox_x->setValue(rect.x());
    ui->spinBox_y->setValue(rect.y());
    ui->spinBox_w->setValue(rect.width());
    ui->spinBox_h->setValue(rect.height());
}

void Widget::on_pushButton_clicked()
{
    m_tempfilename = QFileDialog::getOpenFileName(this, "open file", "./", "Images(*.png *.bmp *.jpg *.jpeg)");
    if (m_tempfilename.isEmpty()) return;

    QImage img(m_tempfilename);
    ui->view->addImage(&img);
}

void Widget::on_spinBox_x_editingFinished()
{
    ui->view->setROI(QRect(ui->spinBox_x->value(), ui->spinBox_y->value(), ui->spinBox_w->value(), ui->spinBox_h->value()));
}

void Widget::on_spinBox_y_editingFinished()
{
    ui->view->setROI(QRect(ui->spinBox_x->value(), ui->spinBox_y->value(), ui->spinBox_w->value(), ui->spinBox_h->value()));
}

void Widget::on_spinBox_w_editingFinished()
{
    ui->view->setROI(QRect(ui->spinBox_x->value(), ui->spinBox_y->value(), ui->spinBox_w->value(), ui->spinBox_h->value()));
}

void Widget::on_spinBox_h_editingFinished()
{
    ui->view->setROI(QRect(ui->spinBox_x->value(), ui->spinBox_y->value(), ui->spinBox_w->value(), ui->spinBox_h->value()));
}

void Widget::on_pushButton_2_clicked()
{
    m_filenames = QFileDialog::getOpenFileNames(this, tr("Open File"), "", tr("Images(*.bmp *.jpg *.jpeg *.png *.tiff)"));
    ui->listWidget->clear();
    for (const QString& filename : m_filenames)
        ui->listWidget->addItem(filename);
}

void Widget::on_pushButton_3_clicked()
{
    m_savePath = QFileDialog::getExistingDirectory(this, tr("Save Path"));
    ui->lineEdit_2->setText(m_savePath);
}

void func1(const cv::Mat& templ, const cv::Mat& src, const cv::Mat& templ_roi, QRect roi, const QString& filename, const QString& savePath)
{
    cv::Mat matchImg;
    cv::matchTemplate(src, templ_roi, matchImg, cv::TM_SQDIFF);
    cv::normalize(matchImg, matchImg, 0, 1, cv::NORM_MINMAX);

    cv::Point minPoint;
    cv::Point maxPoint;
    double* minVal = nullptr;
    double* maxVal = nullptr;
    cv::minMaxLoc(matchImg, minVal, maxVal, &minPoint, &maxPoint);

    cv::Mat templ_clone, src_clone;

    cv::copyMakeBorder(templ, templ_clone,
        roi.y() > minPoint.y ? 0 : (minPoint.y - roi.y()),
        (templ.rows - roi.y()) > (src.rows - minPoint.y) ? 0 : (src.rows - minPoint.y - templ.rows + roi.y()),
        roi.x() > minPoint.x ? 0 : (minPoint.x - roi.x()),
        (templ.cols - roi.x()) > (src.cols - minPoint.x) ? 0 : (src.cols - minPoint.x - templ.cols + roi.x()),
        cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::copyMakeBorder(src, src_clone,
        minPoint.y > roi.y() ? 0 : (roi.y() - minPoint.y),
        (src.rows - minPoint.y) > (templ.rows - roi.y()) ? 0 : (templ.rows - roi.y() - src.rows + minPoint.y),
        minPoint.x > roi.x() ? 0 : (roi.x() - minPoint.x),
        (src.cols - minPoint.x) > (templ.cols - roi.x()) ? 0 : (templ.cols - roi.x() - src.cols + minPoint.x),
        cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat dst;
    cv::addWeighted(templ_clone, 0.5, src_clone, 0.5, 0, dst);

    QFileInfo info(filename);
    QString path = (savePath.isEmpty() ? "." : savePath) + "/" + info.fileName();
    cv::imwrite(path.toLocal8Bit().data(), dst);
}

cv::RotatedRect getBoundingRect(const std::vector<cv::Point>& contour)
{
    if (contour.empty()) return {};

    cv::RotatedRect rect = minAreaRect(contour);
    if (rect.angle < -45)
    {
        rect.angle += 90;
        float temp = rect.size.width;
        rect.size.width = rect.size.height;
        rect.size.height = temp;
    }

    return rect;
}

cv::RotatedRect func2(const cv::Mat& templ, const cv::Mat& templ_roi, int num_of_templ)
{
    cv::Mat matchImg;
    cv::matchTemplate(templ, templ_roi, matchImg, cv::TM_SQDIFF);
    cv::normalize(matchImg, matchImg, 0, 1, cv::NORM_MINMAX);

    QVector<cv::Point> minPoints(num_of_templ);
    QVector<float> minValue(num_of_templ);

    for (int i = 0; i < num_of_templ; i++)
        minValue[i] = 1;

    for (int y = 0, rows = matchImg.rows; y < rows; y++)
    {
        for (int x = 0, cols = matchImg.cols; x < cols; x++)
        {
            float v = matchImg.at<float>(y, x);
            for (int index = 0; index < num_of_templ; index++)
            {
                if (v < minValue[index])
                {
                    minValue.insert(index, v);
                    minValue.removeLast();
                    minPoints.insert(index, cv::Point(x, y));
                    minPoints.removeLast();
                    break;
                }
            }
        }
    }

    std::vector<cv::Point> temp_minPoints(static_cast<size_t>(num_of_templ));
    for (int i = 0; i < num_of_templ; i++)
        temp_minPoints[static_cast<uint>(i)] = minPoints[i];
    return getBoundingRect(temp_minPoints);
}

void Widget::on_pushButton_4_clicked()
{
    cv::Mat templ = cv::imread(m_tempfilename.toLocal8Bit().data(), cv::IMREAD_ANYCOLOR);
    const QRect roi = ui->view->getROI();
    cv::Mat templ_roi = templ(cv::Rect(roi.x(), roi.y(), roi.width(), roi.height())).clone();

    cv::RotatedRect rrect_templ = func2(templ, templ_roi, m_num_mark);
    cv::Point center_templ = rrect_templ.center;

    for (const QString& filename : m_filenames)
    {
        cv::Mat src = cv::imread(filename.toLocal8Bit().data(), cv::IMREAD_ANYCOLOR);
        if (src.channels() != templ_roi.channels()) continue;
        //func1(templ, src, templ_roi, roi, filename, m_savePath);

        cv::RotatedRect rrect_src = func2(templ, templ_roi, m_num_mark);
        cv::Mat affine_matrix = cv::getRotationMatrix2D(rrect_src.center, static_cast<double>(rrect_templ.angle - rrect_src.angle), 1.0);
        cv::warpAffine(src, src, affine_matrix, src.size());

        cv::Mat templ_clone, src_clone;
        cv::Point center_src = rrect_src.center;
        cv::copyMakeBorder(templ, templ_clone,
            center_templ.y > center_src.y ? 0 : (center_src.y - center_templ.y),
            (templ.rows - center_templ.y) > (src.rows - center_src.y) ? 0 : (src.rows - center_src.y - templ.rows + center_templ.y),
            center_templ.x > center_src.x ? 0 : (center_src.x - center_templ.x),
            (templ.cols - center_templ.x) > (src.cols - center_src.x) ? 0 : (src.cols - center_src.x - templ.cols + center_templ.x),
            cv::BORDER_CONSTANT, cv::Scalar::all(0));
        cv::copyMakeBorder(src, src_clone,
            center_src.y > center_templ.y ? 0 : (center_templ.y - center_src.y),
            (src.rows - center_src.y) > (templ.rows - center_templ.y) ? 0 : (templ.rows - center_templ.y - src.rows + center_src.y),
            center_src.x > center_templ.x ? 0 : (center_templ.x - center_src.x),
            (src.cols - center_src.x) > (templ.cols - center_templ.x) ? 0 : (templ.cols - center_templ.x - src.cols + center_src.x),
            cv::BORDER_CONSTANT, cv::Scalar::all(0));
        cv::Mat dst;
        cv::addWeighted(templ_clone, 0.5, src_clone, 0.5, 0, dst);

        QFileInfo info(filename);
        QString path = (m_savePath.isEmpty() ? "." : m_savePath) + "/" + info.fileName();
        cv::imwrite(path.toLocal8Bit().data(), dst);

    }

    QMessageBox::information(this, "info", "finished!");
}

void Widget::on_spinBox_valueChanged(int arg1)
{
    m_num_mark = arg1;
}
