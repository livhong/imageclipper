#include <stdio.h>
#include <dirent.h>
#include <sys/time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Point point1;
Point point2;

const int FRAME_WIDTH = 64;
const int FRAME_HEIGHT = 128;

int img_width = 0;
int img_height = 0;

Mat srcImage;
Mat copyImg;

string window_name = "test";

string src_image_dir = "testcases/";
vector<string> fileNames(0);
unsigned int file_index = 0;

void mouseCallback(int event,int x,int y,int flags,void* param){

	if(event==EVENT_LBUTTONDOWN){
		point1.x = x;
		point1.y = y;

		if(x+FRAME_WIDTH>img_width){//右下角的点超出img_width
			point2.x = img_width;
			point1.x = img_width - FRAME_WIDTH;
		}else{
			point2.x = x + FRAME_WIDTH;
		}
		if(y+FRAME_HEIGHT>img_height){//右下角的点超出img_height
			point2.y = img_height;
			point1.y = img_height - FRAME_HEIGHT;
		}else{
			point2.y = y + FRAME_HEIGHT;
		}
		copyImg = srcImage.clone();
		rectangle(copyImg, Rect(point1, point2), Scalar(0, 0, 255));
		imshow(window_name, copyImg);
	}

}

void init(){

	struct dirent* ep;
	DIR* dp = opendir(src_image_dir.c_str());
	if(dp != NULL){
		while((ep = readdir(dp))){
			string filename = (string)ep->d_name;
			if(filename.size()-1!=filename.find_last_of(".")){
				fileNames.push_back(src_image_dir+filename);
			}
		}
	}

	if(fileNames.size()==0){
		cout << "No input image files!" << endl;
		exit(0);
	}

	srcImage = imread(fileNames[0]);
	copyImg = srcImage.clone();
	imshow(window_name,copyImg);
	img_width = srcImage.cols;
	img_height = srcImage.rows;
	setMouseCallback(window_name, mouseCallback);

}

void go_to_next_file(){
	file_index++;
	if(file_index<fileNames.size()){
		srcImage = imread(fileNames[file_index]);
		copyImg = srcImage.clone();
		img_width = srcImage.cols;
		img_height = srcImage.rows;
	}else{
		file_index = fileNames.size()-1;
	}
	point1.x = 0;
	point1.y = 0;
	point2.x = 0;
	point2.y = 0;
}

void go_to_prev_file(){
	file_index--;
	if(file_index>=0){
		srcImage = imread(fileNames[file_index]);
		copyImg = srcImage.clone();
		img_width = srcImage.cols;
		img_height = srcImage.rows;
	}else{
		file_index = 0;
	}
	point1.x = 0;
	point1.y = 0;
	point2.x = 0;
	point2.y = 0;
}

void extend(){
	resize(srcImage, srcImage, Size(1.1*srcImage.cols, 1.1*srcImage.rows));
	copyImg = srcImage.clone();
	img_width = srcImage.cols;
	img_height = srcImage.rows;
}

void narrow(){
	resize(srcImage, srcImage, Size(1/1.1*srcImage.cols, 1/1.1*srcImage.rows));
	copyImg = srcImage.clone();
	img_width = srcImage.cols;
	img_height = srcImage.rows;
}

void refresh(){
	srcImage = imread(fileNames[file_index]);
	copyImg = srcImage.clone();
	img_width = srcImage.cols;
	img_height = srcImage.rows;
}

long getCurrentTime()
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char** argv){

	init();

	int c = 0;

	while(c!='q'){
		c = waitKey(0);
		switch(c){
		case 'w':
			if(point1.y>0){
				point1.y--;
				point2.y--;
			}
			break;
		case 'a':
			if(point1.x>0){
				point1.x--;
				point2.x--;
			}
			break;
		case 's':
			if(point2.y<img_height-1){
				point1.y++;
				point2.y++;
			}
			break;
		case 'd':
			if(point2.x<img_width-1){
				point1.x++;
				point2.x++;
			}
			break;
		case '\r':{
				Rect myRect(point1, point2);
				Mat dest(srcImage, myRect);
				char buff[100];
				snprintf(buff, sizeof(buff), "output/clip_%ld.jpg", getCurrentTime());
				string fileName = buff;
				imwrite(fileName, dest);
				cout<<fileName<<endl;
			}
			break;
		case 'n':{
				go_to_next_file();
			}
			break;
		case 'p':{
				go_to_prev_file();
			}
			break;
		case ']':
			extend();
			break;
		case '[':
			narrow();
			break;
		case 'r':
			refresh();
			break;
		}
		copyImg = srcImage.clone();
		rectangle(copyImg, Rect(point1, point2), Scalar(0, 0, 255));
		imshow(window_name, copyImg);
	}


	return 0;//success
}
