#include "ImageProcess.h"
#include "ui_subwindow.h"
#include <QLabel>
#include <QDebug>
#include <iostream>
ImageProcess::ImageProcess(QObject *parent) : QThread(parent) {
    subWidget = new QWidget();
    ui.setupUi(subWidget);

    method = 5;
    xbee = new Xbee("/dev/ttyUSB0");
    subWidget->show();
}

ImageProcess::~ImageProcess() {
    delete this;
}

void ImageProcess::run() {

    qDebug() << "New thread started successfully!!";

}

void ImageProcess::processImage(cv::Mat &image) {
    cv::Mat process;
    vector<vector<Point> > squares;

    // Determine how to process the image by mode
    switch(method) {
        case 0: process = image;
                break;
        case 1: process = edgeDetection(image);
                break;
        case 2: process = findCircles(image);
                break;
        case 3: findSquares(image, squares);
                drawSquares(image, squares);
                process = image;
                break;
        case 4: process = findLines(image);
                break;
        case 5: process = skin_detect(image);
                break;
        default:process = image;
                break;
    }

    // Display processed image to another widget.
    QPixmap pix = QPixmap::fromImage(imageConvert(process));
    ui.displayLabel->setPixmap(pix);

    // Free the memory to avoid overflow.
    image.release();
}
bool ImageProcess::R1(int R, int G, int B) {
    bool e1 = (R>95) && (G>40) && (B>20) && ((max(R,max(G,B)) - min(R, min(G,B)))>15) && (abs(R-G)>15) && (R>G) && (R>B);
    bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
    return (e1||e2);
}

bool ImageProcess::R2(float Y, float Cr, float Cb) {
    bool e3 = Cr <= 1.5862*Cb+20;
    bool e4 = Cr >= 0.3448*Cb+76.2069;
    bool e5 = Cr >= -4.5652*Cb+234.5652;
    bool e6 = Cr <= -1.15*Cb+301.75;
    bool e7 = Cr <= -2.2857*Cb+432.85;
    return e3 && e4 && e5 && e6 && e7;
}

bool ImageProcess::R3(float H, float S, float V) {
    return (H<25) || (H > 230);
}
cv::Mat ImageProcess::skin_detect (cv::Mat &src)
{
    Mat dst = src.clone();

        Vec3b cwhite = Vec3b::all(255);
        Vec3b cblack = Vec3b::all(0);

        Mat src_ycrcb, src_hsv;
        // OpenCV scales the YCrCb components, so that they
        // cover the whole value range of [0,255], so there's
        // no need to scale the values:
        cvtColor(src, src_ycrcb, CV_BGR2YCrCb);
        // OpenCV scales the Hue Channel to [0,180] for
        // 8bit images, so make sure we are operating on
        // the full spectrum from [0,360] by using floating
        // point precision:
        src.convertTo(src_hsv, CV_32FC3);
        cvtColor(src_hsv, src_hsv, CV_BGR2HSV);
        // Now scale the values between [0,255]:
        normalize(src_hsv, src_hsv, 0.0, 255.0, NORM_MINMAX, CV_32FC3);
        int count = 0;
        int count_left = 0;
        int count_middle = 0;
        int count_right = 0;
        for(int i = 0; i < src.rows; i++) {
            for(int j = 0; j < src.cols; j++) {

                Vec3b pix_bgr = src.ptr<Vec3b>(i)[j];
                int B = pix_bgr.val[0];
                int G = pix_bgr.val[1];
                int R = pix_bgr.val[2];
                // apply rgb rule
                bool a = R1(R,G,B);

                Vec3b pix_ycrcb = src_ycrcb.ptr<Vec3b>(i)[j];
                int Y = pix_ycrcb.val[0];
                int Cr = pix_ycrcb.val[1];
                int Cb = pix_ycrcb.val[2];
                // apply ycrcb rule
                bool b = R2(Y,Cr,Cb);

                Vec3f pix_hsv = src_hsv.ptr<Vec3f>(i)[j];
                float H = pix_hsv.val[0];
                float S = pix_hsv.val[1];
                float V = pix_hsv.val[2];
                // apply hsv rule
                bool c = R3(H,S,V);

                if(!(a&&b&&c)){
                    dst.ptr<Vec3b>(i)[j] = cblack;
                }
		// the camera range in Nitrogen6x is 250~1280
                else{
                    count ++;
                    if (j<500&&j>250){
                        count_left ++;
                    }
                    else if (j<1280&&j>1030){
                        count_right ++;
                    }
                    else if(j<1030&&j>500){
                        count_middle ++;
                    }
                }
                if (j==200){
                    dst.ptr<Vec3b>(i)[j] = cwhite;
                }
                else if(j==430){
                    dst.ptr<Vec3b>(i)[j] = cwhite;
                }
            }
        }
        int THRESHOLD_1 = 7000;
        int THRESHOLD = 3000;

        if (count_middle<THRESHOLD&&count_left<THRESHOLD&&count_right<THRESHOLD){
            // backward slowly
            xbee->start();
            xbee->setDir('5');
            xbee->start();
            cout<<"backward"<<endl;
        }
        else if (count_middle<THRESHOLD_1&&count_left<THRESHOLD_1&&count_right<THRESHOLD_1){
            // stop
            xbee->setDir('4');
            xbee->start();
            cout<<"stop"<<endl;
        }
        else if ((count_left>count_middle)&&(count_left>count_right)){
            // turn left
            xbee->setDir('2');
            xbee->start();
            cout<<"left"<<endl;
        }
        else if ((count_right>count_middle)&&(count_left<count_right)){
            // turn right
            xbee->setDir('3');
            xbee->start();
            cout<<"right"<<endl;
        }
        else if (count_left<count_middle&&count_middle>count_right){
            // forward
            xbee->setDir('1');
            xbee->start();
            cout<<"foward"<<endl;
        }
        else{
            // stop
            xbee->setDir('4');
            xbee->start();
            cout<<"stop"<<endl;
        }

        return dst;
}

cv::Mat ImageProcess::skin_detect(cv::Mat &image)
{

    int avg_cb = 120;//YCbCr顏色空間膚色cb的平均值
    int avg_cr = 155;//YCbCr顏色空間膚色cr的平均值
    int skin_range = 10;//YCbCr顏色空間膚色的範圍

    int iLowY = 0;
    int iHighY = 20;

    int iLowcb = 10;
    int iHighcb = 150;

    int iLowcr = 60;
    int iHighcr = 255;
    cv::Mat dist;
    cv::cvtColor(image, dist,COLOR_BGR2HSV);
    cv::Mat imgThresholded;

    inRange(dist, Scalar(iLowY, iLowcb, iLowcr), Scalar(iHighY, iHighcb, iHighcr), imgThresholded);

    //morphological opening (remove small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //morphological closing (fill small holes in the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    Moments oMoments = moments(imgThresholded);
      double dM01 = oMoments.m01;
      double dM10 = oMoments.m10;
      double dArea = oMoments.m00;
      int iLastX = -1;
       int iLastY = -1;
      if (dArea > 20000)
        {
         //calculate the position of the ball
         cout<<"True"<<endl;
        }
      else{
          cout<<"False"<<endl;
      }

    return dist;
}



cv::Mat ImageProcess::edgeDetection(cv::Mat &image) {
    cv::Mat edge;
    int lowThreshold = 10;
    int ratio = 3;
    int kernel_size = 3;

    // Convert it to gray
    cv::cvtColor(image, edge, CV_BGR2GRAY);

    // Run the edge detector on grayscale
    cv::blur(edge, edge, cv::Size(9,9));
    cv::Canny(edge, edge, lowThreshold, lowThreshold*ratio, kernel_size);

    return edge;
}

void ImageProcess::findSquares(cv::Mat &image, vector<vector<Point> > &squares) {
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    int thresh = 50, N = 11;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        cv::mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cv::Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                cv::dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                // tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                cv::approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    cv::isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}

void ImageProcess::drawSquares(cv::Mat &image, vector<vector<Point> > &squares) {
    // the function draws all the squares in the image
    for( size_t i = 0; i < squares.size(); i++ ) {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();

        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
    }
}

cv::Mat ImageProcess::findCircles(cv::Mat &image) {
    cv::Mat circle;
    vector<Vec3f> circles;

    // Convert it to gray
    cvtColor(image, circle, CV_BGR2GRAY);

    // Reduce the noise so we avoid false circle detection
    GaussianBlur(circle, circle, Size(9, 9), 2, 2);

    // Apply the Hough Transform to find the circles
    HoughCircles(circle, circles, CV_HOUGH_GRADIENT, 1, circle.rows/16, 150, 30, 0, 0);

    // Draw the circles detected
    for(size_t i = 0; i < circles.size(); i++ ) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);

        // circle center
        cv::circle(image, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        cv::circle(image, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
    }

    return image;
}

cv::Mat ImageProcess::findLines(cv::Mat &image) {
    cv::Mat line;
    vector<Vec4i> lines;

    // Convert it to gray
    cvtColor(image, line, CV_BGR2GRAY);

    // Detect the edges of the image by using a Canny detector
    Canny(line, line, 50, 200, 3);

    // Apply the Hough Transform to find the line segments
    HoughLinesP(line, lines, 1, CV_PI/180, 50, 50, 10);

    // Draw the line segments detected
    for(size_t i = 0; i < lines.size(); i++) {
      Vec4i l = lines[i];

      cv::line(image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }

    return image;
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double ImageProcess::angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// The following slots are used to set image processing mode.
void ImageProcess::changeToNormal() {
    method = 0;
}

void ImageProcess::changeToEdgeDetection() {
    method = 1;
}

void ImageProcess::changeToCircleDetection() {
    method = 2;
}

void ImageProcess::changeToSquareDetection() {
    method = 3;
}

void ImageProcess::changeToLineDetection() {
    method = 4;
}

QImage ImageProcess::imageConvert(cv::Mat &matImage) {
    QImage::Format format;

    // If we use edge detection, we will use gray scale to display image.
    switch(method) {
        case 1: format = QImage::Format_Indexed8;
                break;
        default:format = QImage::Format_RGB888;
                break;
    }

    // Convert processed openCV frame to Qt's image format in order to display.
    QImage qImage(
        (uchar*)matImage.data,
        matImage.cols,
        matImage.rows,
        matImage.step,
        format
        );

    return qImage.rgbSwapped().mirrored(true, false);
}
